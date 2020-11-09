/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeTraceabilityTest
 Authors: Roberto Mucci, Gianlugi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaAttributeTraceabilityTest.h"
#include "albaAttributeTraceability.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestStaticAllocation()
{
  albaAttributeTraceability vm;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestDynamicAllocation()
{
  albaAttributeTraceability *vm = new albaAttributeTraceability();
  delete vm;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestAddTraceabilityEvent()
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 
 
  at1->AddTraceabilityEvent("trial1","test", "testParameter", "17/11/2008", "testApp","Build_1","testUser","NATURAL");
  at1->AddTraceabilityEvent("trial2","test1", "testParameter1", "18/11/2008", "testApp1", "Build_2", "testUser1", "SYNTHETIC");

  
	if (at1->m_TraceabilityVector[0].m_TrialEvent == "trial1" &&
		at1->m_TraceabilityVector[0].m_OperationName == "test" &&
		at1->m_TraceabilityVector[0].m_Parameters == "testParameter"  &&
		at1->m_TraceabilityVector[0].m_Date == "17/11/2008"  &&
		at1->m_TraceabilityVector[0].m_AppStamp == "testApp" &&
		at1->m_TraceabilityVector[0].m_BuildNum == "Build_1" &&
		at1->m_TraceabilityVector[0].m_OperatorID == "testUser" &&
		at1->m_TraceabilityVector[0].m_IsNatural == "NATURAL" &&
		at1->m_TraceabilityVector[1].m_TrialEvent == "trial2" &&
		at1->m_TraceabilityVector[1].m_OperationName == "test1" &&
		at1->m_TraceabilityVector[1].m_Parameters == "testParameter1" &&
		at1->m_TraceabilityVector[1].m_Date == "18/11/2008" &&
		at1->m_TraceabilityVector[1].m_AppStamp == "testApp1" &&
		at1->m_TraceabilityVector[1].m_BuildNum == "Build_2" &&
		at1->m_TraceabilityVector[1].m_OperatorID == "testUser1" &&
		at1->m_TraceabilityVector[1].m_IsNatural == "SYNTHETIC")
	{
		result = true;
	}

  TEST_RESULT;

  delete at1;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestRemoveTraceabilityEvent()
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 

  at1->AddTraceabilityEvent("trial1", "test", "testParameter", "17/11/2008", "testApp", "Build_1", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("trial2", "test1", "testParameter1", "18/11/2008", "testApp1","Build_2", "testUser1", "SYNTHETIC");

  result = (at1->m_TraceabilityVector.size() == 2);
  TEST_RESULT;


  at1->RemoveTraceabilityEvent();
  result = (at1->m_TraceabilityVector.size() == 1);
  TEST_RESULT;

  if(at1->m_TraceabilityVector[0].m_TrialEvent == "trial1" &&
    at1->m_TraceabilityVector[0].m_OperationName == "test" &&
    at1->m_TraceabilityVector[0].m_Parameters == "testParameter" &&
    at1->m_TraceabilityVector[0].m_Date == "17/11/2008" &&
		at1->m_TraceabilityVector[0].m_AppStamp == "testApp" &&
		at1->m_TraceabilityVector[0].m_BuildNum == "Build_1" &&
		at1->m_TraceabilityVector[0].m_OperatorID == "testUser" &&
    at1->m_TraceabilityVector[0].m_IsNatural == "NATURAL")
  {
    result = true;
  }
  else
  {
    result = false;
  }

  TEST_RESULT

  delete at1;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestGetLastTrialEvent()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 

  at1->AddTraceabilityEvent("trial1", "test", "testParameter", "17/11/2008", "testApp","build_0", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("trial2", "test1", "testParameter1", "18/11/2008", "testApp1", "build_1", "testUser1", "SYNTHETIC");

  albaString event = at1->GetLastTrialEvent();
  result = (strcmp(event.GetCStr(), "trial2") == 0);

  TEST_RESULT

  delete at1;
}
//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 
  albaAttributeTraceability *at2 = new albaAttributeTraceability();

  at1->AddTraceabilityEvent("trial1", "test", "testParameter", "17/11/2008", "testApp", "build_1", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("trial2", "test1", "testParameter1", "18/11/2008", "testApp1","build_2", "testUser1", "NATURAL1");

  at2->DeepCopy(at1);

  int vecSize = at1->m_TraceabilityVector.size();
  for (int i = 0; i < vecSize; i++)
  {
    if(at1->m_TraceabilityVector[i].m_TrialEvent != at2->m_TraceabilityVector[i].m_TrialEvent ||
      at1->m_TraceabilityVector[i].m_OperationName != at2->m_TraceabilityVector[i].m_OperationName ||
      at1->m_TraceabilityVector[i].m_Parameters != at2->m_TraceabilityVector[i].m_Parameters ||
			at1->m_TraceabilityVector[i].m_Date != at2->m_TraceabilityVector[i].m_Date ||
      at1->m_TraceabilityVector[i].m_AppStamp != at2->m_TraceabilityVector[i].m_AppStamp ||
			at1->m_TraceabilityVector[i].m_BuildNum != at2->m_TraceabilityVector[i].m_BuildNum ||
      at1->m_TraceabilityVector[i].m_OperatorID != at2->m_TraceabilityVector[i].m_OperatorID)
    {
      result = false;
      break;
    }
    else
      result = true;
  }

  TEST_RESULT;
  
  delete at1;
  delete at2;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability();
	albaAttributeTraceability *at2 = new albaAttributeTraceability();
	albaAttributeTraceability *at3 = new albaAttributeTraceability();

  at1->AddTraceabilityEvent("trial1","test", "testParameter", "17/11/2008", "testApp", "build_1", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("trial2", "test1", "testParameter1", "18/11/2008", "testApp1", "build_2", "testUser1", "NATURAL1");

  at2->AddTraceabilityEvent("trial1", "test", "testParameter", "17/11/2008", "testApp", "build_1", "testUser", "NATURAL");
  at2->AddTraceabilityEvent("trial2", "test1", "testParameter1", "18/11/2008", "testApp1", "build_2", "testUser1", "NATURAL1");

	at3->AddTraceabilityEvent("trial1", "test", "testParameter", "17/11/2008", "testApp", "build_1", "testUser", "NATURAL");
	at3->AddTraceabilityEvent("trialDiff", "test1", "testParameter1", "18/11/2008", "testApp1", "build_2", "testUser1", "NATURAL1");

	result = at2->Equals(at1);
	TEST_RESULT;

	result = at2->Equals(at1);
	TEST_RESULT;

  delete at1;
  delete at2;
	delete at3;
}
