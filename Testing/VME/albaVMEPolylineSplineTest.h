/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineSplineTest
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEPolylineSplineTest_H__
#define __CPP_UNIT_albaVMEPolylineSplineTest_H__

#include "albaTest.h"

class vtkPolyData;

class albaVMEPolylineSplineTest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaVMEPolylineSplineTest );
	CPPUNIT_TEST( TestSetData );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestSetData();
	double CalculateLengthSubstitute(vtkPolyData *data);
};

#endif
