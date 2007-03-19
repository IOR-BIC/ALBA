/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkRemoveCellsFilterTest.cpp,v $
Language:  C++
Date:      $Date: 2007-03-19 15:29:56 $
Version:   $Revision: 1.2 $
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
#include "vtkInteractorStyleSwitch.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataNormals.h"

#include "vtkSphereSource.h"
#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkRemoveCellsFilter.h"

#include "vtkRemoveCellsFilterTest.h"

void vtkRemoveCellsFilterTest::setUp()
{
 
}

void vtkRemoveCellsFilterTest::tearDown()
{

}

void vtkRemoveCellsFilterTest::TestFixture()
{

}

void vtkRemoveCellsFilterTest::RenderData( vtkPolyData *data )
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

void vtkRemoveCellsFilterTest::TestRemoveMarkedCells()
{
  
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

	vtkMAFSmartPointer<vtkRemoveCellsFilter> rc;
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

void vtkRemoveCellsFilterTest::TestMarkCell()
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkMAFSmartPointer<vtkRemoveCellsFilter> rc;
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


void vtkRemoveCellsFilterTest::TestUndoMarks()
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkMAFSmartPointer<vtkRemoveCellsFilter> rc;
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