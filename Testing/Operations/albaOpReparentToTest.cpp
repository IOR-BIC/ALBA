/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpReparentToTest
 Authors: Matteo Giacomoni
 
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
#include "albaOpReparentToTest.h"

#include "albaOpReparentTo.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaSmartPointer.h"
#include "albaMatrix.h"
#include "albaAbsMatrixPipe.h"
#include "albaTransform.h"

#include "vtkImageData.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"

#define START_TRANSLATION_X  -5
#define START_TRANSLATION_Y	 10
#define START_TRANSLATION_Z   0
#define START_ROTATION_X      0
#define START_ROTATION_Y	    0
#define START_ROTATION_Z      0
#define NEW_TRANSLATION_X    10
#define NEW_TRANSLATION_Y    15
#define NEW_TRANSLATION_Z    -2
#define NEW_ROTATION_X        0
#define NEW_ROTATION_Y	      0
#define NEW_ROTATION_Z        0

//-----------------------------------------------------------
void albaOpReparentToTest::Test() 
//-----------------------------------------------------------
{
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	// create a Volume
  vtkImageData *ImageData;
	vtkNEW(ImageData);
  ImageData->SetDimensions(5, 5, 5);
  ImageData->SetOrigin(-1, -1, -1);
  ImageData->SetSpacing(1,1,1);

	albaSmartPointer<albaVMEVolumeGray> volume;
	volume->SetData(ImageData,0);

	root->AddChild(volume);

	// create a Surface
	vtkSphereSource *SphereSource;
	vtkNEW(SphereSource);
  SphereSource->Update();

	albaSmartPointer<albaVMESurface> surface;
	surface->SetData(SphereSource->GetOutput(),0);
	surface->SetPose(START_TRANSLATION_X,START_TRANSLATION_Y,START_TRANSLATION_Z,START_ROTATION_X,START_ROTATION_Y,START_ROTATION_Z,0);
	root->AddChild(surface);

	CPPUNIT_ASSERT(root->GetNumberOfChildren()==2);
	CPPUNIT_ASSERT(surface->GetParent()==root);
	CPPUNIT_ASSERT(volume->GetParent()==root);
	CPPUNIT_ASSERT(surface->GetNumberOfChildren()==0);
	CPPUNIT_ASSERT(volume->GetNumberOfChildren()==0);

	albaVMEVolumeGray *volumeBefore;
	albaNEW(volumeBefore);
	volumeBefore->DeepCopy(volume);
	volumeBefore->Update();

	albaMatrix *VolumeMatrixBefore=volumeBefore->GetOutput()->GetAbsMatrix();

	//Create operation
	albaOpReparentTo *OpReparent=new albaOpReparentTo("reparent");
	OpReparent->SetInput(volume);
	OpReparent->SetTargetVme(surface);
	OpReparent->OpRun();
	OpReparent->OpDo();

	volume->GetOutput()->Update();
	volume->Modified();
	volume->Update();

	albaMatrix *VolumeMatrixAfter=volume->GetOutput()->GetAbsMatrix();

	double PosVolumeAfter[3],PosVolumeBefore[3],RotVolumeAfter[3],RotVolumeBefore[3];
	albaTransform::GetPosition(*VolumeMatrixAfter,PosVolumeAfter);
	albaTransform::GetPosition(*VolumeMatrixBefore,PosVolumeBefore);
	albaTransform::GetOrientation(*VolumeMatrixAfter,RotVolumeAfter);
	albaTransform::GetOrientation(*VolumeMatrixBefore,RotVolumeBefore);

	//Check Pose Matrix
	for(int i=0;i<3;i++)
	{
		CPPUNIT_ASSERT(PosVolumeAfter[i]==PosVolumeBefore[i] && RotVolumeAfter[i]==RotVolumeBefore[i]);
	}
	
	//Translate of the surface
	albaSmartPointer<albaTransform> tran;
  tran->Scale(1,1,1,POST_MULTIPLY);
  tran->RotateY(NEW_ROTATION_Y, POST_MULTIPLY);
  tran->RotateX(NEW_ROTATION_X, POST_MULTIPLY);
  tran->RotateZ(NEW_ROTATION_Z, POST_MULTIPLY);
  tran->SetPosition(NEW_TRANSLATION_X,NEW_TRANSLATION_Y,NEW_TRANSLATION_Z);
  surface->SetAbsMatrix(tran->GetMatrix(), 0);
	surface->Update();
	VolumeMatrixAfter=volume->GetOutput()->GetAbsMatrix();

	albaTransform::GetPosition(*VolumeMatrixAfter,PosVolumeAfter);
	albaTransform::GetOrientation(*VolumeMatrixAfter,RotVolumeAfter);
	
	//Check Pose Matrix after 2° translation
	double diff[3];
	diff[0]=(NEW_TRANSLATION_X-START_TRANSLATION_X);
	diff[1]=(NEW_TRANSLATION_Y-START_TRANSLATION_Y);
	diff[2]=(NEW_TRANSLATION_Z-START_TRANSLATION_Z);
	for(int i=0;i<3;i++)
	 CPPUNIT_ASSERT(PosVolumeAfter[i]==diff[i]);

	
	CPPUNIT_ASSERT(root->GetNumberOfChildren()==1);
	CPPUNIT_ASSERT(surface->GetNumberOfChildren()==1);
	CPPUNIT_ASSERT(volume->GetNumberOfChildren()==0);
	CPPUNIT_ASSERT(volume->GetParent()==(albaVME*)surface);

	OpReparent->OpUndo();
	volume->Update();
	VolumeMatrixAfter=volume->GetOutput()->GetAbsMatrix();
	CPPUNIT_ASSERT(root->GetNumberOfChildren()==2);
	CPPUNIT_ASSERT(surface->GetParent()==root);
	CPPUNIT_ASSERT(volume->GetParent()==root);
	CPPUNIT_ASSERT(surface->GetNumberOfChildren()==0);
	CPPUNIT_ASSERT(volume->GetNumberOfChildren()==0);
	for(int i=0;i<3;i++)
	 CPPUNIT_ASSERT(PosVolumeAfter[i]==diff[i]);

	root = NULL;
	vtkDEL(ImageData);
	vtkDEL(SphereSource);
	albaDEL(volumeBefore);
	albaDEL(OpReparent);
	albaDEL(storage);
}
