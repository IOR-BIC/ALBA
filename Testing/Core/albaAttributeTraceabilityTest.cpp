/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeTraceabilityTest
 Authors: Roberto Mucci
 
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
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability vm;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *vm = new albaAttributeTraceability();
  delete vm;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestAddTraceabilityEvent()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 
 
  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "SYNTHETIC");

  
  if(at1->m_TraceabilityVector[0].m_TrialEvent == "test" &&
      at1->m_TraceabilityVector[0].m_OperationName == "testParameter" &&
      at1->m_TraceabilityVector[0].m_Parameters == "17/11/2008" &&
      at1->m_TraceabilityVector[0].m_Date == "testApp" &&
      at1->m_TraceabilityVector[0].m_AppStamp == "testUser" &&
      at1->m_TraceabilityVector[0].m_OperatorID == "NATURAL" &&
      at1->m_TraceabilityVector[1].m_TrialEvent == "test1" &&
      at1->m_TraceabilityVector[1].m_OperationName == "testParameter1" &&
      at1->m_TraceabilityVector[1].m_Parameters == "18/11/2008" &&
      at1->m_TraceabilityVector[1].m_Date == "testApp1" &&
      at1->m_TraceabilityVector[1].m_AppStamp == "testUser1" &&
      at1->m_TraceabilityVector[1].m_OperatorID == "SYNTHETIC")
    {
      result = true;
    }

  TEST_RESULT;

  delete at1;
}

//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestRemoveTraceabilityEvent()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "SYNTHETIC");

  result = (at1->m_TraceabilityVector.size() == 2);
  TEST_RESULT;


  at1->RemoveTraceabilityEvent();
  result = (at1->m_TraceabilityVector.size() == 1);
  TEST_RESULT;

  if(at1->m_TraceabilityVector[0].m_TrialEvent == "test" &&
    at1->m_TraceabilityVector[0].m_OperationName == "testParameter" &&
    at1->m_TraceabilityVector[0].m_Parameters == "17/11/2008" &&
    at1->m_TraceabilityVector[0].m_Date == "testApp" &&
    at1->m_TraceabilityVector[0].m_AppStamp == "testUser" &&
    at1->m_TraceabilityVector[0].m_OperatorID == "NATURAL")
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

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "SYNTHETIC");

  albaString event = at1->GetLastTrialEvent();
  result = (strcmp(event.GetCStr(), "test1") == 0);

  TEST_RESULT

  delete at1;
}
//----------------------------------------------------------------------------
void albaAttributeTraceabilityTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaAttributeTraceability *at1 = new albaAttributeTraceability(); 
  albaAttributeTraceability *at2 = new albaAttributeTraceability();

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "NATURAL1");

  at2->DeepCopy(at1);

  int vecSize = at1->m_TraceabilityVector.size();
  for (int i = 0; i < vecSize; i++)
  {
    if(at1->m_TraceabilityVector[i].m_TrialEvent != at2->m_TraceabilityVector[i].m_TrialEvent ||
      at1->m_TraceabilityVector[i].m_OperationName != at2->m_TraceabilityVector[i].m_OperationName ||
      at1->m_TraceabilityVector[i].m_Parameters != at2->m_TraceabilityVector[i].m_Parameters ||
      at1->m_TraceabilityVector[i].m_Date != at2->m_TraceabilityVector[i].m_Date ||
      at1->m_TraceabilityVector[i].m_AppStamp != at2->m_TraceabilityVector[i].m_AppStamp ||
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

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "NATURAL1");

  at2->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at2->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "NATURAL1");

  result = at2->Equals(at1);
  TEST_RESULT;

  delete at1;
  delete at2;
}
