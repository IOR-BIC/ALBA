/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRemoteStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-08 15:00:21 $
  Version:   $Revision: 1.4 $
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

#include <wx/tokenzr.h>

#include "mafRemoteStorage.h"
#include "mafRemoteFileManager.h"

#include "mafDirectory.h"
#include "mafCurlUtility.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafRemoteStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafRemoteStorage::mafRemoteStorage()
//------------------------------------------------------------------------------
{
  m_HostName = "";
  m_LocalCacheFolder = wxGetCwd();
  m_LocalMSFFolder = "";
  m_RemoteRepository = "";
  m_RemoteMSF = "";
  m_IsRemoteMSF = false;

  mafNEW(m_RemoteFileManager);
  m_RemoteFileManager->Start();
}

//------------------------------------------------------------------------------
mafRemoteStorage::~mafRemoteStorage()
//------------------------------------------------------------------------------
{
  m_RemoteFileManager->Stop();
  mafDEL(m_RemoteFileManager);
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
  m_RemoteFileManager->SetUsername(usr);
}
//------------------------------------------------------------------------------
void mafRemoteStorage::SetPassword(mafString pwd)
//------------------------------------------------------------------------------
{
  m_RemoteFileManager->SetPassword(pwd);
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
  m_RemoteFileManager->SetRemotePort(port);
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
  m_RemoteFileManager->EnableAuthentication(protocol.Equals("https"));

  if (m_IsRemoteMSF)
  {
    // Download the file if it is not present into the cache
    mafString local_filename;
    //res = DownloadRemoteFile(filename, local_filename);
    //---------------------------------- Build folder into the local cache to put the downloaded file
    wxString path, name, ext, tmpFolder;
    wxString baseName = wxFileNameFromPath(filename.GetCStr());
    wxSplitPath(baseName,&path,&name,&ext);

    if (ext == "msf")
    {
      m_RemoteMSF = filename;
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
    local_filename += wxFileNameFromPath(filename.GetCStr()).c_str();

    if (wxFileExists(local_filename.GetCStr()))
    {
      return MAF_OK;
    }
    //-----------------------------------------------
    res = m_RemoteFileManager->DownloadRemoteFile(filename, local_filename);
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
      save_res = m_RemoteFileManager->UploadLocalFile(fullpathname.c_str(), remote_file);
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
    wxString path, name, ext;
    mafString query_string;
    wxSplitPath(baseName,&path,&name,&ext);
    query_string = m_RemoteRepository;
    query_string += "/";
    query_string += "msfList?prefix=";
    query_string += name;

    struct msfTreeSearchReult chunk;
    chunk.memory=NULL; // we expect realloc(NULL, size) to work 
    chunk.size = 0;    // no data at this point 

    int res = m_RemoteFileManager->ListRemoteDirectory(query_string, chunk);
    if (res == MAF_OK)
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
