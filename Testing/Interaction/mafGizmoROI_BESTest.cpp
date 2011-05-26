/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoROI_BESTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:56 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
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

#include "mafGizmoROI_BESTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoROI_BES.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafObserver.h"
#include "mafTransform.h"
#include "mafRefSys.h"

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

void mafGizmoROI_BESTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoROI_BESTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoROI_BESTest::CreateTestData()
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

void mafGizmoROI_BESTest::TestConstructor()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoROI_BES);
  mafSleep(500);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoROI_BES);

}


void mafGizmoROI_BESTest::TestSetListener()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);

  gizmoROI_BES->SetListener(gizmoROI_BES);
  
  // using friend mafGizmoROI_BES
  CPPUNIT_ASSERT(gizmoROI_BES->m_Listener == gizmoROI_BES);

  gizmoROI_BES->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoROI_BES->m_Listener == NULL);

  cppDEL(gizmoROI_BES);
}

void mafGizmoROI_BESTest::TestSetInput()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);

  gizmoROI_BES->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoROI_BES
  CPPUNIT_ASSERT(gizmoROI_BES->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoROI_BES);
}



void mafGizmoROI_BESTest::TestFixture()
{
	
}

void mafGizmoROI_BESTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoROI_BESTest::CreateRenderStuff()
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

void mafGizmoROI_BESTest::RenderGizmo( mafGizmoROI_BES *gizmoROI_BES )
{
  RenderData(gizmoROI_BES->m_OutlineGizmo->m_BoxGizmo->GetOutput()->GetVTKData());

  for (int i = 0; i < 6; i++)
  {
    RenderData(gizmoROI_BES->m_GHandle[i]->m_BoxGizmo->GetOutput()->GetVTKData());
  }
}

void mafGizmoROI_BESTest::TestSetGetBounds()
{
 
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  
  double inBounds[6] = {0,2.5,0,2.5,0,2.5};
  gizmoROI_BES->SetBounds(inBounds);

  double outBounds[6] = {0,0,0,0,0,0};
  gizmoROI_BES->GetBounds(outBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(inBounds[var] == outBounds[var]);
  }

  cppDEL(gizmoROI_BES);
}



void mafGizmoROI_BESTest::TestHighlight()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI_BES->Highlight(component);
  } 
  
  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI_BES);

}

void mafGizmoROI_BESTest::TestHighlightOff()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI_BES->Highlight(component);
  } 
  gizmoROI_BES->HighlightOff();

  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI_BES);
  
}


void mafGizmoROI_BESTest::TestSetConstrainRefSys()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);

  mafMatrix *refSys = mafMatrix::New();
  refSys->SetElement(0,3,5);
  refSys->SetElement(1,3,5);
  refSys->SetElement(2,3,5);
  refSys->SetElement(0,3,5);

  gizmoROI_BES->SetConstrainRefSys(refSys);

  for (int i =0; i < 6; i++)
  {
    mafMatrix *mat = gizmoROI_BES->m_GHandle[i]->m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == refSys);
  }

  cppDEL(gizmoROI_BES);
  // TODO: REFACTOR THIS 
  // gizmoROI_BES destructor destroy the refSys mafMatrix also. This is dangerous and leaks prone.
  // To check.
}

void mafGizmoROI_BESTest::TestReset()
{
  double oldBounds[6] = {0,0,0,0,0,0};
  m_GizmoInputSurface->GetOutput()->GetBounds(oldBounds);

  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  
  double newBounds[6] = {0,1,0,1,0,1};
  gizmoROI_BES->SetBounds(newBounds);
  
  gizmoROI_BES->Reset();

  double getBounds[6] = {0,0,0,0,0,0};
  gizmoROI_BES->GetBounds(getBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(getBounds[var] == oldBounds[var]);
  }

  cppDEL(gizmoROI_BES);

}

void mafGizmoROI_BESTest::TestShow()
{

  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  gizmoROI_BES->Show(true);
  cppDEL(gizmoROI_BES);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}



void mafGizmoROI_BESTest::TestShowHandles()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  gizmoROI_BES->ShowHandles(true);
  cppDEL(gizmoROI_BES);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

void mafGizmoROI_BESTest::TestShowROI()
{
  mafGizmoROI_BES *gizmoROI_BES = new mafGizmoROI_BES(m_GizmoInputSurface);
  gizmoROI_BES->ShowROI(true);
  cppDEL(gizmoROI_BES);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

