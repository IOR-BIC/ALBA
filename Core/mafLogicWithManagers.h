/*=========================================================================

 Program: MAF2
 Module: mafLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafLogicWithManagers_H__
#define __mafLogicWithManagers_H__


//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafGUIPanel.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUIVMEChooser.h"
#include "mafVMEManager.h"
#include "mafGUIMDIFrame.h"
#include "mafSideBar.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafViewManager;
class mafOpManager;
class mafGUIMaterialChooser;
class mafVME;
class mafInteractionManager;
class mafPrintSupport;
class mafRemoteLogic;
class mafDeviceButtonsPadMouse;
class mafGUISettingsDialog;
class mafGUIApplicationLayoutSettings;
class mafGUISettings;
class mafGUISettingsHelp;
class mafUser;
class mafWizardManager;
class mafWizard;
class mafGUITimeBar;
class mafWXLog;
class mafVTKLog;
class mafGUILocaleSettings;
class mafGUIMeasureUnitSettings;
class mafGUIApplicationSettings;
class mafGUISettingsStorage;
class mafGUISettingsTimeBar;

//----------------------------------------------------------------------------
// mafLogicWithManagers :
//----------------------------------------------------------------------------
/** 
mafLogicWithManagers provide:
- mdi main frame
- menu    (can be avoided by calling plugmenu(false); )
- toolbar (can be avoided by calling plugtoolbarbar(false); )
- logbar  (can be avoided by calling pluglogbar(false); )
- timebar (can be avoided by calling plugtimebar(false); )
- sidebar (can be avoided by calling plugsidebar(false); )
- the VMEManager
- the OperationManager, and the support for plugging operations
- the ViewManager, and the support for plugging views
and manage all the Events related to Nodes,Operations and Views.
Each Evt. will call a corresponding evt. handler.

The PlugXXX functions must be called before the first Show().
In derived classes it is recommended to pass to mafLogicWithGUI::OnEvent()
the unhandled events.

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
  -- call Show              it is before Init, to eventually show the Progress-bar during a Load
  -- call Init(argc,argv)   will call MSFNew or MSFLoad

Tech NOTE: 
Proper Initialization must follow this order
1- CTOR:
2- Configure: (create the GUI el. - cant be in the CTOR because calls virtual functions)
in sub classes also create the manager
3- Plug Op, Plug View , ...
4- Show:
5- Init: Calls FileOpen or FileNew
	
*/
class MAF_EXPORT mafLogicWithManagers: public mafObserver
{
public:
               mafLogicWithManagers(mafGUIMDIFrame *mdiFrame=NULL);
	virtual     ~mafLogicWithManagers(); 

  enum UPLOAD_FLAGS
  {
    UPLOAD_SELECTED_VME = 0,
    UPLOAD_SUBTREE,
    UPLOAD_TREE,
    UPLOAD_COMPRESSED_VME,
    UPLOAD_COMPRESSED_SUBTREE,
    UPLOAD_COMPRESSED_TREE,
  };

  /** */
  virtual void OnEvent(mafEventBase *maf_event);

  /**  Plug a new view */
	virtual void Plug(mafView* view, bool visibleInMenu = true);


  /**  Plug a new operation and its undo flag: if the operation does not support
  undo the undo flag has no effect */
	virtual void Plug(mafOp *op, wxString menuPath = "", bool canUndo = true, mafGUISettings *setting = NULL);
	
  /** Configure the application.
  At this point are plugged all the managers, the side-bar docking panel. 
  Are plugged also all the setting to the dialogs interface. */
  virtual void Configure();

	/** Creates menu and tool bars*/
	void CreateToolBarsAndPanels();

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

  /** Must be called before Configure */
  void PlugWizardManager(bool b){m_UseWizardManager=b;};

  /**  Plug a new wizard */
  virtual void Plug(mafWizard *wizard, wxString menuPath = "");

	/**Set the application stamp to the VMEManager and Shows the application*/
  virtual void Show();

  /** Fill the View and operation menu's  */
	void FillMenus();

  /** Return the applications' user.*/
  mafUser *GetUser();

  /** Set the application stamp for the application, 
  if set to OPEN_ALL_DATA let's the application to open all msf file. 
  As default the application stamp is the name of the application and it is set into the Show() method. */
  void SetApplicationStamp(mafString &app_stamp);
  void SetApplicationStamp(std::vector<mafString> app_stamp);

  /** Allow to set the flag for views to by External to the main frame or to by child of parent frame. */
  void SetExternalViewFlag(bool external = false);

  /** Retrieve the value for the external view flag.*/
  bool GetExternalViewFlag();

  /** Manage application exception and allow to save at least the tree. */
  virtual void HandleException();

	/** Set the revision string */
	void SetRevision(mafString revision);

  /** Open a Find VME dialog.*/
  void FindVME();

  /* Set MAF Expert Mode*/
  void MAFExpertModeOn(){(*GetMAFExpertMode()) = TRUE;};
  void MAFExpertModeOff(){(*GetMAFExpertMode())= FALSE;};
  void SetMAFExpertMode(int value){(*GetMAFExpertMode()) = value;};

  /* Set the file extension */
  void SetFileExtension(mafString &extension) {m_Extension = extension;};

	/** Returns the pointer to the main panel of the application.*/
	virtual mafGUIMDIFrame *GetTopWin(){return m_Win;};

	/** Sets the flag to know if Toolbar should be built.*/
	void PlugToolbar(bool plug) {m_PlugToolbar	= plug;};
	/** Sets the flag to know if Side bar should be built.*/
	void PlugSidebar(bool plug, long style = mafSideBar::DOUBLE_NOTEBOOK) {m_PlugControlPanel	= plug; m_SidebarStyle = style;};
	/** Sets the flag to know if Time bar should be built.*/
	void PlugTimebar(bool plug) {m_PlugTimebar	= plug;};
	/** Sets the flag to know if Log bar should be built.*/
	void PlugLogbar(bool plug)	{m_PlugLogPanel		= plug;};

	 /** 
  Show the splash screen for the application. To define your own splash screen image simply
  overwrite the SPLASH_SCREEN image into the picture factory by plugging your .xpm image. \sa mafPictureFactory*/
  virtual void ShowSplashScreen();

  /** 
  Used to give a splash screen image directly from the bitmap without converting it in xpm.*/
  virtual void ShowSplashScreen(wxBitmap &splashImage);

protected:
  //---------------------------------------------------------
  // Description:
  // Method to update the time bounds when 
  // the tree representation change by adding or deleting a vme.
  virtual void UpdateTimeBounds();

  // Description:
  // Set the time to update the time bar.
  virtual void TimeSet(double t);
	//---------------------------------------------------------
  
  /** Redefined to add View,Op,Import,Export menu */
  virtual void CreateMenu();
  /** create a new storage object */
  virtual void CreateStorage(mafEvent *e);
  /** Redefined to add Print buttons */
  virtual void CreateToolbar();

	/** Virtual method to create the side bar.*/
	virtual void CreateControlPanel();
	/** Virtual method to create the time bar.*/
	virtual void CreateTimeBar();
	/** Virtual method to create the log bar.*/
	virtual void CreateLogPanel();
	/** Create a null logger. This is used when no log is due. */
	void CreateNullLog();
	
	
	/** Enable/disable a Toolbar or Menu Item */
	void EnableItem(int item, bool enable);

  // EVENT HANDLERS

  /** FILE NEW evt. handler */
	virtual void OnFileNew();
  /** FILE OPEN evt. handler. 
  By default (file_to_open = NULL) it ask the user to choose a file to open,
  otherwise it open the given one.*/
	virtual void OnFileOpen(const char *file_to_open = NULL);
  /** FILE UPLOAD evt. handler 
  By default (remote_file = NULL) AND the entire msf is uploaded and only the remote directory is asked to the user, 
  otherwise given parameters are managed to upload the file correctly. 'upload_flag' can be:
  UPLOAD_SELECTED_VME, UPLOAD_SUBTREE or UPLOAD_TREE (default) and the corresponding compressed:
  UPLOAD_COMPRESSED_VME, UPLOAD_COMPRESSED_SUBTREE, UPLOAD_COMPRESSED_TREE.*/
  virtual void OnFileUpload(const char *remote_file, unsigned int upload_flag = UPLOAD_TREE);
  /** FILE HISTORY evt. handler */
	virtual void OnFileHistory(int menuId);
  /** FILE SAVE evt. handler */
  virtual void OnFileSave();
  /** FILE SAVEAS evt. handler */
  virtual void OnFileSaveAs();
  /** Called on Quit event. */
  virtual void OnQuit();

  /** Called when a user drag a non msf or zmsf file over the application.
  Example: dragging a VTK file over the application cause the application to inport it through the importer.*/
  virtual void ImportExternalFile(mafString &filename);

  /** Respond to a VME_SELECT evt. Instantiate the 'Select' operation. */
	virtual void VmeSelect(mafEvent &e);
  /** Respond to a VME_SELECTED evt. Update the selection on the tree and view representation. */
	virtual void VmeSelected(mafNode *vme);
  /** Respond to a VME_DCLICKED evt. Manage the 'Double click' on Selected VME. */
  virtual void VmeDoubleClicked(mafEvent &e);
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
	virtual std::vector<mafNode*> VmeChoose(long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = "Choose Node", bool multiSelect = false);

  /** Build a dialog to show all available materials. */	
  virtual void VmeChooseMaterial(mafVME *vme, bool updateProperty);

  //Update Vme Visual Properties
  virtual void VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag);

  /** Called when an operation starts. Disable all menu and lock the Selection */ 
	virtual void OpRunStarting();
  /** Called when an operation stops. Re-enable all menu and unlock the Selection */ 
	virtual void OpRunTerminated();
	/** Show the gui of the running operation or the settings of the selected view. */
	virtual void OpShowGui(bool push_gui, mafGUIPanel *panel);
	/** Hide the setting's gui when a view is destroyed. */
	virtual void OpHideGui(bool view_closed);
  
  /** Show contextual menu for tree when right mouse click arrives.*/
  virtual void TreeContextualMenu(mafEvent &e);

  /** Show contextual menu for views when right mouse click arrives.*/
  virtual void ViewContextualMenu(bool vme_menu);

  /** Create a new view. */
	virtual void ViewCreate(int viewId);
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreated(mafView *v);
	/** Select a view and update the display list for the tree. */
	virtual void ViewSelect();

  virtual void RestoreLayout();

  /** Called when user change the measure unit from menù Options.*/
  void UpdateMeasureUnit();

  /** Enable/Disable menu items when an operation start/end running. */
  void EnableMenuAndToolbar(bool enable);

  /** Called after FileOpen or Save operation */
  virtual void UpdateFrameTitle();

  //-------------------


  /** to be added */
  virtual void ConfigureWizardManager();

  /** Called when an wizard starts. Disable all menu and lock the Selection */ 
  virtual void WizardRunStarting();

  /** Called when an wizard stops. Re-enable all menu and unlock the Selection */ 
  virtual void WizardRunTerminated();

  /** Redefined to add Print buttons */
  virtual void CreateWizardToolbar();



  mafSideBar             *m_SideBar;
  mafVMEManager          *m_VMEManager;
  mafViewManager         *m_ViewManager;
  mafOpManager           *m_OpManager;
  mafInteractionManager  *m_InteractionManager;
  mafRemoteLogic         *m_RemoteLogic;
  
  mafGUIMaterialChooser  *m_MaterialChooser;
  mafPrintSupport     *m_PrintSupport;

  mafDeviceButtonsPadMouse            *m_Mouse;

  wxMenu *m_ImportMenu; 
  wxMenu *m_ExportMenu; 
  wxMenu *m_RecentFileMenu;
  wxMenu *m_OpMenu;
  wxMenu *m_ViewMenu; 

  bool m_CameraLinkingObserverFlag;
  bool m_ExternalViewFlag;

  bool m_UseVMEManager;
  bool m_UseViewManager;
  bool m_UseOpManager;
  bool m_UseInteractionManager;
  mafGUISettingsDialog *m_SettingsDialog;
  mafGUIApplicationLayoutSettings *m_ApplicationLayoutSettings;
  mafGUISettingsHelp *m_HelpSettings;

  mafString m_Revision;
  mafString m_Extension;

  mafUser *m_User; ///< Applications' user

  mafWizardManager *m_WizardManager;
  bool m_UseWizardManager;
  bool m_WizardRunning;
  wxGauge *m_WizardGauge;
  wxStaticText* m_WizardLabel;
  bool m_CancelledBeforeOpStarting;
  wxMenu *m_WizardMenu;

	long               m_ChildFrameStyle;
	mafGUIMDIFrame       *m_Win;
	wxToolBar         *m_ToolBar;
	wxMenuBar         *m_MenuBar;
	wxString					 m_LastSelectedPanel;
	mafGUITimeBar      *m_TimePanel;
	mafString					 m_AppTitle;
	bool               m_LogToFile;
	bool               m_LogAllEvents;
	mafWXLog          *m_Logger;
	mafVTKLog         *m_VtkLog;
	mafGUILocaleSettings *m_LocaleSettings;
	mafGUIMeasureUnitSettings *m_MeasureUnitSettings;
	mafGUIApplicationSettings *m_ApplicationSettings;
	mafGUISettingsStorage  *m_StorageSettings;
	mafGUISettingsTimeBar  *m_TimeBarSettings;

	bool m_Quitting;    ///< Variable that allows to determine if the application is Quitting or not.
	bool m_PlugMenu;    ///< Flag to plug or not the Menu into the application. Default is true.
	bool m_PlugToolbar; ///< Flag to plug or not the Toolbar into the application. Default is true.
	bool m_PlugControlPanel; ///< Flag to plug or not the Side-bar into the application. Default is true.
	long m_SidebarStyle;///< Store the style of the sidebar. Old style (MAF 1.x): SINGLE_NOTEBOOK or new style (MAF 2.x): DOUBLE_NOTEBOOK
	bool m_PlugTimebar; ///< Flag to plug or not the Time-bar into the application. Default is true.
	bool m_PlugLogPanel;  ///< Flag to plug or not the Log area into the application. Default is true.
};
#endif
