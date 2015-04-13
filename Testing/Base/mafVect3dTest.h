/*=========================================================================

 Program: MAF2
 Module: mafVect3dTest
 Authors: Grazia Di Cosmo
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVect3dTest_H__
#define __CPP_UNIT_mafVect3dTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: mafVect3dTest
  Test class for medVect3d
*/
class mafVect3dTest : public CPPUNIT_NS::TestFixture
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafVect3dTest );

  /** macro for test TestSetGetVector */
	CPPUNIT_TEST( TestSetGetVector );

  /** macro for test TestOperation */
	CPPUNIT_TEST( TestOperators );

  /** macro for test TestOperation2Vector */
  CPPUNIT_TEST( TestOperation2Vector );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Set and Get vector */
  void TestSetGetVector();
  /** Test arithmetical operators between vector */
	void TestOperators();
  /** Test Operation between vectors */
  void TestOperation2Vector();
};


#endif
