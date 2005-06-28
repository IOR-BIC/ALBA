/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEManager.h,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:49:27 $
  Version:   $Revision: 1.6 $
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
#include  "mafObserver.h"
#include  "mafNode.h"
#include  "mafNodeRoot.h"
#include  "mafVMERoot.h"
#include  "mmgFileHistory.h"
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
 
  /** create a new storage object */
  virtual void CreateNewStorage();

  /** 
  Destroy all nodes (also the root), for each destroyed node the manager send 
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
  void SetFileName (wxString& filename) {m_MSFFile = filename;};
 
	/** Get the filename of the current tree. */
  wxString& GetFileName () {return m_MSFFile;};
 
	/** Link to the main men the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  mafVMERoot *GetRoot();

	/** Set the application stamp; this is the mark of the specific vertical application (must be equal to the application name). */
  void SetApplicationStamp(wxString appstamp) {m_AppStamp = appstamp;};

	/** Set the flag for bak file generation on saving MSF file. */
	void MakeBakFileOnSave(bool bakfile = true) {m_MakeBakFile = bakfile;}

  /** Update vme client data interface from tag. if vme == NULL, the update is propagated from root vme to all the tree. */
  void UpdateFromTag(mafNode *n = NULL);

protected:
  bool              m_Modified;     ///< Used to known when the tree has been modified...

  mafObserver*      m_Listener;
  wxConfigBase*     m_Config;
  mmgFileHistory	  m_FileHistory;

  bool              m_MakeBakFile;
  wxString          m_MSFDir;
  wxString          m_Wildchar;
  wxString          m_AppStamp;     ///< the application stamp for our application
  wxString          m_MSFFile;
  wxString          m_ZipFile;
  wxString          m_MergeFile;
  
  bool              m_LoadingFlag;  ///< used when an MSF is being loaded
  bool              m_Crypting;     ///< used to enable the Crypting in the MSF
  mafVMEStorage*    m_Storage;      
};
#endif
