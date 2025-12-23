/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldTest
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
#include "albaPipeVectorFieldTest.h"
#include "albaPipeVectorField.h"

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

class DummyPipeVectorField : public albaPipeVectorField
{
public:
  albaTypeMacro(DummyPipeVectorField,albaPipeVectorField);
  DummyPipeVectorField(){}
  /*virtual*/ void Create(albaSceneNode *n);

protected:
  // redefinition for avoiding gui components
  /*virtual*/ albaGUI *CreateGui() {return NULL;}
  /*virtual*/ void    ComputeDefaultParameters(){};
  /*virtual*/ void    CreateVTKPipe(){};
  /*virtual*/ void    UpdateVTKPipe(){};
};

//----------------------------------------------------------------------------
void DummyPipeVectorField::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);  

  ComputeDefaultParameters();

  if ((m_ScalarFieldIndex = GetScalarFieldIndex()) < 0)          
    m_ScalarFieldIndex = 0; //no default tensor field, use the first one  

  int nVectors = GetNumberOfVectors() > 0;
  if (nVectors > 0)
  {
    //detect the default one tensor field
    if ((m_VectorFieldIndex = GetVectorFieldIndex("Vector")) < 0)          
      m_VectorFieldIndex = 0; //no default tensor field, use the first one         
  }

  if (nVectors > 0 || m_BCreateVTKPipeAlways)
  {
    //now let us create VTK pipe
    CreateVTKPipe();  
    UpdateVTKPipe();  //and set visual properties as needed  
  }
}

//----------------------------------------------------------------------------
albaCxxTypeMacro(DummyPipeVectorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaPipeVectorFieldTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldTest::BeforeTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldTest::TestCreate()
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

  double bb[6];
  image->GetBounds(bb);

  volume->SetDataByReference(image, 0.);
  volume->Update();


  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *rootscenenode = new albaSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  albaSceneNode *sceneNode = new albaSceneNode(NULL,rootscenenode,volume, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeVectorField *pipe = new DummyPipeVectorField;
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