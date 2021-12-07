/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDummy
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//------------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//------------------------------------------------------------------------------

#include "albaOp.h"
#include "albaVME.h"

//-------------------------------------------------------------------------
/** class for testing */
class albaOpDummy: public albaOp
//-------------------------------------------------------------------------
{
public:
  albaOpDummy(wxString label = "DummyOp",  bool canundo = false, int opType = OPTYPE_OP, bool inputPreserving = false);

  albaTypeMacro(albaOpDummy,albaOp);

  /*virtual*/ albaOp* Copy();
  /** Builds operation's interface. */
  /*virtual*/ void OpRun();
  /** Execute the operation. */
  /*virtual*/ void OpDo();
  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  void End();
	
	static bool m_Executed;
  static bool m_Do;
  static bool m_Undo;
  static bool m_Stop;

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME* vme);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);
};
//----------------------------------------------------------------------------
albaOpDummy::albaOpDummy(wxString label,  bool canundo, int opType, bool inputPreserving)
//----------------------------------------------------------------------------
{

  m_Canundo = true;
  m_OpType = opType;
  m_InputPreserving = inputPreserving;
}
//----------------------------------------------------------------------------
bool albaOpDummy::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
  return vme->IsA("albaVMEDummy");
}
//----------------------------------------------------------------------------
albaOp* albaOpDummy::Copy()
//----------------------------------------------------------------------------
{
  return new albaOpDummy();
}
//----------------------------------------------------------------------------
void albaOpDummy::OpDo()
//----------------------------------------------------------------------------
{
  albaOpDummy::m_Do = true;
}
//----------------------------------------------------------------------------
void albaOpDummy::OpUndo()
//----------------------------------------------------------------------------
{
  m_Undo = true;
}
//----------------------------------------------------------------------------
void albaOpDummy::OpRun()
//----------------------------------------------------------------------------
{
  m_Executed = true; 
}
//----------------------------------------------------------------------------
void albaOpDummy::End()
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpDummy::OpStop(int result)
//----------------------------------------------------------------------------
{
  m_Stop = true;
  albaEventMacro(albaEvent(this,result));        
}

albaCxxTypeMacro(albaOpDummy);