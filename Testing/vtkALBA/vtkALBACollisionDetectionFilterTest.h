/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACollisionDetectionFilterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBACollisionDetectionFilterTEST_H__
#define __CPP_UNIT_vtkALBACollisionDetectionFilterTEST_H__

#include "albaTest.h"

class vtkActor;
class vtkRenderWindowInteractor;
class vtkProperty;

class vtkALBACollisionDetectionFilterTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkALBACollisionDetectionFilterTest );
  /*CPPUNIT_TEST( TestDynamicAllocation );*/
  CPPUNIT_TEST( Test );
  /*CPPUNIT_TEST( TestChangingMatrix );*/
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void Test();
  void TestChangingMatrix();

  void Visualize(vtkActor *actor);
  void AddPolydataToVisualize(vtkPolyData *data, vtkProperty *property = NULL);
};

#endif