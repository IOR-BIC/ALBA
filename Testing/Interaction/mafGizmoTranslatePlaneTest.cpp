/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoTranslatePlaneTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.2.2.4 $
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

#include "mafGizmoTranslatePlaneTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafRefSys.h"
#include "mafGizmoTranslatePlane.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafObserver.h"
#include "mafTransform.h"

#include "mafInteractorGenericMouse.h"

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
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"

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

void mafGizmoTranslatePlaneTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoTranslatePlaneTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoTranslatePlaneTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  mafNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void mafGizmoTranslatePlaneTest::TestConstructor()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::XY);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  
  RenderGizmo(gizmoTranslatePlane);
  mafSleep(500);

  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::XZ);
  RenderGizmo(gizmoTranslatePlane);
  mafSleep(500);
  
  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::YZ);
  RenderGizmo(gizmoTranslatePlane);
  mafSleep(500);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoTranslatePlane);

}

void mafGizmoTranslatePlaneTest::TestSetGetPlane()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
   
  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::XY);
  int axis = gizmoTranslatePlane->GetPlane();

  CPPUNIT_ASSERT( axis == mafGizmoTranslatePlane::XY);

  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::XZ);
  axis = gizmoTranslatePlane->GetPlane();

  CPPUNIT_ASSERT(axis == mafGizmoTranslatePlane::XZ);

  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::YZ);
  axis = gizmoTranslatePlane->GetPlane();
  CPPUNIT_ASSERT(axis == mafGizmoTranslatePlane::YZ);

  cppDEL(gizmoTranslatePlane);
}


void mafGizmoTranslatePlaneTest::TestSetGetLength()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
  
  gizmoTranslatePlane->SetSizeLength(10);
  double coneLength = gizmoTranslatePlane->GetSizeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetConstrainRefSys()
{
  mafMatrix refSys;

  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetConstrainRefSys(&refSys);
  
  // using friend mafGizmoTranslatePlane
  for (int i = 0; i < 2; i++)
  {
    mafMatrix *mat = gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetConstraintModality()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetPlane(mafGizmoTranslatePlane::XY);
  gizmoTranslatePlane->SetConstraintModality(mafGizmoTranslatePlane::XY, mafInteractorConstraint::LOCK);
  
  // using friend mafGizmoTranslatePlane
  for (int i = 0; i < 2; i++)
  {
    int constrainModality = gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetConstraintModality(mafGizmoTranslatePlane::XY);
    CPPUNIT_ASSERT(constrainModality  ==  mafInteractorConstraint::LOCK);
  } 

  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetListener()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetListener(gizmoTranslatePlane);
  
  // using friend mafGizmoTranslatePlane
  CPPUNIT_ASSERT(gizmoTranslatePlane->m_Listener == gizmoTranslatePlane);

  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetInput()
{
  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoTranslatePlane
  CPPUNIT_ASSERT(gizmoTranslatePlane->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoTranslatePlane);
}


void mafGizmoTranslatePlaneTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetListener(listener);
 
  gizmoTranslatePlane->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslatePlane);

  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetGetAbsPose()
{
  mafMatrix absPose;

  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetAbsPose(&absPose);
  
  mafMatrix *gipo = gizmoTranslatePlane->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestSetStep()
{
  mafMatrix absPose;
  double position[3] = {5,0,0};
  mafTransform::SetPosition(absPose, position);

  mafGizmoTranslatePlane *gizmoTranslatePlane = new mafGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetStep(mafInteractorConstraint::Y, 2);

  for (int i = 0; i < 2; i++)
  {
    CPPUNIT_ASSERT(
      gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetStep(mafInteractorConstraint::Y)
      == 2);
  }

  cppDEL(gizmoTranslatePlane);
}

void mafGizmoTranslatePlaneTest::TestFixture()
{
	
}

void mafGizmoTranslatePlaneTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoTranslatePlaneTest::CreateRenderStuff()
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

void mafGizmoTranslatePlaneTest::RenderGizmo( mafGizmoTranslatePlane * gizmoTranslatePlane )
{
  for (int i = 0; i < 3; i++) 
  { 
    RenderData(gizmoTranslatePlane->m_Gizmo[i]->GetOutput()->GetVTKData()); 
  }
}

