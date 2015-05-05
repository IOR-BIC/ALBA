/*=========================================================================

 Program: MAF2
 Module: mafGizmoPolylineGraphTest
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

#include "mafGizmoPolylineGraphTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoPolylineGraph.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafObserver.h"
#include "mafTransform.h"

#include "mafInteractorGenericMouse.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
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

void mafGizmoPolylineGraphTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoPolylineGraphTest::tearDown()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoPolylineGraphTest::CreateTestData()
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  mafNEW(m_GizmoInputSurface);

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkMAFSmartPointer<vtkTubeFilter> tube;
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);

  mafNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void mafGizmoPolylineGraphTest::TestConstructor()
{
  mafGizmoPolylineGraph *gizmoPolylineGraph = mafGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);

  // m_RenderWindowInteractor->Start();

  gizmoPolylineGraph->Delete();
}

void mafGizmoPolylineGraphTest::TestSetInput()
{
  mafGizmoPolylineGraph *gizmoPolylineGraph = mafGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoPolylineGraph
  CPPUNIT_ASSERT(gizmoPolylineGraph->GetInput() == m_GizmoInputSurface);

  gizmoPolylineGraph->Delete();
}

void mafGizmoPolylineGraphTest::TestSetGizmoLength()
{
  mafGizmoPolylineGraph *gizmoPolylineGraph = mafGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetGizmoLength(20);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);

  gizmoPolylineGraph->Delete();
}

void mafGizmoPolylineGraphTest::TestFixture()
{
	
}

void mafGizmoPolylineGraphTest::RenderData( vtkDataSet *data )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInput(data);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  
  m_RenderWindow->Render();
  
  mapper->Delete();
  actor->Delete();
} 
  
void mafGizmoPolylineGraphTest::CreateRenderStuff()
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

void mafGizmoPolylineGraphTest::RenderGizmo( mafGizmoPolylineGraph *gizmoPolylineGraph )
{
  RenderData(gizmoPolylineGraph->GetOutput()->GetData());
}

void mafGizmoPolylineGraphTest::TestShow()
{
  mafGizmoPolylineGraph *gizmoPolylineGraph = mafGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);

  gizmoPolylineGraph->Show(true);
  gizmoPolylineGraph->Delete();
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

