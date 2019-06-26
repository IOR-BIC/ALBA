/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalarMatrixTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaVMEOutputScalarMatrixTest.h"

#include "albaVMEOutputScalarMatrix.h"
#include "albaVMEScalarMatrix.h"

#include "albaSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_Result = false;
  in_data.set_size(10,20);
  in_data.fill(1.0);
  in_data[5][10] = 0.0;
}

//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaVMEOutputScalarMatrix *outputScalarMatrix =albaVMEOutputScalarMatrix::New();
  albaDEL(outputScalarMatrix);
}
//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::TestGetScalarData()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMEScalarMatrix> scalarMatrix;
	scalarMatrix->SetData(in_data,0.0);

  m_Result = in_data.rows() == scalarMatrix->GetScalarOutput()->GetScalarData().rows() &&
             in_data.cols() == scalarMatrix->GetScalarOutput()->GetScalarData().cols() &&
             scalarMatrix->GetScalarOutput()->GetScalarData().get(5,10) == 0.0 && 
             scalarMatrix->GetScalarOutput()->GetScalarData().get(0,0) == 1.0;
  
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::TestGetVTKData_UpdateVTKRepresentation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalarMatrix> scalarMatrix;
  scalarMatrix->SetData(in_data,0.0);
  scalarMatrix->SetScalarIdForZCoordinate(0); //represent the line that output will retrieve
  scalarMatrix->GetScalarOutput()->Update();

	vtkPolyData *polyData0 = vtkPolyData::SafeDownCast(scalarMatrix->GetScalarOutput()->GetVTKData()); //implicit call to UpdateVTKRepresentation()
  
  int i=0;
  for(;i<polyData0->GetNumberOfPoints();i++)
  {
    double point[3];
    polyData0->GetPoint(i, point);
    
    /*
    x coordinate is time
    y coordinate is progress
    z coordinate is the value
    */

    m_Result = point[0] == 0. && point[1] == i && point[2] == 1;
    TEST_RESULT;
  }

  vtkCellArray *lines0 = polyData0->GetLines();
  m_Result = lines0->GetNumberOfCells() == in_data.cols()-1; 
  TEST_RESULT;
  

  scalarMatrix->SetScalarIdForZCoordinate(5); //represent the line that output will retrieve
  scalarMatrix->GetScalarOutput()->Update();
  scalarMatrix->GetScalarOutput()->UpdateVTKRepresentation();

  vtkPolyData *polyData5 = vtkPolyData::SafeDownCast(scalarMatrix->GetScalarOutput()->GetVTKData()); //implicit call to UpdateVTKRepresentation()
  
  int h=0;
  for(;h<polyData5->GetNumberOfPoints();h++)
  {
    double point[3];
    polyData5->GetPoint(h, point);

    /*
    x coordinate is time
    y coordinate is progress
    z coordinate is the value
    */
    bool value = (h==10?point[2] == 0:point[2] == 1); //row 5 col 10 element is 0.0
    m_Result = point[0] == 0. && point[1] == h && value;
    TEST_RESULT;
  }

  vtkCellArray *lines5 = polyData5->GetLines();
  m_Result = lines5->GetNumberOfCells() == in_data.cols()-1;

  TEST_RESULT;
  
}

//----------------------------------------------------------------------------
void albaVMEOutputScalarMatrixTest::TestUpdate_GetNumberOfRows_GetNumberOfCols()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalarMatrix> scalarMatrix;
  scalarMatrix->SetData(in_data,0.0);
	scalarMatrix->GetScalarOutput()->Update();
	
	m_Result = albaString("10").Equals(scalarMatrix->GetScalarOutput()->GetNumberOfRows());
	TEST_RESULT;

	m_Result = albaString("20").Equals(scalarMatrix->GetScalarOutput()->GetNumberOfColumns());
	TEST_RESULT;

}
