/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaHTMLTemplateParserTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaHTMLTemplateParserTest_H__
#define __CPP_UNIT_albaHTMLTemplateParserTest_H__

#include "albaTest.h"

class albaHTMLTemplateParserTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaHTMLTemplateParserTest );
  CPPUNIT_TEST( TestConstructorDestuctor );
  CPPUNIT_TEST( TestSetWriteTemplateFile );
  CPPUNIT_TEST( TestSetGetTemplateString );
  CPPUNIT_TEST( TestParse );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestConstructorDestuctor();
    void TestSetWriteTemplateFile();
    void TestSetGetTemplateString();
    void TestParse();
};



#endif
