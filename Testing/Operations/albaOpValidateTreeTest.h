/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpValidateTreeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpValidateTreeTest_H__
#define __CPP_UNIT_albaOpValidateTreeTest_H__

#include "albaTest.h"

/**
  Class Name: albaOpValidateTreeTest
  Test class for albaOpValidateTree.
*/
class albaOpValidateTreeTest : public albaTest
{
  /** test suite starting */
  CPPUNIT_TEST_SUITE( albaOpValidateTreeTest );
  /** test OpRun */
  CPPUNIT_TEST( TestOpRun );
  /** test Accept */
  CPPUNIT_TEST( TestAccept );
  /** test Tree Validation with correct tree */
  CPPUNIT_TEST( TestValidateTree_ValidTree );
  /** test Tree Validation with invalid node */
  CPPUNIT_TEST( TestValidateTree_InvalidNode );
  /** test Tree Validation with link not present */
  CPPUNIT_TEST( TestValidateTree_LinkNotPresent );
  /** test Tree Validation with link null */
  CPPUNIT_TEST( TestValidateTree_LinkNull );
  /** test Tree Validation with link binary file not present */
  CPPUNIT_TEST( TestValidateTree_BinaryFileNotPresent );
  /** test Tree Validation with empty url */
  CPPUNIT_TEST( TestValidateTree_UrlEmpty );
  /** test Tree Validation with item not present */
  CPPUNIT_TEST( TestValidateTree_ItemNotPresent ); //deprecated , see function body for explanation
  /** test Tree Validation with not coherent max item id  */
  CPPUNIT_TEST( TestValidateTree_MaxItemIdPatched );
  /** test Tree Validation with archive not present */
  CPPUNIT_TEST( TestValidateTree_ArchiveFileNotPresent );
  
  /** test suite ending  */
  CPPUNIT_TEST_SUITE_END();

  protected:
    /** test OpRun */
    void TestOpRun();
    /** test Accept */
    void TestAccept();
    /** test Tree Validation with correct tree */
    void TestValidateTree_ValidTree();
    /** test Tree Validation with invalid node */
    void TestValidateTree_InvalidNode();
    /** test Tree Validation with link not present */
    void TestValidateTree_LinkNotPresent();
    /** test Tree Validation with link null */
    void TestValidateTree_LinkNull();
    /** test Tree Validation with link binary file not present */
    void TestValidateTree_BinaryFileNotPresent();
    /** test Tree Validation with empty url */
    void TestValidateTree_UrlEmpty();
    /** test Tree Validation with item not present */
    void TestValidateTree_ItemNotPresent();
    /** test Tree Validation with not coherent max item id  */
    void TestValidateTree_MaxItemIdPatched();
    /** test Tree Validation with archive not present */
    void TestValidateTree_ArchiveFileNotPresent();
    
};

#endif
