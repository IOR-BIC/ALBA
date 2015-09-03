/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputScalarTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEOutputScalarTest.h"

#include "mafVMEOutputScalar.h"
#include "mafVMEScalar.h"

#include "mafSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEOutputScalarTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputScalarTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafVMEOutputScalar *outputScalar = mafVMEOutputScalar::New();
  mafDEL(outputScalar);
}
//----------------------------------------------------------------------------
void mafVMEOutputScalarTest::TestGetScalarData()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafVMEScalar> scalar;
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
void mafVMEOutputScalarTest::TestGetVTKData_TestUpdateVTKRepresentation()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafVMEScalar> scalar;
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
void mafVMEOutputScalarTest::TestUpdate_GetScalarDataString()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafVMEScalar> scalar;
	scalar->SetData(12.5, 0.0);
	scalar->SetData(25.0, 1.0);

	scalar->GetScalarOutput()->Update();
	//strange polydata representation

	m_Result = mafString("12.5").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;

	scalar->SetTimeStamp(0.5);
	scalar->GetScalarOutput()->Update();
	m_Result = mafString("12.5").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;

	scalar->SetTimeStamp(1.0);
	scalar->GetScalarOutput()->Update();
	m_Result = mafString("25").Equals(scalar->GetScalarOutput()->GetScalarDataString());
	TEST_RESULT;  
}
