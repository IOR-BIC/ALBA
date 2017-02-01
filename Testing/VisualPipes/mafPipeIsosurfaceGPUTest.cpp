/*=========================================================================

 Program: MAF2
 Module: mafPipeIsosurfaceGPUTest
 Authors: Daniele Giunchi
 
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
#include "mafPipeIsosurfaceGPUTest.h"
#include "mafPipeIsosurfaceGPU.h"

#include "mafSceneNode.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkStructuredPointsReader.h"
#include "vtkCamera.h"
#include "vtkRectilinearGrid.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum ID_TEST_FUNCTIONS
{
  ID_CONTOUR_COMPARING = 0,
  ID_OPACITY_COMPARING = 2,
};

//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestPipeExecutionCountour()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double scalarValue[2] = {1.0,0.0};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(scalarValue[v]);

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();
    
    printf("\n Visualization: \n");

		COMPARE_IMAGES("TestPipeExecutionCountour", v + ID_CONTOUR_COMPARING);

    m_Renderer->RemoveAllProps();
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestPipeExecutionOpacity()
//----------------------------------------------------------------------------
{

  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();

    printf("\n Visualization: \n");

		COMPARE_IMAGES("TestPipeExecutionOpacity", v + ID_OPACITY_COMPARING);

    m_Renderer->RemoveAllProps();
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestExtractIsosurface()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();

    printf("\n Visualization and Isosurface Extraction. \n");
    pipeIso->ExctractIsosurface();

    vtkPolyData *isoSurface = vtkPolyData::SafeDownCast(volumeInput->GetChild(0)->GetOutput()->GetVTKData());
    isoSurface->Update();

    CPPUNIT_ASSERT(isoSurface && isoSurface->GetNumberOfPoints() != 0);

    m_Renderer->RemoveAllProps();
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);
}