/*=========================================================================

 Program: MAF2
 Module: mafOpInteractionDebuggerTest
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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafOpInteractionDebuggerTest.h"
#include "mafOpInteractionDebugger.h"
#include "mafVMEGeneric.h"
#include "mafVMEPolylineGraph.h"
#include "mafOp.h"

//-----------------------------------------------------------
void mafOpInteractionDebuggerTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpInteractionDebugger *debugger = new mafOpInteractionDebugger();
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void mafOpInteractionDebuggerTest::TestOnEvent()
// //-----------------------------------------------------------
// {
//   // Cannot test: All GUI Interaction
// }
//-----------------------------------------------------------
void mafOpInteractionDebuggerTest::TestCopy()
//-----------------------------------------------------------
{
  mafOpInteractionDebugger *debugger = new mafOpInteractionDebugger();
  
  mafOp *debugger_cpy = debugger->Copy();

  CPPUNIT_ASSERT(debugger_cpy->IsA("mafOpInteractionDebugger"));

  cppDEL(debugger);
  cppDEL(debugger_cpy);
}
//-----------------------------------------------------------
void mafOpInteractionDebuggerTest::TestAccept()
//-----------------------------------------------------------
{
  mafOpInteractionDebugger *debugger = new mafOpInteractionDebugger();

  mafVMEGeneric *vme;
  mafNEW(vme);

  CPPUNIT_ASSERT(debugger->Accept(NULL) == true); // Always returns true
  CPPUNIT_ASSERT(debugger->Accept(vme) == true);

  mafDEL(vme);
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void mafOpInteractionDebuggerTest::TestOpRun()
// //-----------------------------------------------------------
// {
//   Cannot test: All GUI creation
// }
//-----------------------------------------------------------
void mafOpInteractionDebuggerTest::TestConstrainAccept()
//-----------------------------------------------------------
{
  mafOpInteractionDebugger *debugger = new mafOpInteractionDebugger();
  mafVMEGeneric *vme;
  mafNEW(vme);
  mafVMEPolylineGraph *plgraph;
  mafNEW(plgraph);

  CPPUNIT_ASSERT(debugger->ConstrainAccept(NULL) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(vme) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(plgraph) == true);

  mafDEL(vme);
  mafDEL(plgraph);
  cppDEL(debugger);
}
//-----------------------------------------------------------
void mafOpInteractionDebuggerTest::TestOpDo()
//-----------------------------------------------------------
{
  // Nothing to test
}