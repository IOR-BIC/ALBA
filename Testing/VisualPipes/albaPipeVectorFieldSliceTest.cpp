/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldSliceTest
 Authors: Eleonora Mambrini
 
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
#include "albaPipeVectorFieldSliceTest.h"
#include "albaPipeVectorFieldSlice.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEStorage.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"
#include "vtkCamera.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkRenderWindowInteractor.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"
#include "vtkProp3DCollection.h"


//----------------------------------------------------------------------------
void albaPipeVectorFieldSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVectorFieldSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldSliceTest::TestCreate()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	InitializeRenderWindow();
	vtkCamera *cam = m_Renderer->GetActiveCamera();


	albaVMEVolumeGray *volume;
	albaNEW(volume);
	volume->ReparentTo(storage->GetRoot());

	vtkALBASmartPointer<vtkFloatArray> scalarArray;
	scalarArray->SetName("Scalar");

	vtkALBASmartPointer<vtkFloatArray> vectorArray;
	vectorArray->SetNumberOfComponents(3);
	vectorArray->SetName("velocity");

	vtkALBASmartPointer<vtkFloatArray> tensorArray;
	tensorArray->SetNumberOfComponents(9);
	tensorArray->SetName("tensor");


	int x, y, z;
	x = y = z = 10;
	vtkALBASmartPointer<vtkImageData> image;
	double origin[3] = { 0.0,0.0, -45. };
	image->SetOrigin(origin);
	image->SetDimensions(x, y, z);
	image->SetSpacing(10., 10., 10.);

	int i = 0, size = x*y*z;
	for (; i < size; i++) {
		scalarArray->InsertNextTuple1(1.0);
		vectorArray->InsertNextTuple3((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
		tensorArray->InsertNextTuple9((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
	}

	image->GetPointData()->AddArray(vectorArray);
	image->GetPointData()->AddArray(tensorArray);
	image->GetPointData()->SetScalars(scalarArray);
	image->GetPointData()->GetVectors("velocity")->Modified();
	image->GetPointData()->Update();

	image->GetPointData()->SetActiveVectors("velocity");

	double bb[6];
	image->GetBounds(bb);

	volume->SetDataByReference(image, 0.);
	volume->Update();

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
	albaSceneNode *sceneNode = new albaSceneNode(NULL, rootscenenode, volume, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipeVectorFieldSlice *pipe = new albaPipeVectorFieldSlice;
	pipe->Create(sceneNode);


	double focalPoint[3] = { 45.,45.,0. };
	double position[3] = { 45.,45.,100. };

	cam->SetFocalPoint(focalPoint);
	cam->SetPosition(position);

	////////// ACTORS List ///////////////
	vtkProp3DCollection *actorList = pipe->GetAssemblyFront()->GetParts();

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		m_Renderer->AddActor(actor);
		actor = actorList->GetNextProp();
	}

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
	COMPARE_IMAGES("image");

	delete sceneNode;
	delete(rootscenenode);

	volume->ReparentTo(NULL);
	albaDEL(volume);
	albaDEL(storage);
}