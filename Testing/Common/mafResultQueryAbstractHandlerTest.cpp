/*=========================================================================

 Program: MAF2
 Module: mafResultQueryAbstractHandlerTest
 Authors: Daniele Giunchi
 
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

#include "mafResultQueryAbstractHandlerTest.h"
#include "mafResultQueryAbstractHandler.h"

/**
  Class Name: mafResultQueryAbstractHandlerSample
  sample class for testing interface
*/
class mafResultQueryAbstractHandlerSample : public mafResultQueryAbstractHandler
{
public:
  /** constructor */
  mafResultQueryAbstractHandlerSample();
  /** destructor */
  virtual ~mafResultQueryAbstractHandlerSample(){}; 

  /** RTTI macro */
  mafTypeMacro(mafResultQueryAbstractHandlerSample, mafResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafResultQueryAbstractHandlerSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafResultQueryAbstractHandlerSample::mafResultQueryAbstractHandlerSample()
//----------------------------------------------------------------------------
{
  std::vector<std::string> vec1;
  vec1.push_back("test11"); vec1.push_back("test12"); vec1.push_back("test13");
  m_MatrixStringResult.push_back(vec1);
  std::vector<std::string> vec2;
  vec2.push_back("test21"); vec2.push_back("test22"); vec2.push_back("test23");
  m_MatrixStringResult.push_back(vec2);
  std::vector<std::string> vec3;
  vec3.push_back("test31"); vec3.push_back("test32"); vec3.push_back("test33");
  m_MatrixStringResult.push_back(vec3);
  
  m_ColumnsTypeInformation.push_back("type1"); m_ColumnsTypeInformation.push_back("type2"); m_ColumnsTypeInformation.push_back("type3");
  m_ColumnsNameInformation.push_back("title1"); m_ColumnsNameInformation.push_back("title2"); m_ColumnsNameInformation.push_back("title3");

  std::vector<mafQueryObject *> vecA;
  vecA.push_back(new mafQueryObject("atest11")); vecA.push_back(new mafQueryObject("atest12")); vecA.push_back(new mafQueryObject("atest13"));
  m_MatrixObjectResult.push_back(vecA);
  std::vector<mafQueryObject *> vecB;
  vecB.push_back(new mafQueryObject("atest21")); vecB.push_back(new mafQueryObject("atest22")); vecB.push_back(new mafQueryObject("atest23"));
  m_MatrixObjectResult.push_back(vecB);
  std::vector<mafQueryObject *> vecC;
  vecC.push_back(new mafQueryObject("atest31")); vecC.push_back(new mafQueryObject("atest32")); vecC.push_back(new mafQueryObject("atest33"));
  m_MatrixObjectResult.push_back(vecC);
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerSample::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool mafResultQueryAbstractHandlerSample::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

// TEST

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafResultQueryAbstractHandlerSample *rq = new mafResultQueryAbstractHandlerSample();
	cppDEL(rq);
}
//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafResultQueryAbstractHandlerSample rq;
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetResultAsStringMatrix()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsStringMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetColumnsTypeInformationAsStringVector()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsTypeInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetColumnsNameInformationAsStringVector()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsNameInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}


//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsObjectsMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfRecords();
  CPPUNIT_ASSERT(value == 3);
}

//----------------------------------------------------------------------------
void mafResultQueryAbstractHandlerTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
  mafResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfFields();
  CPPUNIT_ASSERT(value == 3);
}

