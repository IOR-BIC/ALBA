/*=========================================================================

 Program: MAF2
 Module: vtkMAFAssemblyTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFAssemblyTest_H__
#define __CPP_UNIT_vtkMAFAssemblyTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkRenderer;
class vtkRenderWindow;

class vtkMAFAssemblyTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkMAFAssemblyTest );

  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( PrintSelfTest );
  CPPUNIT_TEST( SetGetVmeTest );
  CPPUNIT_TEST( AddRemovePartTest );
  CPPUNIT_TEST( GetPartsTest );
  CPPUNIT_TEST( GetActorsTest );
  CPPUNIT_TEST( GetVolumesTest );
  CPPUNIT_TEST( RenderOpaqueGeometryTest );
  CPPUNIT_TEST( RenderTranslucentGeometry );
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
  void RenderTranslucentGeometry();
  void InitPathTraversalTest();
  void GetNextPathTest();
  void GetNumberOfPathsTest();
  void GetBoundsTest();
  void GetMTimeTest();
  void ShallowCopyTest();
  void BuildPathsTest(); //?

  void PrepareToRender(vtkRenderer *renderer, vtkRenderWindow *render_window);
  void CompareImages(vtkRenderWindow * renwin, int indexTest);
};




#endif // #ifndef __CPP_UNIT_vtkMAFAssemblyTest_H__