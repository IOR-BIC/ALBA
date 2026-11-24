/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaLogicWithManagers_H__
#define __albaLogicWithManagers_H__


//----------------------------------------------------------------------------
// includes :
#include "albaGUIPanel.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEManager.h"
#include "albaGUIMDIFrame.h"
#include "albaSideBar.h"
#include "albaAbsLogicManager.h"


//----------------------------------------------------------------------------
// forward reference
class albaViewManager;
class albaOpManager;
class albaVME;
class albaVMELandmark;
class albaVMELandmarkCloud;
class albaInteractionManager;
class albaPrintManager;
class albaGUISettingsDialog;
class albaGUIAboutDialog;
class albaGUISplashScreen;
class albaGUISettings;
class albaUser;
class albaWizardManager;
class albaWizard;
class albaGUITimeBar;
class albaWXLog;
class albaVTKLog;
class albaGUIApplicationSettings;
class albaGUISettingsTimeBar;
class albaHelpManager;
class albaSnapshotManager;
class mmaApplicationLayout;
class albaProsthesesDBManager;


//----------------------------------------------------------------------------
// albaLogicWithManagers :
//----------------------------------------------------------------------------
/** 
albaLogicWithManagers provide:
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
In derived classes it is recommended to pass to albaLogicWithGUI::OnEvent()
the unhandled events.

albaLogicWithManagers also provide the following standard operation:
- Select,Cut,Copy,Paste,Delete,Undo,Redo

albaLogicWithManagers also provide to enable/disable the user interface elements following
the requirement of the application status.

Derived classes can replace any of these manager with a customized version.

It is Recommended that Derived Classes call albaLogicWithManagers::OnEvent to 
for every un-handled event.

USAGE - member function MUST be called in this order:
  -- create a albaLogicWithManagers object (-> create the mainwindow)
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
class ALBA_EXPORT albaLogicWithManagers : public albaObserver, public albaAbsLogicManager
{
public:

	albaLogicWithManagers(albaGUIMDIFrame *mdiFrame = NULL);
	virtual     ~albaLogicWithManagers();

	/** Events management */
	virtual void OnEvent(albaEventBase *alba_event);

	/**  Plug a new view */
	virtual void Plug(albaView* view, bool visibleInMenu = true);


	/**  Plug a new operation and its undo flag: if the operation does not support
	undo the undo flag has no effect */
	virtual void Plug(albaOp *op, wxString menuPath = "", bool canUndo = true, albaGUISettings *setting = NULL);

	/** Configure the application.
	At this point are plugged all the managers, the side-bar docking panel.
	Are plugged also all the setting to the dialogs interface. */
	virtual void Configure();

	/** Creates menu and tool bars*/
	void CreateToolBarsAndPanels();

	/** Program Initialization */
	virtual void Init(int argc, char **argv);

	/** Must be called before Configure */
	void PlugVMEManager(bool b) { m_UseVMEManager = b; };

	/** Must be called before Configure */
	void PlugViewManager(bool b) { m_UseViewManager = b; };

	/** Must be called before Configure */
	void PlugOpManager(bool b) { m_UseOpManager = b; };

	/** Must be called before Configure */
	void PlugInteractionManger(bool b) { m_UseInteractionManager = b; }

	/** Must be called before Configure */
	void PlugWizardManager(bool b) { m_UseWizardManager = b; };

	/** Must be called before Configure */
	void PlugSnapshotManager(bool b) { m_UseSnapshotManager = b; };

	/* Initialize Prosthesis DB Manager*/
	void PlugProsthesisDBManager(albaString passPhrase = "");

	/**  Plug a new wizard */
	virtual void Plug(albaWizard *wizard, wxString menuPath = "");

	/**Set the application stamp to the VMEManager and Shows the application*/
	virtual void Show();

	/** Fill the View and operation menu's  */
	void FillMenus();

	/** Return the applications' user.*/
	albaUser *GetUser() { return m_User; };

	/** Set the application stamp for the application,
	if set to OPEN_ALL_DATA let's the application to open all msf file.
	As default the application stamp is the name of the application and it is set into the Show() method. */
	void SetApplicationStamp(albaString &app_stamp);

	/** Manage application exception and allow to save at least the tree. */
	virtual void HandleException();

	/** Manage application Events */
	int AppEventFilter(wxEvent& event);


	/** Set ALBA Expert Mode*/
	void ALBAExpertModeOn() { (*GetALBAExpertMode()) = true; };
	void ALBAExpertModeOff() { (*GetALBAExpertMode()) = false; };
	void SetALBAExpertMode(int value) { (*GetALBAExpertMode()) = value; };

	/** Set the file extension */
	void SetFileExtension(albaString &extension) { m_Extension = extension; };

	/** Returns the pointer to the main panel of the application.*/
	virtual albaGUIMDIFrame *GetTopWin() { return m_Win; };

	/** Sets the flag to know if Toolbar should be built.*/
	void PlugToolbar(bool plug) { m_PlugToolbar = plug; };
	/** Sets the flag to know if Side bar should be built.*/
	void PlugSidebar(bool plug) { m_PlugControlPanel = plug; };
	/** Sets the flag to know if Time bar should be built.*/
	void PlugTimebar(bool plug) { m_PlugTimebar = plug; };
	/** Sets the flag to know if Log bar should be built.*/
	void PlugLogbar(bool plug) { m_PlugLogPanel = plug; };

	
	/**
	Used to give a splash screen image directly from the bitmap without converting it in xpm.*/
	virtual void ShowSplashScreen(wxBitmap &splashImage, wxString message ="", int pos=2, wxColour color = *wxBLACK);

	static void ShowWebSite(wxString url);
	
	/** Returns true if there is a Running Operation */
	bool IsOperationRunning() { return m_RunningOperation; }

	// VME
	virtual const char* GetMsfFileExtension() { return m_Extension.GetCStr(); }
		
	/* Return the Operation Manager */
	albaOpManager * GetOpManager() const { return m_OpManager; }

	virtual albaViewManager *GetViewManager() { return m_ViewManager; }

	virtual void PrintImage(albaVMEImage *img);

	/** Get  application layout*/
	mmaApplicationLayout* GetLayout() { return m_AppLayout; };

	// Inherited via albaAbsLogicManager
	virtual albaProsthesesDBManager * GetProsthesesDBManager();

	/** Plugs a list of standard operations, importer and exporters included (see albaLogicStandardPlugs) */
	void PlugStandardOperations();

	/** Plugs a list of standard views (see albaLogicStandardPlugs) */
	void PlugStandardViews();

	/** Returns AboutImage */
	albaString GetAboutImage() const { return m_AboutImage; }

	/** Sets AboutImage */
	void SetAboutImage(albaString aboutImage) { m_AboutImage = aboutImage; }

	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme);

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme);

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme);
	
	
	/** Get the App Build Number */
	virtual const char* GetAppBuildNum();
	/** Set the App Build Number */
	virtual void SetAppBuildNum(char* buildNum);
	/** Set Alba Build Number*/
	virtual const char* GetAlbaBuildNum();
	/** Get Alba Build Number*/
	virtual void SetAlbaBuildNum(char* buildNum);
	/** Get the link to the citation Paper */
	const char *GetCitationPaper() { return m_CitationPaper.GetCStr(); }
	/** Set the link to the citation Paper */
	void SetCitationPaper(char *link) { m_CitationPaper = link; }

	/** Select a view: the side bar, the interaction and the operations work on it */
	void SelectView(albaView *view) override;

	/** Return the selected view, NULL if none */
	albaView *GetSelectedView() override { return m_SelectedView; }


	bool IsTimeVaringEnabled() override;

protected:
	//---------------------------------------------------------
	// Description:
	// Method to update the time bounds when 
	// the tree representation change by adding or deleting a vme.
	virtual void UpdateTimeBounds();

	// Description:
	// Set the time to update the time bar.
	virtual void TimeSet(double t) override;
	//---------------------------------------------------------

	/** Redefined to add View,Op,Import,Export menu */
	virtual void CreateMenu();
	/** Called by the VME manager to replace its storage: delete oldStorage (if any) and return a new one */
	virtual albaVMEStorage *CreateStorage(albaVMEStorage *oldStorage) override;

	/** This method creates and add the Toolbar, for specific app toolbar override the CreateToolBar method*/
	virtual void CreateAndPlugToolbar();

	/**This method creates the toolbar, override this method for specific app toolbars*/
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

	/** Stores application layout */
	void StoreLayout();

	/** Restores application layout */
	virtual void RestoreLayout();

	// EVENT HANDLERS

	/** Create a new project, asking to save the opened one: return ALBA_OK if created */
	virtual int FileNew() override;
	/** Open a project file. By default (file_to_open = NULL) it ask the user to choose a file to open,
	otherwise it open the given one: return ALBA_ERROR if not opened */
	virtual int FileOpen(const char *file_to_open = NULL) override;
	/** FILE HISTORY evt. handler */
	virtual void OnFileHistory(int menuId);
	/** Save the project: return ALBA_ERROR if not saved */
	virtual int FileSave() override;
	/** Save the project asking the file name, if "legacy" is true saves file as old alba file format: return ALBA_ERROR if not saved */
	virtual int FileSaveAs(bool legacy = false) override;
	/** Open again the current project file: return ALBA_ERROR if there is none or it is not opened */
	virtual int FileReload() override;
	/** Create a snapshot of the selected view: return ALBA_ERROR if not created */
	virtual int CreateSnapshot() override;

	/** Called on Quit event. */
	virtual void OnQuit();

	/** Called when a user drag a non msf or zmsf file over the application.
	Example: dragging a VTK file over the application cause the application to inport it through the importer.*/
	virtual void ImportExternalFile(albaString &filename);

	/** Select the vme running albaOpSelect: the selection can be undone. */
	virtual void VmeSelect(albaVME *vme);
	/** Called when a vme has been selected. Update the selection on the tree and view representation. */
	virtual void VmeSelected(albaVME *vme);

	/** Called from VmeSelect to select a LM */
	void SelectLandmark(albaVMELandmark *lm, bool select);

	/** Show/Hide the vme. */
	virtual void VmeShow(albaVME *vme, bool visibility);

	/** Called from VmeShow to show a LM cloud */
	void ShowLandmarkCloud(albaVMELandmarkCloud * lmc, bool visibility);

	/** Show VME in the side bar */
	void ShowInSideBar(albaVME * vme, bool visibility);

	/* Called from VmeShow to show landmarks*/
	void ShowLandmark(albaVMELandmark * lm, bool visibility);

	/** Respond to a vme Modified evt.*/
	virtual void VmeModified(albaVME *vme);

	/** Add a new vme to the tree. */
	virtual void VmeAdd(albaVME *vme);
	/** Called by the VME manager for each VME added to the tree: tell it to SideBar, ViewManager, ecc.. */
	virtual void VmeAdded(albaVME *vme) override;
	/** Remove a vme from the tree. */
	virtual void VmeRemove(albaVME *vme);
	/** Called by the VME manager for each VME that is going to be removed: tell it to SideBar, ViewManager, ecc.. */
	virtual void VmeRemoving(albaVME *vme) override;
	/** called from VME after removing*/
	virtual void VmeRemoved();

	virtual void VmeVisualModeChanged(albaVME * vme);

	/** Show the tree in a dialog and return the VME chosen by the user, NULL on cancel. */
	virtual albaVME *ChooseVME(albaString title = "Choose Node", void *acceptFunction = NULL, albaVME *selected = NULL) override;

	/** Called by the op manager when an operation starts: disable all menu and lock the Selection */
	virtual void OpRunStarting() override;

	/** Called by the op manager when an operation stops: re-enable all menu and unlock the Selection; during a wizard, continue it */
	virtual void OpRunTerminated(bool runOk) override;

	/** Called by an operation to show its gui (panel) on the side bar */
	virtual void OpShowGui(albaGUIPanel *panel) override;

	/** Called by an operation to remove its gui from the side bar */
	virtual void OpHideGui() override;

	/** Called by the view manager when it creates a view: create the frame of the view and attach the view to it */
	virtual void CreateViewFrame(albaView *v) override;

	/** Called by the view manager when it deletes a view: update side bar, interaction and menus */
	virtual void ViewDeleted(albaView *view) override;

	/** Tile the view frames horizontally or vertically */
	virtual void TileViewFrames(bool horizontally) override;

	/** Cascade the view frames */
	virtual void CascadeViewFrames() override;

	/** Show the tree in a dialog and return the VMEs chosen by the user, an empty vector on cancel. */
	virtual std::vector<albaVME*> ChooseVMEs(albaString title = "Choose Nodes", void *acceptFunction = NULL, albaVME *selected = NULL) override;
		

	/** Show the contextual menu of the tree, with the items of vme if vmeMenu is true */
	virtual void ShowTreeContextualMenu(albaGUIVMETree *tree, albaVME *vme, bool vmeMenu) override;

	/** Show the contextual menu of the selected view, with the VME items if vmeMenu is true */
	virtual void ShowViewContextualMenu(bool vmeMenu) override;

	/** Create a new view. */
	virtual void ViewCreate(int viewId);

	/** Called when user change the measure unit from men� Options.*/
	void UpdateMeasureUnit();

	/** Enable/Disable menu items when an operation start/end running. */
	virtual void EnableMenuAndToolbar();

	/** Update the title of the main window (opened file or running wizard) */
	virtual void UpdateFrameTitle() override;

	//-------------------


	/** to be added */
	virtual void ConfigureWizardManager();

	/** Called by the wizard manager when a wizard starts. Disable all menu, lock the Selection and the close of the view */
	virtual void WizardRunStarting() override;

	/** Called by the wizard manager when a wizard stops. Re-enable all menu, unlock the Selection and the close of the view */
	virtual void WizardRunTerminated() override;

	/** Run an operation for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(const char *opName) override;

	/** Run an operation for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(albaOp *op) override;

	/** Run an operation (by id, e.g. OP_DELETE) for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(int opId) override;

	/** Called after the request of an operation by the wizard: if the operation did not start the wizard continues (with failure) */
	void WizardContinueIfOpNotStarted();

	/** Redefined to add Print buttons */
	virtual void CreateWizardToolbar();



	albaAbsSideBar          *m_SideBar;
	albaVMEManager          *m_VMEManager;
	albaViewManager         *m_ViewManager;
	albaOpManager           *m_OpManager;
	albaInteractionManager  *m_InteractionManager;
	albaHelpManager				 *m_HelpManager;
	albaSnapshotManager		 *m_SnapshotManager;
	albaVMELandmark				 *m_SelectedLandmark;
	albaProsthesesDBManager *m_ProsthesisDBManager;


	albaPrintManager     *m_PrintSupport;


	mmaApplicationLayout *m_AppLayout;

	wxMenu *m_ImportMenu;
	wxMenu *m_ExportMenu;
	wxMenu *m_RecentFileMenu;
	wxMenu *m_OpMenu;
	wxMenu *m_ViewMenu;

	bool m_UseVMEManager;
	bool m_UseViewManager;
	bool m_UseOpManager;
	bool m_UseInteractionManager;
	bool m_UseHelpManager;
	bool m_UseSnapshotManager;

	albaView *m_SelectedView; ///< the view selected with SelectView

	albaGUISettingsDialog	*m_SettingsDialog;
	albaGUIAboutDialog			*m_AboutDialog;

	albaString m_AlbaBuildNum;
	albaString m_AppBuildNum;
	albaString m_CitationPaper;
	albaString m_Extension;

	albaUser *m_User; ///< Applications' user

	albaWizardManager *m_WizardManager;
	bool m_UseWizardManager;
	bool m_WizardRunning;
	bool m_RunningOperation;
	wxGauge *m_WizardGauge;
	wxStaticText* m_WizardLabel;
	bool m_CancelledBeforeOpStarting;
	wxMenu *m_WizardMenu;
	albaGUISplashScreen* m_SplashScreen;

	long							m_ChildFrameStyle;
	albaGUIMDIFrame		*m_Win;
	wxToolBar         *m_ToolBar;
	wxMenuBar         *m_MenuBar;
	wxString					m_LastSelectedPanel;
	albaGUITimeBar			*m_TimePanel;
	wxString					m_AppTitle;
	albaString				m_AboutImage;

	bool							m_LogToFile;
	albaWXLog          *m_Logger;
	albaVTKLog         *m_VtkLog;

	
	albaGUIApplicationSettings *m_ApplicationSettings;
	albaGUISettingsTimeBar  *m_TimeBarSettings;


	bool m_Quitting;    ///< Variable that allows to determine if the application is Quitting or not.
	bool m_PlugMenu;    ///< Flag to plug or not the Menu into the application. Default is true.
	bool m_PlugToolbar; ///< Flag to plug or not the Toolbar into the application. Default is true.
	bool m_PlugControlPanel; ///< Flag to plug or not the Side-bar into the application. Default is true.
	long m_SidebarStyle;///< Store the style of the sidebar. Old style (ALBA 1.x): SINGLE_NOTEBOOK or new style (ALBA 2.x): DOUBLE_NOTEBOOK
	bool m_PlugTimebar; ///< Flag to plug or not the Time-bar into the application. Default is true.
	bool m_PlugLogPanel;  ///< Flag to plug or not the Log area into the application. Default is true.
	bool m_ShowStorageSettings; ///<Flag to show storage setting default is false.
	bool m_FatalExptionOccurred;
	bool m_OpeningMSF;
private:
	
};
#endif
