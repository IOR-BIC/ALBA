/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMeshTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpExporterMeshTest_H__
#define __CPP_UNIT_mafOpExporterMeshTest_H__

#include "mafTest.h"

class mafOpExporterMeshTest : public mafTest
{
  public:
 
  CPPUNIT_TEST_SUITE( mafOpExporterMeshTest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestExporterMesh );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestConstructor();
  void TestExporterMesh();
	  
};

#endif
