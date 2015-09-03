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

#include "mafTest.h"

class vtkRenderer;
class vtkRenderWindow;

class mafAvatar3DConeTest : public mafTest
{
  public:
  
    /** Executed before each test */
    void BeforeTest();

    /** Executed after each test */
    void AfterTest();

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
