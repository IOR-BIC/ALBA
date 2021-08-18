/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaASCIIImporterUtilityTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaASCIIImporterUtilityTest.h"
#include "albaASCIIImporterUtility.h"
#include "albaString.h"
#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

#include <iostream>

//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::TestConstructor()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtilityTest *utility = new albaASCIIImporterUtilityTest();
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::GetNumberOfRowsTest()
//----------------------------------------------------------------------------
{
   albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfRows() == 3);
  cppDEL(utility);

}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::GetNumberOfColsTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfCols() == 3);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::GetNumberOfScalarsTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);


  CPPUNIT_ASSERT(utility->GetNumberOfScalars() == 9);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::GetScalarTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);


  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);


  CPPUNIT_ASSERT(utility->GetScalar(1,1) == 5.0);
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::GetMatrixTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;

	/* Create a Matrix equals to the one stored in the ASCII file;
	1.0 2.0 3.0
	4.0 5.0 6.0
	7.0 8.0 9.0
	*/

	double m1[3][3] = { {1,2,3},{4,5,6},{7,8,9} };
	double row[3];


  filename<<"/Test_ASCIIImporterUtility/matrix_01.txt";
  utility->ReadFile(filename);
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);

	for (int i = 0; i < 3; i++)
	{
		utility->ExtractRow(i, row);
		for (int j = 0; j < 3; j++)
			CPPUNIT_ASSERT(row[j] == m1[i][j]);
	}

	cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::ExtractRowTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;

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
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);

  std::vector<double> v2;
  utility->ExtractRow(0,v2);
  double vd2[3];
  utility->ExtractRow(0,vd2);

  CPPUNIT_ASSERT((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));

  CPPUNIT_ASSERT((vd1[0] == vd2[0]) && (vd1[1] == vd2[1]) && (vd1[2] == vd2[2]));
  
  cppDEL(utility);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtilityTest::ExtractColumnTest()
//----------------------------------------------------------------------------
{
  albaASCIIImporterUtility *utility = new albaASCIIImporterUtility();
  albaString filename = ALBA_DATA_ROOT;

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
  
 
  CPPUNIT_ASSERT(utility->ReadFile(filename) == ALBA_OK);

  std::vector<double> v2;
  utility->ExtractColumn(0,v2);
  double vd2[3];
  utility->ExtractColumn(0,vd2);

  CPPUNIT_ASSERT((v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]));
  CPPUNIT_ASSERT((vd1[0] == vd2[0]) && (vd1[1] == vd2[1]) && (vd1[2] == vd2[2]));
  
  cppDEL(utility);
}
