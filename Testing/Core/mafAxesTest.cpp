/*=========================================================================

 Program: MAF2
 Module: mafAxesTest
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

#include "mafAxesTest.h"
#include "mafAxes.h"
#include "mafVMESurface.h"
#include "vtkMAFLocalAxisCoordinate.h"

#include "vtkActor2D.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "vtkSphereSource.h"
#include "vtkCamera.h"

void mafAxesTest::BeforeTest()
{
  m_VMESurfaceSphere = NULL;
  m_VMESurfaceSphere = mafVMESurface::New();
  
  m_Renderer = vtkRenderer::New();

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();
  
  m_VMESurfaceSphere->SetData(sphere->GetOutput(),0);
  vtkDataSet *sphereData = m_VMESurfaceSphere->GetOutput()->GetVTKData();
  CPPUNIT_ASSERT(sphereData);
}

void mafAxesTest::AfterTest()
{
  mafDEL(m_VMESurfaceSphere);
  vtkDEL(m_Renderer);
}

void mafAxesTest::TestFixture()
{

}

void mafAxesTest::RenderData( vtkDataSet *data )
{
  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  
  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(m_Renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);
  
  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkMAFSmartPointer<vtkDataSetMapper> mapper;
  mapper->SetInputData(data);
  mapper->ScalarVisibilityOn();
  
  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetOpacity(0.5);

  m_Renderer->AddActor(actor);
  
  renderWindow->Render();

  renderWindowInteractor->Start();

}

void mafAxesTest::TestRenderData()
{
  
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();
  
  // remove comment for render window interaction
  // RenderData(sphere->GetOutput());
  
}

void mafAxesTest::TestConstructorDestructor()
{
  // This is failing. Renderer cannot be NULL
  // mafAxes *axes = new mafAxes(NULL, NULL);

  mafAxes *axes = new mafAxes(m_Renderer, m_VMESurfaceSphere);
  axes->SetVisibility(true);  

  // remove comment for render window interaction
  // RenderData(m_VMESurfaceSphere->GetOutput()->GetVTKData());

  cppDEL(axes);
  
}

void mafAxesTest::TestSetVisibility()
{
  mafAxes *axes = new mafAxes(m_Renderer, m_VMESurfaceSphere);
  
  // using friend class for testing
  int visibility = axes->m_AxesActor2D->GetVisibility();
  CPPUNIT_ASSERT(visibility == 0);

  axes->SetVisibility(true);
  // using friend class for testing
  visibility = axes->m_AxesActor2D->GetVisibility();
  CPPUNIT_ASSERT(visibility == 1);

  // remove comment for render window interaction
  // RenderData(m_VMESurfaceSphere->GetOutput()->GetVTKData());

  cppDEL(axes);
}

void mafAxesTest::TestSetPose()
{
  mafAxes *axes = new mafAxes(m_Renderer, m_VMESurfaceSphere);
  
  vtkTransform *transform = vtkTransform::New();
  transform->PostMultiply();
  transform->Translate(10,10,10);
  transform->RotateX(90);

  vtkMatrix4x4 *inputMatrix = vtkMatrix4x4::New();
  inputMatrix->DeepCopy(transform->GetMatrix());

  axes->SetPose(inputMatrix);

  // using friend class for testing
  vtkMatrix4x4 *axesMatrix = ((vtkMAFLocalAxisCoordinate*) (axes->m_Coord))->GetMatrix();
  
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(inputMatrix->GetElement(r,c) == axesMatrix->GetElement(r,c));
    }
  }
  
  // remove comment for render window interaction
  // RenderData(m_VMESurfaceSphere->GetOutput()->GetVTKData());

  inputMatrix->Identity();
  
  // TODO: API IMPROVEMENT NEEDED  
  // testing without arguments, anyway this method is quite ambigous.
  axes->SetPose();

  // using friend class for testing
  axesMatrix = ((vtkMAFLocalAxisCoordinate*) (axes->m_Coord))->GetMatrix();

  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(inputMatrix->GetElement(r,c) == axesMatrix->GetElement(r,c));
    }
  }

  // remove comment for render window interaction
  // RenderData(m_VMESurfaceSphere->GetOutput()->GetVTKData());

  vtkDEL(inputMatrix);
  vtkDEL(transform);
  cppDEL(axes);

}