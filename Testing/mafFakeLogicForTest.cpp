/*=========================================================================

 Program: MAF2
 Module:  mafFakeLogicForTest.cpp
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafFakeLogicForTest.h"
#include "mafVME.h"

//-------------------------------------------------------------------------
mafFakeLogicForTest::mafFakeLogicForTest()
{
	
}

//-------------------------------------------------------------------------
mafFakeLogicForTest::~mafFakeLogicForTest()
{
	ClearCalls();
}

// VME ////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeSelect(mafVME *vme)
{
	AddCall(mafFakeLogicForTest::FKL_VME_SELECT, vme);
}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeSelected(mafVME *vme){}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeShow(mafVME *vme, bool visibility)
{
	AddCall(mafFakeLogicForTest::FKL_VME_SHOW, vme);
}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeAdd(mafVME *vme)
{
	AddCall(mafFakeLogicForTest::FKL_VME_ADD, vme);
}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeRemove(mafVME *vme)
{
	AddCall(mafFakeLogicForTest::FKL_VME_REMOVE, vme);
	vme->ReparentTo(NULL);
}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeModified(mafVME* vme)
{
	AddCall(mafFakeLogicForTest::FKL_VME_MODIFIED, vme);
}

//-------------------------------------------------------------------------
void mafFakeLogicForTest::VmeVisualModeChanged(mafVME * vme)
{
	AddCall(mafFakeLogicForTest::FKL_VME_VISUAL_MODE_CHANGED, vme);
}

// Camera /////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void mafFakeLogicForTest::CameraUpdate(){}
//-------------------------------------------------------------------------
void mafFakeLogicForTest::CameraReset(){}


// UTILS /////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void mafFakeLogicForTest::ClearCalls()
{
	m_Calls.clear();
}

//-------------------------------------------------------------------------
mafFakeLogicForTest::calls mafFakeLogicForTest::GetCall(int callNum)
{
	return m_Calls[callNum];
}

//-------------------------------------------------------------------------
void mafFakeLogicForTest::AddCall(testFunctions func, mafVME* vme)
{
	calls call;
	call.testFunction = func;
	call.vme = vme;

	m_Calls.push_back(call);
}

//----------------------------------------------------------------------------
bool mafFakeLogicForTest::IsOperationRunning() { return false; }

//----------------------------------------------------------------------------
const char* mafFakeLogicForTest::GetMsfFileExtension()
{
	return "msf";
}

//----------------------------------------------------------------------------
void mafFakeLogicForTest::VmeRemoved()
{
	AddCall(mafFakeLogicForTest::FKL_VME_REMOVED, NULL);
}

//----------------------------------------------------------------------------
void mafFakeLogicForTest::PrintImage(mafVMEImage *img)
{
	AddCall(mafFakeLogicForTest::FKL_PRINT_IMAGE, (mafVME*)img);
}
