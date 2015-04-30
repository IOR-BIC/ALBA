/*=========================================================================

 Program: MAF2
 Module: mafPipeTensorFieldTest
 Authors: Roberto Mucci
 
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
#include "mafPipeTensorFieldTest.h"
#include "mafPipeTensorField.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEStorage.h"

#include "vtkCamera.h"
#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPoints.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"
#include "vtkProp3DCollection.h"

class DummyPipeTensorField : public mafPipeTensorField
{
public:
  mafTypeMacro(DummyPipeTensorField,mafPipeTensorField);
  DummyPipeTensorField(){}
  /*virtual*/ void Create(mafSceneNode *n);

protected:
  // redefinition for avoiding gui components
  /*virtual*/ mafGUI *CreateGui() {return NULL;}
  /*virtual*/ void    ComputeDefaultParameters(){};
  /*virtual*/ void    CreateVTKPipe(){};
  /*virtual*/ void    UpdateVTKPipe(){};
};

//----------------------------------------------------------------------------
void DummyPipeTensorField::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);  

  ComputeDefaultParameters();

  if ((m_ScalarFieldIndex = GetScalarFieldIndex()) < 0)          
    m_ScalarFieldIndex = 0; //no default tensor field, use the first one  

  int nTensors = GetNumberOfTensors() > 0;
  if (nTensors > 0)
  {
    //detect the default one tensor field
    if ((m_TensorFieldIndex = GetTensorFieldIndex("velocity")) < 0)          
      m_TensorFieldIndex = 0; //no default tensor field, use the first one         
  }

  if (nTensors > 0 || m_BCreateVTKPipeAlways)
  {
    //now let us create VTK pipe
    CreateVTKPipe();  
    UpdateVTKPipe();  //and set visual properties as needed  
  }
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(DummyPipeTensorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafPipeTensorFieldTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeTensorFieldTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void mafPipeTensorFieldTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void mafPipeTensorFieldTest::TestCreate()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  vtkCamera *cam = m_Renderer->GetActiveCamera();

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  //////////////////////////////////////////////////////////////////////////

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");


  vtkMAFSmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("velocity");


  int x,y,z;
  x = y = z = 10;
  vtkMAFSmartPointer<vtkStructuredPoints> image;
  double origin[3] = {0.0,0.0, -45.};
  image->SetOrigin(origin);
  image->SetDimensions(x,y,z);
  image->SetSpacing(10.,10.,10.);

  int i = 0, size = x*y*z;
  for(;i<size;i++){
    scalarArray->InsertNextTuple1(1.0);
    vectorArray->InsertNextTuple3(1.0,2.0,3.0);
    tensorArray->InsertNextTuple9((float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX);
  }

  image->GetPointData()->AddArray(vectorArray);
  image->GetPointData()->AddArray(tensorArray);

  image->GetPointData()->SetScalars(scalarArray);
  image->GetPointData()->GetTensors("velocity")->Modified();
  image->GetPointData()->Update();

  image->GetPointData()->SetActiveTensors("velocity");

  image->Update();

  double bb[6];
  image->GetBounds(bb);

  volume->SetDataByReference(image, 0.);
  volume->Update();


  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,volume, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeTensorField *pipe = new DummyPipeTensorField;
  pipe->Create(sceneNode);

  double focalPoint[3] = {45.,45.,0.};
  double position[3] = {45.,45.,100.};

  cam->SetFocalPoint(focalPoint);
  cam->SetPosition(position);

  ////////// ACTORS List ///////////////
  vtkProp3DCollection *actorList = pipe->GetAssemblyFront()->GetParts();

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  delete pipe;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;
  delete(rootscenenode);

  volume->ReparentTo(NULL);
  mafDEL(volume);

  mafDEL(storage);
}
