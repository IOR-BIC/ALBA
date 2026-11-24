/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewManager
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewManager_H__
#define __albaViewManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUISettings.h"
#include "albaServiceClient.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMERoot;
class albaView;
class albaGUIViewPanel;
class albaRWIBase;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,long);
#endif

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum VieweSettingsEvents
{	
	ID_VIEW_SETTING_MAXIMIZE = MINID,
	ID_VIEW_SETTING_OPEN_ONLY_ONE,
	ID_VIEW_SETTING_COLOR,
	ID_VIEW_SETTING_COLOR_DEFAULT,
};

//----------------------------------------------------------------------------
// albaViewManager :
//----------------------------------------------------------------------------
/** An implementation of albaViewManager with [D]ynamic view [C]reation feature */
class ALBA_EXPORT albaViewManager : albaObserver, albaGUISettings, public albaServiceClient, public albaAbsViewManager
{

public:
  albaViewManager();
 ~albaViewManager(); 
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  virtual void OnEvent(albaEventBase *alba_event);

  /** Fill the main men� with plugged views. */
	void FillMenu (wxMenu* menu);

  /** Check if the view 'v' should be hidden from the 'View' menu.*/
  bool IsVisibleInMenu(albaView* v);

  /** Add the vme to all views. */
  void VmeAdd(albaVME *n);

  /** Remove the vme from all views. */
  void VmeRemove(albaVME *n);

  /** Show the selection of the vme in all views. */
  void VmeSelect(albaVME *vme);

  /** Show/Hide the vme in the selected view. */
  void VmeShow(albaVME *vme, bool show);

  /** inform the views that a vme was modified */
  void VmeModified(albaVME *vme); //SIL. 9-3-2005: 

  /** Add the View to the view-list. */
	virtual void ViewAdd(albaView *view, bool visibleInMenu = true);
  
	/** Store the selected view and its render window. */
  void ViewSelected(albaView *view);
  
  /** Activate the view from software to simulate the click on the view (used by remote). */
  void Activate(albaView *view);

	/** Create a view at index 'id' of the view-list. */
  virtual albaView *ViewCreate(int id) override;
  
	/** Create a view given the type. */
  virtual albaView *ViewCreate(wxString label);
  
	/** Insert the view into the view-list, fill the view with the vme tree and select the selected vme. */
  void ViewInsert(albaView *view);
  
	/** Remove the view from the view-list and delete it (called by its frame when it is closed) */
  virtual void ViewDelete(albaView *view) override;

	/** Move and resize the frame of the open view with the given label */
  virtual void ViewResize(const char *label, int x, int y, int width, int height) override;

	/** Activate the view with the given label, creating it if it is not open */
	virtual void ViewOpen(const char *label) override;

	/** Delete the view with the given label (if open) and destroy its frame */
	virtual void ViewClose(const char *label) override;
  
	/** Delete all view. */
  virtual void ViewDeleteAll();

	/** Reset the camera to fit the selected vme (if sel = true) or all (if sel = false). apply to the selected view only. */
  virtual void CameraReset(bool sel = false) override;

	/** Reset the camera to fit the specified vme. apply to the selected view only. */
  void CameraReset(albaVME *vme);   

	/** Update the camera for all opened views; optionally the Update can be limited for the selected view. */
  virtual void CameraUpdate(bool only_selected = false) override;

	/** Postpone the camera updates until the matching DisableCameraUpdateSkip; calls can be nested */
	virtual void EnableCameraUpdateSkip() override;

	/** End a camera update skip: the outermost one updates the cameras if an update was requested meanwhile */
	virtual void DisableCameraUpdateSkip() override;

	/** Update the visual properties of vme (the selected VME if NULL) in all the views, from its tags if fromTag is true */
	virtual void PropertyUpdate(albaVME *vme = NULL, bool fromTag = false) override;

  /** Return the selected view. */
	albaView *GetSelectedView();

  /** Return the root of the vme tree. */
  albaVME *GetCurrentRoot() {return (albaVME*)m_RootVme;};

  /** Return the created view-list. */
  albaView *GetList() {return m_ViewList;};

  /** Return a view (defined by label) if is present on open wiews list*/
  virtual albaView *GetFromOpenedViews(const char *label) override;

  /** Return the plugged view-list. */
  albaView **GetListTemplate() {return m_ViewTemplate;};

  /** Empty. */
  void OnQuit();

  /** Return the view pointer from view's id and multiplicity. */
  albaView *GetView(int id, int mult) {return m_ViewMatrixID[id][mult];};


	/* Return the gui for Settings Dialog*/
	albaGUI* GetSettingsGui();

	void OpenOnlyViewForType(int chose) { m_ViewOpenOnlyOne = chose; };
	void MaximizeViewOnOpen(int chose) { m_ViewMaximize = chose; };
	void EnableViewSettings(bool enable = true);

protected:

	/** Initialize the application settings.*/
	void InitializeSettings();

  albaView       *m_ViewList;  // created view list

  albaView       *m_ViewTemplate[MAXVIEW];   // view template vector
  int            m_TemplateNum;       // number of template

  albaObserver   *m_Listener;
  albaVMERoot    *m_RootVme;
  albaVME       *m_SelectedVme;
  albaView       *m_SelectedView;
  albaRWIBase    *m_SelectedRWI;
	albaView       *m_ViewBeingCreated;
  albaView       *m_ViewMatrixID[MAXVIEW][MAXVIEW];  ///< Matrix to access views directly by (id, multiplicity)

  std::vector<long> m_IdInvisibleMenuList; ///< List of views that are no visible into the 'View' menu item.

	albaGUI *m_SettingsGui;

	wxColour m_ViewColorBackground;
	int m_ViewMaximize;
	int m_ViewOpenOnlyOne;
	int m_SkipCameraUpdate; ///< nesting level of EnableCameraUpdateSkip
	bool m_CameraUpdateRequested; ///< true when a camera update arrived during a skip

  /** test friend */
  friend class albaViewManagerTest;
};
#endif
