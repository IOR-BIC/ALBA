/*=========================================================================

 Program: MAF2
 Module: mafAxesTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAxesTest_H__
#define __CPP_UNIT_mafAxesTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkRenderer.h"
#include "mafVMESurface.h"

class mafAxesTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafAxesTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructorDestructor );
    CPPUNIT_TEST( TestSetVisibility);
    CPPUNIT_TEST( TestSetPose );
    CPPUNIT_TEST( TestRenderData );
    CPPUNIT_TEST_SUITE_END();

  private:

    void TestConstructorDestructor();
    void TestFixture();
    void TestRenderData();
  	void TestSetVisibility();
    void TestSetPose();

    void RenderData(vtkDataSet *data);
    
    mafVMESurface *m_VMESurfaceSphere;
    vtkRenderer *m_Renderer;
};




#endif
