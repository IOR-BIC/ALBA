/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRemoteFileManager.h,v $
  Language:  C++
  Date:      $Date: 2007-03-08 14:57:11 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafRemoteFileManager_h__
#define __mafRemoteFileManager_h__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafAgentThreaded.h"
#include "mafCurlUtility.h"
#include <curl/curl.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** A class for download / upload remote files in an asynchronous way.*/  
class MAF_EXPORT mafRemoteFileManager : public mafAgentThreaded
{
public:
  mafTypeMacro(mafRemoteFileManager, mafAgentThreaded);

  /** Download a remote file into the local cache and write in the 'local_filename' the path to reach it.*/
  int DownloadRemoteFile(mafString remote_filename, mafString &downloaded_filename);

  /** Upload local file to the remote repository.*/
  int UploadLocalFile(mafString local_filename, mafString remote_filename);

  /** Retrieve the file list that match the input path prefix and store the result in memory into the struct.*/
  int ListRemoteDirectory(mafString & queryString, msfTreeSearchReult &chunk);

  /** use this function to start (initialize) the file manager.*/
  virtual int Start();

  /** use this function to stop (shutdown) the file manager.*/
  virtual void Stop();

  /** Set the user name to be used to connect through a given protocol. 
  If no user name is set, anonymous is used.*/
  void SetUsername(mafString usr);

  /** Set the password for the user name required for connection.*/
  void SetPassword(mafString pwd);

  /** Set the address of the remote host.*/
  void SetRemotePort(int port);

  /** Allow to turn on or off the https encryption.*/
  void EnableAuthentication(bool enable);

  /** process events sent to the device */
  virtual void OnEvent(mafEventBase *event);

protected:
  mafRemoteFileManager();
  virtual ~mafRemoteFileManager();

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
  mafRemoteFileManager(const mafRemoteFileManager&);  // Not implemented.
  void operator=(const mafRemoteFileManager&);  // Not implemented.

  /** hidden to avoid usage: use Start() instead */
  int Initialize() {return Superclass::Initialize();}

  /** hidden to avoid usage: use Stop() instead */
  void Shutdown() {Superclass::Shutdown();} 
};

extern int FileTransferProgressCall(mafObserver *listener, double t, double d, double ultotal, double ulnow);
extern int FileDownload(void *buffer, size_t size, size_t nmemb, void *stream);
extern size_t FileUpload(void *ptr, size_t size, size_t nmemb, void *stream);

#endif // _mafRemoteStorage_h_
