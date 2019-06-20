/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpInteractionDebuggerTest
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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "albaOpInteractionDebuggerTest.h"
#include "albaOpInteractionDebugger.h"
#include "albaVMEGeneric.h"
#include "albaVMEPolylineGraph.h"
#include "albaOp.h"

//-----------------------------------------------------------
void albaOpInteractionDebuggerTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpInteractionDebugger *debugger = new albaOpInteractionDebugger();
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void albaOpInteractionDebuggerTest::TestOnEvent()
// //-----------------------------------------------------------
// {
//   // Cannot test: All GUI Interaction
// }
//-----------------------------------------------------------
void albaOpInteractionDebuggerTest::TestCopy()
//-----------------------------------------------------------
{
  albaOpInteractionDebugger *debugger = new albaOpInteractionDebugger();
  
  albaOp *debugger_cpy = debugger->Copy();

  CPPUNIT_ASSERT(debugger_cpy->IsA("albaOpInteractionDebugger"));

  cppDEL(debugger);
  cppDEL(debugger_cpy);
}
//-----------------------------------------------------------
void albaOpInteractionDebuggerTest::TestAccept()
//-----------------------------------------------------------
{
  albaOpInteractionDebugger *debugger = new albaOpInteractionDebugger();

  albaVMEGeneric *vme;
  albaNEW(vme);

  CPPUNIT_ASSERT(debugger->Accept(NULL) == true); // Always returns true
  CPPUNIT_ASSERT(debugger->Accept(vme) == true);

  albaDEL(vme);
  cppDEL(debugger);
}
// //-----------------------------------------------------------
// void albaOpInteractionDebuggerTest::TestOpRun()
// //-----------------------------------------------------------
// {
//   Cannot test: All GUI creation
// }
//-----------------------------------------------------------
void albaOpInteractionDebuggerTest::TestConstrainAccept()
//-----------------------------------------------------------
{
  albaOpInteractionDebugger *debugger = new albaOpInteractionDebugger();
  albaVMEGeneric *vme;
  albaNEW(vme);
  albaVMEPolylineGraph *plgraph;
  albaNEW(plgraph);

  CPPUNIT_ASSERT(debugger->ConstrainAccept(NULL) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(vme) == false);
  CPPUNIT_ASSERT(debugger->ConstrainAccept(plgraph) == true);

  albaDEL(vme);
  albaDEL(plgraph);
  cppDEL(debugger);
}
//-----------------------------------------------------------
void albaOpInteractionDebuggerTest::TestOpDo()
//-----------------------------------------------------------
{
  // Nothing to test
}