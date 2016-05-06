/*=========================================================================

 Program: MAF2
 Module: mafOpCreateProber
 Authors: Paolo Quadrani
 
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


#include "mafOpCreateProber.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEProber.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateProber);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateProber::mafOpCreateProber(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Prober  = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateProber::~mafOpCreateProber( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Prober);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateProber::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateProber(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateProber::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL);
}
//----------------------------------------------------------------------------
void mafOpCreateProber::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Prober);
  m_Prober->SetName("prober");
  m_Output = m_Prober;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateProber::OpDo()
//----------------------------------------------------------------------------
{
  m_Prober->ReparentTo(m_Input);
}
