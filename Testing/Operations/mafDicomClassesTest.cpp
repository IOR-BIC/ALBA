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

#define D_SliceABSFileName "FILE_NAME"
#define D_DcmInstanceNumber 11
#define D_DcmCardiacNumberOfImages 18
#define D_DcmTriggerTime 120.5
#define D_DcmDescription "DESCR"
#define D_DcmDate "1-1-2000"
#define D_DcmPatientName "Name"
#define D_DcmBirthDate "1-1-1900"
#define D_DcmCardiacNumberOfImages 1
#define D_DcmTriggerTime 1
#define D_DcmStudyID "Id-Study-1"
#define D_DcmSeriesID "Id-Series-1"
#define D_Position { 0.0, 1.0, 2.0 }
#define D_NewPosition { 4.0, 5.0, 6.0}
#define D_Orientation { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 }
#define D_NewOrientation { 0.0, 1.0, 0.0, 1.0, 0.0, 0.0 }
#define D_NewNum 7

//----------------------------------------------------------------------------
mafDicomSlice * mafDicomClassesTest::CreateBaseSlice()
{
	vtkMAFSmartPointer<vtkImageReader> reader;
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/VTK_Volumes/volume.vtk";
	reader->SetFileName(fileName.GetCStr());
	reader->Update();


	//this will be deleted from slice destructor
	double patientPos[3] = D_Position;
	vtkImageData *imData;
	vtkNEW(imData);
	imData->DeepCopy(reader->GetOutput());
	imData->SetOrigin(patientPos);

		
	double patientOri[6] = D_Orientation;
	mafDicomSlice *sliceDicom = new mafDicomSlice(D_SliceABSFileName, patientOri, imData,D_DcmDescription,D_DcmDate, D_DcmPatientName,D_DcmBirthDate, D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);
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

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),D_SliceABSFileName) == 0);

	sliceDicom->SetSliceABSFileName("newName");
  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),"newName") == 0);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmInstanceNumber()
{
  mafDicomSlice *sliceDicom = CreateBaseSlice();

  CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_DcmInstanceNumber);

	sliceDicom->SetDcmInstanceNumber(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmCardiacNumberOfImages()
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = CreateBaseSlice();

  CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_DcmCardiacNumberOfImages);

	sliceDicom->SetDcmCardiacNumberOfImages(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmTriggerTime()
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = CreateBaseSlice();
  
  CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_DcmTriggerTime);

	sliceDicom->SetDcmTriggerTime(D_NewNum);
	CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_NewNum);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetVTKImageData()
{
	//create imagedata
	vtkMAFSmartPointer<vtkImageReader> reader;
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTK_Volumes/volume.vtk";
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

	//Using the default constructor
	mafDicomSlice *sliceDicom = CreateBaseSlice();
	
  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfPoints() == reader->GetOutput()->GetNumberOfPoints());
  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfCells() == reader->GetOutput()->GetNumberOfCells());

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

	sliceDicom->SetDcmModality("Modality");

	CPPUNIT_ASSERT(strcmp(sliceDicom->GetDcmModality(), "Modality") == 0);

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

/*
//-----------------------------------------------------------
void mafDicomClassesTest::TestSliceGetDcmImageOrientationPatientMatrix()
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();

  sliceDicom->SetDcmImageOrientationPatient(patientOri);

  double testOri[6];
  vtkMAFSmartPointer<vtkMatrix4x4> matrix;
  double Vx0,Vx1,Vx2,Vy0,Vy1,Vy2;
  sliceDicom->GetOrientation(matrix);

  Vx0 = matrix->GetElement(0,0);
  Vx1 = matrix->GetElement(1,0);
  Vx2 = matrix->GetElement(2,0);
  Vy0 = matrix->GetElement(0,1);
  Vy1 = matrix->GetElement(1,1);
  Vy2 = matrix->GetElement(2,1);

  CPPUNIT_ASSERT(Vx0 == patientOri[0]);
  CPPUNIT_ASSERT(Vx1 == patientOri[1]);
  CPPUNIT_ASSERT(Vx2 == patientOri[2]);
  CPPUNIT_ASSERT(Vy0 == patientOri[3]);
  CPPUNIT_ASSERT(Vy1 == patientOri[4]);
  CPPUNIT_ASSERT(Vy2 == patientOri[5]);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  sliceDicom->GetOrientation(matrix);

  Vx0 = matrix->GetElement(0,0);
  Vx1 = matrix->GetElement(1,0);
  Vx2 = matrix->GetElement(2,0);
  Vy0 = matrix->GetElement(0,1);
  Vy1 = matrix->GetElement(1,1);
  Vy2 = matrix->GetElement(2,1);

  CPPUNIT_ASSERT(Vx0 == patientOri[0]);
  CPPUNIT_ASSERT(Vx1 == patientOri[1]);
  CPPUNIT_ASSERT(Vx2 == patientOri[2]);
  CPPUNIT_ASSERT(Vy0 == patientOri[3]);
  CPPUNIT_ASSERT(Vy1 == patientOri[4]);
  CPPUNIT_ASSERT(Vy2 == patientOri[5]);

  cppDEL(sliceDicom);
}
*/
