/*=========================================================================

 Program: MAF2
 Module:  mafFakeLogicForTest.h
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _mafFakeLogicForTest_H
#define _mafFakeLogicForTest_H

#include "mafAbsLogicManager.h"

class mafFakeLogicForTest : public mafAbsLogicManager
{
public:


protected:

	// VME
	virtual void VmeSelect(mafVME *vme);

	virtual void VmeSelected(mafVME *vme);

	virtual void VmeShow(mafVME *vme, bool visibility);

	virtual void VmeAdd(mafVME *vme);

	virtual void VmeAdded(mafVME *vme);

	virtual void VmeRemove(mafVME *vme);

	virtual void VmeRemoving(mafVME *vme);

	virtual void VmeVisualModeChanged(mafVME * vme);

	// Camera
	virtual void CameraUpdate();

	virtual void CameraReset();
};
#endif