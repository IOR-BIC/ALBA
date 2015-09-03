/*=========================================================================

 Program: MAF2
 Module: mafAttributeTraceabilityTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafAttributeTraceabilityTest.h"
#include "mafAttributeTraceability.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafAttributeTraceabilityTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafAttributeTraceabilityTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability vm;
}

//----------------------------------------------------------------------------
void mafAttributeTraceabilityTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *vm = new mafAttributeTraceability();
  delete vm;
}

//----------------------------------------------------------------------------
void mafAttributeTraceabilityTest::TestAddTraceabilityEvent()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *at1 = new mafAttributeTraceability(); 
 
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
void mafAttributeTraceabilityTest::TestRemoveTraceabilityEvent()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *at1 = new mafAttributeTraceability(); 

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
void mafAttributeTraceabilityTest::TestGetLastTrialEvent()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *at1 = new mafAttributeTraceability(); 

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "SYNTHETIC");

  mafString event = at1->GetLastTrialEvent();
  result = (strcmp(event.GetCStr(), "test1") == 0);

  TEST_RESULT

  delete at1;
}
//----------------------------------------------------------------------------
void mafAttributeTraceabilityTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *at1 = new mafAttributeTraceability(); 
  mafAttributeTraceability *at2 = new mafAttributeTraceability();

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
void mafAttributeTraceabilityTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafAttributeTraceability *at1 = new mafAttributeTraceability();
  mafAttributeTraceability *at2 = new mafAttributeTraceability();

  at1->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at1->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "NATURAL1");

  at2->AddTraceabilityEvent("test", "testParameter", "17/11/2008", "testApp", "testUser", "NATURAL");
  at2->AddTraceabilityEvent("test1", "testParameter1", "18/11/2008", "testApp1", "testUser1", "NATURAL1");

  result = at2->Equals(at1);
  TEST_RESULT;

  delete at1;
  delete at2;
}
