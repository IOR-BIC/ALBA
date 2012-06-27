/*=========================================================================

 Program: MAF2
 Module: mafOpTestVMEChooser
 Authors: Paolo Quadrani
 
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


#include "mafOpTestVMEChooser.h"
#include "mafGUI.h"
#include "mafGUIVMEChooserTree.h"

//----------------------------------------------------------------------------
mafOpTestVMEChooser::mafOpTestVMEChooser(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOpTestVMEChooser::~mafOpTestVMEChooser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpTestVMEChooser::Copy()
//----------------------------------------------------------------------------
{
  return new mafOpTestVMEChooser(m_Label);
}
//----------------------------------------------------------------------------
void mafOpTestVMEChooser::OpStop(int result)
//----------------------------------------------------------------------------
 {
   HideGui();
   mafEventMacro(mafEvent(this,result));
 }
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum VME_CHOOSER_WIDGET_ID
{
  ID_VME_CHOOSER = MINID,
  ID_VME_CHOOSER_FLAT,
  CHOSE_WITH_ACCEPT,
  CHOSE_WITH_ACCEPT_AND_FLAT
};
//----------------------------------------------------------------------------
void mafOpTestVMEChooser::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Button(ID_VME_CHOOSER, "Choose VME");
  m_Gui->Button(ID_VME_CHOOSER_FLAT, "Choose Flat");
  m_Gui->Button(CHOSE_WITH_ACCEPT, "Accept 'vme generic 1'");
  m_Gui->Button(CHOSE_WITH_ACCEPT_AND_FLAT, "Accept Flat");
  m_Gui->Divider();
  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpTestVMEChooser::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      case ID_VME_CHOOSER:
      {
        e->SetId(VME_CHOOSE);
        mafEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      }
      break;
      case ID_VME_CHOOSER_FLAT:
        e->SetId(VME_CHOOSE);
        e->SetBool(true);
        mafEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      break;
      case CHOSE_WITH_ACCEPT:
      {
        mafString title = "Choose 'vme generic 1'";
        e->SetId(VME_CHOOSE);
        e->SetString(&title);
        e->SetArg((long)&mafOpTestVMEChooser::Validate);
        mafEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      }
      break;
      case CHOSE_WITH_ACCEPT_AND_FLAT:
        {
          mafString title = "Choose 'vme generic 1'";
          e->SetId(VME_CHOOSE);
          e->SetString(&title);
          e->SetArg((long)&mafOpTestVMEChooser::Validate);
          e->SetBool(true);
          mafEventMacro(*e);
          m_ChoosedNode = e->GetVme();
        }
        break;
      default:
        mafEventMacro(*e);
      break;
    }  
  }
}
//----------------------------------------------------------------------------
void mafOpTestVMEChooser::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpTestVMEChooser::OpUndo()
//----------------------------------------------------------------------------
{
}
