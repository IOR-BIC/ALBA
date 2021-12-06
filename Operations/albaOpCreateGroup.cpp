/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateGroup
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


#include "albaOpCreateGroup.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMERoot.h"
#include "albaVMEGroup.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateGroup);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateGroup::albaOpCreateGroup(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Group   = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateGroup::~albaOpCreateGroup()
//----------------------------------------------------------------------------
{
  albaDEL(m_Group);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateGroup::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateGroup(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateGroup::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreateGroup::OpRun()
//----------------------------------------------------------------------------
{
  albaNEW(m_Group);
  m_Group->SetName("Group");
  m_Output = m_Group;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateGroup::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsALBAType(albaVMERoot))
    m_Group->ReparentTo(m_Input);
  else
    GetLogicManager()->VmeAdd(m_Group);
}
