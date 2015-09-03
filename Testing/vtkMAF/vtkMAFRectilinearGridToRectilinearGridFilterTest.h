/*=========================================================================

 Program: MAF2
 Module: vtkMAFRectilinearGridToRectilinearGridFilterTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_RECTILINEARGRIDTORECTILINEARGRIDFILTERTEST_H__
#define __CPP_UNIT_RECTILINEARGRIDTORECTILINEARGRIDFILTERTEST_H__

#include "mafTest.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"

class vtkMAFRectilinearGridToRectilinearGridFilterTest : public mafTest
{
public:

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( vtkMAFRectilinearGridToRectilinearGridFilterTest );
  CPPUNIT_TEST(TestGetInput);
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST_SUITE_END();

//protected:
private:
  void TestFixture();
  void TestGetInput();
};

#endif
