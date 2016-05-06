/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGroup
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


#include "mafOpCreateGroup.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERoot.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateGroup);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateGroup::mafOpCreateGroup(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateGroup::~mafOpCreateGroup()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGroup::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateGroup(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateGroup::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateGroup::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Group);
  m_Group->SetName("group");
  m_Output = m_Group;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateGroup::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsMAFType(mafVMERoot))
    m_Group->ReparentTo(m_Input);
  else
    mafEventMacro(mafEvent(this, VME_ADD, m_Group));
}
