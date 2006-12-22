/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRemoteStorage.h,v $
  Language:  C++
  Date:      $Date: 2006-12-22 11:13:56 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafRemoteStorage_h__
#define __mafRemoteStorage_h__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafVMEStorage.h"
#include <curl/curl.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** A storage class for MSF remote files.
  This is an implementation of remote storage object for storing MSF remote files.
  @sa mafVMEStorage
    - 
*/  
class mafRemoteStorage: public mafVMEStorage
{
public:
  mafTypeMacro(mafRemoteStorage,mafVMEStorage)

  mafRemoteStorage();
  virtual ~mafRemoteStorage();
  
  /** resolve an URL download remote file and provide local filename to be used as input */
  virtual int ResolveInputURL(const char *url, mafString &filename);

  /** resolve an URL and provide a local filename to be used as output, then it is uploaded to the original remote msf */
  virtual int StoreToURL(const char *filename, const char *url);

  /** populate the list of file in the local cache directory */
  virtual int OpenDirectory(const char *pathname);

  virtual const char* GetTmpFolder();

  /** Set the user name to be used to connect through a given protocol. 
  If no user name is set, anonymous is used.*/
  void SetUsername(mafString usr);

  /** Set the password for the user name required for connection.*/
  void SetPassword(mafString pwd);

  /** Set the address of the remote host.*/
  void SetHostName(mafString host);

  /** Set the address of the remote host.*/
  void SetRemotePort(int port);

  /** Set the folder in which download the remote VME.*/
  void SetLocalCacheFolder(mafString cache);

  /** Download a remote file into the local cache and write in the 'local_filename' the path to reach it.*/
  int DownloadRemoteFile(mafString remote_filename, mafString &local_filename);

  /** Upload local file to the remote repository.*/
  int UploadLocalFile(mafString local_filename, mafString remote_filename);

  static mafRemoteStorage *m_ProgressListener;

protected:
  /** Show messages when errors comes up.*/
  void ErrorManager(int err_num);

  /** Create a filename list of the local MSF directory. */
  int OpenLocalMSFDirectory();

  /** Check if a file is into the local MSF cache folder. */
  bool IsFileInLocalDirectory(const char *filename);

  mafString m_UserName;
  mafString m_Pwd;
  mafString m_HostName;
  mafString m_LocalCacheFolder;
  mafString m_LocalMSFFolder;
  mafString m_RemoteRepository;
  mafString m_RemoteMSF;
  std::set<mafString> m_LocalFilesDictionary;
  int m_Port;

  bool m_EnableCertificateAuthentication;
  bool m_IsRemoteMSF;

  CURL      *m_Curl;
  CURLcode   m_Result;
  struct curl_slist *m_Headerlist;
  struct stat FileInfo;
  FILE      *m_LocalStream;
  double     m_SpeedUpload;
  double     m_TotalTime;
};

extern int FileTransferProgressCall(mafObserver *listener, double t, double d, double ultotal, double ulnow);
extern int FileDownload(void *buffer, size_t size, size_t nmemb, void *stream);
extern size_t FileUpload(void *ptr, size_t size, size_t nmemb, void *stream);

#endif // _mafRemoteStorage_h_
