/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeSlice_BESTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeVolumeSlice_BESTest_H__
#define __CPP_UNIT_mafPipeVolumeSlice_BESTest_H__

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

class mafPipeVolumeSlice_BESTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafPipeVolumeSlice_BESTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST( TestPipeExecution_SetSliceOpacity );
  CPPUNIT_TEST( TestPipeExecution_SetLutRange );
  CPPUNIT_TEST( TestPipeExecution_SetColorLookupTable );
  CPPUNIT_TEST( TestPipeExecution_TicksOnOff );


  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestPipeExecution();
  void TestPipeExecution_SetSliceOpacity();
  void TestPipeExecution_SetLutRange();
  void TestPipeExecution_SetColorLookupTable();
  void TestPipeExecution_TicksOnOff();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare a list of already saved images (control images) with renderized image from render window
  @imageIndex: index X of the image. Original Image is imageX.jpg, created imaged is compX.jpg
  */
  void CompareImages(int imageIndex);
};

#endif