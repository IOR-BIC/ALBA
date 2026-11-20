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
class albaOpManager;
class albaView;
class albaProsthesesDBManager;
class albaObserver;
class albaInteractionTool;

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

	/** Show the tree in a dialog titled title and return the VME chosen by the user, NULL on cancel.
	acceptFunction is a bool (*)(albaVME *) that tells which VMEs can be chosen (all if NULL), selected is the VME selected when the dialog opens. */
	virtual albaVME *ChooseVME(albaString title = "Choose Node", void *acceptFunction = NULL, albaVME *selected = NULL) = 0;

	/** Same as ChooseVME, but the user can choose more VMEs; returns an empty vector on cancel. */
	virtual std::vector<albaVME*> ChooseVMEs(albaString title = "Choose Nodes", void *acceptFunction = NULL, albaVME *selected = NULL) = 0;

	//Logic
	/** Returns true if there is a Running Operation */
	virtual bool IsOperationRunning() = 0;

	virtual const char* GetMsfFileExtension() = 0;

	virtual void PrintImage(albaVMEImage *img) = 0;

	//Tree 
	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme) = 0;

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme) = 0;

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme) = 0;

	//Views
	/** Select a view: the side bar, the interaction and the operations work on it */
	virtual void SelectView(albaView *view) = 0;
	/** Return the selected view, NULL if none */
	virtual albaView *GetSelectedView() = 0;

	//Interaction
	/** Push a tool on the tool stack of the positional event router: it receives the events before the default interaction */
	virtual void PushTool(albaInteractionTool *tool) = 0;
	/** Remove a tool from the tool stack of the positional event router */
	virtual void PopTool(albaInteractionTool *tool) = 0;
	
	//Managers
	/** Get the Operation Manager */
	virtual albaOpManager * GetOpManager() const = 0;
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