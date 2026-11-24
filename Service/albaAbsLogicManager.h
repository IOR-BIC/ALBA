/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaAbsLogicManager.h
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaAbsLogicManager_H
#define _albaAbsLogicManager_H

#include "albaConfigure.h"
#include "albaString.h"
#include <vector>

class albaVME;
class albaVMEImage;
class albaVMEStorage;
class albaView;
class albaProsthesesDBManager;
class albaObserver;
class albaGUIPanel;
class albaOp;
class albaGUIVMETree;

class ALBA_EXPORT albaAbsLogicManager
{
public:

	// VME
	virtual void VmeSelect(albaVME *vme) = 0;

	virtual void VmeSelected(albaVME *vme) = 0;

	virtual void VmeShow(albaVME *vme, bool visibility) = 0;

	virtual void VmeAdd(albaVME *vme) = 0;

	virtual void VmeRemove(albaVME *vme) = 0;

	virtual void VmeRemoved() = 0;

	virtual void VmeModified(albaVME* vme) = 0;

	virtual void VmeVisualModeChanged(albaVME * vme) = 0;

	/** Called by the VME manager for each VME added to the tree */
	virtual void VmeAdded(albaVME *vme) = 0;

	/** Called by the VME manager for each VME that is going to be removed from the tree */
	virtual void VmeRemoving(albaVME *vme) = 0;

	/** Called by the VME manager to replace its storage: delete oldStorage (if any) and return a new one */
	virtual albaVMEStorage *CreateStorage(albaVMEStorage *oldStorage) = 0;

	/** Show the tree in a dialog titled title and return the VME chosen by the user, NULL on cancel.
	acceptFunction is a bool (*)(albaVME *) that tells which VMEs can be chosen (all if NULL), selected is the VME selected when the dialog opens. */
	virtual albaVME *ChooseVME(albaString title = "Choose Node", void *acceptFunction = NULL, albaVME *selected = NULL) = 0;

	/** Same as ChooseVME, but the user can choose more VMEs; returns an empty vector on cancel. */
	virtual std::vector<albaVME*> ChooseVMEs(albaString title = "Choose Nodes", void *acceptFunction = NULL, albaVME *selected = NULL) = 0;

	//Logic
	/** Returns true if there is a Running Operation */
	virtual bool IsOperationRunning() = 0;

	/** Called by the op manager when an operation starts (not for the nested ones): menus and selection are locked */
	virtual void OpRunStarting() = 0;

	/** Called by the op manager when an operation ends (not for the nested ones), runOk tells if it was executed */
	virtual void OpRunTerminated(bool runOk) = 0;

	/** Called by an operation to show its gui (panel) on the side bar */
	virtual void OpShowGui(albaGUIPanel *panel) = 0;

	/** Called by an operation to remove its gui from the side bar */
	virtual void OpHideGui() = 0;

	virtual const char* GetMsfFileExtension() = 0;

	virtual void PrintImage(albaVMEImage *img) = 0;

	//Tree 
	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme) = 0;

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme) = 0;

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme) = 0;

	/** Called by the tree on the right click: show its contextual menu, with the items of vme if vmeMenu is true */
	virtual void ShowTreeContextualMenu(albaGUIVMETree *tree, albaVME *vme, bool vmeMenu) = 0;

	//Views
	/** Select a view (also called by the view frames when they are activated or clicked): the view manager,
	the side bar, the interaction and the operations work on it. Nothing else happens if it is already selected */
	virtual void SelectView(albaView *view) = 0;
	/** Return the selected view, NULL if none */
	virtual albaView *GetSelectedView() = 0;

	/** Called by the view manager when it creates a view: create the frame of the view and attach the view to it */
	virtual void CreateViewFrame(albaView *view) = 0;

	/** Called by the view manager when it deletes a view */
	virtual void ViewDeleted(albaView *view) = 0;

	/** Update the title of the main window (opened file or running wizard) */
	virtual void UpdateFrameTitle() = 0;

	/** Tile the view frames horizontally or vertically */
	virtual void TileViewFrames(bool horizontally) = 0;

	/** Cascade the view frames */
	virtual void CascadeViewFrames() = 0;

	/** Called on the right click in a view: show the contextual menu of the selected view, with the VME items if vmeMenu is true */
	virtual void ShowViewContextualMenu(bool vmeMenu) = 0;

	//Time
	/** Set the current time of the application: the VMEs and the views are updated */
	virtual void TimeSet(double t) = 0;

	//Files
	/** Create a new project, asking to save the opened one: return ALBA_OK if created */
	virtual int FileNew() = 0;

	/** Open the given project file, or ask the user to choose one if file is NULL: return ALBA_ERROR if not opened */
	virtual int FileOpen(const char *file = NULL) = 0;

	/** Save the project: return ALBA_ERROR if not saved */
	virtual int FileSave() = 0;

	/** Save the project asking the file name, if "legacy" is true in the old file format: return ALBA_ERROR if not saved */
	virtual int FileSaveAs(bool legacy = false) = 0;

	/** Open again the current project file: return ALBA_ERROR if there is none or it is not opened */
	virtual int FileReload() = 0;

	/** Create a snapshot of the selected view: return ALBA_ERROR if not created */
	virtual int CreateSnapshot() = 0;

	//Wizards
	/** Called by the wizard manager when a wizard starts: menus, operations and selection are locked */
	virtual void WizardRunStarting() = 0;

	/** Called by the wizard manager when a wizard ends: menus, operations and selection are unlocked */
	virtual void WizardRunTerminated() = 0;

	/** Run an operation for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(const char *opName) = 0;

	/** Run an operation for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(albaOp *op) = 0;

	/** Run an operation (by id, e.g. OP_DELETE) for the running wizard, which continues when the operation ends (or at once if it does not start) */
	virtual void WizardRunOp(int opId) = 0;

	
	//Managers
	/** Get the Prostheses DB Manager */
	virtual albaProsthesesDBManager * GetProsthesesDBManager() = 0;

	//Versioning
	/** Get the App Build Number */
	virtual const char* GetAppBuildNum() = 0;
	/** Set the App Build Number */
	virtual void SetAppBuildNum(char *buildNum) = 0;
	/** Set Alba Build Number*/
	virtual const char* GetAlbaBuildNum() = 0;
	/** Get Alba Build Number*/
	virtual void SetAlbaBuildNum(char *buildNum) = 0;

	/** Get the link to the citation Paper */
	virtual const char *GetCitationPaper() = 0;
	/** Set the link to the citation Paper */
	virtual void SetCitationPaper(char *link) = 0;


	//App Settings
	virtual bool IsTimeVaringEnabled() = 0;
};
#endif