/*=========================================================================

 Program: MAF2
 Module: mafGizmoBoundingBoxTest
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

#include "mafGizmoBoundingBoxTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoBoundingBox.h"
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
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"

void mafGizmoBoundingBoxTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoBoundingBoxTest::tearDown()
{
  mafSleep(500);

  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoBoundingBoxTest::CreateTestData()
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

void mafGizmoBoundingBoxTest::TestConstructor()
{
  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoBoundingBox);
  mafSleep(500);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoBoundingBox);

}


void mafGizmoBoundingBoxTest::TestSetListener()
{
  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);

  gizmoBoundingBox->SetListener(gizmoBoundingBox);
  
  // using friend mafGizmoBoundingBox
  CPPUNIT_ASSERT(gizmoBoundingBox->m_Listener == gizmoBoundingBox);

  gizmoBoundingBox->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoBoundingBox->m_Listener == NULL);

  cppDEL(gizmoBoundingBox);
}

void mafGizmoBoundingBoxTest::TestSetInput()
{
  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);

  gizmoBoundingBox->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoBoundingBox
  CPPUNIT_ASSERT(gizmoBoundingBox->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoBoundingBox);
}


void mafGizmoBoundingBoxTest::TestShow()
{

  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);
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

void mafGizmoBoundingBoxTest::TestSetGetAbsPose()
{
  mafMatrix absPose;
  absPose.SetElement(0,3,10);

  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);
  gizmoBoundingBox->SetAbsPose(&absPose);
  
  mafMatrix *gipo = gizmoBoundingBox->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoBoundingBox);
}

void mafGizmoBoundingBoxTest::TestSetGetPose()
{
  mafMatrix pose;
  pose.SetElement(0,3,10);

  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);
  gizmoBoundingBox->SetPose(&pose);

  mafMatrix *gipo = gizmoBoundingBox->GetPose();
  CPPUNIT_ASSERT(gipo->Equals(&pose));

  cppDEL(gizmoBoundingBox);
}

void mafGizmoBoundingBoxTest::TestFixture()
{
	
}

void mafGizmoBoundingBoxTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoBoundingBoxTest::CreateRenderStuff()
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

void mafGizmoBoundingBoxTest::RenderGizmo( mafGizmoBoundingBox *gizmoBoundingBox )
{
  RenderData(gizmoBoundingBox->m_BoxGizmo->GetOutput()->GetVTKData());
}

void mafGizmoBoundingBoxTest::TestSetGetBounds()
{
 
  mafGizmoBoundingBox *gizmoBoundingBox = new mafGizmoBoundingBox(m_GizmoInputSurface);
  
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
