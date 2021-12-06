/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateMeter
 Authors: Paolo Quadrani
 
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


#include "albaOpCreateMeter.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMEMeter.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateMeter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateMeter::albaOpCreateMeter(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateMeter::~albaOpCreateMeter( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_Meter);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateMeter::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateMeter(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateMeter::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreateMeter::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_Meter);
  m_Meter->SetName("meter");
  m_Output = m_Meter;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateMeter::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
