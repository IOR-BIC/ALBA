/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaScalarVectorTest
 Authors: Alberto Losi
 
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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "albaScalarVectorTest.h"
#include "albaScalarVector.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaScalarVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_ScalarVector = albaScalarVector::New();
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_ScalarVector);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaScalarVector* ScalarVector = albaScalarVector::New();
  albaDEL(ScalarVector);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestGetScalarVector()
//----------------------------------------------------------------------------
{
  std::vector<double> svector (5);
  double scalars[5];
  scalars[0]=0.0;
  scalars[1]=0.1;
  scalars[2]=1.1;
  scalars[3]=2.2;
  scalars[4]=2.1;

  m_ScalarVector->SetScalar(0.0,scalars[0]);
  m_ScalarVector->SetScalar(1.0,scalars[1]);
  m_ScalarVector->SetScalar(1.2,scalars[2]);
  m_ScalarVector->SetScalar(2.3,scalars[3]);
  m_ScalarVector->SetScalar(4.1,scalars[4]);

  m_ScalarVector->GetScalarVector(svector);
  result=true;
  for(int i=0;i<5;i++)
    if(svector[i]!=scalars[i])
      result=false;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestAppendScalar()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->AppendScalar(1.2,1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetNumberOfScalars() == 3);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)1) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)2) == 1.1);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestGetNumberOfScalars()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->SetScalar(1.2,1.1);
  m_ScalarVector->SetScalar(2.3,2.2);
  m_ScalarVector->SetScalar(4.1,2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetNumberOfScalars() == 5);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestGetNearestScalar()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->SetScalar(1.2,1.1);
  m_ScalarVector->SetScalar(2.3,2.2);
  m_ScalarVector->SetScalar(4.1,8.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(3.8) == 8.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(0.6) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(1.15) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(1.05) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(0.1) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetNearestScalar(2.6) == 2.2);
 
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestGetScalarBefore()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->SetScalar(1.2,1.1);
  m_ScalarVector->SetScalar(2.3,2.2);
  m_ScalarVector->SetScalar(4.1,2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetScalarBefore(1.4) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalarBefore(1.1) == 0.1);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestGetScalar()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->SetScalar(1.2,1.1);
  m_ScalarVector->SetScalar(2.3,2.2);
  m_ScalarVector->SetScalar(4.1,2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)1) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)4) == 2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)0.0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)1.0) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)1.2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)2.3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)4.1) == 2.1);
}
//----------------------------------------------------------------------------
void albaScalarVectorTest::TestSetScalar()
//----------------------------------------------------------------------------
{
  m_ScalarVector->SetScalar(0.0,0.0);
  m_ScalarVector->SetScalar(1.0,0.1);
  m_ScalarVector->SetScalar(1.2,1.1);
  m_ScalarVector->SetScalar(2.3,2.2);
  m_ScalarVector->SetScalar(4.1,2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)1) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((int)4) == 2.1);

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)0.0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)1.0) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)1.2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)2.3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((albaTimeStamp)4.1) == 2.1);
}
