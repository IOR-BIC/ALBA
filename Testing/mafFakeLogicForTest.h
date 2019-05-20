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
#include <vector>

class mafFakeLogicForTest : public mafAbsLogicManager
{

public:

	enum testFunctions
	{
		FKL_VME_SELECT,
		FKL_VME_ADD,
		FKL_VME_REMOVE,
		FKL_VME_MODIFIED,
		FKL_VME_SHOW,
		FKL_VME_VISUAL_MODE_CHANGED,
		FKL_VME_REMOVED,
		FKL_PRINT_IMAGE
	};

	struct calls
	{
		testFunctions testFunction;
		mafVME *vme;
	};
	
	mafFakeLogicForTest();
	~mafFakeLogicForTest();

	// VME
	virtual void VmeSelect(mafVME *vme);

	virtual void VmeSelected(mafVME *vme);

	virtual void VmeShow(mafVME *vme, bool visibility);

	virtual void VmeAdd(mafVME *vme);

	virtual void VmeRemove(mafVME *vme);

	virtual void VmeModified(mafVME* vme);

	virtual void VmeVisualModeChanged(mafVME * vme);

	// Camera
	virtual void CameraUpdate();

	virtual void CameraReset();

	void ClearCalls();

	calls GetCall(int callNum);
	
	void AddCall(testFunctions func, mafVME* vme);

	virtual bool IsOperationRunning();
	
	virtual const char* GetMsfFileExtension() override;
	
	virtual void VmeRemoved() override;
	
	virtual void PrintImage(mafVMEImage *img) override;

	virtual mafOpManager * GetOpManager() const override;

protected:
	
	std::vector<calls> m_Calls;
};
#endif