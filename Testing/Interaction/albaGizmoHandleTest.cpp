/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoHandleTest
 Authors: Stefano Perticoni
 
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

#include "albaGizmoHandleTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaGizmoHandle.h"
#include "albaString.h"
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

void albaGizmoHandleTest::BeforeTest()
{
 	CreateTestData();
}

void albaGizmoHandleTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoHandleTest::CreateTestData()
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

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

void albaGizmoHandleTest::TestFixture()
{
	
}
  
void albaGizmoHandleTest::TestConstructor()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  cppDEL(gizmoHandle);
}


void albaGizmoHandleTest::TestSetGetListener()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetListener() == NULL);

  gizmoHandle->SetListener(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetListener() == m_GizmoInputSurface);
  cppDEL(gizmoHandle);
}

void albaGizmoHandleTest::TestSetGetInput()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoHandle->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoHandle);
}


void albaGizmoHandleTest::TestHighlight()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);

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

void albaGizmoHandleTest::TestShow()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);    
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == false);
  gizmoHandle->Show(true);
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == true);
  gizmoHandle->ShowOff();
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == false);
  gizmoHandle->ShowOn();
  CPPUNIT_ASSERT(gizmoHandle->GetShow() == true);
  cppDEL(gizmoHandle);
}


void albaGizmoHandleTest::TestShowShadingPlane()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == false);
  gizmoHandle->ShowShadingPlane(true);
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == true);
  gizmoHandle->ShowShadingPlaneOff();
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == false);
  gizmoHandle->ShowShadingPlaneOn();
  
  CPPUNIT_ASSERT(gizmoHandle->GetShowShadingPlaneOn() == true);
  cppDEL(gizmoHandle);  

}

void albaGizmoHandleTest::TestSetGetLength()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);

  gizmoHandle->SetLength(10);
  double length = gizmoHandle->GetLength();
  CPPUNIT_ASSERT(length == 10);
  cppDEL(gizmoHandle);
}

void albaGizmoHandleTest::TestSetGetAbsPose()
{
  albaMatrix inputABSPose;
  inputABSPose.SetElement(0,3,10);

  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetAbsPose(&inputABSPose);

  albaMatrix *gizmoABSPose = gizmoHandle->GetAbsPose();
  CPPUNIT_ASSERT(gizmoABSPose->Equals(&inputABSPose));

  cppDEL(gizmoHandle);
}

void albaGizmoHandleTest::TestSetGetPose()
{
  albaMatrix inputPose;
  inputPose.SetElement(0,3,10);

  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetPose(&inputPose);

  albaMatrix *gizmoPose = gizmoHandle->GetPose();
  CPPUNIT_ASSERT(gizmoPose->Equals(&inputPose));

  cppDEL(gizmoHandle);
}

void albaGizmoHandleTest::TestSetGetConstrainRefSys()
{
  albaMatrix inputABSPose;
  inputABSPose.SetElement(0,3,10);
  
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  gizmoHandle->SetConstrainRefSys(&inputABSPose);

  albaMatrix *constrainRefSysMatrix = gizmoHandle->GetConstrainRefSys();
  CPPUNIT_ASSERT(constrainRefSysMatrix->Equals(&inputABSPose));
  cppDEL(gizmoHandle);
}

void albaGizmoHandleTest::TestSetGetType()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoHandle->GetType() == albaGizmoHandle::XMIN);

  gizmoHandle->SetType(albaGizmoHandle::XMAX);
  CPPUNIT_ASSERT(gizmoHandle->GetType() == albaGizmoHandle::XMAX);
  cppDEL(gizmoHandle);

}

void albaGizmoHandleTest::TestSetGetBounds()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);
    
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

void albaGizmoHandleTest::TestGetHandleCenter()
{
  albaGizmoHandle *gizmoHandle = new albaGizmoHandle(m_GizmoInputSurface);

  double inBounds[6] = {0,1,0,1,0,1} ;
  gizmoHandle->SetBounds(inBounds);
  
  double center[3];

  gizmoHandle->GetHandleCenter(albaGizmoHandle::XMIN, center);

  CPPUNIT_ASSERT(center[0] == 0);
  CPPUNIT_ASSERT(center[1] == 0.5);
  CPPUNIT_ASSERT(center[2] == 0.5);

  cppDEL(gizmoHandle);
}
