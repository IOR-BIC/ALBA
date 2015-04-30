/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldTest
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
#include "mafPipeVectorFieldTest.h"
#include "mafPipeVectorField.h"

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

class DummyPipeVectorField : public mafPipeVectorField
{
public:
  mafTypeMacro(DummyPipeVectorField,mafPipeVectorField);
  DummyPipeVectorField(){}
  /*virtual*/ void Create(mafSceneNode *n);

protected:
  // redefinition for avoiding gui components
  /*virtual*/ mafGUI *CreateGui() {return NULL;}
  /*virtual*/ void    ComputeDefaultParameters(){};
  /*virtual*/ void    CreateVTKPipe(){};
  /*virtual*/ void    UpdateVTKPipe(){};
};

//----------------------------------------------------------------------------
void DummyPipeVectorField::Create(mafSceneNode *n)
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
mafCxxTypeMacro(DummyPipeVectorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafPipeVectorFieldTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldTest::TestCreate()
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

  int volumeDimensions[3];
  volumeDimensions[0] = 2;
  volumeDimensions[1] = 2;
  volumeDimensions[2] = 2;

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetNumberOfComponents(1);
  scalarArray->SetName("Scalar");
  //scalarArray->InsertNextTuple1(1.0);

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetDimensions(volumeDimensions[0],volumeDimensions[1],volumeDimensions[2]);
  image->SetSpacing(1.,1.,1.);

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
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

  mafVMEOutput *output = volume->GetOutput();
  output->Update();
  vtkPointData *pointData = output->GetVTKData()->GetPointData();
  pointData->Update();

  pointData->AddArray(vectorArray);
  pointData->SetActiveVectors("Vector");

  double bb[6];
  image->GetBounds(bb);

  volume->SetDataByReference(image, 0.);
  volume->Update();


  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,volume, m_Renderer);

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