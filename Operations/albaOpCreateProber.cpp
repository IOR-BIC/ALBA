/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateProber
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


#include "albaOpCreateProber.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEVolume.h"
#include "albaVMEProber.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateProber);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateProber::albaOpCreateProber(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Prober  = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateProber::~albaOpCreateProber( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_Prober);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateProber::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateProber(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateProber::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
void albaOpCreateProber::OpRun()
//----------------------------------------------------------------------------
{
  albaNEW(m_Prober);
  m_Prober->SetName("prober");
  m_Output = m_Prober;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateProber::OpDo()
//----------------------------------------------------------------------------
{
  m_Prober->ReparentTo(m_Input);
}
