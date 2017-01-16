/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleIsotropicTest
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

#include "mafGizmoScaleIsotropicTest.h"
#include "mafInteractionTests.h"

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
#include "mafServiceLocator.h"
#include "mafFakeLogicForTest.h"


void mafGizmoScaleIsotropicTest::BeforeTest()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoScaleIsotropicTest::AfterTest()
{
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
	mafFakeLogicForTest *logic = (mafFakeLogicForTest*)mafServiceLocator::GetLogicManager();
	logic->ClearCalls();

  mafGizmoScaleIsotropic *gizmoScaleIsotropic = new mafGizmoScaleIsotropic(m_GizmoInputSurface);
 
  gizmoScaleIsotropic->Show(true);
  
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == mafFakeLogicForTest::VME_SHOW);
		  
  cppDEL(gizmoScaleIsotropic);
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

