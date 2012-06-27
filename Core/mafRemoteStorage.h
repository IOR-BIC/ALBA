/*=========================================================================

 Program: MAF2
 Module: mafRemoteStorage
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mmdRemoteFileManager;

/** A storage class for MSF remote files.
  This is an implementation of remote storage object for storing MSF remote files.
  File download is non blocking action. While the data is downloading how use the item 
  should shows its bounding box at least.
  @sa mafVMEStorage
    - 
*/  
class MAF_EXPORT mafRemoteStorage: public mafVMEStorage
{
public:
  mafTypeMacro(mafRemoteStorage, mafVMEStorage)

  mafRemoteStorage();
  virtual ~mafRemoteStorage();
  
  /** Add the mmdRemoteFileManager device and initialize it.*/
  void Initialize();

  /** resolve an URL download remote file and provide local filename to be used as input */
  virtual int ResolveInputURL(const char *url, mafString &filename, mafObserver *observer = NULL);

  /** resolve an URL and provide a local filename to be used as output, then it is uploaded to the original remote msf */
  virtual int StoreToURL(const char *filename, const char *url);

  /** populate the list of file in the local cache directory */
  virtual int OpenDirectory(const char *pathname);

  virtual const char* GetTmpFolder();

  /** Set the user name to be used to connect through a given protocol. 
  If no user name is set, anonymous is used.*/
  void SetUsername(const mafString &usr);

  /** Set the password for the user name required for connection.*/
  void SetPassword(const mafString &pwd);

  /** Set the address of the remote host.*/
  void SetHostName(const mafString &host);

  /** Set the address of the remote host.*/
  void SetRemotePort(const int &port);

  mmdRemoteFileManager *GetRemoteFileManager() {return m_RemoteFileManager;};

protected:
  /** Create a filename list of the local MSF directory. */
  int OpenLocalMSFDirectory();

  /** Check if a file is into the local MSF cache folder. */
  bool IsFileInLocalDirectory(const char *filename);

  mafString m_HostName;
  mafString m_LocalMSFFolder;
  mafString m_RemoteRepository;
  mafString m_RemoteMSF;
  std::set<mafString> m_LocalFilesDictionary;

  bool m_IsRemoteMSF;

  mmdRemoteFileManager *m_RemoteFileManager;
};
#endif // _mafRemoteStorage_h_
