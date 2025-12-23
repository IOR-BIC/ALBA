/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeMirrorTest
 Authors: Gianluigi Crimi
 
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

#include "albaOpVolumeMirrorTest.h"
#include "albaOpVolumeMirror.h"

#include "albaString.h"
#include "albaVMEVolume.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>
#include "albaVMEStorage.h"
#include "albaOpImporterVTK.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

//-----------------------------------------------------------
void albaOpVolumeMirrorTest::TestDynamicAllocation() 
{
	albaOpVolumeMirror *vm=new albaOpVolumeMirror();
	albaDEL(vm);
}

//-----------------------------------------------------------
void albaOpVolumeMirrorTest::TestOpRun() 
{
	//Initialize Storage
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	//Import volume
	albaOpImporterVTK *importerVTK = new albaOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
	importerVTK->SetInput(storage->GetRoot());
	albaString filename = ALBA_DATA_ROOT;
	filename << "/VTK_Volumes/CropTestVolumeSP.vtk";
	importerVTK->SetFileName(filename);
	importerVTK->OpRun();
	albaVMEVolumeGray *baseVolume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	baseVolume->Update();
	baseVolume->GetOutput()->Update();
	
	//Check if the input volume is a VMEVolumeGray
	CPPUNIT_ASSERT(baseVolume != NULL);

	albaOpVolumeMirror *volumeMirrorOp=new albaOpVolumeMirror("Surface Mirror");
	volumeMirrorOp->TestModeOn();

	//Test Accept	
	CPPUNIT_ASSERT(volumeMirrorOp->Accept(baseVolume));

	vtkImageData * baseVolVtk = vtkImageData::SafeDownCast(baseVolume->GetOutput()->GetVTKData());
	vtkDataArray* baseVolScalars = baseVolVtk->GetPointData()->GetScalars();

	int dims[3];
	baseVolVtk->GetDimensions(dims);

	///////////////
	//X mirror Test
	albaVMEVolumeGray *xMirrorVolume;
	albaNEW(xMirrorVolume);
	xMirrorVolume->DeepCopy(baseVolume);
	xMirrorVolume->Update();
	xMirrorVolume->GetOutput()->Update();

	volumeMirrorOp->SetInput(xMirrorVolume);
  volumeMirrorOp->OpRun();
	volumeMirrorOp->OpDo();
	xMirrorVolume->GetOutput()->Update();
	vtkImageData * xMirrorVolVtk = vtkImageData::SafeDownCast(xMirrorVolume->GetOutput()->GetVTKData());
	vtkDataArray* xMirrorVolScalars = xMirrorVolVtk->GetPointData()->GetScalars();
		
	bool xMirrorOK = true;

	for (int i = 0; i < dims[0] && xMirrorOK; i++)
		for (int j = 0; j < dims[1] && xMirrorOK; j++)
			for (int k = 0; k < dims[2] && xMirrorOK; k++)
			{
				int xMirrorP = i + j*dims[0] + k*dims[0] * dims[1];
				int baseP = (dims[0]-(i+1)) + j*dims[0] + k*dims[0] * dims[1];
				if (xMirrorVolScalars->GetTuple1(xMirrorP) != baseVolScalars->GetTuple1(baseP))
					xMirrorOK = false;
			}

	CPPUNIT_ASSERT(xMirrorOK);

	albaDEL(xMirrorVolume);

	///////////////
	//Y mirror Test
	albaVMEVolumeGray *yMirrorVolume;
	albaNEW(yMirrorVolume);
	yMirrorVolume->DeepCopy(baseVolume);
	yMirrorVolume->Update();
	yMirrorVolume->GetOutput()->Update();

	volumeMirrorOp->SetInput(yMirrorVolume);
	volumeMirrorOp->SetMirrorX(false);
	volumeMirrorOp->SetMirrorY(true);
	volumeMirrorOp->OpRun();
	volumeMirrorOp->OpDo();
	yMirrorVolume->GetOutput()->Update();
	vtkImageData * yMirrorVolVtk = vtkImageData::SafeDownCast(yMirrorVolume->GetOutput()->GetVTKData());
	vtkDataArray* yMirrorVolScalars = yMirrorVolVtk->GetPointData()->GetScalars();

	bool yMirrorOK = true;

	for (int i = 0; i < dims[0] && yMirrorOK; i++)
		for (int j = 0; j < dims[1] && yMirrorOK; j++)
			for (int k = 0; k < dims[2] && yMirrorOK; k++)
			{
				int yMirrorP = i + j*dims[0] + k*dims[0] * dims[1];
				int baseP = i + (dims[1] - (j + 1))*dims[0] + k*dims[0] * dims[1];
				if (yMirrorVolScalars->GetTuple1(yMirrorP) != baseVolScalars->GetTuple1(baseP))
					yMirrorOK = false;
			}

	CPPUNIT_ASSERT(yMirrorOK);

	albaDEL(yMirrorVolume);

	///////////////
	//Z mirror Test
	albaVMEVolumeGray *zMirrorVolume;
	albaNEW(zMirrorVolume);
	zMirrorVolume->DeepCopy(baseVolume);
	zMirrorVolume->Update();
	zMirrorVolume->GetOutput()->Update();

	volumeMirrorOp->SetInput(zMirrorVolume);
	volumeMirrorOp->SetMirrorY(false);
	volumeMirrorOp->SetMirrorZ(true);
	volumeMirrorOp->OpRun();
	volumeMirrorOp->OpDo();
	zMirrorVolume->GetOutput()->Update();
	vtkImageData * zMirrorVolVtk = vtkImageData::SafeDownCast(zMirrorVolume->GetOutput()->GetVTKData());
	vtkDataArray* zMirrorVolScalars = zMirrorVolVtk->GetPointData()->GetScalars();

	bool zMirrorOK = true;

	for (int i = 0; i < dims[0] && zMirrorOK; i++)
		for (int j = 0; j < dims[1] && zMirrorOK; j++)
			for (int k = 0; k < dims[2] && zMirrorOK; k++)
			{
				int zMirrorP = i + j*dims[0] + k*dims[0] * dims[1];
				int baseP = i + j*dims[0] + (dims[2] - (k + 1))*dims[0] * dims[1];
				if (zMirrorVolScalars->GetTuple1(zMirrorP) != baseVolScalars->GetTuple1(baseP))
					zMirrorOK = false;
			}

	CPPUNIT_ASSERT(zMirrorOK);

	albaDEL(zMirrorVolume);

	///////////////
	//ALL mirror Test
	albaVMEVolumeGray *allMirrorVolume;
	albaNEW(allMirrorVolume);
	allMirrorVolume->DeepCopy(baseVolume);
	allMirrorVolume->Update();
	allMirrorVolume->GetOutput()->Update();

	volumeMirrorOp->SetInput(allMirrorVolume);
	volumeMirrorOp->SetMirrorX(true);
	volumeMirrorOp->SetMirrorY(true);
	volumeMirrorOp->SetMirrorZ(true);
	volumeMirrorOp->OpRun();
	volumeMirrorOp->OpDo();
	allMirrorVolume->GetOutput()->Update();
	vtkImageData * allMirrorVolVtk = vtkImageData::SafeDownCast(allMirrorVolume->GetOutput()->GetVTKData());
	vtkDataArray* allMirrorVolScalars = allMirrorVolVtk->GetPointData()->GetScalars();

	bool allMirrorOK = true;

	for (int i = 0; i < dims[0] && allMirrorOK; i++)
		for (int j = 0; j < dims[1] && allMirrorOK; j++)
			for (int k = 0; k < dims[2] && allMirrorOK; k++)
			{
				int allMirrorP = i + j*dims[0] + k*dims[0] * dims[1];
				int baseP = (dims[0] - (i + 1)) + (dims[1] - (j + 1))*dims[0] + (dims[2] - (k + 1))*dims[0] * dims[1];
				if (allMirrorVolScalars->GetTuple1(allMirrorP) != baseVolScalars->GetTuple1(baseP))
					allMirrorOK = false;
			}

	CPPUNIT_ASSERT(allMirrorOK);

	albaDEL(allMirrorVolume);
}