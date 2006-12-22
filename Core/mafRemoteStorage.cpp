/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRemoteStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-22 11:14:13 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafRemoteStorage.h"
#include <wx/tokenzr.h>

#include "mafDirectory.h"
#include "mafCurlUtility.h"

// Used with FTP Upload
#define UPLOAD_FILE_AS  "while-uploading.txt"
#define RENAME_FILE_TO  "renamed-and-fine.txt"

#include <curl/types.h>
#include <curl/easy.h>

struct LocalFileDownloaded {
  char *filename;
  FILE *stream;
};

mafRemoteStorage * mafRemoteStorage::m_ProgressListener = NULL;

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafRemoteStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafRemoteStorage::mafRemoteStorage()
//------------------------------------------------------------------------------
{
  m_UserName = ""; // blank user name means anonymous user.
  m_Pwd = "";
  m_HostName = "";
  m_Port = 0;
  m_LocalCacheFolder = wxGetCwd();
  m_LocalMSFFolder = "";
  m_RemoteRepository = "";
  m_RemoteMSF = "";
  m_EnableCertificateAuthentication = false;
  m_IsRemoteMSF = false;
  m_SpeedUpload = 0.0;
  m_TotalTime   = 0.0;

  m_LocalStream = NULL;
  m_Headerlist  = NULL;

  mafRemoteStorage::m_ProgressListener = this;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  m_Curl = curl_easy_init();
}

//------------------------------------------------------------------------------
mafRemoteStorage::~mafRemoteStorage()
//------------------------------------------------------------------------------
{
  curl_easy_cleanup(m_Curl);
  curl_global_cleanup();
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetLocalCacheFolder(mafString cache)
//------------------------------------------------------------------------------
{
  m_LocalCacheFolder = cache;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetUsername(mafString usr)
//------------------------------------------------------------------------------
{
  m_UserName = usr;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetPassword(mafString pwd)
//------------------------------------------------------------------------------
{
  m_Pwd = pwd;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetHostName(mafString host)
//------------------------------------------------------------------------------
{
  m_HostName = host;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetRemotePort(int port)
//------------------------------------------------------------------------------
{
  m_Port = port;
}
//------------------------------------------------------------------------------
int mafRemoteStorage::ResolveInputURL(const char *url, mafString &filename)
//------------------------------------------------------------------------------
{
  int res = MAF_OK;
  wxString path;
  path = wxPathOnly(url);
  if (path.IsEmpty())
  {
    wxString base_path;
    base_path = wxPathOnly(m_ParserURL.GetCStr());

    filename = base_path;

    if (!base_path.IsEmpty())
      filename << "/";

    filename << url;
  }
  else
  {
    filename = url;
  }

  mafString protocol = "";
  m_IsRemoteMSF = IsRemote(filename,protocol);
  m_EnableCertificateAuthentication = protocol.Equals("https");

  if (m_IsRemoteMSF)
  {
    // Download the file if it is not present into the cache
    mafString local_filename;
    res = DownloadRemoteFile(filename, local_filename);
    m_RemoteRepository = wxPathOnly(filename.GetCStr()).c_str();
    filename = local_filename;
  }
  else
  {
    m_RemoteRepository = "";
  }

  return res;
}
//------------------------------------------------------------------------------
int mafRemoteStorage::StoreToURL(const char *filename, const char *url)
//------------------------------------------------------------------------------
{
  int save_res = MAF_ERROR;
  if (m_IsRemoteMSF)
  {
    assert(url); // NULL url not yet supported

    // currently no real URL support
    wxString path, base_path, fullpathname;
    path = wxPathOnly(url);

    if (path.IsEmpty())
    {
      // if local file prepends base_path
      base_path = m_LocalMSFFolder.GetCStr();
      if (!base_path.IsEmpty())
      {
        fullpathname = base_path + "/" + url;
      }
      else
      {
        fullpathname = url;
      }

      if (IsFileInDirectory(url)) // IsFileInDirectory accepts URL specifications
      {
        // remove old file if present.
        // With remote files should be sufficient overwrite it by uploading the new one.
        if (IsFileInLocalDirectory(url))
        {
          DeleteURL(url);
        }
      }

      // currently only local files are supported
      save_res = wxRenameFile(filename, fullpathname) ? MAF_OK : MAF_ERROR;
    }
    else
    {
      wxString baseName = wxFileNameFromPath(url);
      fullpathname = m_LocalMSFFolder;
      fullpathname += "\\";
      fullpathname += baseName;
      // remove old file if present
      wxRemoveFile(fullpathname);
      // currently only local files are supported
      save_res = wxRenameFile(filename, fullpathname) ? MAF_OK : MAF_ERROR;
    }
    if (save_res == MAF_OK && !m_RemoteRepository.IsEmpty())
    {
      mafString remote_file = m_RemoteRepository + "/" + wxFileNameFromPath(url);
      save_res = UploadLocalFile(fullpathname.c_str(), remote_file);
    }
  }
  else
  {
    return mafXMLStorage::StoreToURL(filename,url);
  }
  return save_res;
}
//----------------------------------------------------------------------------
bool mafRemoteStorage::IsFileInLocalDirectory(const char *filename)
//----------------------------------------------------------------------------
{
  return m_LocalFilesDictionary.find(filename)!=m_LocalFilesDictionary.end();
}

//----------------------------------------------------------------------------
int mafRemoteStorage::OpenDirectory(const char *pathname)
//----------------------------------------------------------------------------
{
  if (m_IsRemoteMSF)
  {
    wxString baseName = wxFileNameFromPath(m_RemoteMSF.GetCStr());
    wxString path, name, ext, query_string;
    wxSplitPath(baseName,&path,&name,&ext);
    query_string = m_RemoteRepository;
    query_string += "/";
    query_string += "msfList?prefix=";
    query_string += name;

    struct msfTreeSearchReult chunk;
    chunk.memory=NULL; // we expect realloc(NULL, size) to work 
    chunk.size = 0;    // no data at this point 

    mafString auth = m_UserName;
    auth += ":";
    auth += m_Pwd;
    curl_easy_reset(m_Curl);
    curl_easy_setopt(m_Curl, CURLOPT_URL, query_string.c_str());
    curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
    curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());
    curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &chunk);

    m_Result = curl_easy_perform(m_Curl);
    if (m_Result == CURLE_OK)
    {
      m_FilesDictionary.clear();
      wxString msf_list = chunk.memory;
      wxStringTokenizer tkz(msf_list, "\n");
      while (tkz.HasMoreTokens())
      {
        m_FilesDictionary.insert(tkz.GetNextToken().c_str());
      }
    }
    else
      return MAF_ERROR;
    return OpenLocalMSFDirectory();
  }
  else
  {
    return mafXMLStorage::OpenDirectory(pathname);
  }
}
//------------------------------------------------------------------------------
int mafRemoteStorage::OpenLocalMSFDirectory()
//------------------------------------------------------------------------------
{
  mafDirectory dir;
  if (!dir.Load(m_LocalMSFFolder))
    return MAF_ERROR;

  m_LocalFilesDictionary.clear();

  for (int i=0;i<dir.GetNumberOfFiles();i++)
  {
    const char *fullname=dir.GetFile(i);  
    const char *filename=mafString::BaseName(fullname);
    m_LocalFilesDictionary.insert(filename);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafRemoteStorage::DownloadRemoteFile(mafString remote_filename, mafString &local_filename)
//------------------------------------------------------------------------------
{
  wxString path, name, ext, tmpFolder;
  wxString baseName = wxFileNameFromPath(remote_filename.GetCStr());
  wxSplitPath(baseName,&path,&name,&ext);

  if (ext == "msf")
  {
    m_RemoteMSF = remote_filename;
    tmpFolder = m_LocalCacheFolder;
    tmpFolder += "\\";
    tmpFolder += name.c_str();

    if (!wxDirExists(tmpFolder))
    {
      wxMkdir(tmpFolder);
    }
    m_LocalMSFFolder = tmpFolder;
  }

  local_filename = m_LocalMSFFolder;
  local_filename += "\\";
  local_filename += wxFileNameFromPath(remote_filename.GetCStr()).c_str();

  if (wxFileExists(local_filename.GetCStr()))
  {
    return MAF_OK;
  }
   
  struct LocalFileDownloaded localfile = 
  {
    (char *)local_filename.GetCStr(), // name to store the file as if successful
    NULL
  };

  mafString s = wxString::Format(_("downloading %s. Please wait..."),wxFileNameFromPath(remote_filename.GetCStr())).c_str();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&s));

  mafString auth = m_UserName;
  auth += ":";
  auth += m_Pwd;
  curl_easy_reset(m_Curl);
  curl_easy_setopt(m_Curl, CURLOPT_URL, remote_filename.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, FileDownload);
  curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &localfile);
  curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
  curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_NOPROGRESS, FALSE);
  curl_easy_setopt(m_Curl, CURLOPT_PROGRESSFUNCTION, FileTransferProgressCall);
  curl_easy_setopt(m_Curl, CURLOPT_PROGRESSDATA, m_ProgressListener);
  if (m_EnableCertificateAuthentication)
  {
    curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYHOST, 0);
  }

  m_Result = curl_easy_perform(m_Curl);
  bool curl_error = m_Result != CURLE_OK;

  if(localfile.stream)
    fclose(localfile.stream);

  if (curl_error && localfile.stream)
  {
    wxRemoveFile(localfile.filename);
  }

  s = _("Ready");
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&s));
  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if (curl_error)
  {
    ErrorManager(m_Result);
    return MAF_ERROR;
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
const char* mafRemoteStorage::GetTmpFolder()
//------------------------------------------------------------------------------
{
  if (m_IsRemoteMSF)
  {
    if (m_TmpFolder.IsEmpty())
    {
      m_DefaultTmpFolder = m_LocalMSFFolder;
      m_DefaultTmpFolder << "/";
      return m_DefaultTmpFolder;
    }
    else
    {
      return Superclass::GetTmpFolder();
    }
  }
  else
  {
    return mafXMLStorage::GetTmpFolder();
  }
}

//------------------------------------------------------------------------------
int mafRemoteStorage::UploadLocalFile(mafString local_filename, mafString remote_filename)
//------------------------------------------------------------------------------
{
  mafString protocol = "";
  bool is_remote = IsRemote(remote_filename, protocol);

  wxString path,short_name,ext;
  wxSplitPath(remote_filename,&path,&short_name,&ext);
  if (ext == "msf")
    m_LocalStream = fopen(local_filename.GetCStr(), "rt");
  else
    m_LocalStream = fopen(local_filename.GetCStr(), "rb");

  if(!m_LocalStream)
  {
    wxMessageBox(_("Error uploading file!!"),_("Error"));
    return MAF_ERROR;
  }

  // get the file size
  stat(local_filename.GetCStr(), &FileInfo); 

  mafString auth = m_UserName;
  auth += ":";
  auth += m_Pwd;
  curl_easy_reset(m_Curl);
  curl_easy_setopt(m_Curl, CURLOPT_URL, remote_filename.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_NOPROGRESS, FALSE);
  curl_easy_setopt(m_Curl, CURLOPT_UPLOAD, TRUE);
  curl_easy_setopt(m_Curl, CURLOPT_READFUNCTION,FileUpload);
  curl_easy_setopt(m_Curl, CURLOPT_READDATA, m_LocalStream);
  curl_easy_setopt(m_Curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)FileInfo.st_size);

  if (is_remote)
  {
    curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
    curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());
    if (protocol.Equals("http") || protocol.Equals("https"))
    {
      m_EnableCertificateAuthentication = protocol.Equals("https");
      if (m_EnableCertificateAuthentication)
      {
        curl_easy_setopt(m_Curl, CURLOPT_SSL_VERIFYHOST, 0);
      }
      curl_easy_setopt(m_Curl, CURLOPT_PUT, TRUE);
    }
  }

  m_Result = curl_easy_perform(m_Curl);

  // now extract transfer info
  curl_easy_getinfo(m_Curl, CURLINFO_SPEED_UPLOAD, &m_SpeedUpload);
  curl_easy_getinfo(m_Curl, CURLINFO_TOTAL_TIME, &m_TotalTime);

  if (m_LocalStream)
  {
    fclose(m_LocalStream);
  }

  if (m_Result != CURLE_OK)
  {
    ErrorManager(m_Result);
    return MAF_ERROR;
  }

  //mafLogMessage(_("Speed: %.3f bytes/sec during %.3f seconds\n"), m_SpeedUpload, m_TotalTime);

  return MAF_OK;
}
//------------------------------------------------------------------------------
void mafRemoteStorage::ErrorManager(int err_num)
//------------------------------------------------------------------------------
{
  mafString err_string = curl_easy_strerror((CURLcode)err_num);
  wxMessageBox(err_string.GetCStr(), _("Warning"));
}


////////////////////////////////////////////////////////////////////////////////
// Global callback used by cURL 
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int FileDownload(void *buffer, size_t size, size_t nmemb, void *stream)
//------------------------------------------------------------------------------
{
  struct LocalFileDownloaded *out = (struct LocalFileDownloaded *)stream;
  if(out && !out->stream) 
  {
    // open file for writing
    out->stream = fopen(out->filename, "wb");
    if(!out->stream)
      return -1; // failure, can't open file to write
  }
  return fwrite(buffer, size, nmemb, out->stream);
}
//------------------------------------------------------------------------------
size_t FileUpload(void *ptr, size_t size, size_t nmemb, void *stream)
//------------------------------------------------------------------------------
{
  FILE *f = (FILE *)stream;
  size_t n;

  if (ferror(f))
    return CURL_READFUNC_ABORT;

  n = fread(ptr, size, nmemb, f) * size;

  return n;
}
//------------------------------------------------------------------------------
int FileTransferProgressCall(mafObserver *listener, double t, double d, double ultotal, double ulnow)
//------------------------------------------------------------------------------
{
  long progress = (long)(d*100.0/t);
  mafRemoteStorage::m_ProgressListener->OnEvent(&mafEvent(mafRemoteStorage::m_ProgressListener,PROGRESSBAR_SET_VALUE,progress));
  return 0;
}
