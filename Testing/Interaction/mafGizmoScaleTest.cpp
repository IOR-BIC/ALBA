/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleTest
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

#include "mafGizmoScaleTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoScale.h"
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

void mafGizmoScaleTest::setUp()
{
  CreateTestData();
}

void mafGizmoScaleTest::tearDown()
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}

void mafGizmoScaleTest::CreateTestData()
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

void mafGizmoScaleTest::TestConstructor()
{
  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface, NULL , false);
  cppDEL(gizmoScale);
}


void mafGizmoScaleTest::TestSetGetListener()
{
  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface,NULL, false);
  CPPUNIT_ASSERT(gizmoScale->GetListener() == NULL);

  gizmoScale->SetListener(gizmoScale);
  
  CPPUNIT_ASSERT(gizmoScale->GetListener() == gizmoScale);

  cppDEL(gizmoScale);
}

void mafGizmoScaleTest::TestSetGetInput()
{
  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface,NULL, false);
  gizmoScale->SetInput(m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoScale->GetInput() == m_GizmoInputSurface);
  
  cppDEL(gizmoScale);
}


void mafGizmoScaleTest::TestShow()
{
  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface,NULL, false); 
  CPPUNIT_ASSERT_EQUAL(gizmoScale->m_Visibility , false);
  
  gizmoScale->Show(true);
  CPPUNIT_ASSERT_EQUAL(gizmoScale->m_Visibility , true);

  cppDEL(gizmoScale);
}

void mafGizmoScaleTest::TestSetGetAbsPose()
{
  mafMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface,NULL, false);
  gizmoScale->SetAbsPose(&matrix);
  
  mafMatrix *absPose = gizmoScale->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoScale);
}

void mafGizmoScaleTest::TestSetGetRefSys()
{
  mafGizmoScale *gizmoScale = new mafGizmoScale(m_GizmoInputSurface,NULL, false);
  
  gizmoScale->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoScale->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoScale);
}


void mafGizmoScaleTest::TestFixture()
{
	
}

  



