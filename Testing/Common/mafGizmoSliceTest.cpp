/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoSliceTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-01 14:52:55 $
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

#include "mafGizmoSliceTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoSlice.h"
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

/** testing facility to receive events */
//----------------------------------------------------------------------------
class mockListener : public mafObserver 
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void mafGizmoSliceTest::setUp()
//----------------------------------------------------------------------------
{
  CreateTestData();
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::tearDown()
//----------------------------------------------------------------------------
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}
//----------------------------------------------------------------------------
void mafGizmoSliceTest::CreateTestData()
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
//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestConstructor()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestShow()
//----------------------------------------------------------------------------
{
  mockListener *listener = new mockListener();

  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetListener(listener);
  gizmoSlice->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoSlice);

  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetAbsPose()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetAbsPose() == NULL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetModality()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetModality() == mafGizmoSlice::G_LOCAL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetMovingModality()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetGizmoMovingModalityToBound();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 0); //mafGizmoSlice::BOUND

  gizmoSlice->SetGizmoMovingModalityToSnap();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 1); //mafGizmoSlice::SNAP
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestGetGui()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetGui() == NULL);

  cppDEL(gizmoSlice);  
}

void mafGizmoSliceTest::TestFixture()
{

}
  



