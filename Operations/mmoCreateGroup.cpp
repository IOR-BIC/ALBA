/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGroup.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 09:47:06 $
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


#include "mmoCreateGroup.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERoot.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateGroup::mmoCreateGroup(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
mmoCreateGroup::~mmoCreateGroup( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateGroup::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateGroup(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateGroup::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mmoCreateGroup::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_Group);
  m_Group->SetName("group");
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateGroup::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsMAFType(mafVMERoot))
    m_Group->ReparentTo(mafVME::SafeDownCast(m_Input));
  else
    mafEventMacro(mafEvent(this, VME_ADD, m_Group));
}
//----------------------------------------------------------------------------
void mmoCreateGroup::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Group);
  mafEventMacro(mafEvent(this, VME_REMOVE, m_Group));
}
