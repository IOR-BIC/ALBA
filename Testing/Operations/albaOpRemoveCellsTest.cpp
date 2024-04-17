/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRemoveCellsTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOpRemoveCellsTest.h"
#include <iostream>

#include "albaOpRemoveCells.h"
#include "albaString.h"
#include "albaVME.h"
#include "albaVMESurface.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"

#include "vtkSphereSource.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"

void albaOpRemoveCellsTest::BeforeTest()
{
  //Create storage
  
  m_Storage = albaVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
  m_Storage->GetRoot()->Initialize();

  m_Root = m_Storage->GetRoot();

  // create a Surface
  vtkALBASmartPointer<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(10);
  sphereSource->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphereSource->GetOutput(),0);
  m_Root->AddChild(surface);
  
  m_Surface = albaVMESurface::SafeDownCast(m_Root->GetChild(0));
  CPPUNIT_ASSERT(m_Surface);
  CPPUNIT_ASSERT(m_Surface->GetOutput()->GetVTKData());

  // generate vtk data
  CPPUNIT_ASSERT(m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells());
  
}

void albaOpRemoveCellsTest::AfterTest()
{
  albaDEL(m_Storage);
}

void albaOpRemoveCellsTest::TestFixture()
{

}

void albaOpRemoveCellsTest::TestConstructor()
{
	albaOpRemoveCells *removeCells=new albaOpRemoveCells("Remove Cells");
	removeCells->TestModeOn();
	
  albaDEL(removeCells);

}

void albaOpRemoveCellsTest::RenderData( vtkPolyData *data )
{
  vtkALBASmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkALBASmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputData(data);
  mapper->ScalarVisibilityOn();

  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();

  renderWindowInteractor->Start();
}

void albaOpRemoveCellsTest::TestRemoveCells()
{
  int numInputTriangles = m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells();
  
  CPPUNIT_ASSERT_EQUAL(96,numInputTriangles);

  //Init importer 
  albaOpRemoveCells *removeCells=new albaOpRemoveCells("Remove Cells");
  removeCells->TestModeOn();
  removeCells->SetInput(m_Surface);
  removeCells->OpRun();
  removeCells->SetDiameter(10);
  removeCells->SetSeed(1);
  removeCells->MarkCells();
  
  // RenderData(vtkPolyData::SafeDownCast(m_Surface->GetOutput()->GetVTKData()));

  removeCells->RemoveCells();
  removeCells->OpDo();

  // RenderData(vtkPolyData::SafeDownCast(m_Surface->GetOutput()->GetVTKData()));
  
  // need to update vtk data again
  int numOutputTriangles = m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells();

  CPPUNIT_ASSERT_EQUAL(84,numOutputTriangles);

  albaDEL(removeCells);
}
