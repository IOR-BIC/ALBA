/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldGlyphsTest
 Authors: Roberto Muccis
 
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
#include "albaPipeVectorFieldGlyphsTest.h"
#include "albaPipeVectorFieldGlyphs.h"

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

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"


//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphsTest::BeforeTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphsTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphsTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphsTest::TestCreate()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	InitializeRenderWindow();

	albaVMEVolumeGray *testVolume;
	albaNEW(testVolume);
	testVolume->ReparentTo(storage->GetRoot());

	int volumeDimensions[3];
	volumeDimensions[0] = 5;
	volumeDimensions[1] = 5;
	volumeDimensions[2] = 5;

	vtkALBASmartPointer<vtkImageData> imageData;
	imageData->SetDimensions(volumeDimensions[0], volumeDimensions[1], volumeDimensions[2]);
	imageData->SetSpacing(1., 1., 1.);

	vtkALBASmartPointer<vtkFloatArray> scalarArray;
	scalarArray->SetNumberOfComponents(1);
	scalarArray->SetName("Scalar");

	vtkALBASmartPointer<vtkFloatArray> vectorArray;
	vectorArray->SetNumberOfComponents(3);
	vectorArray->SetNumberOfTuples(volumeDimensions[0] * volumeDimensions[1] * volumeDimensions[2]);
	vectorArray->SetName("Vector");

	for (int i = 0; i < volumeDimensions[0] * volumeDimensions[1] * volumeDimensions[2]; i++)
	{
		scalarArray->InsertNextTuple1(i % 3);
		vectorArray->SetTuple3(i, i, i, i);
	}

	imageData->GetPointData()->SetScalars(scalarArray);
	imageData->GetPointData()->SetActiveScalars("Scalar");


	testVolume->SetData(imageData, 0.);
	testVolume->Update();

	albaVMEOutput *output = testVolume->GetOutput();
	output->Update();
	vtkPointData *pointData = output->GetVTKData()->GetPointData();
	pointData->Update();

	pointData->AddArray(vectorArray);
	pointData->SetActiveVectors("Vector");

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
	albaSceneNode *sceneNode = new albaSceneNode(NULL, rootscenenode, testVolume, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipeVectorFieldGlyphs *pipe = new albaPipeVectorFieldGlyphs;
	pipe->Create(sceneNode);
	pipe->m_RenFront = m_Renderer;

	vtkPropCollection *actorList = vtkPropCollection::New();
	pipe->GetAssemblyFront()->GetActors(actorList);

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

	vtkDEL(actorList);

	delete sceneNode;
	delete(rootscenenode);

	testVolume->ReparentTo(NULL);
	albaDEL(testVolume);
	albaDEL(storage);
}