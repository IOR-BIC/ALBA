/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTransformTest_H__
#define __CPP_UNIT_albaTransformTest_H__

#include "albaTest.h"

/** Test for albaTransform; Use this suite to trace memory problems */
class albaTransformTest : public albaTest
{
  public: 

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( albaTransformTest );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestTranslateRotateScale );
    CPPUNIT_TEST( TestPolarDecomposition );// leaked! Leak source is albaMatrix
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();
    void TestTranslateRotateScale();
    void TestPolarDecomposition();
};


#endif
