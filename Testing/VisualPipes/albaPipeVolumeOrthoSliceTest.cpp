/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeOrthoSliceTest
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
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
#include "albaPipeVolumeOrthoSliceTest.h"
#include "albaPipeVolumeOrthoSlice.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "albaGUILutPreset.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkStructuredPointsReader.h"
#include "vtkCamera.h"
#include "vtkLookupTable.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>
#include <fstream>
#include "vtkImageData.h"

enum PIPE_BOX_ACTORS
{
  PIPE_BOX_ACTOR,
  PIPE_BOX_ACTOR_WIRED,
  PIPE_BOX_ACTOR_OUTLINE_CORNER,
  PIPE_BOX_NUMBER_OF_ACTORS,
};

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
}

enum ID_TEST
{
	ID_TEST_PIPEEXECUTION = 0,
	ID_TEST_PIPEEXECUTION_SLICEOPACITY = 10,
	ID_TEST_PIPEEXECUTION_LUTRANGE = 20,
	ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE = 30,
	ID_TEST_PIPEEXECUTION_TICKS = 40,
	ID_TEST_PIPEEXECUTION_SHOWSLIDER = 50,
	ID_TEST_PIPEEXECUTION_INTERPOLATION = 60,
};

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = albaPipeVolumeOrthoSlice::SLICE_X ; direction<=albaPipeVolumeOrthoSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case albaPipeVolumeOrthoSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);
			
      char *strings="Slice";
      printf("\n Visualization: %s \n", strings);

			m_Renderer->ResetCamera();
			m_RenderWindow->Render();
      COMPARE_IMAGES("TestPipeExecution", 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution_SetSliceOpacity()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = albaPipeVolumeOrthoSlice::SLICE_X ; direction<=albaPipeVolumeOrthoSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double opacity = 0.5;
    //opacity = rand()/RAND_MAX;

    pipeSlice->SetSliceOpacity(opacity);

    CPPUNIT_ASSERT(opacity == pipeSlice->GetSliceOpacity());

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case albaPipeVolumeOrthoSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";
			printf("\n Visualization: %s \n", strings);

			m_Renderer->ResetCamera();
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestPipeExecution_SetSliceOpacity", ID_TEST_PIPEEXECUTION_SLICEOPACITY + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution_SetLutRange()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = albaPipeVolumeOrthoSlice::SLICE_X ; direction<=albaPipeVolumeOrthoSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double range[2], range_out[2];
    range[0] = 0.2;
    range[1] = 0.5;

    pipeSlice->SetLutRange(range[0], range[1]);
    pipeSlice->GetLutRange(range_out);

    CPPUNIT_ASSERT(range[0] == range_out[0] && range[1] == range_out[1]);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case albaPipeVolumeOrthoSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);

			m_Renderer->ResetCamera();

      char *strings="Slice";
			printf("\n Visualization: %s \n", strings);

			m_Renderer->ResetCamera();
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestPipeExecution_SetLutRange", ID_TEST_PIPEEXECUTION_LUTRANGE + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution_SetInterpolation()
{
	////// Create VME (import vtkData) ////////////////////
	vtkStructuredPointsReader *importer;
	vtkNEW(importer);
	albaString filename1 = ALBA_DATA_ROOT;
	filename1 << "/Test_PipeVolumeSlice/VolumeSP.vtk";
	importer->SetFileName(filename1.GetCStr());
	importer->Update();

	albaVMEVolumeGray *volumeInput;
	albaNEW(volumeInput);
	volumeInput->SetData((vtkImageData*)importer->GetOutput(), 0.0);
	volumeInput->GetOutput()->GetVTKData()->Update();
	volumeInput->GetOutput()->Update();
	volumeInput->Update();

	mmaVolumeMaterial *material;
	albaNEW(material);

	albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, volumeInput, NULL);

	double zValue[3][3] = { { 4.0,4.0,0.0 },{ 4.0,4.0,1.0 },{ 4.0,4.0,2.0 } };

	for (int direction = albaPipeVolumeOrthoSlice::SLICE_X; direction <= albaPipeVolumeOrthoSlice::SLICE_Z; direction++)
	{
		/////////// Pipe Instance and Creation ///////////
		albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
		pipeSlice->InitializeSliceParameters(direction, zValue[0], true);
		pipeSlice->Create(sceneNode);
		pipeSlice->SetInterpolationOff();

		double range[2], range_out[2];
		range[0] = 0.2;
		range[1] = 0.5;

		pipeSlice->SetLutRange(range[0], range[1]);
		pipeSlice->GetLutRange(range_out);

		CPPUNIT_ASSERT(range[0] == range_out[0] && range[1] == range_out[1]);

		////////// ACTORS List ///////////////
		vtkPropCollection *actorList = vtkPropCollection::New();
		pipeSlice->GetAssemblyFront()->GetActors(actorList);

		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		while (actor)
		{
			m_Renderer->AddActor(actor);
			actor = actorList->GetNextProp();
		}
		double x, y, z, vx, vy, vz;
		switch (direction)
		{
			case albaPipeVolumeOrthoSlice::SLICE_X:
			{
				//x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
				x = 1; y = 0; z = 0; vx = 0; vy = 0; vz = 1;
			}
			break;
			case albaPipeVolumeOrthoSlice::SLICE_Y:
			{
				x = 0; y = -1; z = 0; vx = 0; vy = 0; vz = 1;
			}
			break;
			case albaPipeVolumeOrthoSlice::SLICE_Z:
			{
				//x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
				x = 0; y = 0; z = -1; vx = 0; vy = -1; vz = 0;
			}
			break;
		}

		m_Renderer->GetActiveCamera()->ParallelProjectionOn();
		m_Renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
		m_Renderer->GetActiveCamera()->SetPosition(x * 100, y * 100, z * 100);
		m_Renderer->GetActiveCamera()->SetViewUp(vx, vy, vz);
		m_Renderer->GetActiveCamera()->SetClippingRange(0.1, 1000);

		for (int i = 0; i < 3; i++)
		{
			//
			double normals[3];
			normals[0] = 0.0;
			normals[1] = 0.0;
			normals[2] = 0.0;
			pipeSlice->SetSlice(zValue[i], normals);

			m_Renderer->ResetCamera();

			char *strings = "Slice";
			printf("\n Visualization: %s \n", strings);

			m_Renderer->ResetCamera();
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestPipeExecution_SetInterpolation", ID_TEST_PIPEEXECUTION_INTERPOLATION + 3 * direction + i);
		}

		m_Renderer->RemoveAllProps();
		vtkDEL(actorList);
		sceneNode->DeletePipe();
	}

	delete sceneNode;

	albaDEL(material);
	albaDEL(volumeInput);
	vtkDEL(importer);
}


//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution_SetColorLookupTable()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = albaPipeVolumeOrthoSlice::SLICE_X ; direction<=albaPipeVolumeOrthoSlice::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    vtkLookupTable *lut = vtkLookupTable::New();
    lutPreset(6, lut);

    pipeSlice->SetColorLookupTable(lut);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case albaPipeVolumeOrthoSlice::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case albaPipeVolumeOrthoSlice::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";
			printf("\n Visualization: %s \n", strings);

      m_Renderer->ResetCamera();
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestPipeExecution_SetColorLookupTable", ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE + 3 * direction + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    vtkDEL(lut);
		sceneNode->DeletePipe();
  }

  delete sceneNode;
  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(importer);
}

//----------------------------------------------------------------------------
void albaPipeVolumeOrthoSliceTest::TestPipeExecution_TicksOnOff()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  int direction = albaPipeVolumeOrthoSlice::SLICE_X;

  for (int showticks = 0; showticks<2; showticks++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeOrthoSlice *pipeSlice = new albaPipeVolumeOrthoSlice;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);
    if(showticks)
      pipeSlice->ShowTICKsOn();
    else
      pipeSlice->ShowTICKsOff();
		
    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeSlice->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }
    double x,y,z,vx,vy,vz;

    x=1 ;y=0; z=0; vx=0; vy=0; vz=1;

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = i/10;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";
			printf("\n Visualization: %s \n", strings);

      m_Renderer->ResetCamera(); 
      m_RenderWindow->Render();
      COMPARE_IMAGES("TestPipeExecution_TicksOnOff", ID_TEST_PIPEEXECUTION_TICKS + showticks * 3 + i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();
  }  

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(importer);
}