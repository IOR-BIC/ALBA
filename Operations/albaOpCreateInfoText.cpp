/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateInfoText
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


#include "albaOpCreateInfoText.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMERoot.h"
#include "albaVMEInfoText.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateInfoText);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateInfoText::albaOpCreateInfoText(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_InfoText   = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateInfoText::~albaOpCreateInfoText()
//----------------------------------------------------------------------------
{
  albaDEL(m_InfoText);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateInfoText::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateInfoText(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateInfoText::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreateInfoText::OpRun()
//----------------------------------------------------------------------------
{
  albaNEW(m_InfoText);
  m_InfoText->SetName("InfoText");
  m_Output = m_InfoText;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateInfoText::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->IsALBAType(albaVMERoot))
    m_InfoText->ReparentTo(m_Input);
  else
    GetLogicManager()->VmeAdd(m_InfoText);
}
