/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaResultQueryAbstractHandlerTest
 Authors: Daniele Giunchi
 
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

#include "albaResultQueryAbstractHandlerTest.h"
#include "albaResultQueryAbstractHandler.h"

/**
  Class Name: albaResultQueryAbstractHandlerSample
  sample class for testing interface
*/
class albaResultQueryAbstractHandlerSample : public albaResultQueryAbstractHandler
{
public:
  /** constructor */
  albaResultQueryAbstractHandlerSample();
  /** destructor */
  virtual ~albaResultQueryAbstractHandlerSample(){}; 

  /** RTTI macro */
  albaTypeMacro(albaResultQueryAbstractHandlerSample, albaResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaResultQueryAbstractHandlerSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaResultQueryAbstractHandlerSample::albaResultQueryAbstractHandlerSample()
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

  std::vector<albaQueryObject *> vecA;
  vecA.push_back(new albaQueryObject("atest11")); vecA.push_back(new albaQueryObject("atest12")); vecA.push_back(new albaQueryObject("atest13"));
  m_MatrixObjectResult.push_back(vecA);
  std::vector<albaQueryObject *> vecB;
  vecB.push_back(new albaQueryObject("atest21")); vecB.push_back(new albaQueryObject("atest22")); vecB.push_back(new albaQueryObject("atest23"));
  m_MatrixObjectResult.push_back(vecB);
  std::vector<albaQueryObject *> vecC;
  vecC.push_back(new albaQueryObject("atest31")); vecC.push_back(new albaQueryObject("atest32")); vecC.push_back(new albaQueryObject("atest33"));
  m_MatrixObjectResult.push_back(vecC);
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerSample::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool albaResultQueryAbstractHandlerSample::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaResultQueryAbstractHandlerSample *rq = new albaResultQueryAbstractHandlerSample();
	cppDEL(rq);
}
//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaResultQueryAbstractHandlerSample rq;
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetResultAsStringMatrix()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsStringMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetColumnsTypeInformationAsStringVector()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsTypeInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetColumnsNameInformationAsStringVector()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetColumnsNameInformationAsStringVector().size();
  CPPUNIT_ASSERT(value != 0);
}


//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetResultAsObjectsMatrix().size();
  CPPUNIT_ASSERT(value != 0);
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfRecords();
  CPPUNIT_ASSERT(value == 3);
}

//----------------------------------------------------------------------------
void albaResultQueryAbstractHandlerTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
  albaResultQueryAbstractHandlerSample rq;
  int value = rq.GetNumberOfFields();
  CPPUNIT_ASSERT(value == 3);
}

