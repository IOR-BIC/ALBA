/*=========================================================================

 Program: MAF2
 Module: mafProgressBarHelperTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafProgressBarHelperTest_H
#define CPP_UNIT_mafProgressBarHelperTest_H

#include "mafTest.h"
#include "mafObserver.h"


class mafProgressBarHelperTest : public mafObserver, public mafTest
{
public:
  CPPUNIT_TEST_SUITE( mafProgressBarHelperTest );
  CPPUNIT_TEST( CreateDestroyTest );
	CPPUNIT_TEST( InitProgressBarTest );
	CPPUNIT_TEST( CloseProgressBarTest );
	CPPUNIT_TEST( UpdateProgressBarTest );
	CPPUNIT_TEST( GetSetTextModeTest );
	CPPUNIT_TEST( TextModeTest );
  CPPUNIT_TEST_SUITE_END();

	/** Test specific stuff executed before each test */
	virtual void BeforeTest();

protected:
  void CreateDestroyTest();
	void InitProgressBarTest();
	void CloseProgressBarTest();
	void UpdateProgressBarTest();
	void GetSetTextModeTest();
	void TextModeTest();
		
	virtual void OnEvent(mafEventBase *maf_event);

	int m_UpdateEventNum;
	int m_InitEventNum;
	int m_CloseEventNum;
};


#endif