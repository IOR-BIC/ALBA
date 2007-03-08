/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.h,v $
  Language:  C++
  Date:      $Date: 2007-03-08 15:00:46 $
  Version:   $Revision: 1.13 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
#include "mmgFileHistory.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class mafVMEStorage;

//----------------------------------------------------------------------------
// mafVMEManager :
//----------------------------------------------------------------------------
class mafVMEManager: public mafObserver
{
public:
       mafVMEManager();
      ~mafVMEManager(); 

  void SetListener(mafObserver *listener) {m_Listener = listener;};
  void OnEvent(mafEventBase *e);
 
  /** 
  Destroy all nodes (also the root), for each destroyed node the manager send 
  an event VME_REMOVING, then search the root and select it. */ 
  void MSFNew   (bool notify_root_creation = true);
  
	/** Open the msf filename. */
	void MSFOpen(wxString filename);
  
	/** Open the msf from file history. */
  void MSFOpen(int file_id);

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(wxString filename);

  /** Save the msf tree into a compressed file. */
  void ZIPSave(wxString file = "");
  
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
  void SetFileName (wxString& filename) {m_MSFFile = filename;};
 
	/** Get the filename of the current tree. */
  wxString& GetFileName () {return m_MSFFile;};
 
	/** Link to the main men the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  mafVMERoot *GetRoot();

  /** Return the storage associated to the current tree.*/
  mafVMEStorage *GetStorage();

	/** Set the application stamp; this is the mark of the specific vertical application (must be equal to the application name). */
  void SetApplicationStamp(mafString &appstamp) {m_AppStamp = appstamp;};

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

protected:
  /** Create a compressed msf file.*/
  bool MakeZip(const wxString& zipname, wxArrayString *files);

  bool              m_Modified;     ///< Used to known when the tree has been modified...

  mafObserver*      m_Listener;
  wxConfigBase*     m_Config;
  mmgFileHistory	  m_FileHistory;
  int               m_FileHistoryIdx;

  bool              m_MakeBakFile;
  wxString          m_MSFDir;
  mafString         m_AppStamp;     ///< the application stamp for our application
  wxString          m_MSFFile;
  wxString          m_ZipFile;
  wxString          m_TmpDir;
  wxString          m_MergeFile;

  mafString         m_LocalCacheFolder;
  mafString         m_Host;
  mafString         m_User;
  mafString         m_Pwd;
  int               m_Port;
  
  bool              m_LoadingFlag;  ///< used when an MSF is being loaded
  bool              m_Crypting;     ///< used to enable the Crypting in the MSF
  mafVMEStorage*    m_Storage;      
};
#endif
