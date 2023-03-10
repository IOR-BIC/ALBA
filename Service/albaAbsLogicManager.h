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

class albaVME;
class albaVMEImage;
class albaOpManager;
class albaViewManager;
class albaProsthesesDBManager;

class ALBA_EXPORT albaAbsLogicManager
{
public:

	// VME
	virtual void VmeSelect(albaVME *vme) = 0;

	virtual void VmeSelected(albaVME *vme) = 0;

	virtual void VmeShow(albaVME *vme, bool visibility) = 0;

	virtual void VmeAdd(albaVME *vme) = 0;

	//virtual void VmeAdded(albaVME *vme) = 0;

	virtual void VmeRemove(albaVME *vme) = 0;

	virtual void VmeRemoved() = 0;

	virtual void VmeModified(albaVME* vme) = 0;

	virtual void VmeVisualModeChanged(albaVME * vme) = 0;

	//Camera
	virtual void CameraUpdate() = 0;

	virtual void CameraReset() = 0;

	virtual void EnableCameraUpdateSkip() = 0;

	virtual void DisableCameraUpdateSkip() = 0;

	/** Returns true if there is a Running Operation */
	virtual bool IsOperationRunning() = 0;

	virtual const char* GetMsfFileExtension() = 0;

	virtual void PrintImage(albaVMEImage *img) = 0;

	virtual albaOpManager * GetOpManager() const = 0;

	virtual albaViewManager *GetViewManager() = 0;

	virtual albaProsthesesDBManager * GetProsthesesDBManager() = 0;
};
#endif