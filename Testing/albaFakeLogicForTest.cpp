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
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include <algorithm>

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

//-------------------------------------------------------------------------
albaVME *albaFakeLogicForTest::ChooseVME(albaString title, void *acceptFunction, albaVME *selected)
{
	std::vector<albaVME*> accepted = ChooseVMEs(title, acceptFunction, selected);
	return accepted.empty() ? NULL : accepted[0];
}

//-------------------------------------------------------------------------
std::vector<albaVME*> albaFakeLogicForTest::ChooseVMEs(albaString title, void *acceptFunction, albaVME *selected)
{
	typedef bool(*AcceptFunction)(albaVME *);
	AcceptFunction accept = (AcceptFunction)acceptFunction;

	m_ChooseCalls++;
	std::vector<albaVME*> accepted;
	for (int i = 0; i < m_ChooseCandidates.size(); i++)
		if (accept == NULL || accept(m_ChooseCandidates[i]))
			accepted.push_back(m_ChooseCandidates[i]);

	return accepted;
}



// UTILS /////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void albaFakeLogicForTest::ClearCalls()
{
	m_Calls.clear();
	m_ChooseCalls = 0;
}

//-------------------------------------------------------------------------
albaFakeLogicForTest::calls albaFakeLogicForTest::GetCall(int callNum)
{
	return m_Calls[callNum];
}

//-------------------------------------------------------------------------
int albaFakeLogicForTest::GetNumberOfCalls(testFunctions func)
{
	int count = 0;
	for (int i = 0; i < m_Calls.size(); i++)
		if (m_Calls[i].testFunction == func)
			count++;
	return count;
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
albaVMEStorage *albaFakeLogicForTest::CreateStorage(albaVMEStorage *oldStorage)
{
	albaDEL(oldStorage);
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	return storage;
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

//----------------------------------------------------------------------------
albaProsthesesDBManager * albaFakeLogicForTest::GetProsthesesDBManager()
{
	return NULL;
}


//----------------------------------------------------------------------------
bool albaFakeLogicForTest::IsVMEExpanded(albaVME *vme)
{
	return true;
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::CollapseVME(albaVME *vme)
{
	return;
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::ExpandVME(albaVME *vme)
{
	return;
}

const char* albaFakeLogicForTest::GetAppBuildNum()
{
	return "";
}

void albaFakeLogicForTest::SetAppBuildNum(char* buildNum)
{
}

const char* albaFakeLogicForTest::GetAlbaBuildNum()
{
	return "";
}

void albaFakeLogicForTest::SetAlbaBuildNum(char *buildNum)
{
}

const char *albaFakeLogicForTest::GetCitationPaper()
{
	return "";
}

void albaFakeLogicForTest::SetCitationPaper(char *link)
{
	return;
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::PushTool(albaInteractionTool *tool)
{
	if (!IsToolPushed(tool))
		m_PushedTools.push_back(tool);
}

//----------------------------------------------------------------------------
void albaFakeLogicForTest::PopTool(albaInteractionTool *tool)
{
	std::vector<albaInteractionTool *>::iterator it = std::find(m_PushedTools.begin(), m_PushedTools.end(), tool);
	if (it != m_PushedTools.end())
		m_PushedTools.erase(it);
}

//----------------------------------------------------------------------------
bool albaFakeLogicForTest::IsToolPushed(albaInteractionTool *tool)
{
	return std::find(m_PushedTools.begin(), m_PushedTools.end(), tool) != m_PushedTools.end();
}

//----------------------------------------------------------------------------
bool albaFakeLogicForTest::IsTimeVaringEnabled()
{
	return false;
}
