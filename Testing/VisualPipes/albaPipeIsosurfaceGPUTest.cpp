/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurfaceGPUTest
 Authors: Daniele Giunchi
 
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
#include "albaPipeIsosurfaceGPUTest.h"
#include "albaPipeIsosurfaceGPU.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBAAssembly.h"

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
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

enum ID_TEST_FUNCTIONS
{
  ID_CONTOUR_COMPARING = 0,
  ID_OPACITY_COMPARING = 2,
};

//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::TestPipeExecutionCountour()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double scalarValue[2] = {1.0,0.0};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeIsosurfaceGPU *pipeIso = new albaPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&albaEvent(this, albaPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(scalarValue[v]);

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();
    
		COMPARE_IMAGES("TestPipeExecutionCountour", v + ID_CONTOUR_COMPARING);

    m_Renderer->RemoveAllProps();
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::TestPipeExecutionOpacity()
//----------------------------------------------------------------------------
{

  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeIsosurfaceGPU *pipeIso = new albaPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&albaEvent(this, albaPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }

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

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceGPUTest::TestExtractIsosurface()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeIsosurfaceGPU *pipeIso = new albaPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&albaEvent(this, albaPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }

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

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(Importer);
}