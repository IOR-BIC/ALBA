/*=========================================================================

 Program: MAF2
 Module: mafPipeTensorFieldSurfaceTest
 Authors: Eleonora Mambrini
 
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
#include "mafPipeTensorFieldSurfaceTest.h"
#include "mafPipeTensorFieldSurface.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEVolumeGray.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <fstream>


//----------------------------------------------------------------------------
void mafPipeTensorFieldSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeTensorFieldSurfaceTest::TestCreate()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////

  vtkRenderer *frontRenderer;
  vtkNEW(frontRenderer);
  frontRenderer->SetBackground(0.1, 0.1, 0.1);

  vtkRenderWindow *renderWindow = vtkRenderWindow::New();
  renderWindow->AddRenderer(frontRenderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(200,0);

  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //////////////////////////////////////////////////////////////////////////

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");
  scalarArray->InsertNextTuple1(1.0);

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");
  vectorArray->InsertNextTuple3(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("Tensor");
  tensorArray->InsertNextTuple9(1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetDimensions(10,10,10);
  image->SetSpacing(1.,1.,1.);

  image->GetPointData()->SetScalars(scalarArray);

  volume->SetDataByReference(image, 0.);
  volume->Update();

  volume->GetOutput()->GetVTKData();
  volume->GetOutput()->GetVTKData()->GetPointData();
  volume->GetOutput()->GetVTKData()->GetPointData()->AddArray(vectorArray);
  volume->GetOutput()->GetVTKData()->GetPointData()->AddArray(tensorArray);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,volume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeTensorFieldSurface *pipe = new mafPipeTensorFieldSurface;
  pipe->Create(sceneNode);


  //renderWindow->Render();
  //CompareImages(0);

  delete pipe;
  delete sceneNode;
  delete(rootscenenode);

  vtkDEL(renderWindowInteractor);
  vtkDEL(renderWindow);
  vtkDEL(frontRenderer);


  volume->ReparentTo(NULL);
  mafDEL(volume);

  mafDEL(storage);

}
