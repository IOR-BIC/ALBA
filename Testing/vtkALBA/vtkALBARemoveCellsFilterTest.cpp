/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARemoveCellsFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBASmartPointer.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"

#include "vtkSphereSource.h"
#include "vtkCamera.h"
#include "vtkALBARemoveCellsFilter.h"

#include "vtkALBARemoveCellsFilterTest.h"

#include <assert.h>

void vtkALBARemoveCellsFilterTest::TestFixture()
{

}

void vtkALBARemoveCellsFilterTest::RenderData( vtkPolyData *data )
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

void vtkALBARemoveCellsFilterTest::TestRemoveMarkedCells()
{
  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

	vtkALBASmartPointer<vtkALBARemoveCellsFilter> rc;
  rc->SetInputConnection(sphere->GetOutputPort());
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

void vtkALBARemoveCellsFilterTest::TestMarkCell()
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBARemoveCellsFilter> rc;
  rc->SetInputConnection(sphere->GetOutputPort());
  rc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  // mark half sphere
  for (int i = 0;i < nc/2; i++)
  {
    rc->MarkCell(i);
  }

  rc->Update();
}


void vtkALBARemoveCellsFilterTest::TestUndoMarks()
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBARemoveCellsFilter> rc;
  rc->SetInputConnection(sphere->GetOutputPort());
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
}
