/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIVMEChooser
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



//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIVMEChooser.h"
#include "albaDecl.h"

#include "albaVME.h"
#include "albaServiceClient.h"
#include "albaLogicWithManagers.h"
#include "mmaApplicationLayout.h"

//----------------------------------------------------------------------------
albaGUIVMEChooser::albaGUIVMEChooser(albaGUICheckTree *tree, wxString dialog_title, void *vme_accept_function, long style, bool multiSelect, albaVME *subTree)
: albaGUIDialog(dialog_title,albaCLOSEWINDOW | albaRESIZABLE | albaCLOSE | albaOK )
//----------------------------------------------------------------------------
{
  if(vme_accept_function == 0)
    m_ValidateCallback = NULL;
  else
    m_ValidateCallback = (ValidateCallBackType)vme_accept_function;

  m_ChooserTree = new albaGUIVMEChooserTree(this,tree,m_ValidateCallback,-1,false,true,style, multiSelect,subTree);
  m_ChooserTree->SetListener(this);
  m_ChooserTree->SetTitle("");
	m_ChooserTree->SetMinSize(wxSize(100, 200));

	LoadLayout();

  Add(m_ChooserTree,1,wxEXPAND);
  m_OkButton->Enable(false);
}

//----------------------------------------------------------------------------
albaGUIVMEChooser::~albaGUIVMEChooser()
//----------------------------------------------------------------------------
{
	SaveLayout();

  delete m_ChooserTree;
}
//----------------------------------------------------------------------------
std::vector<albaVME*> albaGUIVMEChooser::ShowChooserDialog()
//----------------------------------------------------------------------------
{
  if(ShowModal() == wxID_OK)
  {
		return m_ChooserTree->GetChoosedNode();
  }
	else
  {
     std::vector<albaVME*> nodeVector;
     nodeVector.clear();
     return nodeVector;
  }
}
//----------------------------------------------------------------------------
void albaGUIVMEChooser::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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

//----------------------------------------------------------------------------
void albaGUIVMEChooser::LoadLayout()
{
	mmaApplicationLayout* layout = ((albaLogicWithManagers*)GetLogicManager())->GetLayout();

	if (layout)
	{
		int pos[2], size[2];
		layout->GetVMEChooserInfo(pos, size);

		m_ChooserTree->SetSize(wxSize(size[0], size[1]));
		SetSize(wxSize(size[0], size[1]));
		SetMinSize(wxSize(MAX(100, size[0]), MAX(200, size[1])));

		if (pos[0] > 0 && pos[1] > 0) // else centered
			SetPosition(wxPoint(MAX(0, pos[0]), MAX(0, pos[1])));
	}
}
//----------------------------------------------------------------------------
void albaGUIVMEChooser::SaveLayout()
{
	mmaApplicationLayout* layout = ((albaLogicWithManagers*)GetLogicManager())->GetLayout();

	if (layout)
	{
		int pos[2], size[2];

		m_ChooserTree->GetSize(&size[0], &size[1]);
		GetPosition(&pos[0], &pos[1]);
		layout->SetVMEChooserInfo(pos, size);
	}
}