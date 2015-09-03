/*=========================================================================

 Program: MAF2
 Module: mafTransformFrameTest
 Authors: Stefano Perticoni
 
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
#include "mafTransformFrameTest.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafTransformBase.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"
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
void mafTransformFrameTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::BeforeTest()
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

  m_InputMatrix = mafMatrix::New();
  m_InputMatrix->SetVTKMatrix(m_InputFrameTransform->GetMatrix());

  m_InputFrameMatrix = mafMatrix::New();
  m_InputFrameMatrix->SetVTKMatrix(m_InputFrameTransform->GetMatrix());

  //create target frame transform
  m_TargetFrameTransform = vtkTransform::New();
  m_TargetFrameTransform->RotateX(30);
  m_TargetFrameTransform->RotateY(60);
  m_TargetFrameTransform->RotateZ(70);

  cerr << "#### Target Frame refsys matrix ####" << std::endl; 
  m_TargetFrameTransform->GetMatrix()->Print(cerr);

  m_TargetFrameMatrix = mafMatrix::New();
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
void mafTransformFrameTest::AfterTest()
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
void mafTransformFrameTest::TestMafTransformFrameConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetInputMafTransformBase()
//----------------------------------------------------------------------------
{
  mafTransformFrame* inputTransformFrame;
  inputTransformFrame = mafTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("mafTransformBase"));

  mafTransformFrame* transformFrame = NULL;
  
  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  transformFrame->SetInput(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInput() == inputTransformFrame);

  transformFrame->Delete();
  
  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetInputMafMatrix()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_InputFrameMatrix->IsA("mafMatrix"));

  transformFrame->SetInput(m_InputFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetInput()->GetMatrix().Equals(m_InputFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestGetInput()
//----------------------------------------------------------------------------
{
  mafTransformFrame* inputTransformFrame;
  inputTransformFrame = mafTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("mafTransformBase"));

  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInput() == NULL);

  transformFrame->SetInput(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInput() == inputTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetInputFrameMafMatrix()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_InputFrameMatrix->IsA("mafMatrix"));

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->SetInputFrame(m_InputFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame()->GetMatrix().Equals(m_InputFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetInputFrameMafTransformBase()
//----------------------------------------------------------------------------
{
  mafTransformFrame* inputTransformFrame;
  inputTransformFrame = mafTransformFrame::New();

  CPPUNIT_ASSERT(inputTransformFrame->IsA("mafTransformBase"));

  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->SetInputFrame(inputTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == inputTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestGetInputFrame()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetInputFrame() == NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetTargetFrameMafMatrix()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(m_TargetFrameMatrix->IsA("mafMatrix"));

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->SetTargetFrame(m_TargetFrameMatrix);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame()->GetMatrix().Equals(m_TargetFrameMatrix));

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestSetTargetFrameMafTransformBase()
//----------------------------------------------------------------------------
{
  mafTransformFrame* targetTransformFrame;
  targetTransformFrame = mafTransformFrame::New();

  CPPUNIT_ASSERT(targetTransformFrame->IsA("mafTransformBase"));

  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->SetTargetFrame(targetTransformFrame);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == targetTransformFrame);

  transformFrame->Delete();

  // no need to destroy this since it is registered with transformFrame
  // inputTransformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestGetTargetFrame()
//----------------------------------------------------------------------------
{
  mafTransformFrame* transformFrame = NULL;

  transformFrame = mafTransformFrame::New();
  CPPUNIT_ASSERT(transformFrame != NULL);

  CPPUNIT_ASSERT(transformFrame->GetTargetFrame() == NULL);

  transformFrame->Delete();
}

//----------------------------------------------------------------------------
void mafTransformFrameTest::TestGetMTime()
//----------------------------------------------------------------------------
{
  mafTransformFrame *inputMatrixInTargetRefSys = mafTransformFrame::New();

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
void mafTransformFrameTest::TestMatrixTransformationFromInputFrameToTargetFrameWithRendering()
//-------------------------------------------------------------------------
{
  //create mafTransformFrame
  mafTransformFrame *inputMatrixInTargetRefSys = mafTransformFrame::New();
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

  inputFrameMapper->SetInput(inputFrameAxes->GetOutput());
  targetFrameMapper->SetInput(targeFrameAxes->GetOutput());
  cubeMapper->SetInput(cube->GetOutput());
  coneMapper->SetInput(cone->GetOutput());

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

  // Cube represents input matrix in target frame from mafTransformFrame
  vtkMatrix4x4::Multiply4x4(m_TargetFrameMatrix->GetVTKMatrix(), inputMatrixInTargetRefSys->GetMatrix().GetVTKMatrix(), cubeAbsPose);
  cubeActor->SetUserMatrix(cubeAbsPose);

  //  absolute pose must be the same for cone and cube
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(mafEquals(coneAbsPose->GetElement(r,c), cubeAbsPose->GetElement(r,c)));
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

  //mafSleep(200);

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
