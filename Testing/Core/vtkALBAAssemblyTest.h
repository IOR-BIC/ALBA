/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAAssemblyTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAAssemblyTest_H__
#define __CPP_UNIT_vtkALBAAssemblyTest_H__

#include "albaTest.h"

class vtkALBAAssemblyTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( vtkALBAAssemblyTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( PrintSelfTest );
  CPPUNIT_TEST( SetGetVmeTest );
  CPPUNIT_TEST( AddRemovePartTest );
  CPPUNIT_TEST( GetPartsTest );
  CPPUNIT_TEST( GetActorsTest );
  CPPUNIT_TEST( GetVolumesTest );
  CPPUNIT_TEST( RenderOpaqueGeometryTest );
  CPPUNIT_TEST( RenderTranslucentPolygonalGeometryTest );
  CPPUNIT_TEST( InitPathTraversalTest );
  CPPUNIT_TEST( GetNextPathTest );
  CPPUNIT_TEST( GetNumberOfPathsTest );
  CPPUNIT_TEST( GetBoundsTest );
  CPPUNIT_TEST( GetMTimeTest );
  CPPUNIT_TEST( ShallowCopyTest );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void DynamicAllocationTest();
  void PrintSelfTest();
  void SetGetVmeTest();
  void AddRemovePartTest();
  void GetPartsTest();
  void GetActorsTest();
  void GetVolumesTest();
  void RenderOpaqueGeometryTest();
  void RenderTranslucentPolygonalGeometryTest();
  void InitPathTraversalTest();
  void GetNextPathTest();
  void GetNumberOfPathsTest();
  void GetBoundsTest();
  void GetMTimeTest();
  void ShallowCopyTest();
  void BuildPathsTest(); //?

};

#endif // #ifndef __CPP_UNIT_vtkALBAAssemblyTest_H__