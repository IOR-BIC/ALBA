/*=========================================================================

 Program: MAF2
 Module: mafDataVectorTest
 Authors: Matteo Giacomoni
 
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
#include "mafDataVectorTest.h"

#include "mafDataVector.h"
#include "mafVMEItemVTK.h"

//----------------------------------------------------------------------------
void mafDataVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataVectorTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataVectorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafDataVector mv;
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafDataVector *mv = new mafDataVector();
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestSetSingleFileMode()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();

  // Default Single File Mode is TRUE
  CPPUNIT_ASSERT(dv->GetSingleFileMode() == true);
  
  dv->SetSingleFileMode(false);

  CPPUNIT_ASSERT(dv->GetSingleFileMode() == false);

  // If Single File Mode is Modified IsDataModified() Should return TRUE
  CPPUNIT_ASSERT(dv->IsDataModified() == true);

  cppDEL(dv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestAppendItem()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();
  mafVMEItemVTK *item0,*item1;
  mafNEW(item0);
  mafNEW(item1);

  item0->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);

  dv->AppendItem(item0);
  dv->AppendItem(item1);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  mafDEL(item0);
  mafDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestPrependItem()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();
  mafVMEItemVTK *item0,*item1;
  mafNEW(item0);
  mafNEW(item1);

  item0->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);

  dv->PrependItem(item0);
  dv->PrependItem(item1);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  mafDEL(item0);
  mafDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestInsertItem()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();
  mafVMEItemVTK *item0a,*item1,*item0b;
  mafNEW(item0a);
  mafNEW(item1);
  mafNEW(item0b);

  item0a->SetTimeStamp(0.0);
  item1->SetTimeStamp(1.0);
  item0b->SetTimeStamp(0.0);

  dv->InsertItem(item0a);
  dv->InsertItem(item1);
  dv->InsertItem(item0b);

  CPPUNIT_ASSERT(dv->GetItem(0.0)==item0b);
  CPPUNIT_ASSERT(dv->GetItem(1.0)==item1);

  mafDEL(item0a);
  mafDEL(item0b);
  mafDEL(item1);
  cppDEL(dv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestModified()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();
  
  dv->Modified();

  CPPUNIT_ASSERT(dv->IsDataModified() == true);

  CPPUNIT_ASSERT(dv->GetMTime() != 0);

  cppDEL(dv);
}
//----------------------------------------------------------------------------
void mafDataVectorTest::TestSetCrypting()
//----------------------------------------------------------------------------
{
  mafDataVector *dv = new mafDataVector();

  // Default Crypting Mode is FALSE
  CPPUNIT_ASSERT(dv->GetCrypting() == false);

  dv->SetCrypting(true);

  CPPUNIT_ASSERT(dv->GetCrypting() == true);

  cppDEL(dv);
}
