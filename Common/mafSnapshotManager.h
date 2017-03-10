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
#include "wx\iconbndl.h"
#include <set>
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class mafGUIButton;
class mafGUIDialog;
class mafGUIPicButton;
class mafGUISettingsDialog;
class mafVME;
class mafVMEGroup;
class mafVMEImage;
class vtkImageData;

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
	void ShowSnapshotPreview(mafVME *node);

	/*Get Number of snapshots*/
	int GetNSnapshots() { return m_ImagesList.size(); };

	/*Return the snapshots group*/
	mafVMEGroup* GetSnapshotGroup() { return m_SnapshotsGroup; };

protected:

	void FillImageList();
	void UpdateSelectionDialog(int selection);

	int SaveVMEImage(mafVMEImage *image, wxString imageFileName, bool scale = false);
	void SaveImageAs();
	void DeleteImage();

	mafVMEGroup *m_SnapshotsGroup;

	std::vector<wxString> m_ImagesList;
	
	int m_ImageSelection;

	mafGUIDialog			*m_Dialog;
	mafGUIPicButton		*m_PreviewImageButton;
	wxBoxSizer				*m_ImageBoxSizer;
	mafGUIButton			*m_PrevBtn;
	mafGUIButton			*m_NextBtn;

	wxString m_ImagesPath;

private:
	
  friend class mafLogicWithManagers;
};
#endif
