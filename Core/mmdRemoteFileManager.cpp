/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteFileManager.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-21 14:33:53 $
  Version:   $Revision: 1.1 $
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

#include "mmdRemoteFileManager.h"
#include <wx/tokenzr.h>

#include "mafRemoteStorage.h"
#include "mafDirectory.h"
#include "mmuIdFactory.h"

// Used with FTP Upload
#define UPLOAD_FILE_AS  "while-uploading.txt"
#define RENAME_FILE_TO  "renamed-and-fine.txt"

#include <curl/types.h>
#include <curl/easy.h>

struct LocalFileDownloaded {
  char *filename;
  FILE *stream;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdRemoteFileManager::DOWNLOAD_FILE)
MAF_ID_IMP(mmdRemoteFileManager::UPLOAD_FILE)

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmdRemoteFileManager)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmdRemoteFileManager::mmdRemoteFileManager()
//------------------------------------------------------------------------------
{
  m_EnableCertificateAuthentication = false;
  m_SpeedUpload = 0.0;
  m_TotalTime   = 0.0;
  m_LocalStream = NULL;
  m_Headerlist  = NULL;

  m_UserName = ""; // blank user name means anonymous user.
  m_Pwd = "";
  m_Port = 80;
  SetPersistentFlag(true);
  SetAutoStart(true);
}
//------------------------------------------------------------------------------
mmdRemoteFileManager::~mmdRemoteFileManager()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mmdRemoteFileManager::SetUsername(const mafString &usr)
//------------------------------------------------------------------------------
{
  m_UserName = usr;
}
//------------------------------------------------------------------------------
void mmdRemoteFileManager::SetPassword(const mafString &pwd)
//------------------------------------------------------------------------------
{
  m_Pwd = pwd;
}
//------------------------------------------------------------------------------
void mmdRemoteFileManager::SetRemotePort(const int &port)
//------------------------------------------------------------------------------
{
  m_Port = port;
}
//------------------------------------------------------------------------------
int mmdRemoteFileManager::InternalInitialize()
//------------------------------------------------------------------------------
{
  if(Superclass::InternalInitialize()!=MAF_OK)
    return MAF_ERROR;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  m_Curl = curl_easy_init();

  return MAF_OK;
}
//------------------------------------------------------------------------------
void mmdRemoteFileManager::InternalShutdown()
//------------------------------------------------------------------------------
{
  // free curl environment
  curl_easy_cleanup(m_Curl);
  curl_global_cleanup();

  Superclass::InternalShutdown();
}
//----------------------------------------------------------------------------
void mmdRemoteFileManager::EnableAuthentication(bool enable)
//----------------------------------------------------------------------------
{
  m_EnableCertificateAuthentication = enable;
}
//----------------------------------------------------------------------------
int mmdRemoteFileManager::DownloadRemoteFile(mafString remote_filename, mafString &downloaded_filename, mafObserver *observer)
//----------------------------------------------------------------------------
{
  if (observer == NULL)
  {
    struct LocalFileDownloaded localfile = 
    {
      (char *)downloaded_filename.GetCStr(), // name to store the file as if successful
        NULL
    };

    mafString auth = m_UserName;
    auth += ":";
    auth += m_Pwd;
    curl_easy_reset(m_Curl);
    curl_easy_setopt(m_Curl, CURLOPT_URL, remote_filename.GetCStr());
    curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, FileDownload);
    curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, &localfile);
    curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
    curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());

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

    if (curl_error)
    {
      return MAF_ERROR;
    }
    return MAF_OK;
  }
  else
  {
    struct mmuRemoteFileExchange files =
    {
      downloaded_filename,
        remote_filename
    };

    if (PushEvent(DOWNLOAD_FILE, observer, &files))
    {
      return MAF_WAIT;
    }
    return MAF_ERROR;
  }
}
//------------------------------------------------------------------------------
int mmdRemoteFileManager::UploadLocalFile(mafString local_filename, mafString remote_filename, mafObserver *observer)
//------------------------------------------------------------------------------
{
  if (observer == NULL)
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
  else
  {
    struct mmuRemoteFileExchange files =
    {
      local_filename,
        remote_filename
    };

    if (PushEvent(UPLOAD_FILE, observer, &files))
    {
      return MAF_WAIT;
    }
    return MAF_ERROR;
  }
}
//----------------------------------------------------------------------------
int mmdRemoteFileManager::ListRemoteDirectory(mafString & queryString, msfTreeSearchReult &chunk)
//----------------------------------------------------------------------------
{
  mafString auth = m_UserName;
  auth += ":";
  auth += m_Pwd;  
  curl_easy_reset(m_Curl);
  curl_easy_setopt(m_Curl, CURLOPT_URL, queryString.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_PORT, m_Port);
  curl_easy_setopt(m_Curl, CURLOPT_USERPWD, auth.GetCStr());
  curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, chunk);

  m_Result = curl_easy_perform(m_Curl);

  if (m_Result != CURLE_OK)
  {
    return MAF_ERROR;
  }
  return MAF_OK;
}
//----------------------------------------------------------------------------
void mmdRemoteFileManager::OnEvent(mafEventBase *e)
//----------------------------------------------------------------------------
{
  if(e->GetId() == DOWNLOAD_FILE)
  {
    mmuRemoteFileExchange *files = (mmuRemoteFileExchange *)e->GetData();
    DownloadRemoteFile(files->remoteFile, files->localFile);
  }
  else if (e->GetId() == UPLOAD_FILE)
  {
    mmuRemoteFileExchange *files = (mmuRemoteFileExchange *)e->GetData();
    UploadLocalFile(files->localFile, files->remoteFile);
  }
  Superclass::OnEvent(e);
}
//------------------------------------------------------------------------------
void mmdRemoteFileManager::ErrorManager(int err_num)
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

