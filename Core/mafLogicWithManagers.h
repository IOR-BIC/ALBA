/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithManagers.h,v $
  Language:  C++
  Date:      $Date: 2005-11-28 13:03:31 $
  Version:   $Revision: 1.17 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafLogicWithManagers_H__
#define __mafLogicWithManagers_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafLogicWithGUI.h"
#include "mmgPanel.h"
#include "mmgVMEChooser.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafVMEManager;
class mafViewManager;
class mafOpManager;
class mafSideBar;
class mmgMaterialChooser;
class mafVME;
class mafInteractionManager;
class mafPrintSupport;

//----------------------------------------------------------------------------
// mafLogicWithManagers :
//----------------------------------------------------------------------------
/** 
mafLogicWithManagers provide:
- the VMEManager
- the OperationManager, and the support for plugging operations
- the ViewManager, and the support for plugging views
and manage all the Events related to Nodes,Operations and Views.
Each Evt. will call a corresponding evt. handler.

mafLogicWithManagers also provide the following standard operation:
- Select,Cut,Copy,Paste,Delete,Undo,Redo

mafLogicWithManagers also provide to enable/disable the user interface elements following
the requirement of the application status.

Derived classes can replace any of these manager with a customized version.

It is Recommended that Derived Classes call mafLogicWithManagers::OnEvent to 
for every un-handled event.

USAGE - member function MUST be called in this order:
  -- create a mafLogicWithManagers object (-> create the mainwindow)
  -- call PlugXXX:          to customize the GUI and the Managers
  -- call Configure:        to create the GUI and the managers
  -- call Plug              to plug-in operations and views
  -- call Show              it is before Init, to eventually show the Progressbar during a Load
  -- call Init(argc,argv)   will call MSFNew or MSFLoad

*/
class mafLogicWithManagers: public mafLogicWithGUI
{
public:
               mafLogicWithManagers();
	virtual     ~mafLogicWithManagers(); 

  /** */
  virtual void OnEvent(mafEventBase *maf_event);

  /**  Plug a new view */
	virtual void Plug(mafView* view);

  /**  Plug a new operation */
	virtual void Plug(mafOp *op);
	
  /** Configure the application plugging all the managers and the side-bar panel. */
  virtual void Configure();

  /** Program Initialization */
	virtual void Init(int argc, char **argv);

  /** Must be called before Configure */
  void PlugVMEManager(bool b) {m_UseVMEManager =b;};

  /** Must be called before Configure */
  void PlugViewManager(bool b){m_UseViewManager=b;};

  /** Must be called before Configure */
  void PlugOpManager(bool b){m_UseOpManager=b;};

  /** Must be called before Configure */
  void PlugInteractionManger(bool b){m_UseInteractionManager=b;}

  /** Redefined to Fill the menu's */
  virtual void Show();

  /** Set the application stamp for the application, 
  if set to OPEN_ALL_DATA let's the application to open all msf file. 
  As default the application stamp is the name of the application and it is set into the Show() method. */
  void SetApplicationStamp(mafString &app_stamp);

  /** Manage application exception and allow to save at least the tree. */
  void HandleException();

protected:
  //---------------------------------------------------------
  // Description:
  // Method to update the time bounds when 
  // the tree representation change by adding or deleting a vme.
  virtual void UpdateTimeBounds();

  // Description:
  // Set the time to update the time bar.
  virtual void TimeSet(float t);
//---------------------------------------------------------
  
  /**  Redefined to add View,Op,Import,Export menu */
  virtual void CreateMenu();


  // EVENT HANDLERS

  /** FILE NEW evt. handler */
	virtual void OnFileNew();
  /** FILE OPEN evt. handler */
	virtual void OnFileOpen(const char *file_to_open = NULL);
  /** FILE HISTORY evt. handler */
	virtual void OnFileHistory(int menuId);
  /** FILE SAVE evt. handler */
  virtual void OnFileSave();
  /** FILE SAVEAS evt. handler */
  virtual void OnFileSaveAs();
  /** Called on Quit event. */
  virtual void OnQuit();

  /** Respond to a VME_SELECT evt. Instantiate the 'Select' operation. */
	virtual void VmeSelect(mafEvent& e);
	/** Respond to a VME_SELECTED evt. Update the selection on the tree and view representation. */
	virtual void VmeSelected(mafNode *vme);
	/** Respond to a VME_SHOW evt. Show/Hide the vme. */
	virtual void VmeShow(mafNode *vme, bool visibility);
  /** Respond to a VME_Modified evt.*/
	virtual void VmeModified(mafNode *vme);
  /** Respond to a VME_ADD evt. Add a new vme to the tree. */
	virtual void VmeAdd(mafNode *vme);
  /** Respond to a VME_ADDED evt. propagate evt. to SideBar,ViewManager,ecc.. */
	virtual void VmeAdded(mafNode *vme);
  /** Respond to a VME_REMOVE evt. Remove a vme from the tree. */
	virtual void VmeRemove(mafNode *vme);
  /** Respond to a VME_REMOVING evt. propagate evt. to SideBar,ViewManager,ecc.. */
	virtual void VmeRemoving(mafNode *vme);
	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual mafNode *VmeChoose(long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = "Choose Node");

  /** Build a dialog to show all available materials. */	
  virtual void VmeChooseMaterial(mafVME *vme, bool updateProperty);
	
  /** Called when an operation starts. Disable all menu and lock the Selection */ 
	virtual void OpRunStarting();
  /** Called when an operation stops. Re-enable all menu and unlock the Selection */ 
	virtual void OpRunTerminated();
	/** Show the gui of the running operation or the settings of the selected view. */
	virtual void OpShowGui(bool push_gui, mmgPanel *panel);
	/** Hide the setting's gui when a view is destroyed. */
	virtual void OpHideGui(bool view_closed);
  
  /** Show contextual menu for tree when right mouse click arrives.*/
  virtual void TreeContextualMenu(mafEvent &e);

  /** Show contextual menu for views when right mouse click arrives.*/
  void ViewContextualMenu(bool vme_menu);

  /** Create a new view. */
	virtual void ViewCreate(int viewId);
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreated(mafView *v);
	/** Select a view and update the display list for the tree. */
	virtual void ViewSelect();


  /** Enable/Disable menu items when an operation start/end running. */
  void EnableMenuAndToolbar(bool enable);

  /** Called after FileOpen or Save operation */
  void UpdateFrameTitle();

  mafSideBar             *m_SideBar;
  mafVMEManager          *m_VMEManager;
  mafViewManager         *m_ViewManager;
  mafOpManager           *m_OpManager;
  mafInteractionManager  *m_InteractionManager;

  mmgMaterialChooser  *m_MaterialChooser;
  mafPrintSupport     *m_PrintSupport;

  wxMenu *m_ImportMenu; 
  wxMenu *m_ExportMenu; 
  wxMenu *m_RecentFileMenu;
  wxMenu *m_OpMenu;
  wxMenu *m_ViewMenu; 

  bool m_UseVMEManager;
  bool m_UseViewManager;
  bool m_UseOpManager;
  bool m_UseInteractionManager;
};
#endif
