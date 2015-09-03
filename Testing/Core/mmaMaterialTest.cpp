/*=========================================================================

 Program: MAF2
 Module: mmaMaterialTest
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
#include "mmaMaterialTest.h"
#include "mafGUILutPreset.h"
#include "mmaMaterial.h"

#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mmaMaterialTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mmaMaterial m;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mmaMaterial *m = new mmaMaterial();
  delete m;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestSetMaterialTexture()
//----------------------------------------------------------------------------
{
  mmaMaterial *m = new mmaMaterial();
  m->SetMaterialTexture(5);
  int id = m->GetMaterialTextureID();
  result = id == 5;
  TEST_RESULT;
  delete m;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestSetMaterialTexture2()
//----------------------------------------------------------------------------
{
  vtkImageData *image = vtkImageData::New();
  mmaMaterial *m = new mmaMaterial();
  m->SetMaterialTexture(image);
  vtkImageData *tex = m->GetMaterialTexture();
  result = image == tex;
  TEST_RESULT;
  delete m;
  vtkDEL(image);
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mmaMaterial *m1 = new mmaMaterial();
  mmaMaterial *m2 = new mmaMaterial();
  m1->SetMaterialTexture(5);
  m2->DeepCopy(m1);
  int id = m2->GetMaterialTextureID();
  result = id == 5;
  TEST_RESULT;
  delete m1;
  delete m2;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestEquals()
//----------------------------------------------------------------------------
{
  mmaMaterial *m1 = new mmaMaterial();
  mmaMaterial *m2 = new mmaMaterial();
  m1->SetMaterialTexture(5);
  m2->SetMaterialTexture(5);

  // The two materials have different default colors
  // chose randomly into the constructor
  result = !m1->Equals(m2);
  TEST_RESULT;

  m2->DeepCopy(m1);

  result = m1->Equals(m2);
  TEST_RESULT;
  delete m1;
  delete m2;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestUpdateFromLut()
//----------------------------------------------------------------------------
{
  mmaMaterial *m1 = new mmaMaterial();

  // Default LUT is GrayLut: lutPreset(4,m_ColorLut);
  lutPreset(13, m1->m_ColorLut);
  m1->m_ColorLut->SetHueRange(0.0, 0.4);
  m1->m_ColorLut->SetSaturationRange(0.0, 0.5);
  m1->m_ColorLut->SetTableRange(100.0, 300.0);
  m1->UpdateFromLut();

  result = m1->m_NumValues == 256;
  TEST_RESULT;
  result = mafEquals(m1->m_HueRange[0], 0);
  TEST_RESULT;
  result = mafEquals(m1->m_HueRange[1], 0.4);
  TEST_RESULT;
  result = mafEquals(m1->m_SaturationRange[0], 0);
  TEST_RESULT;
  result = mafEquals(m1->m_SaturationRange[1], 0.5);
  TEST_RESULT;
  result = mafEquals(m1->m_TableRange[0], 100.0);
  TEST_RESULT;
  result = mafEquals(m1->m_TableRange[1], 300.0);
  TEST_RESULT;
 
  delete m1;
}

//----------------------------------------------------------------------------
void mmaMaterialTest::TestUpdateProp()
//----------------------------------------------------------------------------
{
  mmaMaterial *m1 = new mmaMaterial();
  m1->m_HueRange[1] = 0.4;
  m1->m_SaturationRange[1] = 0.5;
  m1->m_TableRange[1] = 300.0;
  m1->UpdateProp();

  double hr[2], sr[2], tr[2];
  m1->m_ColorLut->GetHueRange(hr);
  m1->m_ColorLut->GetSaturationRange(sr);
  m1->m_ColorLut->GetTableRange(tr);
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

  delete m1;
}
