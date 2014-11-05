/*=========================================================================

 Program: MAF2Medical
 Module: medOpInteractionDebuggerTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
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

#include "medOpInteractionDebuggerTest.h"
#include "medOpInteractionDebugger.h"
#include "mafVMEGeneric.h"
#include "medVMEPolylineGraph.h"
#include "mafOp.h"

//-----------------------------------------------------------
void medOpInteractionDebuggerTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpInteractionDebugger *debugger = new medOpInteractionDebugger();
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void medOpInteractionDebuggerTest::TestOnEvent()
// //-----------------------------------------------------------
// {
//   // Cannot test: All GUI Interaction
// }
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::TestCopy()
//-----------------------------------------------------------
{
  medOpInteractionDebugger *debugger = new medOpInteractionDebugger();
  
  mafOp *debugger_cpy = debugger->Copy();

  CPPUNIT_ASSERT(debugger_cpy->IsA("medOpInteractionDebugger"));

  cppDEL(debugger);
  cppDEL(debugger_cpy);
}
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::TestAccept()
//-----------------------------------------------------------
{
  medOpInteractionDebugger *debugger = new medOpInteractionDebugger();

  mafVMEGeneric *vme;
  mafNEW(vme);

  CPPUNIT_ASSERT(debugger->Accept(NULL) == true); // Always returns true
  CPPUNIT_ASSERT(debugger->Accept(vme) == true);

  mafDEL(vme);
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void medOpInteractionDebuggerTest::TestOpRun()
// //-----------------------------------------------------------
// {
//   Cannot test: All GUI creation
// }
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::TestConstrainAccept()
//-----------------------------------------------------------
{
  medOpInteractionDebugger *debugger = new medOpInteractionDebugger();
  mafVMEGeneric *vme;
  mafNEW(vme);
  medVMEPolylineGraph *plgraph;
  mafNEW(plgraph);

  CPPUNIT_ASSERT(debugger->ConstrainAccept(NULL) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(vme) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(plgraph) == true);

  mafDEL(vme);
  mafDEL(plgraph);
  cppDEL(debugger);
}
//-----------------------------------------------------------
void medOpInteractionDebuggerTest::TestOpDo()
//-----------------------------------------------------------
{
  // Nothing to test
}