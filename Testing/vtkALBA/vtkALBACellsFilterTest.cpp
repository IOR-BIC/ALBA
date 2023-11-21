/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACellsFilterTest
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
#include "vtkSphereSource.h"
#include "vtkALBACellsFilter.h"
#include "vtkALBACellsFilterTest.h"

//#include <assert.h>

void vtkALBACellsFilterTest::TestFixture()
{

}

void vtkALBACellsFilterTest::TestMarkCell()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

	vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);
 
  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
}



void vtkALBACellsFilterTest::TestUnmarkCell()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->UnmarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkALBACellsFilterTest::TestToggleCell()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->ToggleCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkALBACellsFilterTest::TestUndoMarks()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);

  mc->UndoMarks();
  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

}

void vtkALBACellsFilterTest::TestGetNumberOfMarkedCells()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();
  
  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 0);

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 5;
  int markedCellsIDs[markedCellsNumber] = {0, 10, 20, 30, 40} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 5);
}

void vtkALBACellsFilterTest::TestGetIdMarkedCell()
{  
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  vtkALBASmartPointer<vtkALBACellsFilter> mc;
  mc->SetInput(sphere->GetOutput());
  mc->Update();

  int nc = sphere->GetOutput()->GetNumberOfCells();
  assert(nc != 0);

  const int markedCellsNumber = 3;
  vtkIdType markedCellsIDs[markedCellsNumber] = {0, 10, 20} ;

  for (int i = 0;i < markedCellsNumber; i++)
  {
    mc->MarkCell(markedCellsIDs[i]);
  }

  mc->Update();

  CPPUNIT_ASSERT(mc->GetNumberOfMarkedCells() == 3);
  
  for (int i = 0;i < markedCellsNumber; i++)
  {
    CPPUNIT_ASSERT_EQUAL(mc->GetIdMarkedCell(i) , markedCellsIDs[i]);
  }

}
