/*=========================================================================

 Program: MAF2
 Module:  mafSnapshotManager
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafSnapshotManager_H__
#define __mafSnapshotManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class mafVMEGroup;
class mafGUIImageViewer;

/**
  Class Name: mafSnapshotManager.
*/
class MAF_EXPORT mafSnapshotManager : mafObserver
{
public:

  /** Default constructor */
	mafSnapshotManager();

  /** Default destructor */
  ~mafSnapshotManager();

	/** Event Management */
	virtual void OnEvent(mafEventBase *maf_event);
	
	/* Create snapshot from selected view*/
	void CreateSnapshot(mafVME *node, mafView *selectedView);

	/*Show a Preview of Snapshots*/
	void ShowSnapshotPreview();

	/*Get Number of snapshots*/
	int GetNSnapshots();

	/*Return the snapshots group*/
	mafVMEGroup* GetSnapshotGroup() { return m_SnapshotsGroup; };
	
	/*Set the snapshots group*/
	void SetSnapshotGroup(mafVMEGroup* group) { m_SnapshotsGroup = group; };

protected:

	void SelectImage();

	mafVMEGroup *m_SnapshotsGroup;
	mafGUIImageViewer *m_ImageViewer;

private:
  friend class mafLogicWithManagers;
};
#endif
