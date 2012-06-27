/*=========================================================================

 Program: MAF2
 Module: mafVMEManager
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEManager_H__
#define __mafVMEManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafNode.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafGUIFileHistory.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class mafVMEStorage;
class wxZipFSHandler;
class wxFileSystem;

/** mafVMEManager : Class managing VMEs inside a MAF application.

This class is responsible for managing the VME in a MAF application:
It can add or remove VME to the current tree, add a new tree, load and save a tree from or to an msf (zmsf) file.

*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafVMEManager: public mafObserver
//----------------------------------------------------------------------------
{
public:
  /** constructor */
  mafVMEManager();

  /** destructor */
  ~mafVMEManager(); 

  /** Set the event receiver object*/
  void SetListener(mafObserver *listener) {m_Listener = listener;};

  /** Process events coming from other components */
  void OnEvent(mafEventBase *e);
 
  /** Destroy all nodes (also the root), for each destroyed node the manager send 
  an event VME_REMOVING, then search the root and select it. */ 
  void MSFNew(bool notify_root_creation = true);
  
	/** Open the msf filename. */
	void MSFOpen(mafString filename);
  
	/** Open the msf from file history. */
  void MSFOpen(int file_id);

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(mafString filename);

  /** Save the msf tree into a compressed file. */
  void ZIPSave(mafString file = "");
  
	/** Save the msf tree. */
  void MSFSave();
  
	/** Save the msf tree with different filename. */
  void MSFSaveAs();

  /** Upload MAF data to the remote host with the given 'remote_file' filename.*/
  void Upload(mafString local_file, mafString remote_file);
  
	/** Return true if the tree has been modifyed. */
  bool MSFIsModified() {return m_Modified;};
  
	/** Set the modify flag. */
  void MSFModified(bool modified) {m_Modified = modified;};

	/** Add the vme to the tree. */
  void VmeAdd(mafNode *n);

	/** Remove the vme from the tree. */
  void VmeRemove(mafNode *n);

	/** Get the time bounds of the tree. */
  void TimeGetBounds(double *min, double *max);

	/** Set the time for the tree. */
  void TimeSet(double time);

	/** Send the event VME_REMOVING to inform logic that the vme and its subtree are removed from the tree. */
  void NotifyRemove(mafNode *n);

	/** Send the event VME_ADDED to inform logic that the vme and its subtree are added to the tree. */
  void NotifyAdd(mafNode *n);
  
	/** Show the dialog to confirm the save of the current tree. */
	bool AskConfirmAndSave();
 
  /** Set the filename for the current tree. */
  void SetDirName (mafString dirname) {m_MSFDir = dirname;};

	/** Set the filename for the current tree. */
  void SetFileName (mafString &filename) {m_MSFFile = filename;};
 
	/** Get the filename of the current tree. */
  mafString &GetFileName() {return m_MSFFile;};
 
	/** Link to the main men the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  mafVMERoot *GetRoot();

  /** Return the storage associated to the current tree.*/
  mafVMEStorage *GetStorage();

	/** Set the application stamp; this is the mark of the specific vertical application (is always equal to the application name). */
  void SetApplicationStamp(mafString &appstamp);

  /** Set other application stamps for compatibility with similar applications (the application stamp itself is already present). */
  void SetApplicationStamp(std::vector<mafString> appstamp);

	/** Set the flag for bak file generation on saving MSF file. */
	void MakeBakFileOnSave(bool bakfile = true) {m_MakeBakFile = bakfile;}

  /** Update vme client data interface from tag. if vme == NULL, the update is propagated from root vme to all the tree. */
  void UpdateFromTag(mafNode *n = NULL);

  /** Set the host-name for the remote host.*/
  void SetHost(mafString host) {m_Host = host;};

  /** Set the user-name for the remote host.*/
  void SetUser(mafString user) {m_User = user;};

  /** Set the password for the remote host.*/
  void SetPassword(mafString pwd) {m_Pwd = pwd;};

  /** Set the port for the remote host.*/
  void SetRemotePort(int port) {m_Port = port;};

  /** Set the local cache folder in which will be downloaded remote files.*/
  void SetLocalCacheFolder(mafString cache_folder);

  /** Remove temporary directory used for compressed msf files.*/
  void RemoveTempDirectory();

  /** Set the flag for saving binary files associated to time varying VMEs.*/
  void SetSingleBinaryFile(bool singleFile);

  /** Set the flag for test mode without GUI */
  void SetTestMode(bool testmode){m_TestMode = testmode;}; // Losi 02/16/2010 for test class

protected:
  /** Create a compressed msf file: zmsf.*/
  bool MakeZip(const mafString &zipname, wxArrayString *files);

  /** Set tag with creation date for the node passed as argument.*/
  void AddCreationDate(mafNode *vme);

  bool                m_Modified;         ///< Used to known when the tree has been modified...

  mafObserver*        m_Listener;         ///< Register the event receiver object
  wxConfigBase*       m_Config;           ///< Application configuration for file history management
  wxZipFSHandler      *m_ZipHandler;      ///< Handler for zip archive (used to open zmsf files)
  wxFileSystem        *m_FileSystem;      ///< File system manager
  mafGUIFileHistory	  m_FileHistory;      ///< Used to hold recently opened files
  int                 m_FileHistoryIdx;   ///< Identifier of the file to open

  bool                m_MakeBakFile;      ///< Flag used to create or not the backup file of the saved msf.
  mafString           m_MSFDir;           ///< Directory name in which is present the msf file.
  std::vector<mafString> m_AppStamp;      ///< Application stamps for our application.
  mafString           m_MSFFile;          ///< File name of the data associated to the tree.
  mafString           m_ZipFile;          ///< File name of compressed archive in which save the data associated to the tree.
  mafString           m_TmpDir;           ///< Temporary directory for zmsf extraction

  mafString           m_Host;             ///< Host name for remote storage
  mafString           m_User;             ///< User name for remote storage
  mafString           m_Pwd;              ///< Password for remote storage
  int                 m_Port;             ///< Port number for remote storage
  
  bool                m_LoadingFlag;      ///< used when an MSF is being loaded
  bool                m_Crypting;         ///< used to enable the Encryption for the MSF
  bool                m_SingleBinaryFile; ///< used to store binary files associated to time varying VMEs as multiple files or not.
  mafVMEStorage*      m_Storage;          ///< Associated storage
  bool                m_TestMode;         ///< set true for test mode without GUI

  /** friend test class */
  friend class mafVMEManagerTest; // Losi 02/16/2010 for test class
};
#endif
