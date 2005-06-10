/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewManager.h,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:43:08 $
  Version:   $Revision: 1.8 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafNode;
class mafVMERoot;
class mafView;
class mmgViewPanel;
//class mafRWIBase;
//class mafAction;
//----------------------------------------------------------------------------
// mafViewManager :
//----------------------------------------------------------------------------
/** An implementation of mafViewManager with [D]ynamic view [C]reation feature */
class mafViewManager: public mafObserver
{

public:
  mafViewManager();
 ~mafViewManager(); 
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  virtual void OnEvent(mafEventBase *maf_event);

  /** Fill the main menù with plugged views. */
	void FillMenu (wxMenu* menu);

  /** Add the vme to all views. */
  void VmeAdd(mafNode *n);

  /** Remove the vme from all views. */
  void VmeRemove(mafNode *n);

  /** Show the selection of the vme in all views. */
  void VmeSelect(mafNode *n);

  /** Show/Hide the vme in the selected view. */
  void VmeShow(mafNode *n, bool show);

  /** inform the views that a vme was modified */
  void VmeModified(mafNode *vme); //SIL. 9-3-2005: 

  /** Add the View to the view-list. */
	virtual void ViewAdd(mafView *view);
  
	/** Pass the selected render window to the mouse device. */
  void ViewSelected(mafView *view /*, mafRWIBase *rwi*/);
  
	/** Create a view at index 'id' of the view-list. */
  virtual mafView *ViewCreate(int id);
  
	/** Create a view given the type. */
  virtual mafView *ViewCreate(wxString type);
  
	/** Insert the view into the view-list, fill the view with the vme tree and select the selected vme. */
  void ViewInsert(mafView *view);
  
	/** Remove the view from the view-list and delete it. */
  virtual void ViewDelete(mafView *view);
  
	/** Delete all view. */
  virtual void ViewDeleteAll();

	/** Reset the camera to fit the selected vme (if sel = true) or all (if sel = false). apply to the selected view only. */
  void CameraReset(bool sel = false);

	/** Reset the camera to fit the specified vme. apply to the selected view only. */
  void CameraReset(mafNode *vme);   

	/** Update the camera for the selected view. */
  void CameraUpdate();

	/** Set the FlyTo mode for the selected view. */
	void CameraFlyToMode();

	/** Call PropertyUpdate for all views. */
	void PropertyUpdate(bool fromTag = false);

  /** Return the selected view. */
	mafView *GetSelectedView();

  /** Return the selected render window interactor. */
//  mafRWIBase *GetSelectedRWI();

  /** Return the root of the vme tree. */
  mafNode     *GetCurrentRoot() {return (mafNode*)m_RootVme;};

  /** Enable/Disable the selection from the interactor style. */
  void EnableSelect(bool enable);

  /** Return the view-list. */
  mafView* GetList() {return m_ViewList;};

  /** Empty. */
  void OnQuit();

  /** Return the view pointer from view's id and multiplicity. */
  mafView *GetView(int id, int mult) {return m_ViewMatrixID[id][mult];};

  /** 
  Initialize the action for the mouse device. */
//  void SetMouseAction(mafAction *action);

protected:
//mafISV             *m_is;
//mafAction          *m_MouseAction;
  mafView            *m_ViewList;  // created view list

  mafView            *m_ViewTemplate[MAXVIEW];   // view template vector
  int                 m_TemplateNum;       // number of template

  mafObserver   *m_Listener;
  mafVMERoot    *m_RootVme;
  mafNode       *m_SelectedVme;
  mafView       *m_SelectedView;
//mafRWIBase    *m_selected_rwi;
	mafView       *m_ViewBeingCreated;
  mafView       *m_ViewMatrixID[MAXVIEW][MAXVIEW];  ///< Matrix to access views directly by (id, multiplicity)
};
#endif