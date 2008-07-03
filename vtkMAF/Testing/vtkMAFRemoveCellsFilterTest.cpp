/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFRemoveCellsFilterTest.cpp,v $
Language:  C++
Date:      $Date: 2008-07-03 11:34:43 $
Version:   $Revision: 1.1 $
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
#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFSmartPointer.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"

#include "vtkSphereSource.h"
#include "vtkCamera.h"
#include "vtkMAFRemoveCellsFilter.h"

#include "vtkMAFRemoveCellsFilterTest.h"

void vtkMAFRemoveCellsFilterTest::setUp()
{
 
}

void vtkMAFRemoveCellsFilterTest::tearDown()
{

}

void vtkMAFRemoveCellsFilterTest::TestFixture()
{

}

void vtkMAFRemoveCellsFilterTest::RenderData( vtkPolyData *data )
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

void vtkMAFRemoveCellsFilterTest::TestRemoveMarkedCells()
{
  
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

	vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> rc;
  rc->SetInput(sphere->GetOutput());

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  // mark half sphere
  for (int i = 0;i < nc/2; i++)
  {
    rc->MarkCell(i);
  }

  // remove cells
  rc->RemoveMarkedCells();
  rc->Update();

  // visual feedback
  // RenderData(rc->GetOutput());

  CPPUNIT_ASSERT(rc->GetOutput()->GetNumberOfCells() == (nc/2));
}

void vtkMAFRemoveCellsFilterTest::TestMarkCell()
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> rc;
  rc->SetInput(sphere->GetOutput());

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  // mark half sphere
  for (int i = 0;i < nc/2; i++)
  {
    rc->MarkCell(i);
  }

  rc->Update();

  CPPUNIT_ASSERT(true);

}


void vtkMAFRemoveCellsFilterTest::TestUndoMarks()
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> rc;
  rc->SetInput(sphere->GetOutput());

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  // mark half sphere
  for (int i = 0;i < nc/2; i++)
  {
    rc->MarkCell(i);
  }

  rc->Update();

  rc->UndoMarks();
  rc->Update();

  CPPUNIT_ASSERT(true);

}
