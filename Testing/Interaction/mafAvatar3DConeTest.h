/*=========================================================================

 Program: MAF2
 Module: mafAvatar3DConeTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAvatar3DConeTest_H__
#define __CPP_UNIT_mafAvatar3DConeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkRenderer;
class vtkRenderWindow;

class mafAvatar3DConeTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    /** CPPUNIT fixture: executed before each test */
    void setUp();

    /** CPPUNIT fixture: executed after each test */
    void tearDown();

    CPPUNIT_TEST_SUITE( mafAvatar3DConeTest );
    
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestPick);

    CPPUNIT_TEST_SUITE_END();
    
  protected:
    void TestFixture();
    void TestConstructorDestructor();
    void TestPick();
    void CompareImages();

    vtkRenderer *m_Renderer;
    vtkRenderWindow *m_RenderWindow;
};

#endif
