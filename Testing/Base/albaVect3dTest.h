/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVect3dTest
 Authors: Grazia Di Cosmo
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVect3dTest_H__
#define __CPP_UNIT_albaVect3dTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: albaVect3dTest
  Test class for medVect3d
*/
class albaVect3dTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaVect3dTest );

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
