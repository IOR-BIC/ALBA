/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVMEDataSetAttributesTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterVMEDataSetAttributesTest_H__
#define __CPP_UNIT_mafOpImporterVMEDataSetAttributesTest_H__

#include "mafTest.h"

class mafOpImporterVMEDataSetAttributesTest : public mafTest
{
  public:
  
  CPPUNIT_TEST_SUITE( mafOpImporterVMEDataSetAttributesTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestImportAttributes );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructor();
  void TestImportAttributes();
	  
};

#endif
