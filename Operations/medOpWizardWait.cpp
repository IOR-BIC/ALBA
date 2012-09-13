/*=========================================================================

 Program: MAF2Medical
 Module: medOpComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
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


#include "medOpWizardWait.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafEvent.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpWizardWait);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpWizardWait::medOpWizardWait(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
medOpWizardWait::~medOpWizardWait( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpWizardWait::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpWizardWait(m_Label);
}
//----------------------------------------------------------------------------
bool medOpWizardWait::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
void medOpWizardWait::OpRun()   
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
  m_Gui->Label("Go to next Step:");

  m_Gui->Button(wxOK,"To next Step","");

  
  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpWizardWait::OpDo()
//----------------------------------------------------------------------------
{
 
}

//----------------------------------------------------------------------------
void medOpWizardWait::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        mafEventMacro(mafEvent(this,OP_RUN_OK));
      }
    default:
      mafEventMacro(*e);
      break; 
    }
  }
}