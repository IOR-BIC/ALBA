/*=========================================================================

 Program: MAF2
 Module: mafScalarVectorTest
 Authors: Alberto Losi
 
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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "mafScalarVectorTest.h"
#include "mafScalarVector.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafScalarVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafScalarVectorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_ScalarVector = mafScalarVector::New();
}
//----------------------------------------------------------------------------
void mafScalarVectorTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_ScalarVector);
}
//----------------------------------------------------------------------------
void mafScalarVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafScalarVector* ScalarVector = mafScalarVector::New();
  mafDEL(ScalarVector);
}
//----------------------------------------------------------------------------
void mafScalarVectorTest::TestGetScalarVector()
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
void mafScalarVectorTest::TestAppendScalar()
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
void mafScalarVectorTest::TestGetNumberOfScalars()
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
void mafScalarVectorTest::TestGetNearestScalar()
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
void mafScalarVectorTest::TestGetScalarBefore()
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
void mafScalarVectorTest::TestGetScalar()
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

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)0.0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)1.0) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)1.2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)2.3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)4.1) == 2.1);
}
//----------------------------------------------------------------------------
void mafScalarVectorTest::TestSetScalar()
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

  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)0.0) == 0.0);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)1.0) == 0.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)1.2) == 1.1);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)2.3) == 2.2);
  CPPUNIT_ASSERT(m_ScalarVector->GetScalar((mafTimeStamp)4.1) == 2.1);
}
