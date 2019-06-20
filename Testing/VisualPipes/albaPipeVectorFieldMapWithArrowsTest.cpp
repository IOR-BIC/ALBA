/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldMapWithArrowsTest
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
#include "albaPipeVectorFieldMapWithArrowsTest.h"
#include "albaPipeVectorFieldMapWithArrows.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEVolumeGray.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrowsTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrowsTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);
}
//----------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrowsTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrowsTest::TestCreate()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////

  vtkRenderer *frontRenderer;
  vtkNEW(frontRenderer);
  frontRenderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(frontRenderer);
	
  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  //////////////////////////////////////////////////////////////////////////

  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  int volumeDimensions[3];
  volumeDimensions[0] = 2;
  volumeDimensions[1] = 2;
  volumeDimensions[2] = 2;

  vtkALBASmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetNumberOfComponents(1);
  scalarArray->SetName("Scalar");
  //scalarArray->InsertNextTuple1(1.0);

  vtkALBASmartPointer<vtkImageData> image;
  image->SetDimensions(volumeDimensions[0],volumeDimensions[1],volumeDimensions[2]);
  image->SetSpacing(1.,1.,1.);

  vtkALBASmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);
  vectorArray->SetName("Vector");

  for(int i=0;i< volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2];i++)
  {
    scalarArray->InsertNextTuple1(i%3);
    vectorArray->SetTuple3(i, i, i, i);
  }

  image->GetPointData()->SetScalars(scalarArray);
  image->GetPointData()->SetActiveScalars("Scalar");
  
  volume->SetData(image, 0.);
  volume->Update();

  albaVMEOutput *output = volume->GetOutput();
  output->Update();
  vtkPointData *pointData = output->GetVTKData()->GetPointData();
  pointData->Update();

  pointData->AddArray(vectorArray);
  pointData->SetActiveVectors("Vector");

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  albaSceneNode *sceneNode = new albaSceneNode(NULL,rootscenenode,volume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeVectorFieldMapWithArrows *pipe = new albaPipeVectorFieldMapWithArrows();
  pipe->Create(sceneNode);
  pipe->m_RenFront = frontRenderer;

  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    frontRenderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_RenderWindow->Render();

	COMPARE_IMAGES("TestCreate");

  vtkDEL(actorList);

  delete sceneNode;
  delete(rootscenenode);

  vtkDEL(renderWindowInteractor);
  vtkDEL(frontRenderer);

  volume->ReparentTo(NULL);
  albaDEL(volume);
  albaDEL(storage);
}