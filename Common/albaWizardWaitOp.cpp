/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrapping
 Authors: Gianluigi Crimi
 
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


#include "albaWizardWaitOp.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaEvent.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaWizardWaitOp);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaWizardWaitOp::albaWizardWaitOp(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
albaWizardWaitOp::~albaWizardWaitOp( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaWizardWaitOp::Copy()   
//----------------------------------------------------------------------------
{
	return new albaWizardWaitOp(m_Label);
}
//----------------------------------------------------------------------------
bool albaWizardWaitOp::Accept(albaVME*node)
//----------------------------------------------------------------------------
{
  //can accept everytime
  return true;
}
//----------------------------------------------------------------------------
void albaWizardWaitOp::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->Divider();
  
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");

  //exit op button
  m_Gui->TwoButtons(wxCANCEL,wxOK,"Cancel","To next step");
    
  m_Gui->Divider();

  ShowGui();
}


//----------------------------------------------------------------------------
void albaWizardWaitOp::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case wxOK:
		{
			//exit operation
			OpStop(OP_RUN_OK);
		}
    break;
    case wxCANCEL:
		{
			//exit operation
			OpStop(OP_RUN_CANCEL);
		}
    break;
    default:
      albaEventMacro(*e);
      break; 
    }
  }
}