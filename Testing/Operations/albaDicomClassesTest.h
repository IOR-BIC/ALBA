/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDicomClassesTest
 Authors: Gianluigi Crimi, Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaDicomSliceTest_H
#define CPP_UNIT_albaDicomSliceTest_H

#include "albaTest.h"

class albaDicomSlice;

class albaDicomClassesTest : public albaTest
{

public:
  CPPUNIT_TEST_SUITE( albaDicomClassesTest );

	CPPUNIT_TEST(TestSliceDynamicAllocation);
	CPPUNIT_TEST(TestSliceGetPatientBirthday);
	CPPUNIT_TEST(TestSliceGetPatientName);
	CPPUNIT_TEST(TestSliceGetSliceABSFileName);
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

	CPPUNIT_TEST(TestSeriesDynamicAllocation);
	CPPUNIT_TEST(TestSeriesAddSlice);
	CPPUNIT_TEST(TestSeriesGetSlicesNum);
	CPPUNIT_TEST(TestSeriesIsRotated);
	CPPUNIT_TEST(TestSeriesGetSlices);
	CPPUNIT_TEST(TestSeriesGetSerieID);
	CPPUNIT_TEST(TestSeriesGetAcquisitonNumber);
	CPPUNIT_TEST(TestSeriesGetImageType);
	CPPUNIT_TEST(TestSeriesGetDimensions);
	CPPUNIT_TEST(TestSeriesGetCardiacImagesNum);
	CPPUNIT_TEST(TestSeriesSortSlices);

	CPPUNIT_TEST(TestStudyDynamicAllocation); 
	CPPUNIT_TEST(TestStudyAddSlice);
	CPPUNIT_TEST(TestStudyGetStudyID);
	CPPUNIT_TEST(TestStudyGetSeriesNum);
	CPPUNIT_TEST(TestStudyGetSeries);

	CPPUNIT_TEST(TestStudyListDynamicAllocation);
	CPPUNIT_TEST(TestStudyListAddSlice);
	CPPUNIT_TEST(TestStudyListGetStudy);
	CPPUNIT_TEST(TestStudyListGetStudiesNum);
	CPPUNIT_TEST(TestStudyListGetSeriesTotalNum);
	
  CPPUNIT_TEST_SUITE_END();

protected:
	albaDicomSlice *CreateBaseSlice(double *pos=NULL);

	void TestSliceDynamicAllocation();
	void TestSliceGetPatientBirthday();
	void TestSliceGetPatientName();
	void TestSliceGetSliceABSFileName();
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
	
	void TestSeriesDynamicAllocation();
	void TestSeriesAddSlice();
	void TestSeriesGetSlicesNum();
	void TestSeriesIsRotated();
	void TestSeriesGetSlices();
	void TestSeriesGetSerieID();
	void TestSeriesGetAcquisitonNumber();
	void TestSeriesGetImageType();
	void TestSeriesGetDimensions();
	void TestSeriesGetCardiacImagesNum();
	void TestSeriesSortSlices();
	
	void TestStudyDynamicAllocation();
	void TestStudyAddSlice();
	void TestStudyGetStudyID();
	void TestStudyGetSeriesNum();
	void TestStudyGetSeries();

	void TestStudyListDynamicAllocation();
	void TestStudyListAddSlice();
	void TestStudyListGetStudy();
	void TestStudyListGetStudiesNum();
	void TestStudyListGetSeriesTotalNum();
	
};


#endif
