/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceTest
 Authors: Matteo Giacomoni, Stefano Perticoni, Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMESURFACETEST_H__
#define __CPP_UNIT_ALBAVMESURFACETEST_H__

#include "albaTest.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"


class albaVMESurfaceTest : public albaTest
{

  public:
    // Executed before each test
    void BeforeTest();

    CPPUNIT_TEST_SUITE( albaVMESurfaceTest );
    CPPUNIT_TEST( TestSetData );
    CPPUNIT_TEST( TestVMESurfaceVisualization );
    CPPUNIT_TEST( TestTimeVaryingVMESurfaceTree );
    CPPUNIT_TEST_SUITE_END();

protected:
    void TestSetData();
    void TestVMESurfaceVisualization() ;
    void TestTimeVaryingVMESurfaceTree();

    // helper methods 
    void CreateVMETestTree();
    int PlayTree(albaVMERoot *root, bool ignoreVisibleToTraverse);

    // helper variables 
    albaVMERoot *m_VmeRoot;

};

#endif
