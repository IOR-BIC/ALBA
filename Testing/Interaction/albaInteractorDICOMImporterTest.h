/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorDICOMImporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiDICOMImporterInteractorTest_H__
#define __CPP_UNIT_mmiDICOMImporterInteractorTest_H__

#include "albaTest.h"

class albaInteractorDICOMImporterTest : public albaTest
{
  public:

	CPPUNIT_TEST_SUITE( albaInteractorDICOMImporterTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestStartInteraction );
	CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST( TestOnMouseMove );
	CPPUNIT_TEST( TestOnLeftButtonDown );
	CPPUNIT_TEST( TestOnLeftButtonUp );
	CPPUNIT_TEST_SUITE_END();
  
  protected:
    
	void TestFixture();
	void TestStartInteraction();
	void TestConstructorDestructor();
	void TestOnMouseMove();
	void TestOnLeftButtonDown();
	void TestOnLeftButtonUp();
};

#endif
