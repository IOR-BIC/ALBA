/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPickHelperTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPickHelperTest_H__
#define __CPP_UNIT_albaPickHelperTest_H__

#include "albaTest.h"

class vtkActor;

class albaPickHelperTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaPickHelperTest );
  CPPUNIT_TEST( TestPickCellOnVME );
  CPPUNIT_TEST( TestPickCellMiss );
  CPPUNIT_TEST( TestPickCellPropWithoutVME );
  CPPUNIT_TEST( TestPickCellStopsAtFirstRenderer );
  CPPUNIT_TEST( TestGetPointScalar );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestPickCellOnVME();
  void TestPickCellMiss();
  void TestPickCellPropWithoutVME();
  void TestPickCellStopsAtFirstRenderer();
  void TestGetPointScalar();

  /** Return a new actor showing a sphere of radius 10 centered in the origin */
  vtkActor *NewSphereActor();
};

#endif
