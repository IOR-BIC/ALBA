/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARectilinearGridToRectilinearGridFilterTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBARectilinearGridToRectilinearGridFilterTest.h"

#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include "vtkRectilinearGrid.h"
#include "vtkALBADummyRectilinearGridToRectilinearGridFilter.h"

using namespace std;

//----------------------------------------------------------------------------
void vtkALBARectilinearGridToRectilinearGridFilterTest::TestFixture()
//----------------------------------------------------------------------------
{
}

void vtkALBARectilinearGridToRectilinearGridFilterTest::TestGetInput()
{
  vtkALBASmartPointer<vtkRectilinearGrid> originalRectilinearGrid;

  int dim[3];
  int extent[6] = {0, 127, 128, 255, 0, 127};
  int maxNumOfPieces = 12800;

  originalRectilinearGrid->SetDimensions(dim);
  originalRectilinearGrid->SetExtent(extent);
  originalRectilinearGrid->SetMaximumNumberOfPieces(maxNumOfPieces);

  originalRectilinearGrid->Update();

  originalRectilinearGrid->GetDimensions(dim);

  vtkALBASmartPointer<vtkALBADummyRectilinearGridToRectilinearGridFilter> dummyFilter;
  dummyFilter->SetInput(originalRectilinearGrid);
  dummyFilter->Update();

  vtkRectilinearGrid *outputGrid;
  outputGrid = dummyFilter->GetInput();

  CPPUNIT_ASSERT(outputGrid);

  int *outputGridExtent, *outputGridDimensions;

  outputGridExtent      = outputGrid->GetExtent();
  outputGridDimensions  = outputGrid->GetDimensions();
 
  CPPUNIT_ASSERT(outputGridDimensions);
  CPPUNIT_ASSERT(outputGridExtent);

  CPPUNIT_ASSERT(originalRectilinearGrid->GetLength() == outputGrid->GetLength());
  CPPUNIT_ASSERT(originalRectilinearGrid->GetMaximumNumberOfPieces() == outputGrid->GetMaximumNumberOfPieces());

  for(int i=0; i<3;i++)
  {
    CPPUNIT_ASSERT(extent[i] == outputGridExtent[i]);
    CPPUNIT_ASSERT(extent[i+3] == outputGridExtent[i+3]);
    CPPUNIT_ASSERT(dim[i] == outputGridDimensions[i]);
  }
}