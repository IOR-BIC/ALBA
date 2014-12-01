/*=========================================================================

 Program: MAF2Medical
 Module: mafDicomSliceTest
 Authors: Matteo Giacomoni
 
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
#include "mafDicomSliceTest.h"

#include "mafOpImporterDicomOffis.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReader.h"

#define D_SliceABSFileName "FILE_NAME"
#define D_DcmInstanceNumber 11
#define D_DcmCardiacNumberOfImages 18
#define D_DcmTriggerTime 120.5

//-----------------------------------------------------------
void mafDicomSliceTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafDicomSliceTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetSliceABSFileName() 
//-----------------------------------------------------------
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  sliceDicom->SetSliceABSFileName(D_SliceABSFileName);

  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),D_SliceABSFileName) == 0);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","");
  
  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),D_SliceABSFileName) == 0);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmInstanceNumber() 
//-----------------------------------------------------------
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  sliceDicom->SetDcmInstanceNumber(D_DcmInstanceNumber);

  CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_DcmInstanceNumber);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber);

  CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_DcmInstanceNumber);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmCardiacNumberOfImages() 
//-----------------------------------------------------------
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  sliceDicom->SetDcmCardiacNumberOfImages(D_DcmCardiacNumberOfImages);

  CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_DcmCardiacNumberOfImages);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages);

  CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_DcmCardiacNumberOfImages);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmTriggerTime() 
//-----------------------------------------------------------
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  sliceDicom->SetDcmTriggerTime(D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_DcmTriggerTime);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_DcmTriggerTime);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetVTKImageData() 
//-----------------------------------------------------------
{
  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();
  vtkMAFSmartPointer<vtkImageReader> reader;
  mafString fileName=MAF_DATA_ROOT;
  fileName<<"/VTK_Volumes/volume.vtk";
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  sliceDicom->SetVTKImageData(reader->GetOutput());

  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfPoints() == reader->GetOutput()->GetNumberOfPoints());
  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfCells() == reader->GetOutput()->GetNumberOfCells());

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,reader->GetOutput(),"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfPoints() == reader->GetOutput()->GetNumberOfPoints());
  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfCells() == reader->GetOutput()->GetNumberOfCells());

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmImagePositionPatient() 
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();

  sliceDicom->SetDcmImagePositionPatient(patientPos);

  double testPos[3];
  sliceDicom->GetDcmImagePositionPatient(testPos);
  CPPUNIT_ASSERT(testPos[0] == patientPos[0]);
  CPPUNIT_ASSERT(testPos[1] == patientPos[1]);
  CPPUNIT_ASSERT(testPos[2] == patientPos[2]);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  sliceDicom->GetDcmImagePositionPatient(testPos);
  CPPUNIT_ASSERT(testPos[0] == patientPos[0]);
  CPPUNIT_ASSERT(testPos[1] == patientPos[1]);
  CPPUNIT_ASSERT(testPos[2] == patientPos[2]);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmImageOrientationPatient() 
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  mafDicomSlice *sliceDicom = new mafDicomSlice();

  sliceDicom->SetDcmImageOrientationPatient(patientOri);

  double testOri[6];
  sliceDicom->GetDcmImageOrientationPatient(testOri);
  CPPUNIT_ASSERT(testOri[0] == patientOri[0]);
  CPPUNIT_ASSERT(testOri[1] == patientOri[1]);
  CPPUNIT_ASSERT(testOri[2] == patientOri[2]);
  CPPUNIT_ASSERT(testOri[3] == patientOri[3]);
  CPPUNIT_ASSERT(testOri[4] == patientOri[4]);
  CPPUNIT_ASSERT(testOri[5] == patientOri[5]);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  sliceDicom = new mafDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  sliceDicom->GetDcmImageOrientationPatient(patientOri);
  CPPUNIT_ASSERT(testOri[0] == patientOri[0]);
  CPPUNIT_ASSERT(testOri[1] == patientOri[1]);
  CPPUNIT_ASSERT(testOri[2] == patientOri[2]);
  CPPUNIT_ASSERT(testOri[3] == patientOri[3]);
  CPPUNIT_ASSERT(testOri[4] == patientOri[4]);
  CPPUNIT_ASSERT(testOri[5] == patientOri[5]);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void mafDicomSliceTest::TestGetDcmImageOrientationPatientMatrix() 
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
