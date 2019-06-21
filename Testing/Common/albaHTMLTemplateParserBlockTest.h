/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaHTMLTemplateParserBlockTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaHTMLTemplateParserBlockTest_H__
#define __CPP_UNIT_albaHTMLTemplateParserBlockTest_H__

#include "albaTest.h"

class albaHTMLTemplateParserBlockTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaHTMLTemplateParserBlockTest );
  CPPUNIT_TEST( TestConstructorDestuctor );
  CPPUNIT_TEST( TestAddGetVar );
  CPPUNIT_TEST( TestPushGetNthVar );
  CPPUNIT_TEST( TestAddGetBlock );
  CPPUNIT_TEST( TestPushGetNthBlock );
  CPPUNIT_TEST( TestSetGetIfCondition );
  CPPUNIT_TEST( TestSetGetNloop );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestConstructorDestuctor();
    void TestAddGetVar();
    void TestPushGetNthVar();
    void TestAddGetBlock();
    void TestPushGetNthBlock();
    void TestSetGetIfCondition();
    void TestSetGetNloop();
     
};


#endif
