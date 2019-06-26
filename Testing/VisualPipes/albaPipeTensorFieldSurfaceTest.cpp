/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTensorFieldSurfaceTest
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
#include "albaPipeTensorFieldSurfaceTest.h"
#include "albaPipeTensorFieldSurface.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEVolumeGray.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"

#include <iostream>
#include <fstream>


//----------------------------------------------------------------------------
void albaPipeTensorFieldSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeTensorFieldSurfaceTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);
	vtkNEW(m_RenderWindowInteractor);

	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);
}
//----------------------------------------------------------------------------
void albaPipeTensorFieldSurfaceTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
	vtkDEL(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void albaPipeTensorFieldSurfaceTest::TestCreate()
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
	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  //////////////////////////////////////////////////////////////////////////

  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  vtkALBASmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");
  scalarArray->InsertNextTuple1(1.0);

  vtkALBASmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");
  vectorArray->InsertNextTuple3(1.0,1.0,1.0);

  vtkALBASmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("Tensor");
  tensorArray->InsertNextTuple9(1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);

  vtkALBASmartPointer<vtkImageData> image;
  image->SetDimensions(10,10,10);
  image->SetSpacing(1.,1.,1.);

  image->GetPointData()->SetScalars(scalarArray);

  volume->SetDataByReference(image, 0.);
  volume->Update();

  volume->GetOutput()->GetVTKData();
  volume->GetOutput()->GetVTKData()->GetPointData();
  volume->GetOutput()->GetVTKData()->GetPointData()->AddArray(vectorArray);
  volume->GetOutput()->GetVTKData()->GetPointData()->AddArray(tensorArray);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  albaSceneNode *sceneNode = new albaSceneNode(NULL,rootscenenode,volume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeTensorFieldSurface *pipe = new albaPipeTensorFieldSurface;
  pipe->Create(sceneNode);
	
  m_RenderWindow->Render();

	COMPARE_IMAGES("TestCreate");

  delete sceneNode;
  delete(rootscenenode);

  vtkDEL(frontRenderer);
	
  volume->ReparentTo(NULL);
  albaDEL(volume);

  albaDEL(storage);

}
