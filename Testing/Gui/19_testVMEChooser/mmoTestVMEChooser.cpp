/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmoTestVMEChooser
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


#include "mmoTestVMEChooser.h"
#include "albaGUIGui.h"
#include "albaGUIVMEChooserTree.h"

//----------------------------------------------------------------------------
mmoTestVMEChooser::mmoTestVMEChooser(wxString label)
: albaOp(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmoTestVMEChooser::~mmoTestVMEChooser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* mmoTestVMEChooser::Copy()
//----------------------------------------------------------------------------
{
  return new mmoTestVMEChooser(m_Label);
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpStop(int result)
//----------------------------------------------------------------------------
 {
   HideGui();
   albaEventMacro(albaEvent(this,result));
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
void mmoTestVMEChooser::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUIGui(this);
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
void mmoTestVMEChooser::OnEvent(albaEventBase *event)
//----------------------------------------------------------------------------
{
  if(albaEvent *e = albaEvent::SafeDownCast(event))
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
        albaEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      }
      break;
      case ID_VME_CHOOSER_FLAT:
        e->SetId(VME_CHOOSE);
        e->SetBool(true);
        albaEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      break;
      case CHOSE_WITH_ACCEPT:
      {
        albaString title = "Choose 'vme generic 1'";
        e->SetId(VME_CHOOSE);
        e->SetString(&title);
        e->SetArg((long)&mmoTestVMEChooser::Validate);
        albaEventMacro(*e);
        m_ChoosedNode = e->GetVme();
      }
      break;
      case CHOSE_WITH_ACCEPT_AND_FLAT:
        {
          albaString title = "Choose 'vme generic 1'";
          e->SetId(VME_CHOOSE);
          e->SetString(&title);
          e->SetArg((long)&mmoTestVMEChooser::Validate);
          e->SetBool(true);
          albaEventMacro(*e);
          m_ChoosedNode = e->GetVme();
        }
        break;
      default:
        albaEventMacro(*e);
      break;
    }  
  }
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpUndo()
//----------------------------------------------------------------------------
{
}
