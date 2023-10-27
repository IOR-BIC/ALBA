/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRelationalDatabaseAbstractConnectorTest
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

#include "albaRelationalDatabaseAbstractConnectorTest.h"
#include "albaRelationalDatabaseAbstractConnector.h"

#include "albaResultQueryAbstractHandler.h"

#include "albaString.h"

/**
  Class Name: albaRelationalDatabaseAbstractConnectorSample
  sample class for testing interface
*/
class albaRelationalDatabaseAbstractConnectorSample : public albaRelationalDatabaseAbstractConnector
{
public:
  /** constructor */
  albaRelationalDatabaseAbstractConnectorSample();

  /** overloaded constructor */
  albaRelationalDatabaseAbstractConnectorSample(const char *queryString);
  /** destructor */
  virtual ~albaRelationalDatabaseAbstractConnectorSample(); 

  /** RTTI macro */
  albaTypeMacro(albaRelationalDatabaseAbstractConnectorSample, albaRelationalDatabaseAbstractConnector);

  /** execute the query */
  /*virtual*/ void QueryExecute();

};

/**
Class Name: albaResultQueryAbstractHandlerSampleForConnector
sample class to use with albaRelationalDatabaseAbstractConnectorSample
*/
class albaResultQueryAbstractHandlerSampleForConnector : public albaResultQueryAbstractHandler
{
public:
  /** constructor */
  albaResultQueryAbstractHandlerSampleForConnector();

  /** destructor */
  virtual ~albaResultQueryAbstractHandlerSampleForConnector(){}; 

  /** RTTI macro */
  albaTypeMacro(albaResultQueryAbstractHandlerSampleForConnector, albaResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaRelationalDatabaseAbstractConnectorSample);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
albaRelationalDatabaseAbstractConnectorSample::albaRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new albaResultQueryAbstractHandlerSampleForConnector();
}


//----------------------------------------------------------------------------
albaRelationalDatabaseAbstractConnectorSample::albaRelationalDatabaseAbstractConnectorSample(const char *queryString): albaRelationalDatabaseAbstractConnector(queryString)
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new albaResultQueryAbstractHandlerSampleForConnector();
}

//----------------------------------------------------------------------------
albaRelationalDatabaseAbstractConnectorSample::~albaRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
  delete m_ResultQueryHandler;
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorSample::QueryExecute()
//----------------------------------------------------------------------------
{
  if(m_ResultQueryHandler) {
    delete m_ResultQueryHandler;
  }
}

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaResultQueryAbstractHandlerSampleForConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaResultQueryAbstractHandlerSampleForConnector::albaResultQueryAbstractHandlerSampleForConnector()
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
void albaResultQueryAbstractHandlerSampleForConnector::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool albaResultQueryAbstractHandlerSampleForConnector::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaRelationalDatabaseAbstractConnectorSample *rdc = new albaRelationalDatabaseAbstractConnectorSample();
	cppDEL(rdc);

  albaRelationalDatabaseAbstractConnectorSample *rdc2 = new albaRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
  cppDEL(rdc2);
}
//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaRelationalDatabaseAbstractConnectorSample rdc;

  albaRelationalDatabaseAbstractConnectorSample rdc2 = albaRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestSetGetRelationalDatabaseInformation()
//----------------------------------------------------------------------------
{
    albaRelationalDatabaseInformation info;
    info.m_Url = "http://www.testurl.com";
    info.m_Username = "testUser";
    info.m_Password = "testPassword";
    info.m_Certificate = "testCertificate.xml";
    info.m_ConfName = "configurationName";

    albaRelationalDatabaseAbstractConnectorSample rdc;
    rdc.SetRelationalDatabaseInformation(info);

    const albaRelationalDatabaseInformation compareInfo = rdc.GetRelationalDatabaseInformation();

    CPPUNIT_ASSERT( albaString(info.m_Url.c_str()).Equals(compareInfo.m_Url.c_str()) &&
                    albaString(info.m_Username.c_str()).Equals(compareInfo.m_Username.c_str()) &&
                    albaString(info.m_Password.c_str()).Equals(compareInfo.m_Password.c_str()) &&
                    albaString(info.m_Certificate.c_str()).Equals(compareInfo.m_Certificate.c_str()) &&
                    albaString(info.m_ConfName.c_str()).Equals(compareInfo.m_ConfName.c_str()));
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestSetGetQuery()
//----------------------------------------------------------------------------
{
   albaRelationalDatabaseAbstractConnectorSample rdc;
   albaString query = "SELECT something FROM myTable";
   rdc.SetQuery(query);

   CPPUNIT_ASSERT( albaString(query).Equals(rdc.GetQuery()));
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestIsFailed()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.IsFailed();
    CPPUNIT_ASSERT(result == false);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfFields() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsStringMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsStringMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetColumnsTypeAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetColumnsNameAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetColumnsTypeAsStringVector().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    albaRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsObjectsMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}