/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaFakeLogicForTest.cpp
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaFakeLogicForTest.h"
#include "albaVME.h"

//-------------------------------------------------------------------------
albaFakeLogicForTest::albaFakeLogicForTest()
{
	
}

//-------------------------------------------------------------------------
albaFakeLogicForTest::~albaFakeLogicForTest()
{
	ClearCalls();
}

// VME ////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeSelect(albaVME *vme)
{
	AddCall(albaFakeLogicForTest::FKL_VME_SELECT, vme);
}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeSelected(albaVME *vme){}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeShow(albaVME *vme, bool visibility)
{
	AddCall(albaFakeLogicForTest::FKL_VME_SHOW, vme);
}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeAdd(albaVME *vme)
{
	AddCall(albaFakeLogicForTest::FKL_VME_ADD, vme);
}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeRemove(albaVME *vme)
{
	AddCall(albaFakeLogicForTest::FKL_VME_REMOVE, vme);
	vme->ReparentTo(NULL);
}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeModified(albaVME* vme)
{
	AddCall(albaFakeLogicForTest::FKL_VME_MODIFIED, vme);
}

//-------------------------------------------------------------------------
void albaFakeLogicForTest::VmeVisualModeChanged(albaVME * vme)
{
	AddCall(albaFakeLogicForTest::FKL_VME_VISUAL_MODE_CHANGED, vme);
}

// Camera /////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void albaFakeLogicForTest::CameraUpdate(){}
//-------------------------------------------------------------------------
void albaFakeLogicForTest::CameraReset(){}


// UTILS /////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void albaFakeLogicForTest::ClearCalls()
{
	m_Calls.clear();
}

//-------------------------------------------------------------------------
albaFakeLogicForTest::calls albaFakeLogicForTest::GetCall(int callNum)
{
	return m_Calls[callNum];
}

//-------------------------------------------------------------------------
void albaFakeLogicForTest::AddCall(testFunctions func, albaVME* vme)
{
	calls call;
	call.testFunction = func;
	call.vme = vme;

	m_Calls.push_back(call);
}

//----------------------------------------------------------------------------
bool albaFakeLogicForTest::IsOperationRunning() { return false; }

//----------------------------------------------------------------------------
const char* albaFakeLogicForTest::GetMsfFileExtension()
{
	return "msf";
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::VmeRemoved()
{
	AddCall(albaFakeLogicForTest::FKL_VME_REMOVED, NULL);
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::PrintImage(albaVMEImage *img)
{
	AddCall(albaFakeLogicForTest::FKL_PRINT_IMAGE, (albaVME*)img);
}

//----------------------------------------------------------------------------
albaOpManager * albaFakeLogicForTest::GetOpManager() const
{
	return NULL;
}

//----------------------------------------------------------------------------
albaViewManager * albaFakeLogicForTest::GetViewManager()
{
	return NULL;
}

//----------------------------------------------------------------------------
albaProsthesesDBManager * albaFakeLogicForTest::GetProsthesesDBManager()
{
	return NULL;
}
