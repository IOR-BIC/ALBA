/*=========================================================================

 Program: MAF2
 Module: mafOpRemoveCellsTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpRemoveCellsTest.h"
#include <iostream>

#include "mafOpRemoveCells.h"
#include "mafString.h"
#include "mafNode.h"
#include "mafVMESurface.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"

#include "vtkSphereSource.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"

void mafOpRemoveCellsTest::setUp()
{
  //Create storage
  
  m_Storage = mafVMEStorage::New();
  m_Storage->GetRoot()->SetName("root");
  m_Storage->GetRoot()->Initialize();

  m_Root = m_Storage->GetRoot();

  // create a Surface
  vtkMAFSmartPointer<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(10);
  sphereSource->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphereSource->GetOutput(),0);
  m_Root->AddChild(surface);
  
  m_Surface = mafVMESurface::SafeDownCast(m_Root->GetChild(0));
  CPPUNIT_ASSERT(m_Surface);
  CPPUNIT_ASSERT(m_Surface->GetOutput()->GetVTKData());

  // generate vtk data
  m_Surface->GetOutput()->GetVTKData()->Update();
  CPPUNIT_ASSERT(m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells());
  
}

void mafOpRemoveCellsTest::tearDown()
{
  mafDEL(m_Storage);
}

void mafOpRemoveCellsTest::TestFixture()
{

}

void mafOpRemoveCellsTest::TestConstructor()
{
	mafOpRemoveCells *removeCells=new mafOpRemoveCells("Remove Cells");
	removeCells->TestModeOn();
	
  mafDEL(removeCells);

}

void mafOpRemoveCellsTest::RenderData( vtkPolyData *data )
{
  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInput(data);
  mapper->ScalarVisibilityOn();

  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();

  renderWindowInteractor->Start();

  mafSleep(1000);

}

void mafOpRemoveCellsTest::TestRemoveCells()
{
  int numInputTriangles = m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells();
  
  CPPUNIT_ASSERT_EQUAL(96,numInputTriangles);

  //Init importer 
  mafOpRemoveCells *removeCells=new mafOpRemoveCells("Remove Cells");
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
  m_Surface->GetOutput()->GetVTKData()->Update();
  int numOutputTriangles = m_Surface->GetOutput()->GetVTKData()->GetNumberOfCells();

  CPPUNIT_ASSERT_EQUAL(84,numOutputTriangles);

  mafDEL(removeCells);
}
