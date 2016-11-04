/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldGlyphsTest
 Authors: Roberto Muccis
 
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
#include "mafPipeVectorFieldGlyphsTest.h"
#include "mafPipeVectorFieldGlyphs.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEStorage.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"


//----------------------------------------------------------------------------
void mafPipeVectorFieldGlyphsTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);
	vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeVectorFieldGlyphsTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
	vtkDEL(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldGlyphsTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldGlyphsTest::TestCreate()
//----------------------------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	///////////////// render stuff /////////////////////////

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

	///////////// end render stuff /////////////////////////

	mafVMEVolumeGray *testVolume;
	mafNEW(testVolume);
	testVolume->ReparentTo(storage->GetRoot());

	int volumeDimensions[3];
	volumeDimensions[0] = 5;
	volumeDimensions[1] = 5;
	volumeDimensions[2] = 5;

	vtkMAFSmartPointer<vtkImageData> imageData;
	imageData->SetDimensions(volumeDimensions[0], volumeDimensions[1], volumeDimensions[2]);
	imageData->SetSpacing(1., 1., 1.);

	vtkMAFSmartPointer<vtkFloatArray> scalarArray;
	scalarArray->SetNumberOfComponents(1);
	scalarArray->SetName("Scalar");

	vtkMAFSmartPointer<vtkFloatArray> vectorArray;
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

	mafVMEOutput *output = testVolume->GetOutput();
	output->Update();
	vtkPointData *pointData = output->GetVTKData()->GetPointData();
	pointData->Update();

	pointData->AddArray(vectorArray);
	pointData->SetActiveVectors("Vector");

	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
	mafSceneNode *sceneNode = new mafSceneNode(NULL, rootscenenode, testVolume, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	mafPipeVectorFieldGlyphs *pipe = new mafPipeVectorFieldGlyphs;
	pipe->Create(sceneNode);
	pipe->m_RenFront = m_Renderer;

	vtkPropCollection *actorList = vtkPropCollection::New();
	pipe->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		m_Renderer->AddActor(actor);
		m_RenderWindow->Render();

		actor = actorList->GetNextProp();
	}

	m_RenderWindow->Render();
	COMPARE_IMAGES("image");

	vtkDEL(actorList);

	delete sceneNode;
	delete(rootscenenode);

	testVolume->ReparentTo(NULL);
	mafDEL(testVolume);
	mafDEL(storage);
}