/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTensorFieldTest
 Authors: Roberto Mucci
 
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
#include "albaPipeTensorFieldTest.h"
#include "albaPipeTensorField.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEStorage.h"

#include "vtkCamera.h"
#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageData.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"
#include "vtkProp3DCollection.h"

class DummyPipeTensorField : public albaPipeTensorField
{
public:
  albaTypeMacro(DummyPipeTensorField,albaPipeTensorField);
  DummyPipeTensorField(){}
  /*virtual*/ void Create(albaSceneNode *n);

protected:
  // redefinition for avoiding gui components
  /*virtual*/ albaGUI *CreateGui() {return NULL;}
  /*virtual*/ void    ComputeDefaultParameters(){};
  /*virtual*/ void    CreateVTKPipe(){};
  /*virtual*/ void    UpdateVTKPipe(){};
};

//----------------------------------------------------------------------------
void DummyPipeTensorField::Create(albaSceneNode *n)
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
albaCxxTypeMacro(DummyPipeTensorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaPipeTensorFieldTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeTensorFieldTest::BeforeTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeTensorFieldTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeTensorFieldTest::TestCreate()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  InitializeRenderWindow();

	vtkCamera *cam = m_Renderer->GetActiveCamera();

  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  vtkALBASmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");

  vtkALBASmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");


  vtkALBASmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("velocity");


  int x,y,z;
  x = y = z = 10;
  vtkALBASmartPointer<vtkImageData> image;
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


  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  albaSceneNode *sceneNode = new albaSceneNode(NULL,rootscenenode,volume, m_Renderer);

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
    actor = actorList->GetNextProp();
  }

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
  COMPARE_IMAGES("TestCreate");

  delete sceneNode;
  delete(rootscenenode);

  volume->ReparentTo(NULL);
  albaDEL(volume);

  albaDEL(storage);
}
