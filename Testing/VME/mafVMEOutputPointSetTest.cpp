/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPointSetTest
 Authors: Roberto Mucci
 
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
#include "mafVMEOutputPointSetTest.h"

#include "mafVMEOutputPointSet.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEOutputPointSetTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputPointSetTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPointSet outputPointSet;
}
//----------------------------------------------------------------------------
void mafVMEOutputPointSetTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPointSet *outputPointSet = new mafVMEOutputPointSet();
  cppDEL(outputPointSet);
}
//----------------------------------------------------------------------------
void mafVMEOutputPointSetTest::TestGetPointSetData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;


  landmark->SetPoint(x,y,z);
  
  mafVMEOutputPointSet *outputPointSet = NULL;
  outputPointSet = (mafVMEOutputPointSet*)landmark->GetOutput();
  m_Result = outputPointSet->GetPointSetData() == landmark->GetOutput()->GetVTKData();

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafVMEOutputPointSetTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMELandmark> landmark1;

  double x = 5;
  double y = 10;
  double z = 15.5;

  landmark->SetPoint(x,y,z);
  landmark1->SetPoint(x,y,z);

  mafVMEOutputPointSet *outputPointSet = NULL;
  outputPointSet = (mafVMEOutputPointSet*)landmark->GetOutput();


  mmaMaterial *material;

  m_Result = landmark->GetMaterial() == outputPointSet->GetMaterial();
  TEST_RESULT;
  material = outputPointSet->GetMaterial();


  m_Result = outputPointSet->GetMaterial() == material && landmark->GetMaterial() == material;
  TEST_RESULT;

  mafVMEOutputPointSet *outputPointSet1 = NULL;
  outputPointSet1 = (mafVMEOutputPointSet*)landmark1->GetOutput();
  outputPointSet1->SetMaterial(material);

  m_Result = outputPointSet1->GetMaterial() == outputPointSet->GetMaterial();
  TEST_RESULT;

}
