/*=========================================================================

 Program: MAF2
 Module: mafViewManager
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewManager_H__
#define __mafViewManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMERoot;
class mafView;
class mafGUIViewPanel;
class mafRWIBase;
class mafDeviceButtonsPadMouse;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,long);
#endif

//----------------------------------------------------------------------------
// mafViewManager :
//----------------------------------------------------------------------------
/** An implementation of mafViewManager with [D]ynamic view [C]reation feature */
class MAF_EXPORT mafViewManager: public mafObserver
{

public:
  mafViewManager();
 ~mafViewManager(); 
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void SetRemoteListener(mafObserver *Listener) {m_RemoteListener = Listener;};
  virtual void OnEvent(mafEventBase *maf_event);

  /** Fill the main men� with plugged views. */
	void FillMenu (wxMenu* menu);

  /** Check if the view 'v' should be hidden from the 'View' menu.*/
  bool IsVisibleInMenu(mafView* v);

  /** Add the vme to all views. */
  void VmeAdd(mafVME *n);

  /** Remove the vme from all views. */
  void VmeRemove(mafVME *n);

  /** Show the selection of the vme in all views. */
  void VmeSelect(mafVME *vme);

  /** Show/Hide the vme in the selected view. */
  void VmeShow(mafVME *vme, bool show);

  /** inform the views that a vme was modified */
  void VmeModified(mafVME *vme); //SIL. 9-3-2005: 

  /** Add the View to the view-list. */
	virtual void ViewAdd(mafView *view, bool visibleInMenu = true);
  
	/** Pass the selected render window to the mouse device. */
  void ViewSelected(mafView *view /*, mafRWIBase *rwi*/);
  
  /** Activate the view from software to simulate the click on the view (used by remote). */
  void Activate(mafView *view);

	/** Create a view at index 'id' of the view-list. */
  virtual mafView *ViewCreate(int id);
  
	/** Create a view given the type. */
  virtual mafView *ViewCreate(wxString label);
  
	/** Insert the view into the view-list, fill the view with the vme tree and select the selected vme. */
  void ViewInsert(mafView *view);
  
	/** Remove the view from the view-list and delete it. */
  virtual void ViewDelete(mafView *view);
  
	/** Delete all view. */
  virtual void ViewDeleteAll();

	/** Reset the camera to fit the selected vme (if sel = true) or all (if sel = false). apply to the selected view only. */
  void CameraReset(bool sel = false);

	/** Reset the camera to fit the specified vme. apply to the selected view only. */
  void CameraReset(mafVME *vme);   

	/** Update the camera for all opened views; optionally the Update can be limited for the selected view. */
  void CameraUpdate(bool only_selected = false);

	/** Set the FlyTo mode for the selected view. */
	void CameraFlyToMode();

	/** Call PropertyUpdate for all views. */
	void PropertyUpdate(bool fromTag = false);

  /** Return the selected view. */
	mafView *GetSelectedView();

  /** Return the root of the vme tree. */
  mafVME *GetCurrentRoot() {return (mafVME*)m_RootVme;};

  /** Return the created view-list. */
  mafView *GetList() {return m_ViewList;};

  /** Return a view (defined by label) if is present on open wiews list*/
  mafView *GetFromList(const char *label);

  /** Return the plugged view-list. */
  mafView **GetListTemplate() {return m_ViewTemplate;};

  /** Empty. */
  void OnQuit();

  /** Return the view pointer from view's id and multiplicity. */
  mafView *GetView(int id, int mult) {return m_ViewMatrixID[id][mult];};

  /** Initialize the action for the mouse device.*/
  void SetMouse(mafDeviceButtonsPadMouse *mouse);

  /** Turn On/Off the collaboration status.*/
  void Collaborate(bool status) {m_CollaborateStatus = status;};

  bool m_FromRemote;  ///< Flag used from RemoteLogic to avoid loop

protected:
  mafDeviceButtonsPadMouse      *m_Mouse;
  mafView       *m_ViewList;  // created view list

  mafView       *m_ViewTemplate[MAXVIEW];   // view template vector
  int            m_TemplateNum;       // number of template

  mafObserver   *m_Listener;
  mafObserver   *m_RemoteListener;
  mafVMERoot    *m_RootVme;
  mafVME       *m_SelectedVme;
  mafView       *m_SelectedView;
  mafRWIBase    *m_SelectedRWI;
	mafView       *m_ViewBeingCreated;
  mafView       *m_ViewMatrixID[MAXVIEW][MAXVIEW];  ///< Matrix to access views directly by (id, multiplicity)

  std::vector<long> m_IdInvisibleMenuList; ///< List of views that are no visible into the 'View' menu item.

  bool m_CollaborateStatus;

  /** test friend */
  friend class mafViewManagerTest;
};
#endif
