/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-07 08:35:58 $
  Version:   $Revision: 1.7 $
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
#include "mmgVMEChooser.h"
#include "mafDecl.h"

#include "mafNode.h"

//----------------------------------------------------------------------------
mmgVMEChooser::mmgVMEChooser(mmgCheckTree *tree, wxString dialog_title, long vme_accept_function, long style)
: mmgDialog(dialog_title,mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafOK )
//----------------------------------------------------------------------------
{
  if(vme_accept_function == 0)
    m_ValidateCallback = NULL;
  else
    m_ValidateCallback = (ValidateCallBackType)vme_accept_function;

  m_ChooserTree = new mmgVMEChooserTree(this,tree,m_ValidateCallback,-1,false,true,style);
  m_ChooserTree->SetListener(this);
  m_ChooserTree->SetTitle("");
  m_ChooserTree->SetSize(wxSize(200,400));
  m_ChooserTree->SetMinSize(wxSize(200,400));
  
  Add(m_ChooserTree,1,wxEXPAND);
  m_OkButton->Enable(false);
}
//----------------------------------------------------------------------------
mmgVMEChooser::~mmgVMEChooser()
//----------------------------------------------------------------------------
{
  delete m_ChooserTree;
}
//----------------------------------------------------------------------------
mafNode *mmgVMEChooser::ShowChooserDialog()
//----------------------------------------------------------------------------
{
  if(ShowModal() == wxID_OK)
		return m_ChooserTree->GetChoosedNode();
	else
		return NULL;
}
//----------------------------------------------------------------------------
void mmgVMEChooser::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case VME_SELECTED:
				if(m_ValidateCallback)
				{
					if(m_ValidateCallback(m_ChooserTree->GetChoosedNode()))
						m_OkButton->Enable(true);
					else
						m_OkButton->Enable(false);
				}
				else
				{
					m_OkButton->Enable(true);
				}
      break;
      default:
        e->Log();
      break;
    }
  }
}
