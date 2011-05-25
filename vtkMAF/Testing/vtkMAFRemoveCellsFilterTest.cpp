/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFRemoveCellsFilterTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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

#ifdef WIN32
  Sleep(1000);
#else
  usleep(1000*1000);
#endif

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
