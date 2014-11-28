/*=========================================================================

 Program: MAF2
 Module: mafRelationalDatabaseAbstractConnectorTest
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

#include "mafRelationalDatabaseAbstractConnectorTest.h"
#include "mafRelationalDatabaseAbstractConnector.h"

#include "mafResultQueryAbstractHandler.h"

#include "mafString.h"

/**
  Class Name: mafRelationalDatabaseAbstractConnectorSample
  sample class for testing interface
*/
class mafRelationalDatabaseAbstractConnectorSample : public mafRelationalDatabaseAbstractConnector
{
public:
  /** constructor */
  mafRelationalDatabaseAbstractConnectorSample();

  /** overloaded constructor */
  mafRelationalDatabaseAbstractConnectorSample(const char *queryString);
  /** destructor */
  virtual ~mafRelationalDatabaseAbstractConnectorSample(); 

  /** RTTI macro */
  mafTypeMacro(mafRelationalDatabaseAbstractConnectorSample, mafRelationalDatabaseAbstractConnector);

  /** execute the query */
  /*virtual*/ void QueryExecute();

};

/**
Class Name: mafResultQueryAbstractHandlerSampleForConnector
sample class to use with mafRelationalDatabaseAbstractConnectorSample
*/
class mafResultQueryAbstractHandlerSampleForConnector : public mafResultQueryAbstractHandler
{
public:
  /** constructor */
  mafResultQueryAbstractHandlerSampleForConnector();

  /** destructor */
  virtual ~mafResultQueryAbstractHandlerSampleForConnector(){}; 

  /** RTTI macro */
  mafTypeMacro(mafResultQueryAbstractHandlerSampleForConnector, mafResultQueryAbstractHandler);

  /** load result of Query */
  /*virtual void*/ void LoadQueryResult();

  /** load result of Query */
  /*virtual*/ bool IsFailed();

};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafRelationalDatabaseAbstractConnectorSample);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
mafRelationalDatabaseAbstractConnectorSample::mafRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new mafResultQueryAbstractHandlerSampleForConnector();
}


//----------------------------------------------------------------------------
mafRelationalDatabaseAbstractConnectorSample::mafRelationalDatabaseAbstractConnectorSample(const char *queryString): mafRelationalDatabaseAbstractConnector(queryString)
//----------------------------------------------------------------------------
{
    m_ResultQueryHandler = new mafResultQueryAbstractHandlerSampleForConnector();
}

//----------------------------------------------------------------------------
mafRelationalDatabaseAbstractConnectorSample::~mafRelationalDatabaseAbstractConnectorSample()
//----------------------------------------------------------------------------
{
  delete m_ResultQueryHandler;
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorSample::QueryExecute()
//----------------------------------------------------------------------------
{
  if(m_ResultQueryHandler) {
    delete m_ResultQueryHandler;
  }
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafResultQueryAbstractHandlerSampleForConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafResultQueryAbstractHandlerSampleForConnector::mafResultQueryAbstractHandlerSampleForConnector()
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
void mafResultQueryAbstractHandlerSampleForConnector::LoadQueryResult()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
}
//----------------------------------------------------------------------------
bool mafResultQueryAbstractHandlerSampleForConnector::IsFailed()
//----------------------------------------------------------------------------
{
  //only implemented for implement an interface class
  return false;
}

// TEST

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafRelationalDatabaseAbstractConnectorSample *rdc = new mafRelationalDatabaseAbstractConnectorSample();
	cppDEL(rdc);

  mafRelationalDatabaseAbstractConnectorSample *rdc2 = new mafRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
  cppDEL(rdc2);
}
//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafRelationalDatabaseAbstractConnectorSample rdc;

  mafRelationalDatabaseAbstractConnectorSample rdc2 = mafRelationalDatabaseAbstractConnectorSample("SELECT * FROM table");
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestSetGetRelationalDatabaseInformation()
//----------------------------------------------------------------------------
{
    mafRelationalDatabaseInformation info;
    info.m_Url = "http://www.testurl.com";
    info.m_Username = "testUser";
    info.m_Password = "testPassword";
    info.m_Certificate = "testCertificate.xml";
    info.m_ConfName = "configurationName";

    mafRelationalDatabaseAbstractConnectorSample rdc;
    rdc.SetRelationalDatabaseInformation(info);

    const mafRelationalDatabaseInformation compareInfo = rdc.GetRelationalDatabaseInformation();

    CPPUNIT_ASSERT( mafString(info.m_Url.c_str()).Equals(compareInfo.m_Url.c_str()) &&
                    mafString(info.m_Username.c_str()).Equals(compareInfo.m_Username.c_str()) &&
                    mafString(info.m_Password.c_str()).Equals(compareInfo.m_Password.c_str()) &&
                    mafString(info.m_Certificate.c_str()).Equals(compareInfo.m_Certificate.c_str()) &&
                    mafString(info.m_ConfName.c_str()).Equals(compareInfo.m_ConfName.c_str()));
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestSetGetQuery()
//----------------------------------------------------------------------------
{
   mafRelationalDatabaseAbstractConnectorSample rdc;
   mafString query = "SELECT something FROM myTable";
   rdc.SetQuery(query);

   CPPUNIT_ASSERT( mafString(query).Equals(rdc.GetQuery()));
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestIsFailed()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.IsFailed();
    CPPUNIT_ASSERT(result == false);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetNumberOfRecords()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetNumberOfFields()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfFields() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsStringMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsStringMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetColumnsTypeAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetNumberOfRecords() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetColumnsNameAsStringVector()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetColumnsTypeAsStringVector().size() != 0;
    CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnectorTest::TestGetQueryResultAsObjectsMatrix()
//----------------------------------------------------------------------------
{
    bool result = false;
    mafRelationalDatabaseAbstractConnectorSample rdc;
    result = rdc.GetQueryResultAsObjectsMatrix().size() != 0;
    CPPUNIT_ASSERT(result);
}