/*=========================================================================

 Program: MAF2
 Module: mafOpComputeWrapping
 Authors: Gianluigi Crimi
 
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


#include "mafWizardWaitOp.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafEvent.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafWizardWaitOp);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafWizardWaitOp::mafWizardWaitOp(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafWizardWaitOp::~mafWizardWaitOp( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafWizardWaitOp::Copy()   
//----------------------------------------------------------------------------
{
	return new mafWizardWaitOp(m_Label);
}
//----------------------------------------------------------------------------
bool mafWizardWaitOp::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  //can accept everytime
  return true;
}
//----------------------------------------------------------------------------
void mafWizardWaitOp::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
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
void mafWizardWaitOp::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
      mafEventMacro(*e);
      break; 
    }
  }
}