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

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMERoot;
class albaView;
class albaGUIViewPanel;
class albaRWIBase;
class albaDeviceButtonsPadMouse;

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
class ALBA_EXPORT albaViewManager : albaObserver, albaGUISettings
{

public:
  albaViewManager();
 ~albaViewManager(); 
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  virtual void OnEvent(albaEventBase *alba_event);

  /** Fill the main menù with plugged views. */
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
  
	/** Pass the selected render window to the mouse device. */
  void ViewSelected(albaView *view /*, albaRWIBase *rwi*/);
  
  /** Activate the view from software to simulate the click on the view (used by remote). */
  void Activate(albaView *view);

	/** Create a view at index 'id' of the view-list. */
  virtual albaView *ViewCreate(int id);
  
	/** Create a view given the type. */
  virtual albaView *ViewCreate(wxString label);
  
	/** Insert the view into the view-list, fill the view with the vme tree and select the selected vme. */
  void ViewInsert(albaView *view);
  
	/** Remove the view from the view-list and delete it. */
  virtual void ViewDelete(albaView *view);
  
	/** Delete all view. */
  virtual void ViewDeleteAll();

	/** Reset the camera to fit the selected vme (if sel = true) or all (if sel = false). apply to the selected view only. */
  void CameraReset(bool sel = false);

	/** Reset the camera to fit the specified vme. apply to the selected view only. */
  void CameraReset(albaVME *vme);   

	/** Update the camera for all opened views; optionally the Update can be limited for the selected view. */
  void CameraUpdate(bool only_selected = false);

	/** Set the FlyTo mode for the selected view. */
	void CameraFlyToMode();

	/** Call PropertyUpdate for all views. */
	void PropertyUpdate(bool fromTag = false);

  /** Return the selected view. */
	albaView *GetSelectedView();

  /** Return the root of the vme tree. */
  albaVME *GetCurrentRoot() {return (albaVME*)m_RootVme;};

  /** Return the created view-list. */
  albaView *GetList() {return m_ViewList;};

  /** Return a view (defined by label) if is present on open wiews list*/
  albaView *GetFromList(const char *label);

  /** Return the plugged view-list. */
  albaView **GetListTemplate() {return m_ViewTemplate;};

  /** Empty. */
  void OnQuit();

  /** Return the view pointer from view's id and multiplicity. */
  albaView *GetView(int id, int mult) {return m_ViewMatrixID[id][mult];};

  /** Initialize the action for the mouse device.*/
  void SetMouse(albaDeviceButtonsPadMouse *mouse);

	/* Return the gui for Settings Dialog*/
	albaGUI* GetSettingsGui();

	void OpenOnlyViewForType(int chose) { m_ViewOpenOnlyOne = chose; };
	void MaximizeViewOnOpen(int chose) { m_ViewMaximize = chose; };
	void EnableViewSettings(bool enable = true);

protected:

	/** Initialize the application settings.*/
	void InitializeSettings();

  albaDeviceButtonsPadMouse      *m_Mouse;
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

  /** test friend */
  friend class albaViewManagerTest;
};
#endif
