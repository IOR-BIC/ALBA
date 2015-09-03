/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMeshTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterMeshTest_H__
#define __CPP_UNIT_mafOpImporterMeshTest_H__

#include "mafTest.h"

class mafOpImporterMeshTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafOpImporterMeshTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestImportMesh );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructor();
  void TestImportMesh();
	  
};

#endif
