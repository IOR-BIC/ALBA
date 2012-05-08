/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDicomSliceTest.cpp,v $
Language:  C++
Date:      $Date: 2011-07-18 08:43:22 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medDicomSliceTest.h"

#include "medOpImporterDicomOffis.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReader.h"

#define D_SliceABSFileName "FILE_NAME"
#define D_DcmInstanceNumber 11
#define D_DcmCardiacNumberOfImages 18
#define D_DcmTriggerTime 120.5

//-----------------------------------------------------------
void medDicomSliceTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medDicomSliceTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medDicomSliceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medDicomSlice *sliceDicom = new medDicomSlice();
  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetSliceABSFileName() 
//-----------------------------------------------------------
{
  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();
  sliceDicom->SetSliceABSFileName(D_SliceABSFileName);

  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),D_SliceABSFileName) == 0);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","");
  
  CPPUNIT_ASSERT(strcmp(sliceDicom->GetSliceABSFileName(),D_SliceABSFileName) == 0);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetDcmInstanceNumber() 
//-----------------------------------------------------------
{
  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();
  sliceDicom->SetDcmInstanceNumber(D_DcmInstanceNumber);

  CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_DcmInstanceNumber);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber);

  CPPUNIT_ASSERT(sliceDicom->GetDcmInstanceNumber() == D_DcmInstanceNumber);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetDcmCardiacNumberOfImages() 
//-----------------------------------------------------------
{
  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();
  sliceDicom->SetDcmCardiacNumberOfImages(D_DcmCardiacNumberOfImages);

  CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_DcmCardiacNumberOfImages);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages);

  CPPUNIT_ASSERT(sliceDicom->GetDcmCardiacNumberOfImages() == D_DcmCardiacNumberOfImages);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetDcmTriggerTime() 
//-----------------------------------------------------------
{
  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();
  sliceDicom->SetDcmTriggerTime(D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_DcmTriggerTime);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetDcmTriggerTime() == D_DcmTriggerTime);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetVTKImageData() 
//-----------------------------------------------------------
{
  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();
  vtkMAFSmartPointer<vtkImageReader> reader;
  mafString fileName=MED_DATA_ROOT;
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
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,reader->GetOutput(),"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfPoints() == reader->GetOutput()->GetNumberOfPoints());
  CPPUNIT_ASSERT(sliceDicom->GetVTKImageData()->GetNumberOfCells() == reader->GetOutput()->GetNumberOfCells());

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetDcmImagePositionPatient() 
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();

  sliceDicom->SetDcmImagePositionPatient(patientPos);

  double testPos[3];
  sliceDicom->GetDcmImagePositionPatient(testPos);
  CPPUNIT_ASSERT(testPos[0] == patientPos[0]);
  CPPUNIT_ASSERT(testPos[1] == patientPos[1]);
  CPPUNIT_ASSERT(testPos[2] == patientPos[2]);

  cppDEL(sliceDicom);

  //Using the initialization constructor
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

  sliceDicom->GetDcmImagePositionPatient(testPos);
  CPPUNIT_ASSERT(testPos[0] == patientPos[0]);
  CPPUNIT_ASSERT(testPos[1] == patientPos[1]);
  CPPUNIT_ASSERT(testPos[2] == patientPos[2]);

  cppDEL(sliceDicom);
}
//-----------------------------------------------------------
void medDicomSliceTest::TestGetDcmImageOrientationPatient() 
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();

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
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

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
void medDicomSliceTest::TestGetDcmImageOrientationPatientMatrix() 
//-----------------------------------------------------------
{
  double patientPos[3] = {0.0,1.0,2.0};
  double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

  //Using the default constructor
  medDicomSlice *sliceDicom = new medDicomSlice();

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
  sliceDicom = new medDicomSlice(D_SliceABSFileName,patientPos,patientOri,NULL,"","","","",D_DcmInstanceNumber,D_DcmCardiacNumberOfImages,D_DcmTriggerTime);

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
