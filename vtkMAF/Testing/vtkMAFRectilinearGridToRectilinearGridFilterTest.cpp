/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFRectilinearGridToRectilinearGridFilterTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFRectilinearGridToRectilinearGridFilterTest.h"

#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkRectilinearGrid.h"
#include "vtkMAFDummyRectilinearGridToRectilinearGridFilter.h"

using namespace std;

//----------------------------------------------------------------------------
void vtkMAFRectilinearGridToRectilinearGridFilterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFRectilinearGridToRectilinearGridFilterTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFRectilinearGridToRectilinearGridFilterTest::tearDown()
//----------------------------------------------------------------------------
{
}

void vtkMAFRectilinearGridToRectilinearGridFilterTest::TestGetInput()
{
  vtkMAFSmartPointer<vtkRectilinearGrid> originalRectilinearGrid;

  int dim[3];
  int extent[6] = {0, 127, 128, 255, 0, 127};
  int maxNumOfPieces = 12800;

  originalRectilinearGrid->SetDimensions(dim);
  originalRectilinearGrid->SetExtent(extent);
  originalRectilinearGrid->SetMaximumNumberOfPieces(maxNumOfPieces);

  originalRectilinearGrid->Update();

  originalRectilinearGrid->GetDimensions(dim);

  vtkMAFSmartPointer<vtkMAFDummyRectilinearGridToRectilinearGridFilter> dummyFilter;
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