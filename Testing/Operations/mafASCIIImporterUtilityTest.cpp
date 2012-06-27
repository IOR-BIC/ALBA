/*=========================================================================

 Program: MAF2
 Module: mafASCIIImporterUtilityTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafASCIIImporterUtilityTest.h"
#include "mafASCIIImporterUtility.h"
#include "mafString.h"
#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

#include <iostream>

//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::TestConstructor()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtilityTest *utility = new mafASCIIImporterUtilityTest();
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::GetNumberOfRowsTest()
//----------------------------------------------------------------------------
{
   mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfRows() == 3);
  cppDEL(utility);

}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::GetNumberOfColsTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfCols() == 3);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::GetNumberOfScalarsTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfScalars() == 9);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::GetScalarTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);


  CPPUNIT_ASSERT(utility->GetScalar(1,1) == 5.0);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::GetMatrixTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;

  vnl_matrix<double> m1;

  /* Create a Matrix equals to the one stored in the ASCII file;
  1.0 2.0 3.0
  4.0 5.0 6.0
  7.0 8.0 9.0
  */

  m1.set_size(3,3);
  m1.put(0,0,1.0);
  m1.put(0,1,2.0);
  m1.put(0,2,3.0);
  m1.put(1,0,4.0);
  m1.put(1,1,5.0);
  m1.put(1,2,6.0);
  m1.put(2,0,7.0);
  m1.put(2,1,8.0);
  m1.put(2,2,9.0);


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);

  vnl_matrix<double> m2 = utility->GetMatrix();

  CPPUNIT_ASSERT(m2 == m1);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::ExtractRowTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;

  std::vector<double> v1;
  double vd1[3];

  /* Create a vector equals to the first row of the matrix stored in the ASCII file;
  1.0 2.0 3.0
  */

  v1.push_back(1.0);
  v1.push_back(2.0);
  v1.push_back(3.0);

  vd1[0] = 1.0;
  vd1[1] = 2.0;
  vd1[2] = 3.0;

  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);

  std::vector<double> v2;
  utility->ExtractRow(0,v2);
  double vd2[3];
  utility->ExtractRow(0,vd2);

  CPPUNIT_ASSERT((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));

  CPPUNIT_ASSERT((vd1[0] == vd2[0]) && (vd1[1] == vd2[1]) && (vd1[2] == vd2[2]));
  
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtilityTest::ExtractColumnTest()
//----------------------------------------------------------------------------
{
  mafASCIIImporterUtility *utility = new mafASCIIImporterUtility();
  mafString filename = MAF_DATA_ROOT;

  std::vector<double> v1;
  double vd1[3];

  /* Create a vector equals to the first column of the matrix stored in the ASCII file;
  1.0 4.0 7.0
  */

  v1.push_back(1.0);
  v1.push_back(4.0);
  v1.push_back(7.0);

  vd1[0] = 1.0;
  vd1[1] = 4.0;
  vd1[2] = 7.0;

  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == MAF_OK);

  std::vector<double> v2;
  utility->ExtractColumn(0,v2);
  double vd2[3];
  utility->ExtractColumn(0,vd2);

  CPPUNIT_ASSERT((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));
  CPPUNIT_ASSERT((vd1[0] == vd2[0]) && (vd1[1] == vd2[1]) && (vd1[2] == vd2[2]));
  
  cppDEL(utility);
}
