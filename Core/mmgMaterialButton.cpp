/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMaterialButton.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 10:00:50 $
  Version:   $Revision: 1.6 $
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

#include "mmgMaterialButton.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgValidator.h"
#include "mmaMaterial.h"
#include "mmgPicButton.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum MATERIAL_BUTTON_WIDGET_ID
{
  ID_MATERIAL = MINID
};

//----------------------------------------------------------------------------
mmgMaterialButton::mmgMaterialButton(mafVME *vme, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;
  m_Vme      = vme;

  m_Material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
  if (m_Material == NULL)
  {
    m_Material = mmaMaterial::New();
    m_Vme->SetAttribute("MaterialAttributes", m_Material);
  }

	CreateGui();
}
//----------------------------------------------------------------------------
mmgMaterialButton::~mmgMaterialButton() 
//----------------------------------------------------------------------------
{
  mafDEL(m_Material);
	/*if(m_Gui)	
    m_Gui->SetListener(NULL);*/
	cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
void mmgMaterialButton::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
	m_Gui->Show(true);

  wxStaticText *lab = new wxStaticText(m_Gui, -1, "material", wxDefaultPosition, wxSize(60,16), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  
  m_Material->MakeIcon();
  m_MaterialButton = new mmgPicButton(m_Gui, m_Material->m_Icon, ID_MATERIAL, this);

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
void mmgMaterialButton::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_MATERIAL:
        mafEventMacro(mafEvent(this,VME_CHOOSE_MATERIAL,m_Vme));
        m_MaterialLabel->SetLabel(m_Material->m_MaterialName.GetCStr());
        cppDEL(m_Material->m_Icon);
        UpdateMaterialIcon();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
        e->Log();
    }
	}
}
//----------------------------------------------------------------------------
void mmgMaterialButton::UpdateMaterialIcon() 
//----------------------------------------------------------------------------
{
	if(m_Material->m_Icon == NULL)
	{
		m_Material->MakeIcon();
		m_MaterialButton->SetBitmapLabel(*m_Material->m_Icon);
    m_MaterialButton->Refresh();
	}
  m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgMaterialButton::Enable(bool enable)
//----------------------------------------------------------------------------
{
  m_MaterialButton->Enable(enable);
  m_Gui->Update();
}
