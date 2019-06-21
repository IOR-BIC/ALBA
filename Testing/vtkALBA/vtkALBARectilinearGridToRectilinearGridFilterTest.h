/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARectilinearGridToRectilinearGridFilterTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_RECTILINEARGRIDTORECTILINEARGRIDFILTERTEST_H__
#define __CPP_UNIT_RECTILINEARGRIDTORECTILINEARGRIDFILTERTEST_H__

#include "albaTest.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"

class vtkALBARectilinearGridToRectilinearGridFilterTest : public albaTest
{
public:

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( vtkALBARectilinearGridToRectilinearGridFilterTest );
  CPPUNIT_TEST(TestGetInput);
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST_SUITE_END();

//protected:
private:
  void TestFixture();
  void TestGetInput();
};

#endif
