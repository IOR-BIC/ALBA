/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGenericVme.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:46:29 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
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


#include "mmoCreateGenericVme.h"
#include "mafNodeGeneric.h"

//----------------------------------------------------------------------------
mmoCreateGenericVme::mmoCreateGenericVme(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mmoCreateGenericVme::~mmoCreateGenericVme()
//----------------------------------------------------------------------------
{
  mafDEL(m_vme);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateGenericVme::Copy()
//----------------------------------------------------------------------------
{
  return new mmoCreateGenericVme(m_Label);
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*event);
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpRun()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  mafNEW(m_vme);
  static int counter = 0;
  wxString name = wxString::Format("vme generic %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo(m_Input);
  mafEventMacro(mafEvent(this,VME_ADD,m_vme));
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_vme));
  mafDEL(m_vme);
}









