/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateGenericVme.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:03:10 $
  Version:   $Revision: 1.1 $
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


#include "mafOpCreateGenericVme.h"
#include "mafNodeGeneric.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateGenericVme);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateGenericVme::mafOpCreateGenericVme(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateGenericVme::~mafOpCreateGenericVme()
//----------------------------------------------------------------------------
{
  mafDEL(m_vme);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGenericVme::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpCreateGenericVme(m_Label);
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*event);
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OpRun()
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateGenericVme::OpDo()
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
void mafOpCreateGenericVme::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_vme));
  mafDEL(m_vme);
}









