/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaSnapshotManager
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSnapshotManager_H__
#define __albaSnapshotManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObserver.h"
#include "albaGUI.h"
#include "albaGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class albaVMEGroup;
class albaGUIImageViewer;
class albaDeviceButtonsPadMouse;

/**
  Class Name: albaSnapshotManager.
*/
class ALBA_EXPORT albaSnapshotManager : albaGUISettings, albaObserver
{
public:

	/** Default constructor */
	albaSnapshotManager();

  /** Default destructor */
  ~albaSnapshotManager();

	/** Event Management */
	virtual void OnEvent(albaEventBase *alba_event);
	
	/* Create snapshot from selected view*/
	void CreateSnapshot(albaVME *node, albaView *selectedView);

	/* Find or Create Snapshots group*/
	void FindOrCreateSnapshotGroup(albaVME *root);

	/* Show a Preview of Snapshots*/
	void ShowSnapshotPreview();

	/* Get Number of snapshots*/
	int GetNSnapshots();

	bool HasSnapshots(albaVME *root);

	/* Return the snapshots group*/
	albaVMEGroup* GetSnapshotGroup() { return m_SnapshotsGroup; };
	
	/* Set the snapshots group*/
	void SetSnapshotGroup(albaVMEGroup* group) { m_SnapshotsGroup = group; };

	/* Set Mouse for imageViewer interaction*/
	void SetMouse(albaDeviceButtonsPadMouse *mouse);

	/* Return the gui for Settings Dialog*/
	albaGUI* GetSettingsGui();

protected:

	/** Initialize the application settings.*/
	void InitializeSettings();

	void SelectImage();

	wxString						m_GroupName;
	albaVMEGroup				*m_SnapshotsGroup;
	albaGUIImageViewer	*m_ImageViewer;

	albaGUI							*m_SettingsGui;

	wxColour m_ColorBackground;
	int m_SnapshotsCustomResoluzion;
	int m_SnapshotsWidth;
	int m_SnapshotsHeight;

private:
  friend class albaLogicWithManagers;
};
#endif
