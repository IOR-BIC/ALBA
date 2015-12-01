/*=========================================================================

 Program: MAF2
 Module: mafGUIMaterialButton
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

#include "mafGUIMaterialButton.h"
#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mmaMaterial.h"
#include "mafGUIPicButton.h"

#include "mafVME.h"

//#include "mafMemDbg.h"

//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum MATERIAL_BUTTON_WIDGET_ID
{
  ID_MATERIAL = MINID
};

//----------------------------------------------------------------------------
mafGUIMaterialButton::mafGUIMaterialButton(mafVME *vme, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;

	SetVME(vme);

	CreateGui();
}


void mafGUIMaterialButton::SetVME(mafVME *vme)
{
	m_Vme = vme;

	if(m_Vme)
	{
		m_Material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
		if (m_Material == NULL)
		{
			m_Material = mmaMaterial::New();
			m_Vme->SetAttribute("MaterialAttributes", m_Material);
		}
	}
}


//----------------------------------------------------------------------------
mafGUIMaterialButton::~mafGUIMaterialButton()
//----------------------------------------------------------------------------
{
	if(m_Gui)
    m_Gui->SetListener(NULL);
	cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
void mafGUIMaterialButton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	m_Gui->Show(true);

  wxStaticText *lab = new wxStaticText(m_Gui, -1, "Material", wxDefaultPosition, wxSize(60,16), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  
  m_Material->MakeIcon();
  m_MaterialButton = new mafGUIPicButton(m_Gui, m_Material->m_Icon, ID_MATERIAL, this);

  //m_MaterialLabel = new wxStaticText(m_Gui,ID_MATERIAL,"",wxDefaultPosition, wxSize(100,16), wxST_NO_AUTORESIZE );
  m_MaterialLabel = new wxStaticText(m_Gui,-1,"",wxDefaultPosition, wxSize(100,16), wxST_NO_AUTORESIZE );
  m_MaterialLabel->SetLabel(m_Material->m_MaterialName.GetCStr());

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( lab,          0, wxALIGN_CENTRE|wxRIGHT, 5 );
  sizer->Add( m_MaterialButton, 0, wxLEFT|wxALL,1 );
  sizer->Add( m_MaterialLabel,  0, wxALIGN_CENTRE|wxLEFT, 5  );

  m_Gui->Add(sizer,0,wxLEFT, 5);
	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafGUIMaterialButton::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_MATERIAL:
        mafEventMacro(mafEvent(this,VME_CHOOSE_MATERIAL,m_Vme));
        m_MaterialLabel->SetLabel(m_Material->m_MaterialName.GetCStr());
        UpdateMaterialIcon();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
        e->Log();
    }
	}
}
//----------------------------------------------------------------------------
void mafGUIMaterialButton::UpdateMaterialIcon() 
//----------------------------------------------------------------------------
{
	cppDEL(m_Material->m_Icon);
	
	m_Material->MakeIcon();
	m_MaterialButton->SetBitmapLabel(*m_Material->m_Icon);
  m_MaterialButton->Refresh();

  m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafGUIMaterialButton::Enable(bool enable)
//----------------------------------------------------------------------------
{
  m_MaterialButton->Enable(enable);
  m_Gui->Update();
}
