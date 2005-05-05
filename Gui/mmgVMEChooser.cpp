/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooser.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-05 15:24:13 $
  Version:   $Revision: 1.1 $
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

#include "mmgVMEChooser.h"

#include <wx/laywin.h>
#include <wx/treectrl.h>

#include "mafDecl.h"
#include "mmgVMEChooserTree.h"
#include "mmgDialog.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
mmgVMEChooser::mmgVMEChooser(mmgCheckTree *tree, wxString dialog_title, long vme_accept_function)
//----------------------------------------------------------------------------
{
  m_ChoosedVME = NULL;

  int x_pos,y_pos,w,h,w1,h1,d;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  mafGetFrame()->GetSize(&w1,&h1);
  m_Dialog = new mmgDialog(dialog_title, mafCLOSEWINDOW | mafRESIZABLE );
  m_Dialog->GetSize(&w,&h);
  d = w1/2 - w/2;
  m_Dialog->SetSize(x_pos+d,y_pos+10,w,h);
  
	mmgVMEChooserTree *m_Tree = new mmgVMEChooserTree(m_Dialog, -1, 0, 1, vme_accept_function);
  m_Tree->SetListener(this);
  m_Tree->SetSize(wxSize(100,400));
  m_Tree->SetTitle("");
  m_Tree->FillTree(tree);
  
	m_OkButton = new wxButton(m_Dialog,wxID_OK,"ok",wxDefaultPosition,wxSize(100,25));
  m_OkButton->Enable(false);

  wxButton *cancel = new wxButton(m_Dialog,wxID_CANCEL,"cancel",wxDefaultPosition,wxSize(100,25));

	wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
	h_sizer->Add(m_OkButton,0,wxLEFT);
	h_sizer->Add(cancel,0,wxLEFT);	

  wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
  v_sizer->Add(m_Tree,1,wxEXPAND);
  v_sizer->Add(h_sizer,0,wxCENTRE);

  m_Dialog->SetAutoLayout( TRUE );
  m_Dialog->SetSizer( v_sizer );
  v_sizer->Fit(m_Dialog);
}
//----------------------------------------------------------------------------
mmgVMEChooser::~mmgVMEChooser()
//----------------------------------------------------------------------------
{
	//destroy gui
  delete m_Dialog;
}
//----------------------------------------------------------------------------
mafVME *mmgVMEChooser::ShowModal()
//----------------------------------------------------------------------------
{
	if(m_Dialog->ShowModal() == wxID_OK)
		return m_ChoosedVME;
	else
		return NULL;
}
//----------------------------------------------------------------------------
void mmgVMEChooser::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
      case VME_SELECT:
      {
        /*mafVME *vme = (mafVME*)e->GetArg();
        if(m_AcceptFunction == NULL)
          m_ChoosedVME = vme;
        else
          if(m_AcceptFunction->Validate(vme))
            m_ChoosedVME = vme;
          else
          {
            m_OkButton->Enable(false);
            return;
          }*/
          m_OkButton->Enable(true);
      }
      break;
      default:
        e->Log();
      break;
    }
  }
}