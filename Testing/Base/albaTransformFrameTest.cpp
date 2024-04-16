/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformFrameTest
 Authors: Stefano Perticoni
 
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
#include "albaTransformFrameTest.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaTransformFrame.h"
#include "albaTransformBase.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkAxes.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::BeforeTest()
//----------------------------------------------------------------------------
{
  //create input frame transform
  m_InputFrameTransform = vtkTransform::New();
  m_InputFrameTransform->Translate(1, 2, 3);
  m_InputFrameTransform->RotateX(10);
  m_InputFrameTransform->RotateY(20);
  m_InputFrameTransform->RotateZ(30);

  cerr << "#### Input Frame refsys matrix ####" << std::endl; 
  m_InputFrameTransform->GetMatrix()->Print(cerr);

  m_InputMatrix = albaMatrix::New();
  m_InputMatrix->SetVTKMatrix(m_InputFrameTransform->GetMatrix());

  m_InputFrameMatrix = albaMatrix::New();
  m_InputFrameMatrix->SetVTKMatrix(m_InputFrameTransform->GetMatrix());

  //create target frame transform
  m_TargetFrameTransform = vtkTransform::New();
  m_TargetFrameTransform->RotateX(30);
  m_TargetFrameTransform->RotateY(60);
  m_TargetFrameTransform->RotateZ(70);

  cerr << "#### Target Frame refsys matrix ####" << std::endl; 
  m_TargetFrameTransform->GetMatrix()->Print(cerr);

  m_TargetFrameMatrix = albaMatrix::New();
  m_TargetFrameMatrix->SetVTKMatrix(m_TargetFrameTransform->GetMatrix());

  //create input matrix
  m_InputMatrixTransform = vtkTransform::New();
  m_InputMatrixTransform->RotateX(5);
  m_InputMatrixTransform->RotateY(10);
  m_InputMatrixTransform->RotateZ(15);
  m_InputMatrixTransform->Translate(5, 10, 2);

  cerr << "#### Input matrix in Input Frame RefSys ####" << std::endl; 
  m_InputMatrixTransform->GetMatrix()->Print(cerr);
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_InputFrameMatrix);
  vtkDEL(m_TargetFrameMatrix);
  vtkDEL(m_InputFrameTransform);
  vtkDEL(m_InputMatrix);
  vtkDEL(m_TargetFrameTransform);
  vtkDEL(m_InputMatrixTransform);
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestMafTransformFrameConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetInputMafTransformBase()
//----------------------------------------------------------------------------
{
  albaTransformFrame* inputTransformFrame;
  inputTransformFrame = albaTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("albaTransformBase"));

  albaTransformFrame* transformFrame = NULL;
  
  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  transformFrame->SetInput(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInput() == inputTransformFrame);

  transformFrame->Delete();
  
  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetInputMafMatrix()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_InputFrameMatrix->IsA("albaMatrix"));

  transformFrame->SetInput(m_InputFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetInput()->GetMatrix().Equals(m_InputFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestGetInput()
//----------------------------------------------------------------------------
{
  albaTransformFrame* inputTransformFrame;
  inputTransformFrame = albaTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("albaTransformBase"));

  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInput() == NULL);

  transformFrame->SetInput(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInput() == inputTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetInputFrameMafMatrix()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_InputFrameMatrix->IsA("albaMatrix"));

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->SetInputFrame(m_InputFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame()->GetMatrix().Equals(m_InputFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetInputFrameMafTransformBase()
//----------------------------------------------------------------------------
{
  albaTransformFrame* inputTransformFrame;
  inputTransformFrame = albaTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("albaTransformBase"));

  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->SetInputFrame(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == inputTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestGetInputFrame()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetTargetFrameMafMatrix()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_TargetFrameMatrix->IsA("albaMatrix"));

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->SetTargetFrame(m_TargetFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame()->GetMatrix().Equals(m_TargetFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestSetTargetFrameMafTransformBase()
//----------------------------------------------------------------------------
{
  albaTransformFrame* targetTransformFrame;
  targetTransformFrame = albaTransformFrame::New();

  CPPUNIT_ASSERT(targetTransformFrame->IsA("albaTransformBase"));

  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->SetTargetFrame(targetTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == targetTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestGetTargetFrame()
//----------------------------------------------------------------------------
{
  albaTransformFrame* transformFrame = NULL;

  transformFrame = albaTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  albaTransformFrame *inputMatrixInTargetRefSys = albaTransformFrame::New();

  int mtime0 = inputMatrixInTargetRefSys->GetMTime();

  inputMatrixInTargetRefSys->SetInput(m_InputMatrix);
  int mtime1 = inputMatrixInTargetRefSys->GetMTime();
  CPPUNIT_ASSERT(mtime1>mtime0);
  
  inputMatrixInTargetRefSys->SetInputFrame(m_InputFrameMatrix);

  int mtime2 = inputMatrixInTargetRefSys->GetMTime();
  CPPUNIT_ASSERT(mtime2>mtime1);

  inputMatrixInTargetRefSys->SetTargetFrame(m_TargetFrameMatrix);
  
  int mtime3 = inputMatrixInTargetRefSys->GetMTime();
  CPPUNIT_ASSERT(mtime3>mtime2);

  inputMatrixInTargetRefSys->Delete();
}

//----------------------------------------------------------------------------
void albaTransformFrameTest::TestMatrixTransformationFromInputFrameToTargetFrameWithRendering()
//-------------------------------------------------------------------------
{
  //create albaTransformFrame
  albaTransformFrame *inputMatrixInTargetRefSys = albaTransformFrame::New();
  inputMatrixInTargetRefSys->SetInput(m_InputMatrix);
  inputMatrixInTargetRefSys->SetInputFrame(m_InputFrameMatrix);
  inputMatrixInTargetRefSys->SetTargetFrame(m_TargetFrameMatrix);
  inputMatrixInTargetRefSys->Update();

  cerr << "#### Input matrix in Target Frame RefSys ####" << std::endl; 
  inputMatrixInTargetRefSys->GetVTKTransform()->GetMatrix()->Print(cerr);

  cerr << "####### Test cube and cone ########" << std::endl;
  cerr << "" << std::endl;

  // input frame axis 
  vtkAxes *inputFrameAxes = vtkAxes::New();

  // target frame axis
  vtkAxes *targeFrameAxes = vtkAxes::New();

  // sample cube
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(0.5);
  cube->SetYLength(0.5);
  cube->SetZLength(0.5);

  // sample cone
  vtkConeSource *cone = vtkConeSource::New();

  vtkPolyDataMapper *inputFrameMapper = vtkPolyDataMapper::New();
  vtkPolyDataMapper *targetFrameMapper = vtkPolyDataMapper::New();

  vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
  vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();

  inputFrameMapper->SetInputConnection(inputFrameAxes->GetOutputPort());
  targetFrameMapper->SetInputConnection(targeFrameAxes->GetOutputPort());
  cubeMapper->SetInputConnection(cube->GetOutputPort());
  coneMapper->SetInputConnection(cone->GetOutputPort());

  vtkActor *inputFrameActor = vtkActor::New();
  vtkActor *targetFrameActor = vtkActor::New();
  vtkActor *cubeActor = vtkActor::New();
  vtkActor *coneActor = vtkActor::New();

  inputFrameActor->SetMapper(inputFrameMapper);
  targetFrameActor->SetMapper(targetFrameMapper);

  cubeActor->SetMapper(cubeMapper);
  coneActor->SetMapper(coneMapper);

  inputFrameActor->SetUserMatrix(m_InputFrameMatrix->GetVTKMatrix());
  targetFrameActor->SetUserMatrix(m_TargetFrameMatrix->GetVTKMatrix());

  vtkMatrix4x4 *coneAbsPose = vtkMatrix4x4::New();
  vtkMatrix4x4 *cubeAbsPose = vtkMatrix4x4::New();

  // Cone represents input matrix in input frame
  vtkMatrix4x4::Multiply4x4(m_InputFrameMatrix->GetVTKMatrix(), m_InputMatrix->GetVTKMatrix(), coneAbsPose);
  coneActor->SetUserMatrix(coneAbsPose);

  // Cube represents input matrix in target frame from albaTransformFrame
  vtkMatrix4x4::Multiply4x4(m_TargetFrameMatrix->GetVTKMatrix(), inputMatrixInTargetRefSys->GetMatrix().GetVTKMatrix(), cubeAbsPose);
  cubeActor->SetUserMatrix(cubeAbsPose);

  //  absolute pose must be the same for cone and cube
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(albaEquals(coneAbsPose->GetElement(r,c), cubeAbsPose->GetElement(r,c)));
    }
  }

  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(inputFrameActor);
  renderer->AddActor(targetFrameActor);
  renderer->AddActor(cubeActor);
  renderer->AddActor(coneActor);

  vtkRenderWindow *renderWindow = vtkRenderWindow::New();
  renderWindow->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renderWindow);

  vtkInteractorStyleTrackballCamera *interactorStyleTrackballCamera = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(interactorStyleTrackballCamera);

  renderWindow->Render();
  // remove the following comment for user interaction
  // iren->Start();

  //albaSleep(200);

  cerr << std::endl;
  cerr << "#### cone pose in input ref sys ####" << std::endl;

  m_InputMatrixTransform->GetMatrix()->Print(std::cerr);

  cerr << "#### cube pose in target ref sys ####" << std::endl;

  inputMatrixInTargetRefSys->GetVTKTransform()->GetMatrix()->Print(std::cerr);

  cerr << std::cerr.std::cerr();

  vtkDEL(coneAbsPose);
  vtkDEL(cubeAbsPose);
  vtkDEL(inputMatrixInTargetRefSys);
  vtkDEL(inputFrameAxes);
  vtkDEL(targeFrameAxes);
  vtkDEL(cube);
  vtkDEL(cone);
  vtkDEL(inputFrameMapper);
  vtkDEL(targetFrameMapper);
  vtkDEL(coneMapper);
  vtkDEL(cubeMapper);
  vtkDEL(inputFrameActor);
  vtkDEL(targetFrameActor);
  vtkDEL(coneActor);
  vtkDEL(cubeActor);
  vtkDEL(renderer);
  vtkDEL(renderWindow);
  vtkDEL(iren);
  vtkDEL(interactorStyleTrackballCamera);
}
