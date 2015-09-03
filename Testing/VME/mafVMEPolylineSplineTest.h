/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineSplineTest
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEPolylineSplineTest_H__
#define __CPP_UNIT_mafVMEPolylineSplineTest_H__

#include "mafTest.h"

class vtkPolyData;

class mafVMEPolylineSplineTest : public mafTest
{
	CPPUNIT_TEST_SUITE( mafVMEPolylineSplineTest );
	CPPUNIT_TEST( TestSetData );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestSetData();
	double CalculateLengthSubstitute(vtkPolyData *data);
};

#endif
