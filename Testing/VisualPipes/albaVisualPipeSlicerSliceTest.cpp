/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeSlicerSliceTest
 Authors: Simone Brazzale
 
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
#include "albaVisualPipeSlicerSliceTest.h"

#include "albaVisualPipeSlicerSlice.h"
#include "albaSceneNode.h"
#include "albaOpImporterVTK.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESlicer.h"
#include "mmaMaterial.h"

#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkALBAAssembly.h"

#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


//----------------------------------------------------------------------------
void albaVisualPipeSlicerSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVisualPipeSlicerSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaVisualPipeSlicerSliceTest::AfterTest()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaVisualPipeSlicerSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  /////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaVMERoot *root=storage->GetRoot();

	albaOpImporterVTK *importer=new albaOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

  albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->ReparentTo(root);
	volume->Update();

	double centerV[3], rotV[3];
	albaOBB b;
	rotV[0] = rotV[1] = rotV[2] = 0;
	volume->GetOutput()->GetVMELocalBounds(b);
	b.GetCenter(centerV);

	albaVMESlicer *slicer;
	albaNEW(slicer);

	//Setting standard material to avoid random color selection
	slicer->GetMaterial()->m_Diffuse[0]=0.3;
	slicer->GetMaterial()->m_Diffuse[1]=0.6;
	slicer->GetMaterial()->m_Diffuse[2]=0.9;
	slicer->GetMaterial()->UpdateProp();

	slicer->ReparentTo(volume);
  slicer->SetSlicedVMELink(volume);

	slicer->SetPose(centerV,rotV,0);
	slicer->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  albaSceneNode *sceneNode = new albaSceneNode(NULL, rootscenenode, slicer, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaVisualPipeSlicerSlice *pipe = new albaVisualPipeSlicerSlice();
  pipe->Create(sceneNode);
  pipe->m_RenFront = m_Renderer;

  CPPUNIT_ASSERT(pipe->GetAssemblyFront()!=NULL);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  vtkActor *meshActor;
  meshActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(meshActor != NULL);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution", 1);

  vtkDEL(actorList);

  cppDEL(sceneNode);
  cppDEL(rootscenenode);

  volume->ReparentTo(NULL);
  slicer->ReparentTo(NULL);
  
  albaDEL(slicer);
  cppDEL(importer);
  albaDEL(storage);
}

//----------------------------------------------------------------------------
vtkProp *albaVisualPipeSlicerSliceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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