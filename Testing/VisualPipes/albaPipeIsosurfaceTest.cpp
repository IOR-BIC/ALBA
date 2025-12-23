/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurfaceTest
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
#include "albaPipeIsosurfaceTest.h"
#include "albaPipeIsosurface.h"

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
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>
//----------------------------------------------------------------------------
void albaPipeIsosurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeIsosurfaceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
	vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeIsosurface/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, m_Renderer);

  double scalarValue[2] = {1.0,0.1};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeIsosurface *pipeIso = new albaPipeIsosurface;
    pipeIso->Create(sceneNode);
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

    printf("\n Visualization: \n");

		COMPARE_IMAGES("TestPipeExecution", v);

    m_Renderer->RemoveAllViewProps();
		sceneNode->DeletePipe();
  }

  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(Importer);
}