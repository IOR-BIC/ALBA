/*=========================================================================

 Program: MAF2
 Module: mafDicomClassesTest
 Authors: Gianluigi Crimi, Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafDicomSliceTest_H
#define CPP_UNIT_mafDicomSliceTest_H

#include "mafTest.h"

class mafDicomSlice;

class mafDicomClassesTest : public mafTest
{

public:
  CPPUNIT_TEST_SUITE( mafDicomClassesTest );

	CPPUNIT_TEST(TestSliceDynamicAllocation);
	CPPUNIT_TEST(TestSliceGetPatientBirthday);
	CPPUNIT_TEST(TestSliceGetPatientName);
	CPPUNIT_TEST(TestSliceGetSliceABSFileName);
	CPPUNIT_TEST(TestSliceGetDcmInstanceNumber);
	CPPUNIT_TEST(TestSliceGetDcmCardiacNumberOfImages);
	CPPUNIT_TEST(TestSliceGetDcmTriggerTime);
	CPPUNIT_TEST(TestSliceGetVTKImageData);
	CPPUNIT_TEST(TestSliceGetDcmImageOrientationPatient);
	CPPUNIT_TEST(TestSliceGetUnrotatedOrigin);
	CPPUNIT_TEST(TestSliceGetDescription);
	CPPUNIT_TEST(TestSliceGetDate);
	CPPUNIT_TEST(TestSliceGetDcmModality);
	CPPUNIT_TEST(TestSliceGetPhotometricInterpretation);
	CPPUNIT_TEST(TestSliceGetSeriesID);
	CPPUNIT_TEST(TestSliceGetStudyID);

  CPPUNIT_TEST_SUITE_END();

protected:
	mafDicomSlice *CreateBaseSlice();

	void TestSliceDynamicAllocation();
	void TestSliceGetPatientBirthday();
	void TestSliceGetPatientName();
	void TestSliceGetSliceABSFileName();
  void TestSliceGetDcmInstanceNumber();
  void TestSliceGetDcmCardiacNumberOfImages();
	void TestSliceGetDcmTriggerTime();
	void TestSliceGetVTKImageData();
  void TestSliceGetDcmImageOrientationPatient();
	void TestSliceGetUnrotatedOrigin();
	void TestSliceGetDescription();
	void TestSliceGetDate();
	void TestSliceGetDcmModality();
	void TestSliceGetPhotometricInterpretation();
	void TestSliceGetSeriesID();
	void TestSliceGetStudyID();

};


#endif
