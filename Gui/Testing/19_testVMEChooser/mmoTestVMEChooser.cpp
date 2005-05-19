/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTestVMEChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-19 11:34:48 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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


#include "mmoTestVMEChooser.h"
#include "mmgGui.h"
#include "mmgVMEChooserTree.h"

//----------------------------------------------------------------------------
mmoTestVMEChooser::mmoTestVMEChooser(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmoTestVMEChooser::~mmoTestVMEChooser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoTestVMEChooser::Copy()
//----------------------------------------------------------------------------
{
  return new mmoTestVMEChooser(m_Label);
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpStop(int result)
//----------------------------------------------------------------------------
 {
   delete m_NodeAccept;

   HideGui();
   mafEventMacro(mafEvent(this,result));
 }
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum VME_CHOOSER_WIDGET_ID
{
  ID_VME_CHOOSER = MINID,
  CHOSE_WITH_ACCEPT,
};
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpRun()
//----------------------------------------------------------------------------
{
  m_NodeAccept = new mafNodeAccept();
  
  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);

  m_Gui->Button(ID_VME_CHOOSER, "Choose VME");
  m_Gui->Button(CHOSE_WITH_ACCEPT, "Choose 'vme generic 1'");
  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OnEvent(mafEventBase *event)
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
      case CHOSE_WITH_ACCEPT:
      {
        mafString title = "Choose 'vme generic 1'";
        e->SetId(VME_CHOOSE);
        e->SetString(&title);
        e->SetArg((long)m_NodeAccept);
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
void mmoTestVMEChooser::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoTestVMEChooser::OpUndo()
//----------------------------------------------------------------------------
{
}