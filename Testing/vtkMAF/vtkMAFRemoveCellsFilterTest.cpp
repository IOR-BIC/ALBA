/*=========================================================================

 Program: MAF2
 Module: vtkMAFRemoveCellsFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

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

#include <assert.h>

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

}

void vtkMAFRemoveCellsFilterTest::TestRemoveMarkedCells()
{
  
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

	vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> rc;
  rc->SetInput(sphere->GetOutput());
  rc->Update();

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
  rc->Update();

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
  rc->Update();

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
