/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRemoteStorage.h,v $
  Language:  C++
  Date:      $Date: 2007-03-09 14:14:19 $
  Version:   $Revision: 1.5 $
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

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafRemoteFileManager;

/** A storage class for MSF remote files.
  This is an implementation of remote storage object for storing MSF remote files.
  File download is non blocking action. While the data is downloading how use the item 
  should shows its bounding box at least.
  @sa mafVMEStorage
    - 
*/  
class mafRemoteStorage: public mafVMEStorage
{
public:
  mafTypeMacro(mafRemoteStorage, mafVMEStorage)

  mafRemoteStorage();
  virtual ~mafRemoteStorage();
  
  /** resolve an URL download remote file and provide local filename to be used as input */
  virtual int ResolveInputURL(const char *url, mafString &filename, mafObserver *observer = NULL);

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

  mafRemoteFileManager *GetRemoteFileManager() {return m_RemoteFileManager;};

protected:
  /** Create a filename list of the local MSF directory. */
  int OpenLocalMSFDirectory();

  /** Check if a file is into the local MSF cache folder. */
  bool IsFileInLocalDirectory(const char *filename);

  mafString m_HostName;
  mafString m_LocalCacheFolder;
  mafString m_LocalMSFFolder;
  mafString m_RemoteRepository;
  mafString m_RemoteMSF;
  std::set<mafString> m_LocalFilesDictionary;

  bool m_IsRemoteMSF;

  mafRemoteFileManager *m_RemoteFileManager;
};
#endif // _mafRemoteStorage_h_
