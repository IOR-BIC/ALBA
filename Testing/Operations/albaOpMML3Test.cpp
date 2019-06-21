/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3Test
 Authors: Alberto Losi
 
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
#include "albaOpMML3Test.h"

#include "albaOpMML3.h"

#include "albaVMEGeneric.h"
#include "albaVMEVolumeGray.h"
#include "albaVMELandmark.h"
#include "albaVMESurface.h"

//----------------------------------------------------------------------------
void albaOpMML3Test::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpMML3 *op = new albaOpMML3("");
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestOpDo()
//----------------------------------------------------------------------------
{
  // Empty method
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestOpUndo()
//----------------------------------------------------------------------------
{
  // Empty method
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestAccept()
//----------------------------------------------------------------------------
{
  albaOpMML3 *op = new albaOpMML3("");
  albaVMEVolumeGray *volume;
  albaVMEGeneric *vme;

  albaNEW(volume);
  albaNEW(vme);

  CPPUNIT_ASSERT(op->Accept(volume) == true);
  CPPUNIT_ASSERT(op->Accept(vme) == false);
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  albaDEL(volume);
  albaDEL(vme);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestCopy()
//----------------------------------------------------------------------------
{
  albaOpMML3 *op = new albaOpMML3("");
  albaOp *opcopy = op->Copy();
  CPPUNIT_ASSERT(opcopy != NULL);
  cppDEL(op);
  cppDEL(opcopy);
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestAcceptVMELandmark()
//----------------------------------------------------------------------------
{
  albaOpMML3 *op = new albaOpMML3("");
  albaVMELandmark *landmark;
  albaVMEGeneric *vme;

  albaNEW(landmark);
  albaNEW(vme);

  CPPUNIT_ASSERT(op->AcceptVMELandmark(landmark) == true);
  CPPUNIT_ASSERT(op->AcceptVMELandmark(vme) == false);
  CPPUNIT_ASSERT(op->AcceptVMELandmark(NULL) == false);

  albaDEL(landmark);
  albaDEL(vme);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpMML3Test::TestAcceptVMESurface()
//----------------------------------------------------------------------------
{
  albaOpMML3 *op = new albaOpMML3("");
  albaVMESurface *surface;
  albaVMEGeneric *vme;

  albaNEW(surface);
  albaNEW(vme);

  CPPUNIT_ASSERT(op->AcceptVMESurface(surface) == true);
  CPPUNIT_ASSERT(op->AcceptVMESurface(vme) == false);
  CPPUNIT_ASSERT(op->AcceptVMESurface(NULL) == false);

  albaDEL(surface);
  albaDEL(vme);
  cppDEL(op);
}
