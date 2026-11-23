/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProgressBarHelperTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaProgressBarHelperTest_H
#define CPP_UNIT_albaProgressBarHelperTest_H

#include "albaTest.h"


class albaProgressBarHelperTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE( albaProgressBarHelperTest );
  CPPUNIT_TEST( CreateDestroyTest );
	CPPUNIT_TEST( InitProgressBarTest );
	CPPUNIT_TEST( CloseProgressBarTest );
	CPPUNIT_TEST( UpdateProgressBarTest );
	CPPUNIT_TEST( GetSetTextModeTest );
	CPPUNIT_TEST( TextModeTest );
	CPPUNIT_TEST( BindToVTKFilterTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void CreateDestroyTest();
	void InitProgressBarTest();
	void CloseProgressBarTest();
	void UpdateProgressBarTest();
	void GetSetTextModeTest();
	void TextModeTest();
	void BindToVTKFilterTest();
};


#endif
