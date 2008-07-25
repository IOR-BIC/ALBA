/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIVMEChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIVMEChooser.h"
#include "mafDecl.h"

#include "mafNode.h"

//----------------------------------------------------------------------------
mafGUIVMEChooser::mafGUIVMEChooser(mafGUICheckTree *tree, wxString dialog_title, long vme_accept_function, long style, bool multiSelect)
: mafGUIDialog(dialog_title,mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafOK )
//----------------------------------------------------------------------------
{
  if(vme_accept_function == 0)
    m_ValidateCallback = NULL;
  else
    m_ValidateCallback = (ValidateCallBackType)vme_accept_function;

  m_ChooserTree = new mafGUIVMEChooserTree(this,tree,m_ValidateCallback,-1,false,true,style, multiSelect);
  m_ChooserTree->SetListener(this);
  m_ChooserTree->SetTitle("");
  m_ChooserTree->SetSize(wxSize(200,400));
  m_ChooserTree->SetMinSize(wxSize(200,400));
  
  Add(m_ChooserTree,1,wxEXPAND);
  m_OkButton->Enable(false);
}
//----------------------------------------------------------------------------
mafGUIVMEChooser::~mafGUIVMEChooser()
//----------------------------------------------------------------------------
{
  delete m_ChooserTree;
}
//----------------------------------------------------------------------------
std::vector<mafNode*> mafGUIVMEChooser::ShowChooserDialog()
//----------------------------------------------------------------------------
{
  if(ShowModal() == wxID_OK)
  {
		return m_ChooserTree->GetChoosedNode();
  }
	else
  {
     std::vector<mafNode*> nodeVector;
     nodeVector.clear();
     return nodeVector;
  }
}
//----------------------------------------------------------------------------
void mafGUIVMEChooser::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case VME_SELECTED:
				m_OkButton->Enable(e->GetBool());
      break;
      default:
        e->Log();
      break;
    }
  }
}
