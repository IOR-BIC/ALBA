/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
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


#include "albaOpComputeWrapping.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMEComputeWrapping.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpComputeWrapping);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpComputeWrapping::albaOpComputeWrapping(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
albaOpComputeWrapping::~albaOpComputeWrapping( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_Meter);
}
//----------------------------------------------------------------------------
albaOp* albaOpComputeWrapping::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpComputeWrapping(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpComputeWrapping::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpComputeWrapping::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_Meter);
  m_Meter->SetName("Wrapped Action Line");
  m_Output = m_Meter;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpComputeWrapping::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
