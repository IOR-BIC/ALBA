/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoHandleTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-17 11:48:36 $
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

#include "mafGizmoHandleTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafGizmoHandle.h"
#include "mafString.h"
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

void mafGizmoHandleTest::setUp()
{
 	CreateTestData();
}

void mafGizmoHandleTest::tearDown()
{
  mafSleep(500);

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoHandleTest::CreateTestData()
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

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

void mafGizmoHandleTest::TestFixture()
{
	
}
  
void mafGizmoHandleTest::TestConstructor()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  cppDEL(gizmoHandle);
}


void mafGizmoHandleTest::TestSetGetListener()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetListener() == NULL);

  gizmoHandle->SetListener(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetListener() == m_GizmoInputSurface);
  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestSetGetInput()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoHandle->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoHandle);
}


void mafGizmoHandleTest::TestHighlight()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);

  double color[3] = {0,0,0} ;
  gizmoHandle->GetColor(color);
  
  // default is red
  CPPUNIT_ASSERT(color[0] == 1 && color[1] == 0 && color[2] == 0);
  gizmoHandle->Highlight(true);

  // highlight color is yellow
  gizmoHandle->GetColor(color);
  CPPUNIT_ASSERT(color[0] == 1 && color[1] == 1 && color[2] == 0);

  cppDEL(gizmoHandle);

}

void mafGizmoHandleTest::TestShow()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);    
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == false);
  gizmoHandle->Show(true);
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == true);
  gizmoHandle->ShowOff();
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == false);
  gizmoHandle->ShowOn();
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == true);
  cppDEL(gizmoHandle);
}


void mafGizmoHandleTest::TestShowShadingPlane()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == false);
  gizmoHandle->ShowShadingPlane(true);
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == true);
  gizmoHandle->ShowShadingPlaneOff();
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == false);
  gizmoHandle->ShowShadingPlaneOn();
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == true);
  cppDEL(gizmoHandle);  

}

void mafGizmoHandleTest::TestSetGetLength()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);

  gizmoHandle->SetLength(10);
  double length = gizmoHandle->GetLength();
  CPPUNIT_ASSERT(length == 10);
  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestSetGetAbsPose()
{
  mafMatrix inputABSPose;
  inputABSPose.SetElement(0,3,10);

  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetAbsPose(&inputABSPose);

  mafMatrix *gizmoABSPose = gizmoHandle->GetAbsPose();
  CPPUNIT_ASSERT(gizmoABSPose->Equals(&inputABSPose));

  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestSetGetPose()
{
  mafMatrix inputPose;
  inputPose.SetElement(0,3,10);

  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetPose(&inputPose);

  mafMatrix *gizmoPose = gizmoHandle->GetPose();
  CPPUNIT_ASSERT(gizmoPose->Equals(&inputPose));

  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestSetGetConstrainRefSys()
{
  mafMatrix inputABSPose;
  inputABSPose.SetElement(0,3,10);
  
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetConstrainRefSys(&inputABSPose);

  mafMatrix *constrainRefSysMatrix = gizmoHandle->GetConstrainRefSys();
  CPPUNIT_ASSERT(constrainRefSysMatrix->Equals(&inputABSPose));
  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestSetGetType()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetType() == mafGizmoHandle::XMIN);

  gizmoHandle->SetType(mafGizmoHandle::XMAX);
  CPPUNIT_ASSERT(gizmoHandle->GetType() == mafGizmoHandle::XMAX);
  cppDEL(gizmoHandle);

}

void mafGizmoHandleTest::TestSetGetBounds()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);
    
  double inBounds[6] = {0,1,0,1,0,1} ;
  gizmoHandle->SetBounds(inBounds);

  double outBounds[6];
  gizmoHandle->GetBounds(outBounds);
  
  for (int i = 0; i < 6; i++) 
  { 
    CPPUNIT_ASSERT(inBounds[i]  ==  outBounds[i]);
  }

  cppDEL(gizmoHandle);
}

void mafGizmoHandleTest::TestGetHandleCenter()
{
  mafGizmoHandle *gizmoHandle = new mafGizmoHandle(m_GizmoInputSurface);

  double inBounds[6] = {0,1,0,1,0,1} ;
  gizmoHandle->SetBounds(inBounds);
  
  double center[3];

  gizmoHandle->GetHandleCenter(mafGizmoHandle::XMIN, center);

  CPPUNIT_ASSERT(center[0] == 0);
  CPPUNIT_ASSERT(center[1] == 0.5);
  CPPUNIT_ASSERT(center[2] == 0.5);

  cppDEL(gizmoHandle);
}
