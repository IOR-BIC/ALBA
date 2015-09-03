/*=========================================================================

 Program: MAF2
 Module: mmaVolumeMaterialTest
 Authors: Paolo Quadrani
 
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
#include "mmaVolumeMaterialTest.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial vm;
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *vm = new mmaVolumeMaterial();
  delete vm;
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *vm1 = new mmaVolumeMaterial();
  mmaVolumeMaterial *vm2 = new mmaVolumeMaterial();
  vm1->m_NumValues = 256;
  vm2->DeepCopy(vm1);
  result = vm2->m_NumValues == 256;
  TEST_RESULT;
  
  delete vm1;
  delete vm2;
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestEquals()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *vm1 = new mmaVolumeMaterial();
  mmaVolumeMaterial *vm2 = new mmaVolumeMaterial();
  vm1->m_NumValues = 256;
  vm2->m_NumValues = 256;

  result = vm2->Equals(vm1);
  TEST_RESULT;

  delete vm1;
  delete vm2;
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestUpdateProp()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *vm1 = new mmaVolumeMaterial();
  vm1->m_HueRange[1] = 0.4;
  vm1->m_SaturationRange[1] = 0.5;
  vm1->m_TableRange[1] = 300.0;
  vm1->UpdateProp();

  double hr[2], sr[2], tr[2];
  vm1->m_ColorLut->GetHueRange(hr);
  vm1->m_ColorLut->GetSaturationRange(sr);
  vm1->m_ColorLut->GetTableRange(tr);
  result = mafEquals(hr[0], 0.0);
  TEST_RESULT;
  result = mafEquals(hr[1], 0.4);
  TEST_RESULT;
  result = mafEquals(sr[0], 0.0);
  TEST_RESULT;
  result = mafEquals(sr[1], 0.5);
  TEST_RESULT;
  result = mafEquals(tr[0], 0.0);
  TEST_RESULT;
  result = mafEquals(tr[1], 300.0);
  TEST_RESULT;

  delete vm1;
}

//----------------------------------------------------------------------------
void mmaVolumeMaterialTest::TestUpdateFromTables()
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *vm1 = new mmaVolumeMaterial();
  
  // Set a valid range to allow UpdateFromTables() update m_TableRange member
  vm1->m_TableRange[0] = 0;
  vm1->m_TableRange[0] = 1;
  
  vm1->m_ColorLut->SetHueRange(0.0, 0.4);
  vm1->m_ColorLut->SetSaturationRange(0.0, 0.5);
  vm1->m_ColorLut->SetTableRange(100.0, 300.0);
  vm1->UpdateFromTables();
  
  result = mafEquals(vm1->m_HueRange[0], 0);
  TEST_RESULT;
  result = mafEquals(vm1->m_HueRange[1], 0.4);
  TEST_RESULT;
  result = mafEquals(vm1->m_SaturationRange[0], 0);
  TEST_RESULT;
  result = mafEquals(vm1->m_SaturationRange[1], 0.5);
  TEST_RESULT;
  result = mafEquals(vm1->m_TableRange[0], 100.0);
  TEST_RESULT;
  result = mafEquals(vm1->m_TableRange[1], 300.0);
  TEST_RESULT;

  delete vm1;
}
