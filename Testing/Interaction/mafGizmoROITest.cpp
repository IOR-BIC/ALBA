/*=========================================================================

 Program: MAF2
 Module: mafGizmoROITest
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

#include "mafGizmoROITest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafRefSys.h"
#include "mafGizmoROI.h"
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

void mafGizmoROITest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoROITest::tearDown()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoROITest::CreateTestData()
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

void mafGizmoROITest::TestConstructor()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoROI);
  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoROI);

}


void mafGizmoROITest::TestSetListener()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);

  gizmoROI->SetListener(gizmoROI);
  
  // using friend mafGizmoROI
  CPPUNIT_ASSERT(gizmoROI->m_Listener == gizmoROI);

  gizmoROI->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoROI->m_Listener == NULL);

  cppDEL(gizmoROI);
}

void mafGizmoROITest::TestSetInput()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);

  gizmoROI->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoROI
  CPPUNIT_ASSERT(gizmoROI->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoROI);
}



void mafGizmoROITest::TestFixture()
{
	
}

void mafGizmoROITest::RenderData( vtkDataSet *data )
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
  

void mafGizmoROITest::CreateRenderStuff()
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

void mafGizmoROITest::RenderGizmo( mafGizmoROI *gizmoROI )
{
  RenderData(gizmoROI->m_OutlineGizmo->m_BoxGizmo->GetOutput()->GetVTKData());

  for (int i = 0; i < 6; i++)
  {
    RenderData(gizmoROI->m_GHandle[i]->m_BoxGizmo->GetOutput()->GetVTKData());
  }
}

void mafGizmoROITest::TestSetGetBounds()
{
 
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  
  double inBounds[6] = {0,2.5,0,2.5,0,2.5};
  gizmoROI->SetBounds(inBounds);

  double outBounds[6] = {0,0,0,0,0,0};
  gizmoROI->GetBounds(outBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(inBounds[var] == outBounds[var]);
  }

  cppDEL(gizmoROI);
}



void mafGizmoROITest::TestHighlight()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI->Highlight(component);
  } 
  
  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI);

}

void mafGizmoROITest::TestHighlightOff()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI->Highlight(component);
  } 
  gizmoROI->HighlightOff();

  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI);
  
}


void mafGizmoROITest::TestSetConstrainRefSys()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);

  mafMatrix refSys;
  refSys.SetElement(0,3,5);
  refSys.SetElement(1,3,5);
  refSys.SetElement(2,3,5);
  refSys.SetElement(0,3,5);

  gizmoROI->SetConstrainRefSys(&refSys);

  for (int i =0; i < 6; i++)
  {
    mafMatrix *mat = gizmoROI->m_GHandle[i]->m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoROI);
}

void mafGizmoROITest::TestReset()
{
  double oldBounds[6] = {0,0,0,0,0,0};
  m_GizmoInputSurface->GetOutput()->GetBounds(oldBounds);

  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  
  double newBounds[6] = {0,1,0,1,0,1};
  gizmoROI->SetBounds(newBounds);
  
  gizmoROI->Reset();

  double getBounds[6] = {0,0,0,0,0,0};
  gizmoROI->GetBounds(getBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(getBounds[var] == oldBounds[var]);
  }

  cppDEL(gizmoROI);

}

void mafGizmoROITest::TestShow()
{

  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->Show(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}



void mafGizmoROITest::TestShowHandles()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowHandles(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

void mafGizmoROITest::TestShowShadingPlane()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowShadingPlane(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

void mafGizmoROITest::TestShowROI()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowROI(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}


void mafGizmoROITest::TestEnableMinimumHandleSize()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->EnableMinimumHandleSize(true);
  CPPUNIT_ASSERT(gizmoROI->m_EnableMinimumHandleSize == true);
  cppDEL(gizmoROI);
}

void mafGizmoROITest::TestSetMinimumHandleSize()
{
  mafGizmoROI *gizmoROI = new mafGizmoROI(m_GizmoInputSurface);
  gizmoROI->SetMinimumHandleSize(5);
  CPPUNIT_ASSERT(gizmoROI->m_MinimumHandleSize == 5);
  cppDEL(gizmoROI);
}
