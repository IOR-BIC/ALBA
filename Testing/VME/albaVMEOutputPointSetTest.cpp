/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPointSetTest
 Authors: Roberto Mucci
 
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
#include "albaVMEOutputPointSetTest.h"

#include "albaVMEOutputPointSet.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"

#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaVMEOutputPointSetTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputPointSetTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPointSet outputPointSet;
}
//----------------------------------------------------------------------------
void albaVMEOutputPointSetTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPointSet *outputPointSet = new albaVMEOutputPointSet();
  cppDEL(outputPointSet);
}
//----------------------------------------------------------------------------
void albaVMEOutputPointSetTest::TestGetPointSetData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;


  landmark->SetPoint(x,y,z);
  
  albaVMEOutputPointSet *outputPointSet = NULL;
  outputPointSet = (albaVMEOutputPointSet*)landmark->GetOutput();
  m_Result = outputPointSet->GetPointSetData() == landmark->GetOutput()->GetVTKData();

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMEOutputPointSetTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMELandmark> landmark1;

  double x = 5;
  double y = 10;
  double z = 15.5;

  landmark->SetPoint(x,y,z);
  landmark1->SetPoint(x,y,z);

  albaVMEOutputPointSet *outputPointSet = NULL;
  outputPointSet = (albaVMEOutputPointSet*)landmark->GetOutput();


  mmaMaterial *material= landmark->GetMaterial();
	

  m_Result = material == outputPointSet->GetMaterial();
  TEST_RESULT;
  material = outputPointSet->GetMaterial();


  m_Result = outputPointSet->GetMaterial() == material && landmark->GetMaterial() == material;
  TEST_RESULT;

  albaVMEOutputPointSet *outputPointSet1 = NULL;
  outputPointSet1 = (albaVMEOutputPointSet*)landmark1->GetOutput();
  outputPointSet1->SetMaterial(material);

  m_Result = outputPointSet1->GetMaterial() == outputPointSet->GetMaterial();
  TEST_RESULT;

}
