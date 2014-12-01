/*=========================================================================

 Program: MAF2
 Module: mafOpComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
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


#include "mafOpComputeWrapping.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "medVMEComputeWrapping.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpComputeWrapping);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpComputeWrapping::mafOpComputeWrapping(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
mafOpComputeWrapping::~mafOpComputeWrapping( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* mafOpComputeWrapping::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpComputeWrapping(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpComputeWrapping::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpComputeWrapping::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName("Wrapped Action Line");
  m_Output = m_Meter;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpComputeWrapping::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
