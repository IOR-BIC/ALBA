/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalarTest
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
#include "albaVMEOutputScalarTest.h"

#include "albaVMEOutputScalar.h"
#include "albaVMEScalar.h"

#include "albaSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaVMEOutputScalarTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputScalarTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaVMEOutputScalar *outputScalar = albaVMEOutputScalar::New();
  albaDEL(outputScalar);
}
//----------------------------------------------------------------------------
void albaVMEOutputScalarTest::TestGetScalarData()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMEScalar> scalar;
	scalar->SetData(12.5, 0.0);
	scalar->SetData(25.0, 1.0);

  m_Result = 12.5 == scalar->GetScalarOutput()->GetScalarData();
  TEST_RESULT;

	scalar->SetTimeStamp(0.5);
	m_Result = 12.5 == scalar->GetScalarOutput()->GetScalarData();
	TEST_RESULT;

	scalar->SetTimeStamp(1.0);
	m_Result = 25 == scalar->GetScalarOutput()->GetScalarData();
	TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEOutputScalarTest::TestGetVTKData_TestUpdateVTKRepresentation()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMEScalar> scalar;
	scalar->SetData(12.5, 0.0);
	scalar->SetData(25.0, 1.0);

	vtkPolyData *polyData = vtkPolyData::SafeDownCast(scalar->GetScalarOutput()->GetVTKData()); //implicit call to TestUpdateVTKRepresentation()
  //strange polydata representation

	m_Result = polyData->GetNumberOfCells() == 1 &&
		         polyData->GetNumberOfPoints() == 1 &&
						 polyData->GetNumberOfLines() == 1 &&
						 vtkDoubleArray::SafeDownCast(polyData->GetPointData()->GetScalars())->GetNumberOfTuples() == 1 &&
						 vtkDoubleArray::SafeDownCast(polyData->GetPointData()->GetScalars())->GetNumberOfComponents() == 1 &&
						 vtkDoubleArray::SafeDownCast(polyData->GetPointData()->GetScalars())->GetTuple1(0) == 12.5;

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEOutputScalarTest::TestUpdate_GetScalarDataString()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMEScalar> scalar;
	scalar->SetData(12.5, 0.0);
	scalar->SetData(25.0, 1.0);

	scalar->GetScalarOutput()->Update();
	//strange polydata representation

	m_Result = albaString("12.5").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;

	scalar->SetTimeStamp(0.5);
	scalar->GetScalarOutput()->Update();
	m_Result = albaString("12.5").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;

	scalar->SetTimeStamp(1.0);
	scalar->GetScalarOutput()->Update();
	m_Result = albaString("25").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;  
}
