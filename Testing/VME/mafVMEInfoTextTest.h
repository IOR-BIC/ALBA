/*=========================================================================

 Program: MAF2
 Module: mafVMEInfoTextTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEInfoTextTEST_H__
#define __CPP_UNIT_mafVMEInfoTextTEST_H__

#include "mafTest.h"

class vtkPolyData;

class mafVMEInfoTextTest : public mafTest
{
public :

	CPPUNIT_TEST_SUITE( mafVMEInfoTextTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestGetVTKOutput );
  CPPUNIT_TEST( TestGetOutput );
  CPPUNIT_TEST( TestAddString );
  CPPUNIT_TEST( TestGetSetPosShow );
  CPPUNIT_TEST( TestGetSetPosLabel );
  CPPUNIT_TEST( TestSetTimeStamp );
  CPPUNIT_TEST( TestGetIcon );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestEquals );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
	void TestGetVTKOutput();
  void TestGetOutput();
  void TestAddString();
  void TestGetSetPosShow();
  void TestGetSetPosLabel();
  void TestSetTimeStamp();
  void TestGetIcon();
  void TestDeepCopy();
  void TestEquals();

  bool result;
};

#endif
