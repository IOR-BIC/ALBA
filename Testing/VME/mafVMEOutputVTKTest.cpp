/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputVTKTest.cpp,v $
Language:  C++
Date:      $Date: 2009-08-18 07:51:30 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
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
#include "mafVMEOutputVTKTest.h"

#include "mafVMEOutputVTK.h"
#include "mafVMEGeneric.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputVTKTest::setUp()
//----------------------------------------------------------------------------
{
	m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputVTKTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEOutputVTKTest::TestGetVTKData()
//----------------------------------------------------------------------------
{

  vtkMAFSmartPointer<vtkImageData> data;
  data->SetDimensions(5, 5, 1);
  data->SetOrigin(0, 0, 0);
  data->SetSpacing(1,1,1);
  data->Update();

  mafTimeStamp timestamp;
  timestamp = time(0);
  mafSmartPointer<mafVMEGeneric> vme;
  vme->SetData(data, timestamp);
  vme->Update();

  mafVMEOutputVTK *output = mafVMEOutputVTK::SafeDownCast(vme->GetVTKOutput());
  output->GetVTKData()->Update();
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