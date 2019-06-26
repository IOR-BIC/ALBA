/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEImageTEST_H__
#define __CPP_UNIT_albaVMEImageTEST_H__

#include "albaTest.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"


class albaVMEImageTest : public albaTest
{

  public:
    // Executed before each test
    void BeforeTest();

    CPPUNIT_TEST_SUITE( albaVMEImageTest );
    CPPUNIT_TEST( TestSetData );
    CPPUNIT_TEST( TestVMEImageVisualization );
    CPPUNIT_TEST( TestTimeVaryingVMEImageTree );
    CPPUNIT_TEST_SUITE_END();

protected:
    void TestSetData();
    void TestVMEImageVisualization() ;
    void TestTimeVaryingVMEImageTree();

    // helper methods 
    void CreateVMETestTree();
    int PlayTree(albaVMERoot *root, bool ignoreVisibleToTraverse);

    // helper variables 
    albaVMERoot *m_VmeRoot;

};

#endif
