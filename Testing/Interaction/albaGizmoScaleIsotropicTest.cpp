/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleIsotropicTest
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

#include "albaGizmoScaleIsotropicTest.h"
#include "albaInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoScaleIsotropic.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"

#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"


void albaGizmoScaleIsotropicTest::BeforeTest()
{
  CreateRenderStuff();
	CreateTestData();
}

void albaGizmoScaleIsotropicTest::AfterTest()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoScaleIsotropicTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  albaNEW(m_GizmoInputSurface);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkALBASmartPointer<vtkTubeFilter> tube;
  tube->SetInputConnection(axes->GetOutputPort());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
	tube->Update();
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoScaleIsotropicTest::TestConstructor()
{
  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);
  
  gizmoScaleIsotropic->SetCubeLength(1);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoScaleIsotropic);

  // remove this comment to enable interaction
  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoScaleIsotropic);

}


void albaGizmoScaleIsotropicTest::TestSetGetCubeLength()
{
  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);
  
  gizmoScaleIsotropic->SetCubeLength(10);
  double coneLength = gizmoScaleIsotropic->GetCubeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleIsotropic);
}




void albaGizmoScaleIsotropicTest::TestSetListener()
{
  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);

  gizmoScaleIsotropic->SetListener(gizmoScaleIsotropic);
  
  // using friend albaGizmoScaleIsotropic
  CPPUNIT_ASSERT(gizmoScaleIsotropic->m_Listener == gizmoScaleIsotropic);

  cppDEL(gizmoScaleIsotropic);
}

void albaGizmoScaleIsotropicTest::TestSetInput()
{
  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);

  gizmoScaleIsotropic->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoScaleIsotropic
  CPPUNIT_ASSERT(gizmoScaleIsotropic->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoScaleIsotropic);
}


void albaGizmoScaleIsotropicTest::TestShow()
{
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();

  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);
 
  gizmoScaleIsotropic->Show(true);
  
	CPPUNIT_ASSERT(logic->GetCall(1).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);
		  
  cppDEL(gizmoScaleIsotropic);
}

void albaGizmoScaleIsotropicTest::TestSetGetAbsPose()
{
  albaMatrix absPose;
  absPose.SetElement(0,3,10);

  albaGizmoScaleIsotropic *gizmoScaleIsotropic = new albaGizmoScaleIsotropic(m_GizmoInputSurface);
  gizmoScaleIsotropic->SetAbsPose(&absPose);
  
  albaMatrix *gipo = gizmoScaleIsotropic->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoScaleIsotropic);
}

void albaGizmoScaleIsotropicTest::TestFixture()
{
	
}

void albaGizmoScaleIsotropicTest::RenderData( vtkDataSet *data )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInputData(data);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  
  m_RenderWindow->Render();
  
  mapper->Delete();
  actor->Delete();

} 
  

void albaGizmoScaleIsotropicTest::CreateRenderStuff()
{
  m_Renderer = vtkRenderer::New();
  m_RenderWindow = vtkRenderWindow::New();
  m_RenderWindowInteractor = vtkRenderWindowInteractor::New() ;
  
  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}

void albaGizmoScaleIsotropicTest::RenderGizmo( albaGizmoScaleIsotropic * gizmoScaleIsotropic )
{
  RenderData(gizmoScaleIsotropic->m_CubeGizmo->GetOutput()->GetVTKData());
}

