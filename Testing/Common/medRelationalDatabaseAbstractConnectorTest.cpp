/*=========================================================================

 Program: MAF2Medical
 Module: medRelationalDatabaseAbstractConnectorTest
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

#include "medRelationalDatabaseAbstractConnectorTest.h"
#include "medRelationalDatabaseAbstractConnector.h"

#include "medResultQueryAbstractHandler.h"

#include "mafString.h"

/**
  Class Name: medRelationalDatabaseAbstractConnectorSample
  sample class for testing interface
*/
class medRelationalDatabaseAbstractConnectorSample : public medRelationalDatabaseAbstractConnector
{
public:
  /** constructor */
  medRelationalDatabaseAbstractConnectorSample();

  /** overloaded constructor */
  medRelationalDatabaseAbstractConnectorSample(const char *queryString);
  /** destructor */
  virtual ~medRelationalDatabaseAbstractConnectorSample(); 

  /** RTTI macro */
  mafTypeMacro(medRelationalDatabaseAbstractConnectorSample, medRelationalDatabaseAbstractConnector);

  /** execute the query */
  /*virtual*/ void QueryExecute();

};

/**
Class Name: medResultQueryAbstractHandlerSampleForConnector
sample class to use with medRelationalDatabaseAbstractConnectorSample
*/
class medResultQueryAbstractHandlerSampleForConnector : public medResultQueryAbstractHandler
{
public:
  /** constructor */
  medResultQueryAbstractHandlerSampleForConnector();

  /** destructor */
  virtual ~medResultQueryAbstractHandlerSampleForConnector(){}; 

  /** RTTI macro */
  mafTypeMacro(medResultQueryAbstractHandlerSampleForConnector, medResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
mafCxxTypeMacro(medRelationalDatabaseAbstractConnectorSample);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
medRelationalDatabaseAbstractConnectorSample::medRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new medResultQueryAbstractHandlerSampleForConnector();
}


//----------------------------------------------------------------------------
medRelationalDatabaseAbstractConnectorSample::medRelationalDatabaseAbstractConnectorSample(const char *queryString): medRelationalDatabaseAbstractConnector(queryString)
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new medResultQueryAbstractHandlerSampleForConnector();
}

//----------------------------------------------------------------------------
medRelationalDatabaseAbstractConnectorSample::~medRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
  delete m_ResultQueryHandler;
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorSample::QueryExecute()
//----------------------------------------------------------------------------
{
  if(m_ResultQueryHandler) {
    delete m_ResultQueryHandler;
  }
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(medResultQueryAbstractHandlerSampleForConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medResultQueryAbstractHandlerSampleForConnector::medResultQueryAbstractHandlerSampleForConnector()
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
void medResultQueryAbstractHandlerSampleForConnector::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool medResultQueryAbstractHandlerSampleForConnector::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

// TEST

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	medRelationalDatabaseAbstractConnectorSample *rdc = new medRelationalDatabaseAbstractConnectorSample();
	cppDEL(rdc);

  medRelationalDatabaseAbstractConnectorSample *rdc2 = new medRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
  cppDEL(rdc2);
}
//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	medRelationalDatabaseAbstractConnectorSample rdc;

  medRelationalDatabaseAbstractConnectorSample rdc2 = medRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestSetGetRelationalDatabaseInformation()
//----------------------------------------------------------------------------
{
    medRelationalDatabaseInformation info;
    info.m_Url = "http://www.testurl.com";
    info.m_Username = "testUser";
    info.m_Password = "testPassword";
    info.m_Certificate = "testCertificate.xml";
    info.m_ConfName = "configurationName";

    medRelationalDatabaseAbstractConnectorSample rdc;
    rdc.SetRelationalDatabaseInformation(info);

    const medRelationalDatabaseInformation compareInfo = rdc.GetRelationalDatabaseInformation();

    CPPUNIT_ASSERT( mafString(info.m_Url.c_str()).Equals(compareInfo.m_Url.c_str()) &&
                    mafString(info.m_Username.c_str()).Equals(compareInfo.m_Username.c_str()) &&
                    mafString(info.m_Password.c_str()).Equals(compareInfo.m_Password.c_str()) &&
                    mafString(info.m_Certificate.c_str()).Equals(compareInfo.m_Certificate.c_str()) &&
                    mafString(info.m_ConfName.c_str()).Equals(compareInfo.m_ConfName.c_str()));
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestSetGetQuery()
//----------------------------------------------------------------------------
{
   medRelationalDatabaseAbstractConnectorSample rdc;
   mafString query = "SELECT something FROM myTable";
   rdc.SetQuery(query);

   CPPUNIT_ASSERT( mafString(query).Equals(rdc.GetQuery()));
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestIsFailed()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.IsFailed();
    CPPUNIT_ASSERT(result == false);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfFields() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsStringMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsStringMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetColumnsTypeAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetColumnsNameAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetColumnsTypeAsStringVector().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    medRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsObjectsMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}