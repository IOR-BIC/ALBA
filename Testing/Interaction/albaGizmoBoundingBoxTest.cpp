/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoBoundingBoxTest
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

#include "albaGizmoBoundingBoxTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoBoundingBox.h"
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
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::CreateTestData()
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
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestConstructor()
{
  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoBoundingBox);

  cppDEL(gizmoBoundingBox);

}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestSetListener()
{
  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);

  gizmoBoundingBox->SetListener(gizmoBoundingBox);
  
  // using friend albaGizmoBoundingBox
  CPPUNIT_ASSERT(gizmoBoundingBox->m_Listener == gizmoBoundingBox);

  gizmoBoundingBox->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoBoundingBox->m_Listener == NULL);

  cppDEL(gizmoBoundingBox);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestSetInput()
{
  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);

  gizmoBoundingBox->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoBoundingBox
  CPPUNIT_ASSERT(gizmoBoundingBox->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoBoundingBox);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestShow()
{

  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);
  gizmoBoundingBox->Show(true);
  
  int opacity = -1;
  // using friend 
  opacity = gizmoBoundingBox->m_BoxGizmo->GetMaterial()->m_Prop->GetOpacity();

  CPPUNIT_ASSERT(opacity == 1);


  gizmoBoundingBox->Show(false);  
  // using friend 
  opacity = gizmoBoundingBox->m_BoxGizmo->GetMaterial()->m_Prop->GetOpacity();

  CPPUNIT_ASSERT(opacity == 0);

  cppDEL(gizmoBoundingBox);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestSetGetAbsPose()
{
  albaMatrix absPose;
  absPose.SetElement(0,3,10);

  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);
  gizmoBoundingBox->SetAbsPose(&absPose);
  
  albaMatrix *gipo = gizmoBoundingBox->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoBoundingBox);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestSetGetPose()
{
  albaMatrix pose;
  pose.SetElement(0,3,10);

  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);
  gizmoBoundingBox->SetPose(&pose);

  albaMatrix *gipo = gizmoBoundingBox->GetPose();
  CPPUNIT_ASSERT(gipo->Equals(&pose));

  cppDEL(gizmoBoundingBox);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestFixture()
{
	
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::RenderGizmo( albaGizmoBoundingBox *gizmoBoundingBox )
{
  RenderData(gizmoBoundingBox->m_BoxGizmo->GetOutput()->GetVTKData());
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBoxTest::TestSetGetBounds()
{
 
  albaGizmoBoundingBox *gizmoBoundingBox = new albaGizmoBoundingBox(m_GizmoInputSurface);
  
  double inBounds[6] = {0,5,0,5,0,5};
  gizmoBoundingBox->SetBounds(inBounds);

  double outBounds[6] = {0,0,0,0,0,0};
  gizmoBoundingBox->GetBounds(outBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(inBounds[var] == outBounds[var]);
  }

  cppDEL(gizmoBoundingBox);
}
