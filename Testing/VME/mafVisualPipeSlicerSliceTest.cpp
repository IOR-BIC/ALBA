/*=========================================================================

 Program: MAF2
 Module: mafVisualPipeSlicerSliceTest
 Authors: Simone Brazzale
 
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
#include "mafVisualPipeSlicerSliceTest.h"

#include "mafVisualPipeSlicerSlice.h"
#include "mafSceneNode.h"
#include "mafOpImporterVTK.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESlicer.h"
#include "mmaMaterial.h"

#include "vtkPointData.h"
#include "vtkMAFAssembly.h"

//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(400, 0);
	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  /////// Create VME (import vtkData) ////////////////////
  mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	mafVMERoot *root=storage->GetRoot();

	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

  mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->ReparentTo(root);
	volume->Update();

	double centerV[3], rotV[3];
	mafOBB b;
	rotV[0] = rotV[1] = rotV[2] = 0;
	volume->GetOutput()->GetVMELocalBounds(b);
	b.GetCenter(centerV);

	mafVMESlicer *slicer;
	mafNEW(slicer);

	//Setting standard material to avoid random color selection
	slicer->GetMaterial()->m_Diffuse[0]=0.3;
	slicer->GetMaterial()->m_Diffuse[1]=0.6;
	slicer->GetMaterial()->m_Diffuse[2]=0.9;
	slicer->GetMaterial()->UpdateProp();

	slicer->ReparentTo(volume);
  slicer->SetSlicedVMELink(volume);

	slicer->SetPose(centerV,rotV,0);
	slicer->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL, rootscenenode, slicer, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafVisualPipeSlicerSlice *pipe = new mafVisualPipeSlicerSlice();
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
    m_RenderWindow->Render();
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
  
  mafDEL(slicer);
  cppDEL(importer);
  mafDEL(storage);
}

//----------------------------------------------------------------------------
vtkProp *mafVisualPipeSlicerSliceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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