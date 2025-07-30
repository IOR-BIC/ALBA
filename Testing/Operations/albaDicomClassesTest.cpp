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

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaDicomClassesTest.h"

#include "albaOpImporterDicom.h"

#include "vtkALBASmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReader.h"
#include "vtkDataSetReader.h"
#include "albaGUIDicomSettings.h"
#include "albaSmartPointer.h"


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
#define D_DcmSeriesID "Id-Series-1"
#define D_DcmAcquisitionNumber "1"
#define D_NewDcmAcquisitionNumber "2"
#define D_DcmImageType "img1"
#define D_NewDcmImageType "img2"
#define D_Position { 0.0, 1.0, 2.0 }
#define D_SliceSize { 256, 256 }
#define D_NewPosition { 0.0, 2.0, 1.0 }
#define D_Orientation { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 }
#define D_NewOrientation { 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 }
#define D_NewNum 7

///////////////////////////////albaDicomSlice//////////////////////////////////
//----------------------------------------------------------------------------
albaDicomSlice * albaDicomClassesTest::CreateBaseSlice(double *pos)
{
	//this will be deleted from slice destructor
	double patientPos[3] = D_Position;
	int sliceSize[2] = D_SliceSize;

	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/Dicom/" << D_SliceABSFileName;


	albaDicomSlice *sliceDicom;
	double patientOri[6] = D_Orientation;
	if (pos != NULL)
		sliceDicom = new albaDicomSlice(fileName, patientOri, pos, D_DcmDescription,D_DcmDate, D_DcmPatientName,D_DcmBirthDate, D_DcmCardiacNumberOfImages,D_DcmTriggerTime);
	else 
		sliceDicom = new albaDicomSlice(fileName, patientOri, patientPos, D_DcmDescription, D_DcmDate, D_DcmPatientName, D_DcmBirthDate, D_DcmCardiacNumberOfImages, D_DcmTriggerTime);

	sliceDicom->SetSliceSize(sliceSize);

	return sliceDicom;
}

//-----------------------------------------------------------
void albaDicomClassesTest::TestSliceDynamicAllocation()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
  cppDEL(sliceDicom);
}


//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetPatientBirthday()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPatientBirthday(), D_DcmBirthDate) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetPatientName()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPatientName(), D_DcmPatientName) == 0);

	cppDEL(sliceDicom);
}

//-----------------------------------------------------------
void albaDicomClassesTest::TestSliceGetSliceABSFileName()
{
  albaDicomSlice *sliceDicom = CreateBaseSlice();

	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/Dicom/" << D_SliceABSFileName;

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(), fileName.GetCStr()) == 0);

	sliceDicom->SetSliceABSFileName("newName");
  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),"newName") == 0);

  cppDEL(sliceDicom);
}

//-----------------------------------------------------------
void albaDicomClassesTest::TestSliceGetDcmCardiacNumberOfImages()
{
  //Using the default constructor
  albaDicomSlice *sliceDicom = CreateBaseSlice();

  CPPUNIT_ASSERT(sliceDicom->GetNumberOfCardiacImages() == D_DcmCardiacNumberOfImages);

	sliceDicom->SetNumberOfCardiacImages(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetNumberOfCardiacImages() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void albaDicomClassesTest::TestSliceGetDcmTriggerTime()
{
  //Using the default constructor
  albaDicomSlice *sliceDicom = CreateBaseSlice();
  
  CPPUNIT_ASSERT(sliceDicom->GetTriggerTime() == D_DcmTriggerTime);

	sliceDicom->SetTriggerTime(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetTriggerTime() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void albaDicomClassesTest::TestSliceGetVTKImageData()
{
	//create imagedata 
	vtkALBASmartPointer<vtkDataSetReader> reader;
  albaString fileName=ALBA_DATA_ROOT;
  fileName<<"/Dicom/DicomSlice.vtk";
  reader->SetFileName(fileName.GetCStr());
  reader->Update();
	
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	
	vtkImageData* sliceImageData = sliceDicom->GetNewVTKImageData();
	vtkImageData* readerOutput = vtkImageData::SafeDownCast(reader->GetOutput());
	
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
void albaDicomClassesTest::TestSliceGetDcmImageOrientationPatient()
{
	double patientOri[6] = D_Orientation;

	albaDicomSlice *sliceDicom = CreateBaseSlice();

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
void albaDicomClassesTest::TestSliceGetUnrotatedOrigin()
{
	double patientPos[3] = D_Position;

	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

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
void albaDicomClassesTest::TestSliceGetDescription()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetDescription(), D_DcmDescription) == 0);
	
	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetDate()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetDate(), D_DcmDate) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetDcmModality()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetModality("Modality");

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetModality(), "Modality") == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetPhotometricInterpretation()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetPhotometricInterpretation("PhotoMetric");

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetPhotometricInterpretation(), "PhotoMetric") == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetSeriesID()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetSeriesID(D_DcmSeriesID);

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetSeriesID(), D_DcmSeriesID) == 0);

	cppDEL(sliceDicom);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSliceGetStudyID()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	sliceDicom->SetStudyID(D_DcmStudyID);

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetStudyID(), D_DcmStudyID) == 0);

	cppDEL(sliceDicom);
}

///////////////////////////////albaDicomSeries//////////////////////////////////
//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesDynamicAllocation()
{
	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesAddSlice()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);

	series->AddSlice(sliceDicom);

	CPPUNIT_ASSERT(series->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(series->GetSlice(0) == sliceDicom);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetSlicesNum()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);

	series->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(series->GetSlicesNum() == 1);

	series->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(series->GetSlicesNum() == 2);
	
	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesIsRotated()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	double patientNewOri[6] = D_NewOrientation;
	sliceDicom2->SetDcmImageOrientationPatient(patientNewOri);

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);

	series->AddSlice(sliceDicom);
	CPPUNIT_ASSERT(series->IsRotated() == false);

	series->AddSlice(sliceDicom2);
	CPPUNIT_ASSERT(series->IsRotated() == true);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetSlices()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	
	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	series->AddSlice(sliceDicom);
	series->AddSlice(sliceDicom2);

	std::vector<albaDicomSlice *> slices = series->GetSlices();

	CPPUNIT_ASSERT(slices.size() == 2);
	CPPUNIT_ASSERT(slices[0] == sliceDicom);
	CPPUNIT_ASSERT(slices[1] == sliceDicom2);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetSerieID()
{
	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	
	CPPUNIT_ASSERT(series->GetSerieID() == D_DcmSeriesID);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetAcquisitonNumber()
{
	albaDicomSeries *series1 = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	albaDicomSeries *series2 = new albaDicomSeries(D_DcmSeriesID, D_NewDcmAcquisitionNumber, D_DcmImageType);

	CPPUNIT_ASSERT(series1->GetAcquisitionNumber() == D_DcmAcquisitionNumber);
	CPPUNIT_ASSERT(series2->GetAcquisitionNumber() == D_NewDcmAcquisitionNumber);

	cppDEL(series1);
	cppDEL(series2);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetImageType()
{
	albaDicomSeries *series1 = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	albaDicomSeries *series2 = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_NewDcmImageType);

	CPPUNIT_ASSERT(series1->GetImageType() == D_DcmImageType);
	CPPUNIT_ASSERT(series2->GetImageType() == D_NewDcmImageType);

	cppDEL(series1);
	cppDEL(series2);
}


//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetDimensions()
{
	//Using the default constructor
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	series->AddSlice(sliceDicom);

	const int *dims=series->GetDimensions();

	CPPUNIT_ASSERT(dims[0] == 256);
	CPPUNIT_ASSERT(dims[1] == 256);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesGetCardiacImagesNum()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
	series->AddSlice(sliceDicom);

	CPPUNIT_ASSERT(series->GetCardiacImagesNum() == D_DcmCardiacNumberOfImages);

	cppDEL(series);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestSeriesSortSlices()
{
	double newPos[3] = D_NewPosition;
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice(newPos);

	albaDicomSeries *series = new albaDicomSeries(D_DcmSeriesID, D_DcmAcquisitionNumber, D_DcmImageType);
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

///////////////////////////////albaDicomStudy//////////////////////////////////
//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyDynamicAllocation()
{
	albaDicomStudy *study = new albaDicomStudy(D_DcmStudyID);
	cppDEL(study);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyAddSlice()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaDicomSlice *sliceDicom4 = CreateBaseSlice();

	sliceDicom2->SetAcquisitionNumber(D_NewDcmAcquisitionNumber);
	sliceDicom3->SetSeriesID(D_NewDcmSeriesID);

	albaDicomStudy *study = new albaDicomStudy(D_DcmStudyID);
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	//first slice
	study->AddSlice(sliceDicom,settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(0) == sliceDicom);

	//slice with new AcquisitionNumber
	study->AddSlice(sliceDicom2, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlice(0) == sliceDicom2);

	//slice with new seriesID
	study->AddSlice(sliceDicom3, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 3);
	CPPUNIT_ASSERT(study->GetSeries(2)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(2)->GetSlice(0) == sliceDicom3);

	//slice with already know seriesID and AcquisitionNumber
	study->AddSlice(sliceDicom4, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 3);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(1) == sliceDicom4);

	cppDEL(study);
	cppDEL(settings);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyGetStudyID()
{
	albaDicomStudy *study = new albaDicomStudy(D_DcmStudyID);
	
	CPPUNIT_ASSERT(study->GetStudyID() == D_DcmStudyID);
	
	cppDEL(study);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyGetSeriesNum()
{
	double newPos[3] = D_NewPosition;
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	albaDicomStudy *study = new albaDicomStudy(D_DcmStudyID);

	study->AddSlice(sliceDicom, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 1);
	study->AddSlice(sliceDicom2, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);
	study->AddSlice(sliceDicom3, settings);
	CPPUNIT_ASSERT(study->GetSeriesNum() == 2);

	cppDEL(study);
	cppDEL(settings);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyGetSeries()
{
	double newPos[3] = D_NewPosition;
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	albaDicomStudy *study = new albaDicomStudy(D_DcmStudyID);

	study->AddSlice(sliceDicom, settings);
	study->AddSlice(sliceDicom2, settings);
	study->AddSlice(sliceDicom3, settings);
	
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlicesNum() == 2);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(0) == sliceDicom);
	CPPUNIT_ASSERT(study->GetSeries(0)->GetSlice(1) == sliceDicom3);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlicesNum() == 1);
	CPPUNIT_ASSERT(study->GetSeries(1)->GetSlice(0) == sliceDicom2);

	cppDEL(study);
	cppDEL(settings);
}

/////////////////////////////albaDicomStudyList////////////////////////////////
//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyListDynamicAllocation()
{
	albaDicomStudyList *studyList = new albaDicomStudyList();
	cppDEL(studyList);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyListAddSlice()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);
	
	albaDicomStudyList *studyList = new albaDicomStudyList();

	studyList->AddSlice(sliceDicom,settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 1);
	
	studyList->AddSlice(sliceDicom2, settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 2);

	studyList->AddSlice(sliceDicom3, settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 3);
	
	cppDEL(studyList);
	cppDEL(settings);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyListGetStudy()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	albaDicomStudyList *studyList = new albaDicomStudyList();
	studyList->AddSlice(sliceDicom, settings);
	studyList->AddSlice(sliceDicom2, settings);
	studyList->AddSlice(sliceDicom3, settings);

	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeriesNum() == 2);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeries(0)->GetSlice(0) == sliceDicom);
	CPPUNIT_ASSERT(studyList->GetStudy(0)->GetSeries(1)->GetSlice(0) == sliceDicom2);
	CPPUNIT_ASSERT(studyList->GetStudy(1)->GetSeries(0)->GetSlice(0) == sliceDicom3);
	
	cppDEL(studyList);
	cppDEL(settings);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyListGetStudiesNum()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	albaDicomStudyList *studyList = new albaDicomStudyList();
	studyList->AddSlice(sliceDicom,settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	studyList->AddSlice(sliceDicom2, settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 1);
	studyList->AddSlice(sliceDicom3, settings);
	CPPUNIT_ASSERT(studyList->GetStudiesNum() == 2);

	cppDEL(studyList);
	cppDEL(settings);
}

//----------------------------------------------------------------------------
void albaDicomClassesTest::TestStudyListGetSeriesTotalNum()
{
	albaDicomSlice *sliceDicom = CreateBaseSlice();
	albaDicomSlice *sliceDicom2 = CreateBaseSlice();
	albaDicomSlice *sliceDicom3 = CreateBaseSlice();
	albaGUIDicomSettings *settings=new albaGUIDicomSettings(NULL);
	settings->SetAcquisitionNumberStrategy(albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER);

	sliceDicom2->SetSeriesID(D_NewDcmSeriesID);
	sliceDicom3->SetStudyID(D_NewDcmStudyID);

	albaDicomStudyList *studyList = new albaDicomStudyList();
	studyList->AddSlice(sliceDicom, settings);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 1);
	studyList->AddSlice(sliceDicom2, settings);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 2);
	studyList->AddSlice(sliceDicom3, settings);
	CPPUNIT_ASSERT(studyList->GetSeriesTotalNum() == 3);
	
	cppDEL(studyList);
	cppDEL(settings);
}

