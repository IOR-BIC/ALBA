/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDRegionGrowingLocalGlobalThresholdTest
 Authors: Matteo Giacomoni
 
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

#include "vtkMEDRegionGrowingLocalGlobalThresholdTest.h"
#include "vtkMEDRegionGrowingLocalGlobalThreshold.h"

#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkDataSetWriter.h"

#define X_DIM 4
#define Y_DIM 3
#define Z_DIM 2

#define DELTA 0.1

//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::setUp()
//-------------------------------------------------------------------------
{
  m_Image = vtkImageData::New();

  m_Image->SetDimensions(X_DIM,Y_DIM,Z_DIM);
  m_Image->SetSpacing(1.0,1.0,1.0);
  m_Image->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->SetNumberOfTuples(4*3*2);
  scalars->SetName("Scalars");

  for (int i=0;i<scalars->GetNumberOfTuples();i++)
  {
      scalars->SetTuple1(i,(double)i);
  }

  m_Image->GetPointData()->AddArray(scalars);
  m_Image->GetPointData()->SetActiveScalars("Scalars");
  m_Image->Update();

  scalars->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::tearDown()
//-------------------------------------------------------------------------
{
  m_Image->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestExecute()
//-------------------------------------------------------------------------
{
      
  vtkImageData *imageToExecute = vtkImageData::New();

  imageToExecute->SetDimensions(X_DIM,Y_DIM,Z_DIM);
  imageToExecute->SetSpacing(1.0,1.0,1.0);
  imageToExecute->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->SetNumberOfTuples(4*3*2);
  scalars->SetName("Scalars");

  for (int i=0;i<scalars->GetNumberOfTuples();i++)
  {
    if (((i%12)/5.0 == 1 || (i%12)/6.0 == 1) && i%12 != 0)
    {
      scalars->SetTuple1(i,(double)0);
    }
    else
    {
      scalars->SetTuple1(i,(double)10);
    }

  }

  imageToExecute->GetPointData()->AddArray(scalars);
  imageToExecute->GetPointData()->SetActiveScalars("Scalars");
  imageToExecute->Update();

  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->SetInput(imageToExecute);
  m_Filter->SetUpperThreshold(100);
  m_Filter->SetLowerThreshold(0);
  m_Filter->SetLowerLabel(0);
  m_Filter->SetUpperLabel(100);
  m_Filter->Update();
  vtkImageData *output = m_Filter->GetOutput();
  CPPUNIT_ASSERT( output != NULL );

  double spacingInput[3],spacingOutput[3];
  output->GetSpacing(spacingOutput);
  imageToExecute->GetSpacing(spacingInput);

  for (int i=0;i<3;i++)
  {
  	CPPUNIT_ASSERT( spacingInput[i] == spacingOutput[i] );
  }

  int dimInput[3],dimOutput[3];
  output->GetDimensions(dimOutput);
  imageToExecute->GetDimensions(dimInput);

  for (int i=0;i<3;i++)
  {
    CPPUNIT_ASSERT( dimInput[i] == dimOutput[i] );
  }

  CPPUNIT_ASSERT( output->GetPointData()->GetScalars() != NULL );

  for (int i=0;i<output->GetPointData()->GetScalars()->GetNumberOfTuples();i++)
  {
    if (((i%12)/5.0 == 1 || (i%12)/6.0 == 1) && i%12 != 0)
    {
      CPPUNIT_ASSERT( output->GetPointData()->GetScalars()->GetTuple1(i) == 0  );
    }
    else
    {
      CPPUNIT_ASSERT( output->GetPointData()->GetScalars()->GetTuple1(i) == 100  );
    }
  }

  m_Filter->Delete();
  scalars->Delete();
  imageToExecute->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestComputeStandardDeviation()
//-------------------------------------------------------------------------
{
  int error;
  int indexNearest[26];
  vtkImageData *imBordered = vtkImageData::New();

  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->SetInput(m_Image);
  m_Filter->BorderCreate(imBordered);
  m_Filter->ComputeIndexNearstPoints(0,indexNearest,error,imBordered);
  CPPUNIT_ASSERT( error == FALSE );
  double mean = m_Filter->ComputeMeanValue(0,indexNearest,error,imBordered);
  CPPUNIT_ASSERT( error == FALSE );
  double stdDev = m_Filter->ComputeStandardDeviation(0,indexNearest,mean,error,imBordered);

  CPPUNIT_ASSERT ( stdDev < (5.95 + DELTA) && stdDev > (5.95 - DELTA) );

  m_Filter->Delete();
  imBordered->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestComputeMeanValue()
//-------------------------------------------------------------------------
{
  int error;
  int indexNearest[26];
  vtkImageData *imBordered = vtkImageData::New();

  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->SetInput(m_Image);
  m_Filter->BorderCreate(imBordered);
  m_Filter->ComputeIndexNearstPoints(30,indexNearest,error,imBordered);
  CPPUNIT_ASSERT( error == TRUE );
  m_Filter->ComputeIndexNearstPoints(0,indexNearest,error,imBordered);
  CPPUNIT_ASSERT( error == FALSE );
  double mean = m_Filter->ComputeMeanValue(0,indexNearest,error,imBordered);
  CPPUNIT_ASSERT( error == FALSE && mean == (153.0/26.0));

  m_Filter->Delete();
  imBordered->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestSetOutputScalarType()
//-------------------------------------------------------------------------
{
  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->SetInput(m_Image);
  m_Filter->SetOutputScalarType(VTK_SHORT);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_SHORT );

  m_Filter->SetOutputScalarType(VTK_UNSIGNED_SHORT);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_UNSIGNED_SHORT );

  m_Filter->SetOutputScalarType(VTK_CHAR);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_CHAR );

  m_Filter->SetOutputScalarType(VTK_UNSIGNED_CHAR);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_UNSIGNED_CHAR );

  m_Filter->SetOutputScalarType(VTK_DOUBLE);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_DOUBLE );

  m_Filter->SetOutputScalarType(VTK_FLOAT);
  m_Filter->Update();
  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_FLOAT );

  m_Filter->Delete();
}
//-------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThresholdTest::TestBorderCreate()
//-------------------------------------------------------------------------
{
  vtkImageData *imBordered = vtkImageData::New();

  vtkDoubleArray *scalars = vtkDoubleArray::SafeDownCast(m_Image->GetPointData()->GetScalars());

  CPPUNIT_ASSERT( scalars != NULL );

  m_Filter = vtkMEDRegionGrowingLocalGlobalThreshold::New();
  m_Filter->SetInput(m_Image);
  m_Filter->BorderCreate(imBordered);

  int dimsOut[3];
  imBordered->GetDimensions(dimsOut);

  CPPUNIT_ASSERT( dimsOut[0] == X_DIM+2 && dimsOut[1] == Y_DIM+2 && dimsOut[2] == Z_DIM+2 );

  //Test if the border exist
  int sliceDim = X_DIM*Y_DIM;
  int sliceBorderedDim = (X_DIM+2)*(Y_DIM+2);
  for (int z=1;z<Z_DIM;z++)
  {
    for (int y=1;y<Y_DIM;y++)
    {
      for (int x=1;x<X_DIM;x++)
      {
        double valueBordered = imBordered->GetPointData()->GetScalars()->GetTuple1((z*sliceBorderedDim)+((X_DIM+2)*y)+x);
        double value = scalars->GetTuple1(((z-1)*sliceDim)+((X_DIM*(y-1))+(x-1)));
        CPPUNIT_ASSERT( valueBordered == value );
      }
    }
  }

  m_Filter->Delete();
  imBordered->Delete();
}
