/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleTest
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

#include "albaGizmoScaleTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoScale.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"

#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"

void albaGizmoScaleTest::BeforeTest()
{
  CreateTestData();
}

void albaGizmoScaleTest::AfterTest()
{
   albaDEL(m_GizmoInputSurface);
   albaDEL(m_Root); 
}

void albaGizmoScaleTest::CreateTestData()
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  albaNEW(m_GizmoInputSurface);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkALBASmartPointer<vtkTubeFilter> tube;
  tube->SetInputConnection(axes->GetOutputPort());
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
	tube->Update();
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoScaleTest::TestConstructor()
{
  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface, NULL , false);
  cppDEL(gizmoScale);
}


void albaGizmoScaleTest::TestSetGetListener()
{
  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface,NULL, false);
  CPPUNIT_ASSERT(gizmoScale->GetListener() == NULL);

  gizmoScale->SetListener(gizmoScale);
  
  CPPUNIT_ASSERT(gizmoScale->GetListener() == gizmoScale);

  cppDEL(gizmoScale);
}

void albaGizmoScaleTest::TestSetGetInput()
{
  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface,NULL, false);
  gizmoScale->SetInput(m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoScale->GetInput() == m_GizmoInputSurface);
  
  cppDEL(gizmoScale);
}


void albaGizmoScaleTest::TestShow()
{
  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface,NULL, false); 
  CPPUNIT_ASSERT_EQUAL(gizmoScale->m_Visibility , false);
  
  gizmoScale->Show(true);
  CPPUNIT_ASSERT_EQUAL(gizmoScale->m_Visibility , true);

  cppDEL(gizmoScale);
}

void albaGizmoScaleTest::TestSetGetAbsPose()
{
  albaMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface,NULL, false);
  gizmoScale->SetAbsPose(&matrix);
  
  albaMatrix *absPose = gizmoScale->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoScale);
}

void albaGizmoScaleTest::TestSetGetRefSys()
{
  albaGizmoScale *gizmoScale = new albaGizmoScale(m_GizmoInputSurface,NULL, false);
  
  gizmoScale->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoScale->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoScale);
}


void albaGizmoScaleTest::TestFixture()
{
	
}

  



