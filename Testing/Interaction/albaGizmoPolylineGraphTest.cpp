/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPolylineGraphTest
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

#include "albaGizmoPolylineGraphTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoPolylineGraph.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"
#include "mmaMaterial.h"

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
#include "vtkProperty.h"

void albaGizmoPolylineGraphTest::BeforeTest()
{
  CreateRenderStuff();
	CreateTestData();
}

void albaGizmoPolylineGraphTest::AfterTest()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoPolylineGraphTest::CreateTestData()
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

void albaGizmoPolylineGraphTest::TestConstructor()
{
  albaGizmoPolylineGraph *gizmoPolylineGraph = albaGizmoPolylineGraph::New(m_GizmoInputSurface, NULL,"",false,true);
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);

  // m_RenderWindowInteractor->Start();

  gizmoPolylineGraph->Delete();
}

void albaGizmoPolylineGraphTest::TestSetInput()
{
  albaGizmoPolylineGraph *gizmoPolylineGraph = albaGizmoPolylineGraph::New(m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoPolylineGraph
  CPPUNIT_ASSERT(gizmoPolylineGraph->GetInput() == m_GizmoInputSurface);

  gizmoPolylineGraph->Delete();
}

void albaGizmoPolylineGraphTest::TestSetGizmoLength()
{
  albaGizmoPolylineGraph *gizmoPolylineGraph = albaGizmoPolylineGraph::New(m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetGizmoLength(20);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);

  gizmoPolylineGraph->Delete();
}

void albaGizmoPolylineGraphTest::TestFixture()
{
	
}

void albaGizmoPolylineGraphTest::RenderData( vtkDataSet *data )
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
  
void albaGizmoPolylineGraphTest::CreateRenderStuff()
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

void albaGizmoPolylineGraphTest::RenderGizmo( albaGizmoPolylineGraph *gizmoPolylineGraph )
{
  RenderData(gizmoPolylineGraph->GetOutput()->GetData());
}

void albaGizmoPolylineGraphTest::TestShow()
{
  albaGizmoPolylineGraph *gizmoPolylineGraph = albaGizmoPolylineGraph::New(m_GizmoInputSurface, NULL,"",false,true);

  gizmoPolylineGraph->Show(true);
  gizmoPolylineGraph->Delete();
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

