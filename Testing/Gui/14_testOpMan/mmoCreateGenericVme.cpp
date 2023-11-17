/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmoCreateGenericVme
 Authors: Silvano Imboden
 
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


#include "mmoCreateGenericVme.h"
#include "albaNodeGeneric.h"

//----------------------------------------------------------------------------
mmoCreateGenericVme::mmoCreateGenericVme(wxString label)
: albaOp(label)
//----------------------------------------------------------------------------
{
  m_Canundo = true;
  m_vme = NULL;
}
//----------------------------------------------------------------------------
mmoCreateGenericVme::~mmoCreateGenericVme()
//----------------------------------------------------------------------------
{
  albaDEL(m_vme);
}
//----------------------------------------------------------------------------
albaOp* mmoCreateGenericVme::Copy()
//----------------------------------------------------------------------------
{
  return new mmoCreateGenericVme(m_Label);
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OnEvent(albaEvent& e)
//----------------------------------------------------------------------------
{
  albaEventMacro(e);
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpRun()
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpDo()
//----------------------------------------------------------------------------
{
  assert(!m_vme);

  albaNEW(m_vme);
  static int counter = 0;
  wxString name = albaString::Format("vme generic %d",counter++);
  m_vme->SetName(name);
  m_vme->ReparentTo(m_Input);
  albaEventMacro(albaEvent(this,VME_ADD,m_vme));
}
//----------------------------------------------------------------------------
void mmoCreateGenericVme::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_vme);
  albaEventMacro(albaEvent(this,VME_REMOVE,m_vme));
  albaDEL(m_vme);
}
