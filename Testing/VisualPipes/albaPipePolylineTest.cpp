/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePolylineTest
 Authors: Matteo Giacomoni, Nicola Vanella
 
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
#include "albaPipePolylineTest.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaPipePolyline.h"
#include "albaVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include <iostream>
#include "vtkStructuredPointsReader.h"
#include "albaVMEVolumeGray.h"

enum TESTS_PIPE_SURFACE
{
  TEST_COLOR,
  TEST_POLYLINE,
  TEST_SCALAR,
  TEST_POLYLINE_SPLINE_MODE,
  TEST_TUBE,
  TEST_TUBE_SPLINE_MODE,
  TEST_TUBE_CHANGE_VALUES,
  TEST_TUBE_CAPPING,
  TEST_GLYPH,
  TEST_GLYPH_CHANGE_VALUES,
  TEST_GLYPH_UNCONNECTED,
  TEST_GLYPH_SPLINE_MODE,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipePolylineTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipePolylineTest::BeforeTest()
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
void albaPipePolylineTest::AfterTest()
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipePolylineTest::TestPipeExecution()
{
	////// Create VME (import vtkData) ////////////////////
	vtkALBASmartPointer<vtkDataSetReader> importer;
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_PipePolyline/polyline.vtk";
	importer->SetFileName(filename);
	importer->Update();
	albaSmartPointer<albaVMEPolyline> polyline;
	polyline->SetData((vtkPolyData*)importer->GetOutput(), 0.0);
	polyline->GetOutput()->Update();
	polyline->GetMaterial();
	polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	polyline->Update();

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, polyline, NULL);

	/////////// Pipe Instance and Creation ///////////
	albaPipePolyline *pipePolyline = new albaPipePolyline;
	pipePolyline->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();

	for (int i = 0; i < NUMBER_OF_TEST; i++)
	{
		switch (i)
		{
		case TEST_COLOR:
		{
			double col[3] = { 1.0,0.0,0.0 };
			pipePolyline->SetColor(col);
		}
		break;
		case TEST_POLYLINE:
		{
			pipePolyline->SetRepresentation(albaPipePolyline::LINES);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_POLYLINE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE:
		{
			pipePolyline->SetSplineMode(false);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
			pipePolyline->SetRepresentation(albaPipePolyline::TUBES);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_TUBE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetTubeResolution(5.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_RESOLUTION));
		}
		break;
		case TEST_TUBE_CAPPING:
		{
			pipePolyline->SetTubeCapping(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_CAPPING));
		}
		break;
		case TEST_GLYPH:
		{
			pipePolyline->SetRepresentation(albaPipePolyline::SPHERES);
			pipePolyline->SetShowSphere(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_GLYPH_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetGlyphResolution(5.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RESOLUTION));
		}
		break;
		case TEST_GLYPH_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
			pipePolyline->SetRadius(1.0);
			pipePolyline->SetGlyphResolution(10.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RESOLUTION));
		} 
		break;

		default:
			break;
		}

		pipePolyline->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();

		while (actor)
		{
			m_Renderer->AddActor(actor);
			m_RenderWindow->Render();
			actor = actorList->GetNextProp();
		}

		COMPARE_IMAGES("TestPipeExecution", i);
	}

	vtkDEL(actorList);
	delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipePolylineTest::TestPipeDensityMap()
{
	////// Create VME (import vtkData) ////////////////////
	vtkALBASmartPointer<vtkDataSetReader> importer;
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_PipePolyline/polyline.vtk";
	importer->SetFileName(filename);
	importer->Update();
	albaSmartPointer<albaVMEPolyline> polyline;
	polyline->SetData((vtkPolyData*)importer->GetOutput(), 0.0);
	polyline->GetOutput()->Update();
	polyline->GetMaterial();
	polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	polyline->Update();

	// Create VME (import Volume) ////////////////////
	vtkStructuredPointsReader *volumeImporter;
	vtkNEW(volumeImporter);
	albaString filename1 = ALBA_DATA_ROOT;
	filename1 << "/VTK_Volumes/volume.vtk";
	volumeImporter->SetFileName(filename1.GetCStr());
	volumeImporter->Update();

	albaVMEVolumeGray *volumeInput;
	albaNEW(volumeInput);
	volumeInput->SetData((vtkImageData*)volumeImporter->GetOutput(), 0.0);
	volumeInput->GetOutput()->Update();
	volumeInput->Update();

	vtkDEL(volumeImporter);

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, polyline, NULL);

	/////////// Pipe Instance and Creation ///////////
	albaPipePolyline *pipePolyline = new albaPipePolyline;
	pipePolyline->Create(sceneNode);

	// Enable DensityMap
	//pipePolyline->ManageScalarOnExecutePipe(polyline->GetOutput()->GetVTKData());
	pipePolyline->SetProbeVolume(volumeInput);
	pipePolyline->SetProbeMapActive(true);


	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();

	for (int i = 0; i < NUMBER_OF_TEST; i++)
	{
		switch (i)
		{
		case TEST_COLOR:
		{
			double col[3] = { 1.0,0.0,0.0 };
			pipePolyline->SetColor(col);
		}
		break;
		case TEST_POLYLINE:
		{
			pipePolyline->SetRepresentation(albaPipePolyline::LINES);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_POLYLINE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE:
		{
			pipePolyline->SetSplineMode(false);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
			pipePolyline->SetRepresentation(albaPipePolyline::TUBES);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_TUBE_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
		}
		break;
		case TEST_TUBE_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetTubeResolution(5.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_RESOLUTION));
		}
		break;
		case TEST_TUBE_CAPPING:
		{
			pipePolyline->SetTubeCapping(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_TUBE_CAPPING));
		}
		break;
		case TEST_GLYPH:
		{
			pipePolyline->SetRepresentation(albaPipePolyline::SPHERES);
			pipePolyline->SetShowSphere(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_POLYLINE_REPRESENTATION));
		}
		break;
		case TEST_GLYPH_CHANGE_VALUES:
		{
			pipePolyline->SetRadius(2.0);
			pipePolyline->SetGlyphResolution(5.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RESOLUTION));
		}
		break;
		case TEST_GLYPH_SPLINE_MODE:
		{
			pipePolyline->SetSplineMode(true);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPLINE));
			pipePolyline->SetRadius(1.0);
			pipePolyline->SetGlyphResolution(10.0);
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RADIUS));
			pipePolyline->OnEvent(&albaEvent(this, albaPipePolyline::ID_SPHERE_RESOLUTION));
		}
		break;

		default:
			break;
		}

		pipePolyline->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();

		while (actor)
		{
			m_Renderer->AddActor(actor);
			m_RenderWindow->Render();
			actor = actorList->GetNextProp();
		}

		COMPARE_IMAGES("TestPipeDensityMap", i);
	}

	albaDEL(volumeInput);
	vtkDEL(actorList);
	delete sceneNode;
}