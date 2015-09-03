/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldSurfaceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeVectorFieldSurfaceTest_H__
#define __CPP_UNIT_mafPipeVectorFieldSurfaceTest_H__

#include "mafTest.h"

class vtkRenderWindow;

class mafPipeVectorFieldSurfaceTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafPipeVectorFieldSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:
  /**Compare an already saved image (control image) with renderized image from render window
  */
  void CompareImage();

  void TestFixture();
  void TestCreate();

  vtkRenderWindow *m_RenderWindow;
};

#endif
