/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceSlice_BESTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeSurfaceSlice_BESTest_H__
#define __CPP_UNIT_mafPipeSurfaceSlice_BESTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeSurfaceSlice_BESTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafPipeSurfaceSlice_BESTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSurfacePipeCreation );
  CPPUNIT_TEST( TestSetGetThickness );
  CPPUNIT_TEST( TestSurfacePipeExecution );
  //CPPUNIT_TEST( TestCloudClosePipeExecution );
  //CPPUNIT_TEST( TestCloudOpenPipeExecution );

  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  /** Test the pipe with a mafVMESurface as input */
  void TestSurfacePipeExecution();
  /** Test the pipe with a close mafVMELandamarkCloud as input */
  void TestCloudClosePipeExecution();
  /** Test the pipe with a open mafVMELandamarkCloud as input */
  void TestCloudOpenPipeExecution();
  /** Test pipe creation. */
  void TestSurfacePipeCreation();
  /** Test Set and Get methods for thickness attribute. */
  void TestSetGetThickness();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare a list of already saved images (control images) with renderized image from render window
  @scalarIndex: index X of the image. Original Image is imageX.jpg, created imaged is compX.jpg
  */
  void CompareImages(int scalarIndex = 0);

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif