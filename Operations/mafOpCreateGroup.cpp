/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateGroup.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
bool mafOpCreateGroup::Accept(mafNode *node)
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
    m_Group->ReparentTo(mafVME::SafeDownCast(m_Input));
  else
    mafEventMacro(mafEvent(this, VME_ADD, m_Group));
}
