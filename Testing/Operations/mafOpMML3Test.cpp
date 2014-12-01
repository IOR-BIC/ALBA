/*=========================================================================

 Program: MAF2Medical
 Module: mafOpMML3Test
 Authors: Alberto Losi
 
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
#include "mafOpMML3Test.h"

#include "mafOpMML3.h"

#include "mafVMEGeneric.h"
#include "mafVMEVolumeGray.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
void mafOpMML3Test::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpMML3 *op = new mafOpMML3("");
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestOpDo()
//----------------------------------------------------------------------------
{
  // Empty method
  CPPUNIT_ASSERT(true);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestOpUndo()
//----------------------------------------------------------------------------
{
  // Empty method
  CPPUNIT_ASSERT(true);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestAccept()
//----------------------------------------------------------------------------
{
  mafOpMML3 *op = new mafOpMML3("");
  mafVMEVolumeGray *volume;
  mafVMEGeneric *vme;

  mafNEW(volume);
  mafNEW(vme);

  CPPUNIT_ASSERT(op->Accept(volume) == true);
  CPPUNIT_ASSERT(op->Accept(vme) == false);
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  mafDEL(volume);
  mafDEL(vme);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestCopy()
//----------------------------------------------------------------------------
{
  mafOpMML3 *op = new mafOpMML3("");
  mafOp *opcopy = op->Copy();
  CPPUNIT_ASSERT(opcopy != NULL);
  cppDEL(op);
  cppDEL(opcopy);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestAcceptVMELandmark()
//----------------------------------------------------------------------------
{
  mafOpMML3 *op = new mafOpMML3("");
  mafVMELandmark *landmark;
  mafVMEGeneric *vme;

  mafNEW(landmark);
  mafNEW(vme);

  CPPUNIT_ASSERT(op->AcceptVMELandmark(landmark) == true);
  CPPUNIT_ASSERT(op->AcceptVMELandmark(vme) == false);
  CPPUNIT_ASSERT(op->AcceptVMELandmark(NULL) == false);

  mafDEL(landmark);
  mafDEL(vme);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMML3Test::TestAcceptVMESurface()
//----------------------------------------------------------------------------
{
  mafOpMML3 *op = new mafOpMML3("");
  mafVMESurface *surface;
  mafVMEGeneric *vme;

  mafNEW(surface);
  mafNEW(vme);

  CPPUNIT_ASSERT(op->AcceptVMESurface(surface) == true);
  CPPUNIT_ASSERT(op->AcceptVMESurface(vme) == false);
  CPPUNIT_ASSERT(op->AcceptVMESurface(NULL) == false);

  mafDEL(surface);
  mafDEL(vme);
  cppDEL(op);
}
