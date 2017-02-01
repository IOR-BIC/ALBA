/*=========================================================================

 Program: MAF2
 Module: mafPipePolylineSliceTest
 Authors: Roberto Mucci
 
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
#include "mafPipePolylineSliceTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipePolylineSlice.h"
#include "mafVMEPolyline.h"
#include "mmaMaterial.h"
#include "vtkMAFAssembly.h"

#include "vtkMAFSmartPointer.h"

#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include <iostream>

enum TESTS_PIPE_SURFACE
{
  TEST_RADIUS,
  TEST_THICKNESS,
	TEST_SPHERES,
	TEST_RESOLUTION,
	TEST_FILL,
	NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void mafPipePolylineSliceTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void mafPipePolylineSliceTest::BeforeTest()
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(400, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipePolylineSliceTest::AfterTest()
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineSliceTest::TestPipeExecution()
{
	////// Create VME (import vtkData) ////////////////////
	vtkMAFSmartPointer<vtkDataSetReader> importer;
	mafString filename = MAF_DATA_ROOT;
	filename << "/Test_PipePolylineSlice/Test01.vtk";
	importer->SetFileName(filename);
	importer->Update();

	mafSmartPointer<mafVMEPolyline> polyline;
	polyline->SetData((vtkPolyData*)importer->GetOutput(), 0.0);
	polyline->GetOutput()->Update();
	polyline->GetMaterial();
	polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	polyline->Update();

	//Setting standard material to avoid random color selection
	polyline->GetMaterial()->m_Diffuse[0] = 0.3;
	polyline->GetMaterial()->m_Diffuse[1] = 0.6;
	polyline->GetMaterial()->m_Diffuse[2] = 0.9;
	polyline->GetMaterial()->UpdateProp();

	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *sceneNode;
	sceneNode = new mafSceneNode(NULL, NULL, polyline, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	mafPipePolylineSlice *pipePolylineSlice = new mafPipePolylineSlice;
	pipePolylineSlice->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

	double slice[3];
	slice[0] = slice[1] = 0.0;
	slice[2] = -301.0;

	double normal[3];
	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 1;
	pipePolylineSlice->SetSlice(slice, normal);

	// TEST
	//////////////////////////////////////////////

	for (int i = 0; i < NUMBER_OF_TEST; i++)
	{
		if (i == TEST_RADIUS)
		{
			pipePolylineSlice->SetRepresentation(1);
			pipePolylineSlice->SetRadius(10);
			pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

			actorList->InitTraversal();
			vtkProp *actor = actorList->GetNextProp();
			while (actor)
			{
				m_Renderer->AddActor(actor);
				m_RenderWindow->Render();

				actor = actorList->GetNextProp();
			}

			COMPARE_IMAGES("TestPipeExecution_TestRadius");

			m_Renderer->RemoveAllProps();
		}
		else if (i == TEST_THICKNESS)
		{
			pipePolylineSlice->SetThickness(10);
			pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

			actorList->InitTraversal();
			vtkProp *actor = actorList->GetNextProp();
			while (actor)
			{
				m_Renderer->AddActor(actor);
				m_RenderWindow->Render();

				actor = actorList->GetNextProp();
			}

			COMPARE_IMAGES("TestPipeExecution_TestThickness");

			m_Renderer->RemoveAllProps();
		}
		else if (i == TEST_SPHERES)
		{
			pipePolylineSlice->SetThickness(1);
			pipePolylineSlice->SetRadius(1);
			pipePolylineSlice->SetSphereRadius(3);
			pipePolylineSlice->ShowSpheres(true);
			pipePolylineSlice->UpdateProperty();

			pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

			actorList->InitTraversal();
			vtkProp *actor = actorList->GetNextProp();
			while (actor)
			{
				m_Renderer->AddActor(actor);
				m_RenderWindow->Render();

				actor = actorList->GetNextProp();
			}

			COMPARE_IMAGES("TestPipeExecution_TestSpheres");

			m_Renderer->RemoveAllProps();
		}
		else if (i == TEST_RESOLUTION)
		{
			pipePolylineSlice->SetTubeResolution(100);
			pipePolylineSlice->SetSphereResolution(100);
			pipePolylineSlice->UpdateProperty();

			pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

			actorList->InitTraversal();
			vtkProp *actor = actorList->GetNextProp();
			while (actor)
			{
				m_Renderer->AddActor(actor);
				m_RenderWindow->Render();

				actor = actorList->GetNextProp();
			}

			COMPARE_IMAGES("TestPipeExecution_TestResolution");

			m_Renderer->RemoveAllProps();
		}
		else if (i == TEST_FILL)
		{
			pipePolylineSlice->SetFill(true);
			pipePolylineSlice->UpdateProperty();

			pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

			actorList->InitTraversal();
			vtkProp *actor = actorList->GetNextProp();
			while (actor)
			{
				m_Renderer->AddActor(actor);
				m_RenderWindow->Render();

				actor = actorList->GetNextProp();
			}

			COMPARE_IMAGES("TestPipeExecution_TestFill");

			m_Renderer->RemoveAllProps();
		}
	}

	vtkDEL(actorList);
	delete sceneNode;
}