/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeVolumeRayCastingTest
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

#include "albaVisualPipeVolumeRayCastingTest.h"
#include "albaVisualPipeVolumeRayCasting.h"

#include "mmaMaterial.h"
#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkVolume.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>
#include "vtkImageData.h"

//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCastingTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCastingTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCastingTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCastingTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////

  vtkDataSetReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/VTK_Volumes/Volume.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->GetOutput()->Update();
  volume->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volume->GetOutput())->SetMaterial(material);

  vtkDEL(importer);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaVisualPipeVolumeRayCasting *pipe = new albaVisualPipeVolumeRayCasting;

  pipe->Create(sceneNode);
  pipe->m_RenFront = m_Renderer;

  ////////// ACTORS List ///////////////

  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetVolumes(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    actor = actorList->GetNextProp();
  }

  vtkVolume *volumeActor;
  volumeActor = (vtkVolume *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(volumeActor != NULL);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution", 1);
	
  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(material);
  albaDEL(volume);
}

//----------------------------------------------------------------------------
vtkProp *albaVisualPipeVolumeRayCastingTest::SelectActorToControl(vtkPropCollection *propList, int index)
//----------------------------------------------------------------------------
{
  propList->InitTraversal();
  vtkProp *actor = propList->GetNextProp();
  int count = 0;
  while(actor)
  {   
    if(count == index) return actor;
    actor = propList->GetNextProp();
    count ++;
  }
  return NULL;
}
