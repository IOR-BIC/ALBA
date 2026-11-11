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

class albaVME;
class albaVMEImage;
class albaOpManager;
class albaViewManager;
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

	//Camera
	virtual void CameraUpdate() = 0;

	virtual void CameraReset() = 0;

	virtual void EnableCameraUpdateSkip() = 0;

	virtual void DisableCameraUpdateSkip() = 0;

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

	//Key Events
	/** Register the listener for keydown and keyrelease events */
	virtual void RegisterForKeyEvents(albaObserver *listener) = 0;
	/** Unregister the listener for key events */
	virtual void UnRegisterForKeyEvents(albaObserver *listener) = 0;

	//Interaction
	/** Push a tool on the tool stack of the positional event router: it receives the events before the default interaction */
	virtual void PushTool(albaInteractionTool *tool) = 0;
	/** Remove a tool from the tool stack of the positional event router */
	virtual void PopTool(albaInteractionTool *tool) = 0;
	
	//Managers
	/** Get the Operation Manager */
	virtual albaOpManager * GetOpManager() const = 0;
	/** Get the View Manager */
	virtual albaViewManager *GetViewManager() = 0;
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