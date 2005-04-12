/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeManager.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:06:46 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafNodeManager_H__
#define __mafNodeManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include  "mafEvent.h"
#include  "mafNode.h"
#include  "mafNodeRoot.h"
#include  "mafVMERoot.h"
#include  "mmgFileHistory.h"
//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class mafNodeStorage;
//----------------------------------------------------------------------------
// mafNodeManager :
//----------------------------------------------------------------------------
class mafNodeManager: public mafEventListener
{
public:
       mafNodeManager();
      ~mafNodeManager(); 

  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  void OnEvent(mafEvent& e);
 
  /** 
  Destroy all nodes (olso the root), for each destroyed node the manager send 
  an event VME_REMOVING, then search the root and select it. */ 
  void MSFNew   (bool notify_root_creation = true);
  
	/** Open the msf filename. */
	void MSFOpen(wxString filename);
  
	/** Open the msf from file history. */
  void MSFOpen(int file_id);

  void ZIPOpen(wxString filename);
  
	/** Save the msf tree. */
  void MSFSave();
  
	/** Save the msf tree with different filename. */
  void MSFSaveAs();
  
	/** Return true if the tree has been modifyed. */
  bool MSFIsModified() {return m_modified;};
  
	/** Set the modify flag. */
  void MSFModified(bool modified) {m_modified = modified;};

	/** Add the vme to the tree. */
  void VmeAdd(mafNode *v);

	/** Remove the vme from the tree. */
  void VmeRemove(mafNode *v);

	/** Get the time bounds of the tree. */
  void TimeGetBounds(float *min, float *max);

	/** Set the time for the tree. */
  void TimeSet(float time);

	/** Send the event VME_REMOVING to inform logic that the vme and its subtree are removed from the tree. */
  void NotifyRemove(mafNode *v);

	/** Send the event VME_ADDED to inform logic that the vme and its subtree are added to the tree. */
  void NotifyAdd(mafNode *v);
  
	/** Show the dialog to confirm the save of the current tree. */
	bool AskConfirmAndSave();
 
	/** Set the filename for the current tree. */
  void SetFileName (wxString& filename) {m_msffile = filename;};
 
	/** Get the filename of the current tree. */
  wxString& GetFileName () {return m_msffile;};
 
	/** Link to the main menù the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  mafNodeRoot *GetRoot() { return m_root;};		

	/** Set the application stamp; this is the mark of the specific vertical application (must be equal to the application name). */
  void SetApplicationStamp(wxString appstamp) {m_AppStamp = appstamp;};

	/** Set the flag for bak file generation on saving msf file. */
	void MakeBakFileOnSave(bool bakfile = true) {m_make_bak_file = bakfile;}

  /** Update vme client data interface from tag. if vme == NULL, the update is propagated from root vme to all the tree. */
  void UpdateFromTag(mafNode *vme = NULL);

protected:
  bool m_modified;

  mafNodeRoot       *m_root;
  mafEventListener  *m_Listener;
  wxConfigBase			*m_Config;
  mmgFileHistory	   m_FileHistory;

  bool m_make_bak_file;
  wxString m_msf_dir;
  wxString m_wildc;
  wxString m_AppStamp;
  wxString m_msffile;
  wxString m_zipfile;
  wxString m_mergefile;

  /*
  bool m_Crypting;
  mafNodeStorage    *m_storage;

  */
};
#endif
