/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRemoveCellsTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpRemoveCellsTest_H__
#define __CPP_UNIT_albaOpRemoveCellsTest_H__

#include "albaTest.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"

#include "vtkPolyData.h"

class albaOpRemoveCellsTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( albaOpRemoveCellsTest );
  CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestRemoveCells );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
	void TestConstructor();
  void TestRemoveCells();
	void RenderData( vtkPolyData *data );
  
  albaVMEStorage* m_Storage;
  albaVMERoot* m_Root;
  albaVMESurface *m_Surface;

};

#endif
