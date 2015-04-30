/*=========================================================================

 Program: MAF2
 Module: mafOpRemoveCellsTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpRemoveCellsTest_H__
#define __CPP_UNIT_mafOpRemoveCellsTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkPolyData.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

class mafOpRemoveCellsTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafOpRemoveCellsTest );
  CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestRemoveCells );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
	void TestConstructor();
  void TestRemoveCells();
	void RenderData( vtkPolyData *data );
  
  mafVMEStorage* m_Storage;
  mafVMERoot* m_Root;
  mafVMESurface *m_Surface;

};

#endif
