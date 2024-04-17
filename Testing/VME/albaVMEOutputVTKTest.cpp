/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVTKTest
 Authors: Eleonora Mambrini
 
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
#include "albaVMEOutputVTKTest.h"

#include "albaVMEOutputVTK.h"
#include "albaVMEGeneric.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputVTKTest::TestGetVTKData()
//----------------------------------------------------------------------------
{

  vtkALBASmartPointer<vtkImageData> data;
  data->SetDimensions(5, 5, 1);
  data->SetOrigin(0, 0, 0);
  data->SetSpacing(1,1,1);

  albaTimeStamp timestamp;
  timestamp = time(0);
  albaSmartPointer<albaVMEGeneric> vme;
  vme->SetData(data, timestamp);
  vme->Update();

  albaVMEOutputVTK *output = albaVMEOutputVTK::SafeDownCast(vme->GetVTKOutput());
  output->Update();
  
  m_Result = output != NULL;
  TEST_RESULT;

  m_Result = output->GetVTKData()->GetNumberOfPoints() == data->GetNumberOfPoints();
  TEST_RESULT;

  m_Result = output->GetVTKData()->GetNumberOfCells() == data->GetNumberOfCells();
  TEST_RESULT;

  m_Result =  ( (output->GetVTKData()->GetBounds()[0] == data->GetBounds()[0]) && 
                (output->GetVTKData()->GetBounds()[1] == data->GetBounds()[1]) && 
                (output->GetVTKData()->GetBounds()[2] == data->GetBounds()[2]) );
  TEST_RESULT;


}