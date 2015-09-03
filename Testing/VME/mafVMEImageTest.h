/*=========================================================================

 Program: MAF2
 Module: mafVMEImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEImageTEST_H__
#define __CPP_UNIT_mafVMEImageTEST_H__

#include "mafTest.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"


class mafVMEImageTest : public mafTest
{

  public:
    // Executed before each test
    void BeforeTest();

    CPPUNIT_TEST_SUITE( mafVMEImageTest );
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
    int PlayTree(mafVMERoot *root, bool ignoreVisibleToTraverse);

    // helper variables 
    mafVMERoot *m_VmeRoot;

};

#endif
