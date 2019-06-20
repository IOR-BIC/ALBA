/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCT
 Authors: Stefano Perticoni , Paolo Quadrani
 
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

#include "albaViewCT.h"
#include "albaViewSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"
#include "albaViewSlice.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

const int CT_CHILD_VIEWS_NUMBER  = 10;

enum CT_SUBVIEW_ID
{
	CT1 = 0,
	CT2,
	CT3,
	CT4,
	CT5,
	CT6,
	CT7,
	CT8,
	CT9,
	CT10
};


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewCT);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewCT::albaViewCT(wxString label)
: albaViewCompound(label, 1, 1)
//----------------------------------------------------------------------------
{

	m_BorderColor[0][0] = 1.0; m_BorderColor[0][1] = 0.0; m_BorderColor[0][2] = 0.0;
	m_BorderColor[1][0] = 0.0; m_BorderColor[1][1] = 1.0; m_BorderColor[1][2] = 0.0;
	m_BorderColor[2][0] = 0.0; m_BorderColor[2][1] = 0.0; m_BorderColor[2][2] = 1.0;
	m_BorderColor[3][0] = 1.0; m_BorderColor[3][1] = 1.0; m_BorderColor[3][2] = 0.0;
	m_BorderColor[4][0] = 0.0; m_BorderColor[4][1] = 1.0; m_BorderColor[4][2] = 1.0;
	m_BorderColor[5][0] = 1.0; m_BorderColor[5][1] = 0.0; m_BorderColor[5][2] = 1.0;
	m_BorderColor[6][0] = 0.5; m_BorderColor[6][1] = 0.0; m_BorderColor[6][2] = 0.0;
	m_BorderColor[7][0] = 0.0; m_BorderColor[7][1] = 0.5; m_BorderColor[7][2] = 0.0;
	m_BorderColor[8][0] = 0.0; m_BorderColor[8][1] = 0.0; m_BorderColor[8][2] = 0.5;
	m_BorderColor[9][0] = 0.5; m_BorderColor[9][1] = 0.5; m_BorderColor[9][2] = 0.0;

	m_CurrentVolume = NULL;
	//m_LayoutConfiguration = LAYOUT_CUSTOM;

	m_ViewCTCompound    = NULL;

	m_LayoutWidth = 0;
	m_LayoutHeight = 0;

}
//----------------------------------------------------------------------------
albaViewCT::~albaViewCT()
//----------------------------------------------------------------------------
{
	//m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
	m_ViewCTCompound = NULL;
}
//----------------------------------------------------------------------------
albaView *albaViewCT::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
	albaViewCT *v = new albaViewCT(m_Label);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	for (int i=0;i<m_PluggedChildViewList.size();i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
void albaViewCT::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeShow(vme, show);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewCT::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeRemove(vme);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewCT::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(alba_event->GetId())
		{
			case ID_LAYOUT_HEIGHT:
			case ID_LAYOUT_WIDTH:
				break;
			default:
				albaEventMacro(*alba_event);
		}
	}
	else
		Superclass::OnEvent(alba_event);
}
//-------------------------------------------------------------------------
albaGUI* albaViewCT::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_Gui->Integer(ID_LAYOUT_WIDTH,"Layout Width",&m_LayoutWidth);
	m_Gui->Integer(ID_LAYOUT_HEIGHT,"Layout Height",&m_LayoutHeight);

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewCT::CreateGuiView()
//----------------------------------------------------------------------------
{
	/*m_GuiView = new albaGUI(this);
	wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

	// create three windowing widgets
	//for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	for (int i = 0; i < 1; i++)
	{
		m_LutSliders[i] = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
		m_LutSliders[i]->SetListener(this);
		m_LutSliders[i]->SetSize(10,24);
		m_LutSliders[i]->SetMinSize(wxSize(10,24));
		lutsSizer->Add(m_LutSliders[i],wxALIGN_CENTER|wxRIGHT);
	}
	m_GuiView->Add(lutsSizer);
	m_GuiView->FitGui();
	m_GuiView->Update();
	m_GuiView->Reparent(m_Win);*/
}

//----------------------------------------------------------------------------
void albaViewCT::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewCTCompound = new albaViewCompound("CT view",2,5);
	albaViewSlice *vs = new albaViewSlice("Slice view", CAMERA_PERSPECTIVE);
	m_ViewCTCompound->PlugChildView(vs);
	PlugChildView(m_ViewCTCompound);
}
//----------------------------------------------------------------------------
void albaViewCT::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaViewCT::VmeSelect(albaVME *vme, bool select)
//----------------------------------------------------------------------------
{
	for(int i=0; i<m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeSelect(vme, select);
}