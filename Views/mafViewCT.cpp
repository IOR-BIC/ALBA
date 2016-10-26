/*=========================================================================

 Program: MAF2
 Module: mafViewCT
 Authors: Stefano Perticoni , Paolo Quadrani
 
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

#include "mafViewCT.h"
#include "mafViewSlice.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"
#include "mafViewSlice.h"

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
mafCxxTypeMacro(mafViewCT);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewCT::mafViewCT(wxString label)
: mafViewCompound(label, 1, 1)
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
mafViewCT::~mafViewCT()
//----------------------------------------------------------------------------
{
	//m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
	m_ViewCTCompound = NULL;
}
//----------------------------------------------------------------------------
mafView *mafViewCT::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
	mafViewCT *v = new mafViewCT(m_Label);
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
void mafViewCT::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeShow(vme, show);

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewCT::VmeRemove(mafVME *vme)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeRemove(vme);

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewCT::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(maf_event->GetId())
		{
			case ID_LAYOUT_HEIGHT:
			case ID_LAYOUT_WIDTH:
				break;
			default:
				mafEventMacro(*maf_event);
		}
	}
	else
		Superclass::OnEvent(maf_event);
}
//-------------------------------------------------------------------------
mafGUI* mafViewCT::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = mafView::CreateGui();

	m_Gui->Integer(ID_LAYOUT_WIDTH,"Layout Width",&m_LayoutWidth);
	m_Gui->Integer(ID_LAYOUT_HEIGHT,"Layout Height",&m_LayoutHeight);

	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewCT::CreateGuiView()
//----------------------------------------------------------------------------
{
	/*m_GuiView = new mafGUI(this);
	wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

	// create three windowing widgets
	//for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	for (int i = 0; i < 1; i++)
	{
		m_LutSliders[i] = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
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
void mafViewCT::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewCTCompound = new mafViewCompound("CT view",2,5);
	mafViewSlice *vs = new mafViewSlice("Slice view", CAMERA_PERSPECTIVE);
	m_ViewCTCompound->PlugChildView(vs);
	PlugChildView(m_ViewCTCompound);
}
//----------------------------------------------------------------------------
void mafViewCT::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
}
/*//----------------------------------------------------------------------------
void mafViewCT::LayoutSubViewCustom(int width, int height)
//----------------------------------------------------------------------------
{
	// this implement the Fixed SubViews Layout
	int border = 2;
	int x_pos, c, i;

	int step_width  = (width-border); // /3
	i = 0;
	for (c = 0; c < m_NumOfChildView; c++)
	{
		x_pos = c*(step_width + border);
		m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
		i++;
	}
	((mafViewCompound*)m_ChildViewList[i-1])->OnLayout();
}*/
//----------------------------------------------------------------------------
void mafViewCT::VmeSelect(mafVME *vme, bool select)
//----------------------------------------------------------------------------
{
	for(int i=0; i<m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeSelect(vme, select);
}