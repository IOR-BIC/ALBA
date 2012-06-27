/*=========================================================================

 Program: MAF2
 Module: mafNodeManager
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafNodeManager_H__
#define __mafNodeManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafNode.h"
#include "mafVMERoot.h"
#include "mafVMERoot.h"
#include "mafGUIFileHistory.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,wxString);
#endif

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class mafNodeStorage;

//----------------------------------------------------------------------------
// mafNodeManager :
//----------------------------------------------------------------------------
class MAF_EXPORT mafNodeManager: public mafObserver
{
public:
    mafNodeManager();
  ~mafNodeManager(); 

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);
 
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
  void SetFileName (wxString& filename) {m_MsfFile = filename;};
 
	/** Get the filename of the current tree. */
  wxString& GetFileName () {return m_MsfFile;};
 
	/** Link to the main menù the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  mafVMERoot *GetRoot() { return m_Root;};		

  /** Set the application stamp; this is the mark of the specific vertical application (is often equal to the application name). */
  void SetApplicationStamp(wxString appstamp);
  void SetApplicationStamp(std::vector<wxString> appstamp);

	/** Set the flag for .bak file generation on saving .msf file. */
	void MakeBakFileOnSave(bool bakfile = true) {m_MakeBakFile = bakfile;}

  /** Update vme client data interface from tag. if vme == NULL, the update is propagated from root vme to all the tree. */
  void UpdateFromTag(mafNode *n = NULL);

protected:
  bool m_Modified;

  mafVMERoot      *m_Root;
  mafObserver     *m_Listener;
  wxConfigBase		*m_Config;
  mafGUIFileHistory	 m_FileHistory;

  bool      m_MakeBakFile;
  wxString  m_MsfDir;
  wxString  m_Wildc;
  std::vector<wxString>  m_AppStamp;
  wxString  m_MsfFile;
  wxString  m_ZipFile;
  wxString  m_MergeFile;

  /*
  bool m_Crypting;
  mafNodeStorage    *m_storage;

  */
};
#endif
