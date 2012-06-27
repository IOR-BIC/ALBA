/*=========================================================================

 Program: MAF2
 Module: mafGizmoRotateCircleTest
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

#include "mafGizmoRotateCircleTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafRefSys.h"
#include "mafGizmoRotateCircle.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafObserver.h"
#include "mafTransform.h"

#include "mafInteractorGenericMouse.h"

#include "vtkDiskSource.h"
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

/** testing facility to receive events */
class mockListener : public mafObserver 
{
public:

  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event) {m_Event = maf_event;};

  mafEventBase *GetEvent() {return m_Event;};

private:
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  mafEventBase *m_Event;
};

void mafGizmoRotateCircleTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoRotateCircleTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoRotateCircleTest::CreateTestData()
//----------------------------------------------------------------------------
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

void mafGizmoRotateCircleTest::TestConstructor()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetAxis(mafInteractorConstraint::X);
  
  gizmoRotateCircle->SetRadius(3);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoRotateCircle);
  mafSleep(500);

  gizmoRotateCircle->SetAxis(mafInteractorConstraint::Y);
  RenderGizmo(gizmoRotateCircle);
  mafSleep(500);

  gizmoRotateCircle->SetAxis(mafInteractorConstraint::Z);
  RenderGizmo(gizmoRotateCircle);
  mafSleep(500);
  
  // remove this comment to enable render window interaction
  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoRotateCircle);

}

void mafGizmoRotateCircleTest::TestSetGetAxis()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
   
  int axis = gizmoRotateCircle->GetAxis();

  CPPUNIT_ASSERT( axis == mafGizmoRotateCircle::AXIS::X);

  gizmoRotateCircle->SetAxis(mafGizmoRotateCircle::AXIS::Y);
  axis = gizmoRotateCircle->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoRotateCircle::AXIS::Y);

  gizmoRotateCircle->SetAxis(mafGizmoRotateCircle::AXIS::Z);
  axis = gizmoRotateCircle->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoRotateCircle::AXIS::Z);

  cppDEL(gizmoRotateCircle);
}


void mafGizmoRotateCircleTest::TestSetRadius()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
  
  gizmoRotateCircle->SetRadius(10);
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Circle->GetInnerRadius() == 10);
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Circle->GetOuterRadius() == 10);

  cppDEL(gizmoRotateCircle);
}

void mafGizmoRotateCircleTest::TestSetRefSysMatrix()
{
  mafMatrix refSys;

  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetRefSysMatrix(&refSys);
  
  // using friend mafGizmoRotateCircle
  mafMatrix *mat = gizmoRotateCircle->m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
  CPPUNIT_ASSERT(mat->Equals(&refSys));

  cppDEL(gizmoRotateCircle);
}


void mafGizmoRotateCircleTest::TestSetListener()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetListener(gizmoRotateCircle);
  
  // using friend mafGizmoRotateCircle
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Listener == gizmoRotateCircle);

  cppDEL(gizmoRotateCircle);
}

void mafGizmoRotateCircleTest::TestSetInput()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoRotateCircle
  CPPUNIT_ASSERT(gizmoRotateCircle->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoRotateCircle);
}


void mafGizmoRotateCircleTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetListener(listener);
 
  gizmoRotateCircle->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoRotateCircle);

  cppDEL(gizmoRotateCircle);
}

void mafGizmoRotateCircleTest::TestSetGetAbsPose()
{
  mafMatrix absPose;

  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetAbsPose(&absPose);
  
  mafMatrix *gipo = gizmoRotateCircle->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoRotateCircle);
}


void mafGizmoRotateCircleTest::TestFixture()
{
	
}

void mafGizmoRotateCircleTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoRotateCircleTest::CreateRenderStuff()
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

void mafGizmoRotateCircleTest::RenderGizmo( mafGizmoRotateCircle * gizmoRotateCircle )
{
  RenderData(gizmoRotateCircle->m_Circle->GetOutput());
}

void mafGizmoRotateCircleTest::TestSetGetIsActive()
{
  mafGizmoRotateCircle *gizmoRotateCircle = new mafGizmoRotateCircle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoRotateCircle->GetIsActive() == false);
  gizmoRotateCircle->SetIsActive(true);
  CPPUNIT_ASSERT(gizmoRotateCircle->GetIsActive() == true);
  cppDEL(gizmoRotateCircle);
}
