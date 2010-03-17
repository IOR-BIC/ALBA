/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoPolylineGraphTest.cpp,v $
Language:  C++
Date:      $Date: 2010-03-17 15:44:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGizmoPolylineGraphTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "medGizmoPolylineGraph.h"
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

void medGizmoPolylineGraphTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void medGizmoPolylineGraphTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void medGizmoPolylineGraphTest::CreateTestData()
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

void medGizmoPolylineGraphTest::TestConstructor()
{
  medGizmoPolylineGraph *gizmoPolylineGraph = medGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);
  mafSleep(500);

  // m_RenderWindowInteractor->Start();

  gizmoPolylineGraph->Delete();
}

void medGizmoPolylineGraphTest::TestSetInput()
{
  medGizmoPolylineGraph *gizmoPolylineGraph = medGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetInput(m_GizmoInputSurface);

  // using friend medGizmoPolylineGraph
  CPPUNIT_ASSERT(gizmoPolylineGraph->GetInput() == m_GizmoInputSurface);

  gizmoPolylineGraph->Delete();
}

void medGizmoPolylineGraphTest::TestSetGizmoLength()
{
  medGizmoPolylineGraph *gizmoPolylineGraph = medGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);
  gizmoPolylineGraph->SetGizmoLength(20);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoPolylineGraph);
  mafSleep(500);

  gizmoPolylineGraph->Delete();
}

void medGizmoPolylineGraphTest::TestFixture()
{
	
}

void medGizmoPolylineGraphTest::RenderData( vtkDataSet *data )
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
  
void medGizmoPolylineGraphTest::CreateRenderStuff()
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

void medGizmoPolylineGraphTest::RenderGizmo( medGizmoPolylineGraph *gizmoPolylineGraph )
{
  RenderData(gizmoPolylineGraph->GetOutput()->GetData());
}

void medGizmoPolylineGraphTest::TestShow()
{
  medGizmoPolylineGraph *gizmoPolylineGraph = medGizmoPolylineGraph::New((mafVME*)m_GizmoInputSurface, NULL,"",false,true);

  gizmoPolylineGraph->Show(true);
  gizmoPolylineGraph->Delete();
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

