/*=========================================================================

 Program: MAF2Medical
 Module: medResultQueryAbstractHandlerTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medResultQueryAbstractHandlerTest.h"
#include "medResultQueryAbstractHandler.h"

/**
  Class Name: medResultQueryAbstractHandlerSample
  sample class for testing interface
*/
class medResultQueryAbstractHandlerSample : public medResultQueryAbstractHandler
{
public:
  /** constructor */
  medResultQueryAbstractHandlerSample();
  /** destructor */
  virtual ~medResultQueryAbstractHandlerSample(){}; 

  /** RTTI macro */
  mafTypeMacro(medResultQueryAbstractHandlerSample, medResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
mafCxxTypeMacro(medResultQueryAbstractHandlerSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medResultQueryAbstractHandlerSample::medResultQueryAbstractHandlerSample()
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

  std::vector<medQueryObject *> vecA;
  vecA.push_back(new medQueryObject("atest11")); vecA.push_back(new medQueryObject("atest12")); vecA.push_back(new medQueryObject("atest13"));
  m_MatrixObjectResult.push_back(vecA);
  std::vector<medQueryObject *> vecB;
  vecB.push_back(new medQueryObject("atest21")); vecB.push_back(new medQueryObject("atest22")); vecB.push_back(new medQueryObject("atest23"));
  m_MatrixObjectResult.push_back(vecB);
  std::vector<medQueryObject *> vecC;
  vecC.push_back(new medQueryObject("atest31")); vecC.push_back(new medQueryObject("atest32")); vecC.push_back(new medQueryObject("atest33"));
  m_MatrixObjectResult.push_back(vecC);
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerSample::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool medResultQueryAbstractHandlerSample::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

// TEST

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	medResultQueryAbstractHandlerSample *rq = new medResultQueryAbstractHandlerSample();
	cppDEL(rq);
}
//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	medResultQueryAbstractHandlerSample rq;
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetResultAsStringMatrix()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsStringMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetColumnsTypeInformationAsStringVector()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsTypeInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetColumnsNameInformationAsStringVector()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsNameInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}


//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsObjectsMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfRecords();
  CPPUNIT_ASSERT(value == 3);
}

//----------------------------------------------------------------------------
void medResultQueryAbstractHandlerTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
  medResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfFields();
  CPPUNIT_ASSERT(value == 3);
}

