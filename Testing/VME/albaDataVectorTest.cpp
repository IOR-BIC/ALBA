/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataVectorTest
 Authors: Matteo Giacomoni
 
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
#include "albaDataVectorTest.h"

#include "albaDataVector.h"
#include "albaVMEItemVTK.h"

//----------------------------------------------------------------------------
void albaDataVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaDataVectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaDataVector mv;
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaDataVector *mv = new albaDataVector();
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestSetSingleFileMode()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();

  // Default Single File Mode is true
  CPPUNIT_ASSERT(dv->GetSingleFileMode() == true);
  
  dv->SetSingleFileMode(false);

  CPPUNIT_ASSERT(dv->GetSingleFileMode() == false);

  // If Single File Mode is Modified IsDataModified() Should return true
  CPPUNIT_ASSERT(dv->IsDataModified() == true);

  cppDEL(dv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestAppendItem()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();
  albaVMEItemVTK *item0,*item1;
  albaNEW(item0);
  albaNEW(item1);

  item0->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);

  dv->AppendItem(item0);
  dv->AppendItem(item1);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  albaDEL(item0);
  albaDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestPrependItem()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();
  albaVMEItemVTK *item0,*item1;
  albaNEW(item0);
  albaNEW(item1);

  item0->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);

  dv->PrependItem(item0);
  dv->PrependItem(item1);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  albaDEL(item0);
  albaDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestInsertItem()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();
  albaVMEItemVTK *item0a,*item1,*item0b;
  albaNEW(item0a);
  albaNEW(item1);
  albaNEW(item0b);

  item0a->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);
  item0b->SetTimeStamp(0.0);

  dv->InsertItem(item0a);
  dv->InsertItem(item1);
  dv->InsertItem(item0b);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0b);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  albaDEL(item0a);
  albaDEL(item0b);
  albaDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestModified()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();
  
  dv->Modified();

  CPPUNIT_ASSERT(dv->IsDataModified() == true);

  CPPUNIT_ASSERT(dv->GetMTime() != 0);

  cppDEL(dv);
}
//----------------------------------------------------------------------------
void albaDataVectorTest::TestSetCrypting()
//----------------------------------------------------------------------------
{
  albaDataVector *dv = new albaDataVector();

  // Default Crypting Mode is false
  CPPUNIT_ASSERT(dv->GetCrypting() == false);

  dv->SetCrypting(true);

  CPPUNIT_ASSERT(dv->GetCrypting() == true);

  cppDEL(dv);
}
