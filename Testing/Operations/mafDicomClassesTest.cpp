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

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafDicomClassesTest.h"

#include "mafOpImporterDicomOffis.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReader.h"
#include "vtkDataSetReader.h"


#define D_SliceABSFileName "DicomSlice.dcm"
#define D_DcmInstanceNumber 11
#define D_DcmCardiacNumberOfImages 18
#define D_DcmTriggerTime 120.5
#define D_DcmDescription "DESCR"
#define D_DcmDate "1-1-2000"
#define D_DcmPatientName "Name"
#define D_DcmBirthDate "1-1-1900"
#define D_DcmStudyID "Id-Study-1"
#define D_NewDcmStudyID "Id-Study-2"
#define D_DcmSeriesID "Id-Series-1"
#define D_NewDcmSeriesID "Id-Series-2"
#define D_Position { 0.0, 1.0, 2.0 }
#define D_SliceSize { 256, 256 }
#define D_NewPosition { 0.0, 2.0, 1.0 }
#define D_Orientation { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 }
#define D_NewOrientation { 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 }
#define D_NewNum 7

///////////////////////////////mafDicomSlice//////////////////////////////////
//----------------------------------------------------------------------------
mafDicomSlice * mafDicomClassesTest::CreateBaseSlice(double *pos)
{
	//this will be deleted from slice destructor
	double patientPos[3] = D_Position;
	int sliceSize[2] = D_SliceSize;

	mafString fileName = MAF_DATA_ROOT;
	fileName << "/Dicom/" << D_SliceABSFileName;


	mafDicomSlice *sliceDicom;
	double patientOri[6] = D_Orientation;
	if (pos != NULL)
		sliceDicom = new mafDicomSlice(fileName, patientOri, pos, D_DcmDescription,D_DcmDate, D_DcmPatientName,D_DcmBirthDate, D_DcmCardiacNumberOfImages,D_DcmTriggerTime);
	else 
		sliceDicom = new mafDicomSlice(fileName, patientOri, patientPos, D_DcmDescription, D_DcmDate, D_DcmPatientName, D_DcmBirthDate, D_DcmCardiacNumberOfImages, D_DcmTriggerTime);

	sliceDicom->SetSliceSize(sliceSize);

	return sliceDicom;
}

//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceDynamicAllocation()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
  cppDEL(sliceDicom);
}


//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetPatientBirthday()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPatientBirthday(), D_DcmBirthDate) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetPatientName()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPatientName(), D_DcmPatientName) == 0);

	cppDEL(sliceDicom);
}

//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetSliceABSFileName()
{
  mafDicomSlice *sliceDicom = CreateBaseSlice();

	mafString fileName = MAF_DATA_ROOT;
	fileName << "/Dicom/" << D_SliceABSFileName;

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(), fileName.GetCStr()) == 0);

	sliceDicom->SetSliceABSFileName("newName");
  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),"newName") == 0);

  cppDEL(sliceDicom);
}

//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmCardiacNumberOfImages()
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = CreateBaseSlice();

  CPPUNIT_ASSERT(sliceDicom->GetNumberOfCardiacImages() == D_DcmCardiacNumberOfImages);

	sliceDicom->SetNumberOfCardiacImages(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetNumberOfCardiacImages() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmTriggerTime()
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = CreateBaseSlice();
  
  CPPUNIT_ASSERT(sliceDicom->GetTriggerTime() == D_DcmTriggerTime);

	sliceDicom->SetTriggerTime(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetTriggerTime() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetVTKImageData()
{
	//create imagedata 
	vtkMAFSmartPointer<vtkDataSetReader> reader;
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/Dicom/DicomSlice.vtk";
  reader->SetFileName(fileName.GetCStr());
  reader->Update();
	
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	
	vtkImageData* sliceImageData = sliceDicom->GetNewVTKImageData();
	vtkImageData* readerOutput = vtkImageData::SafeDownCast(reader->GetOutput());
	readerOutput->Update();
	
	double *sliceSpacing = sliceImageData->GetSpacing();
	double *readerSpacing = readerOutput->GetSpacing();
	CPPUNIT_ASSERT(sliceSpacing[0] == readerSpacing[0] && sliceSpacing[1] == readerSpacing[1] && sliceSpacing[2] == readerSpacing[2]);

	int *sliceDims = sliceImageData->GetDimensions();
	int *readerDims = readerOutput->GetDimensions();
	CPPUNIT_ASSERT(sliceDims[0] == readerDims[0] && sliceDims[1] == readerDims[1] && sliceDims[2] == readerDims[2]);

	double *sliceSR = sliceImageData->GetScalarRange();
	double *readerSR = readerOutput->GetScalarRange();
	CPPUNIT_ASSERT(sliceSR[0] == readerSR[0] && sliceSR[1] == readerSR[1]);

	vtkDEL(sliceImageData);
  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmImageOrientationPatient()
{
	double patientOri[6] = D_Orientation;

	mafDicomSlice *sliceDicom = CreateBaseSlice();

  double testOri[6];
  sliceDicom->GetDcmImageOrientationPatient(testOri);
  CPPUNIT_ASSERT(testOri[0] == patientOri[0]);
  CPPUNIT_ASSERT(testOri[1] == patientOri[1]);
  CPPUNIT_ASSERT(testOri[2] == patientOri[2]);
  CPPUNIT_ASSERT(testOri[3] == patientOri[3]);
  CPPUNIT_ASSERT(testOri[4] == patientOri[4]);
  CPPUNIT_ASSERT(testOri[5] == patientOri[5]);

	double patientNewOri[6] = D_NewOrientation;
	sliceDicom->SetDcmImageOrientationPatient(patientNewOri);
	sliceDicom->GetDcmImageOrientationPatient(testOri);
	CPPUNIT_ASSERT(testOri[0] == patientNewOri[0]);
	CPPUNIT_ASSERT(testOri[1] == patientNewOri[1]);
	CPPUNIT_ASSERT(testOri[2] == patientNewOri[2]);
	CPPUNIT_ASSERT(testOri[3] == patientNewOri[3]);
	CPPUNIT_ASSERT(testOri[4] == patientNewOri[4]);
	CPPUNIT_ASSERT(testOri[5] == patientNewOri[5]);
	
  cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetUnrotatedOrigin()
{
	double patientPos[3] = D_Position;

	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	double *testPos;
	testPos=sliceDicom->GetUnrotatedOrigin();
	CPPUNIT_ASSERT(testPos[0] == patientPos[0]);
	CPPUNIT_ASSERT(testPos[1] == patientPos[1]);
	CPPUNIT_ASSERT(testPos[2] == patientPos[2]);

	double patientNewOri[6] = D_NewOrientation;
	sliceDicom->SetDcmImageOrientationPatient(patientNewOri);

	//Test new value after Orientation change
	testPos = sliceDicom->GetUnrotatedOrigin();
	CPPUNIT_ASSERT(testPos[0] == patientPos[1]);
	CPPUNIT_ASSERT(testPos[1] == patientPos[0]);
	CPPUNIT_ASSERT(testPos[2] == -patientPos[2]);
	
	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDescription()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetDescription(), D_DcmDescription) == 0);
	
	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDate()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetDate(), D_DcmDate) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmModality()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetModality("Modality");

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetModality(), "Modality") == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetPhotometricInterpretation()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetPhotometricInterpretation("PhotoMetric");

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPhotometricInterpretation(), "PhotoMetric") == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetSeriesID()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetSeriesID(D_DcmSeriesID);

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetSeriesID(), D_DcmSeriesID) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSliceGetStudyID()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetStudyID(D_DcmStudyID);

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetStudyID(), D_DcmStudyID) == 0);

	cppDEL(sliceDicom);
}

///////////////////////////////mafDicomSeries//////////////////////////////////
//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesDynamicAllocation()
{
	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesAddSlice()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);

	series->AddSlice(sliceDicom);

	CPPUNIT_ASSERT(series->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(series->GetSlice(0) == sliceDicom);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesGetSlicesNum()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);

	series->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(series->GetSlicesNum() == 1);

	series->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(series->GetSlicesNum() == 2);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesIsRotated()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	double patientNewOri[6] = D_NewOrientation;
	sliceDicom2->SetDcmImageOrientationPatient(patientNewOri);

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);

	series->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(series->IsRotated() == false);

	series->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(series->IsRotated() == true);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesGetSlices()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	
	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	series->AddSlice(sliceDicom);
	series->AddSlice(sliceDicom2);

	std::vector<mafDicomSlice *> slices = series->GetSlices();

	CPPUNIT_ASSERT(slices.size() == 2);
	CPPUNIT_ASSERT(slices[0] == sliceDicom);
	CPPUNIT_ASSERT(slices[1] == sliceDicom2);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesGetSerieID()
{
	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	
	CPPUNIT_ASSERT(series->GetSerieID() == D_DcmSeriesID);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesGetDimensions()
{
	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	series->AddSlice(sliceDicom);

	const int *dims=series->GetDimensions();

	CPPUNIT_ASSERT(dims[0] == 256);
	CPPUNIT_ASSERT(dims[1] == 256);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesGetCardiacImagesNum()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	series->AddSlice(sliceDicom);

	CPPUNIT_ASSERT(series->GetCardiacImagesNum() == D_DcmCardiacNumberOfImages);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestSeriesSortSlices()
{
	double newPos[3] = D_NewPosition;
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice(newPos);

	mafDicomSeries *series = new mafDicomSeries(D_DcmSeriesID);
	series->AddSlice(sliceDicom);
	series->AddSlice(sliceDicom2);

	series->SortSlices();

	CPPUNIT_ASSERT(series->GetSlice(0) == sliceDicom2);
	CPPUNIT_ASSERT(series->GetSlice(1) == sliceDicom);

	//Test sort slice with a different orientation.
	double patientNewOri[6] = D_NewOrientation;
	sliceDicom->SetDcmImageOrientationPatient(patientNewOri);
	sliceDicom2->SetDcmImageOrientationPatient(patientNewOri);

	series->SortSlices();

	CPPUNIT_ASSERT(series->GetSlice(0) == sliceDicom);
	CPPUNIT_ASSERT(series->GetSlice(1) == sliceDicom2);
	
	cppDEL(series);
}

///////////////////////////////mafDicomStudy//////////////////////////////////
//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyDynamicAllocation()
{
	mafDicomStudy *study = new mafDicomStudy(D_DcmStudyID);
	cppDEL(study);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyAddSlice()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);

	mafDicomStudy *study = new mafDicomStudy(D_DcmStudyID);

	study->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(0) == sliceDicom);

	study->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlice(0) == sliceDicom2);

	study->AddSlice(sliceDicom3);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(1) == sliceDicom3);

	cppDEL(study);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyGetStudyID()
{
	mafDicomStudy *study = new mafDicomStudy(D_DcmStudyID);
	
	CPPUNIT_ASSERT(study->GetStudyID() == D_DcmStudyID);
	
	cppDEL(study);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyGetSeriesNum()
{
	double newPos[3] = D_NewPosition;
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	mafDicomStudy *study = new mafDicomStudy(D_DcmStudyID);

	study->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 1);
	study->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
	study->AddSlice(sliceDicom3);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyGetSeries()
{
	double newPos[3] = D_NewPosition;
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	mafDicomStudy *study = new mafDicomStudy(D_DcmStudyID);

	study->AddSlice(sliceDicom);
	study->AddSlice(sliceDicom2);
	study->AddSlice(sliceDicom3);
	
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(0) == sliceDicom);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(1) == sliceDicom3);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlice(0) == sliceDicom2);

	cppDEL(study);
}

/////////////////////////////mafDicomStudyList////////////////////////////////
//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyListDynamicAllocation()
{
	mafDicomStudyList *studyList = new mafDicomStudyList();
	cppDEL(studyList);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyListAddSlice()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();
	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);
	
	mafDicomStudyList *studyList = new mafDicomStudyList();

	studyList->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 1);
	
	studyList->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 2);

	studyList->AddSlice(sliceDicom3);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 3);
	
	cppDEL(studyList);

}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyListGetStudy()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();
	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	mafDicomStudyList *studyList = new mafDicomStudyList();
	studyList->AddSlice(sliceDicom);
	studyList->AddSlice(sliceDicom2);
	studyList->AddSlice(sliceDicom3);

	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeriesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeries(0)->GetSlice(0) == sliceDicom);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeries(1)->GetSlice(0) == sliceDicom2);
	CPPUNIT_ASSERT(studyList->GetStudy(1)->GetSeries(0)->GetSlice(0) == sliceDicom3);
	
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyListGetStudiesNum()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();
	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	mafDicomStudyList *studyList = new mafDicomStudyList();
	studyList->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	studyList->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	studyList->AddSlice(sliceDicom3);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);
}

//----------------------------------------------------------------------------
void mafDicomClassesTest::TestStudyListGetSeriesTotalNum()
{
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	mafDicomSlice *sliceDicom2 = CreateBaseSlice();
	mafDicomSlice *sliceDicom3 = CreateBaseSlice();
	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	mafDicomStudyList *studyList = new mafDicomStudyList();
	studyList->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 1);
	studyList->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 2);
	studyList->AddSlice(sliceDicom3);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 3);
}

