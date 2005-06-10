/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:44:10 $
  Version:   $Revision: 1.2 $
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
#include "mmgVMEChooserAccept.h"

#include "mafNode.h"

//----------------------------------------------------------------------------
mmgVMEChooser::mmgVMEChooser(mmgCheckTree *tree, wxString dialog_title, long vme_accept_function, long style)
: mmgDialog(dialog_title,mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafOK )
//----------------------------------------------------------------------------
{
  m_ChooserTree = new mmgVMEChooserTree(this,tree,vme_accept_function,-1,false,true,style);
  m_ChooserTree->SetListener(this);
  m_ChooserTree->SetTitle("");
  m_ChooserTree->SetSize(250,350);
  
  Add(m_ChooserTree,1,wxEXPAND);
  m_ok_button->Enable(false);
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
        m_ok_button->Enable(e->GetBool());
      break;
      default:
        e->Log();
      break;
    }
  }
}
