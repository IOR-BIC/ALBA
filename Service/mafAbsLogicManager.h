/*=========================================================================

 Program: MAF2
 Module:  mafAbsLogicManager.h
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _mafAbsLogicManager_H
#define _mafAbsLogicManager_H

class mafVME;

class mafAbsLogicManager
{
public:

	// VME
	virtual void VmeSelect(mafVME *vme) = 0;

	virtual void VmeSelected(mafVME *vme) = 0;

	virtual void VmeShow(mafVME *vme, bool visibility) = 0;

	virtual void VmeAdd(mafVME *vme) = 0;

	//virtual void VmeAdded(mafVME *vme) = 0;

	virtual void VmeRemove(mafVME *vme) = 0;

	//virtual void VmeRemoving(mafVME *vme) = 0;

	virtual void VmeModified(mafVME* vme) = 0;

	virtual void VmeVisualModeChanged(mafVME * vme) = 0;

	//Camera
	virtual void CameraUpdate() = 0;

	virtual void CameraReset() = 0;

	/** Returns true if there is a Running Operation */
	virtual bool IsOperationRunning() = 0;

	virtual const char* GetMsfFileExtension() = 0;
};
#endif