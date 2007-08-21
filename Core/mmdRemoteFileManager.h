/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteFileManager.h,v $
  Language:  C++
  Date:      $Date: 2007-08-21 14:33:53 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdRemoteFileManager_h__
#define __mmdRemoteFileManager_h__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDevice.h"
#include "mafCurlUtility.h"
#include <curl/curl.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** A class for download / upload remote files in an asynchronous way.*/  
class MAF_EXPORT mmdRemoteFileManager : public mafDevice
{
public:
  /** @ingroup Events
  Issued when the Remote Storage needs to download a remote file.*/
  MAF_ID_DEC(DOWNLOAD_FILE);
  /** @ingroup Events
  Issued when the the Remote Storage needs to upload a file to remote storage.*/
  MAF_ID_DEC(UPLOAD_FILE);

  mafTypeMacro(mmdRemoteFileManager, mafDevice);

  struct mmuRemoteFileExchange
  {
    mafString localFile;
    mafString remoteFile;
  };

  /** Download a remote file into the local cache and write in the 'local_filename' the path to reach it.*/
  int DownloadRemoteFile(mafString remote_filename, mafString &downloaded_filename, mafObserver *observer = NULL);

  /** Upload local file to the remote repository.*/
  int UploadLocalFile(mafString local_filename, mafString remote_filename, mafObserver *observer = NULL);

  /** Retrieve the file list that match the input path prefix and store the result in memory into the struct.*/
  int ListRemoteDirectory(mafString & queryString, msfTreeSearchReult &chunk);

  /** Set the user name to be used to connect through a given protocol. 
  If no user name is set, anonymous is used.*/
  void SetUsername(const mafString &usr);

  /** Set the password for the user name required for connection.*/
  void SetPassword(const mafString &pwd);

  /** Set the address of the remote host.*/
  void SetRemotePort(const int &port);

  /** Allow to turn on or off the https encryption.*/
  void EnableAuthentication(bool enable);

  /** process events sent to the device */
  virtual void OnEvent(mafEventBase *event);

protected:
  mmdRemoteFileManager();
  virtual ~mmdRemoteFileManager();

  /** Show messages when errors comes up.*/
  void ErrorManager(int err_num);

  /** start the remote file manager thread. */
  virtual int InternalInitialize();
  
  /** stop the remote file manager thread. */
  virtual void InternalShutdown();

  bool m_EnableCertificateAuthentication;

  mafString  m_UserName;
  mafString  m_Pwd;
  int        m_Port;
  CURL      *m_Curl;
  CURLcode   m_Result;
  struct curl_slist *m_Headerlist;
  struct stat FileInfo;
  FILE      *m_LocalStream;
  double     m_SpeedUpload;
  double     m_TotalTime;

private:
  mmdRemoteFileManager(const mmdRemoteFileManager&);  // Not implemented.
  void operator=(const mmdRemoteFileManager&);  // Not implemented.
};

extern int FileTransferProgressCall(mafObserver *listener, double t, double d, double ultotal, double ulnow);
extern int FileDownload(void *buffer, size_t size, size_t nmemb, void *stream);
extern size_t FileUpload(void *ptr, size_t size, size_t nmemb, void *stream);

#endif // _mafRemoteStorage_h_

