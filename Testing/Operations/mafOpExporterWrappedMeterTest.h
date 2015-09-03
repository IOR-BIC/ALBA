/*=========================================================================

 Program: MAF2
 Module: mafOpExporterWrappedMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterWrappedMeterTest_H
#define CPP_UNIT_mafOpExporterWrappedMeterTest_H

#include "mafTest.h"

class mafOpExporterWrappedMeterTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpExporterWrappedMeterTest );
  CPPUNIT_TEST( Test );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
	void meterImplement();
};

#endif
