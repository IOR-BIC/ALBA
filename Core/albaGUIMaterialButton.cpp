/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMaterialButton
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

#include "albaGUIMaterialButton.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "mmaMaterial.h"
#include "albaGUIPicButton.h"
#include "albaViewManager.h"


#include "albaVME.h"
#include "vtkProperty.h"

//#include "albaMemDbg.h"

//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum MATERIAL_BUTTON_WIDGET_ID
{
	ID_MATERIAL = MINID,
	ID_OPACITY
};

//----------------------------------------------------------------------------
albaGUIMaterialButton::albaGUIMaterialButton(albaVME *vme, albaObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;

	SetVME(vme);

	CreateGui();
}


void albaGUIMaterialButton::SetVME(albaVME *vme)
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
albaGUIMaterialButton::~albaGUIMaterialButton()
//----------------------------------------------------------------------------
{
	if(m_Gui)
    m_Gui->SetListener(NULL);
	cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
void albaGUIMaterialButton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Show(true);

	wxStaticText *lab = new wxStaticText(m_Gui, -1, "Material", wxDefaultPosition, wxSize(60, 16), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  
  m_Material->MakeIcon();
  m_MaterialButton = new albaGUIPicButton(m_Gui, m_Material->m_Icon, ID_MATERIAL, this);

  //m_MaterialLabel = new wxStaticText(m_Gui,ID_MATERIAL,"",wxDefaultPosition, wxSize(100,16), wxST_NO_AUTORESIZE );
  m_MaterialLabel = new wxStaticText(m_Gui,-1,"",wxDefaultPosition, wxSize(100,16), wxST_NO_AUTORESIZE );
  m_MaterialLabel->SetLabel(m_Material->m_MaterialName.GetCStr());

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( lab,          0, wxALIGN_CENTRE|wxRIGHT, 5 );
  sizer->Add( m_MaterialButton, 0, wxLEFT|wxALL,1 );
  sizer->Add( m_MaterialLabel,  0, wxALIGN_CENTRE|wxLEFT, 5  );

  m_Gui->Add(sizer,0,wxLEFT, 5);
	m_Gui->FloatSlider(ID_OPACITY, "Opacity:", &m_Material->m_Opacity, 0, 1);
	m_Gui->Divider();

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaGUIMaterialButton::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_MATERIAL:
        albaEventMacro(albaEvent(this,VME_CHOOSE_MATERIAL,m_Vme));
        m_MaterialLabel->SetLabel(m_Material->m_MaterialName.GetCStr());
        UpdateMaterialIcon();
				GetLogicManager()->CameraUpdate();
      break;
			case ID_OPACITY:
				m_Material->m_Prop->SetOpacity(m_Material->m_Opacity);
				UpdateMaterialIcon();
				GetLogicManager()->GetViewManager()->PropertyUpdate();

				GetLogicManager()->CameraUpdate();
      default:
        e->Log();
    }
	}
}
//----------------------------------------------------------------------------
void albaGUIMaterialButton::UpdateMaterialIcon() 
//----------------------------------------------------------------------------
{
	cppDEL(m_Material->m_Icon);
	
	m_Material->MakeIcon();
	m_MaterialButton->SetBitmapLabel(*m_Material->m_Icon);
  m_MaterialButton->Refresh();

  m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaGUIMaterialButton::Enable(bool enable)
//----------------------------------------------------------------------------
{
  m_MaterialButton->Enable(enable);
	m_Gui->Enable(ID_OPACITY, enable);
  m_Gui->Update();
}
