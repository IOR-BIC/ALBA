/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoScaleIsotropicTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-17 11:48:37 $
Version:   $Revision: 1.1.2.3 $
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

#include "mafGizmoScaleIsotropicTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoScaleIsotropic.h"
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

/** testing facility to receive events */
class mockListener : public mafObserver 
{
public:
  mockListener() : m_Listener(NULL), m_Event(NULL), m_Id(-1) {}
  virtual ~mockListener() {}

  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  void OnEvent(mafEventBase *maf_event) {m_Id = maf_event->GetId(); m_Event = *maf_event;};

  mafEventBase *GetEvent() {return &m_Event;};

private:
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  mafEventBase m_Event;
  mafID m_Id;
};

void mafGizmoScaleIsotropicTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoScaleIsotropicTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoScaleIsotropicTest::CreateTestData()
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

void mafGizmoScaleIsotropicTest::TestConstructor()
{
  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);
  
  gizmoScaleIsotropic->SetCubeLength(1);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoScaleIsotropic);
  mafSleep(500);

  // remove this comment to enable interaction
  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoScaleIsotropic);

}


void mafGizmoScaleIsotropicTest::TestSetGetCubeLength()
{
  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);
  
  gizmoScaleIsotropic->SetCubeLength(10);
  double coneLength = gizmoScaleIsotropic->GetCubeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleIsotropic);
}




void mafGizmoScaleIsotropicTest::TestSetListener()
{
  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);

  gizmoScaleIsotropic->SetListener(gizmoScaleIsotropic);
  
  // using friend mafGizmoScaleIsotropic
  CPPUNIT_ASSERT(gizmoScaleIsotropic->m_Listener == gizmoScaleIsotropic);

  cppDEL(gizmoScaleIsotropic);
}

void mafGizmoScaleIsotropicTest::TestSetInput()
{
  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);

  gizmoScaleIsotropic->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoScaleIsotropic
  CPPUNIT_ASSERT(gizmoScaleIsotropic->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoScaleIsotropic);
}


void mafGizmoScaleIsotropicTest::TestShow()
{
  mockListener *mockList = new mockListener();

  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);
  gizmoScaleIsotropic->SetListener(mockList);
 
  gizmoScaleIsotropic->Show(true);
  
  mafEventBase *ev = mockList->GetEvent();
  int evId = 0;
  evId = ev->GetId();

  CPPUNIT_ASSERT( evId == VME_SHOW );
  
  cppDEL(gizmoScaleIsotropic);
  cppDEL(mockList);
}

void mafGizmoScaleIsotropicTest::TestSetGetAbsPose()
{
  mafMatrix absPose;
  absPose.SetElement(0,3,10);

  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);
  gizmoScaleIsotropic->SetAbsPose(&absPose);
  
  mafMatrix *gipo = gizmoScaleIsotropic->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoScaleIsotropic);
}

void mafGizmoScaleIsotropicTest::TestFixture()
{
	
}

void mafGizmoScaleIsotropicTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoScaleIsotropicTest::CreateRenderStuff()
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

void mafGizmoScaleIsotropicTest::RenderGizmo( mafGizmoScaleIsotropic * gizmoScaleIsotropic )
{
  RenderData(gizmoScaleIsotropic->m_CubeGizmo->GetOutput()->GetVTKData());
}

