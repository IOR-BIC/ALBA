/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpGarbageCollectMSFDirTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpGarbageCollectMSFDirTest_H__
#define __CPP_UNIT_albaOpGarbageCollectMSFDirTest_H__

#include "albaTest.h"

/**
  Class Name: albaOpGarbageCollectMSFDirTest
  Test class for albaOpGarbageCollectMSFDir.
*/
class albaOpGarbageCollectMSFDirTest : public albaTest
{
  /** test suite starting */
  CPPUNIT_TEST_SUITE( albaOpGarbageCollectMSFDirTest );

  /** test OpRun */
  CPPUNIT_TEST( TestOpRun );

  /** test Tree Validation with correct tree */
  CPPUNIT_TEST( TestGarbageCollect );
  
  /** test suite ending  */
  CPPUNIT_TEST_SUITE_END();

  protected:

    /** test OpRun */
    void TestOpRun();

    /** test Tree Validation with correct tree */
    void TestGarbageCollect();
};

#endif
