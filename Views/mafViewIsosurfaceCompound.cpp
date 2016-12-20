/*=========================================================================

 Program: MAF2
 Module: mafViewIsosurfaceCompound
 Authors: Nicola Vanella
 
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

#include "mafViewIsosurfaceCompound.h"

#include "mafGUI.h"
#include "mafGUIFloatSlider.h"
#include "mafGUIValidator.h"
#include "mafPipeIsosurface.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafViewVTK.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewIsosurfaceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewIsosurfaceCompound::mafViewIsosurfaceCompound( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_ViewVolume = NULL;
	m_SliderContourIso = NULL;
	
	m_ContourValueIso = 0;
}
//----------------------------------------------------------------------------
mafViewIsosurfaceCompound::~mafViewIsosurfaceCompound()
//----------------------------------------------------------------------------
{
	cppDEL(m_SliderContourIso);
}

//----------------------------------------------------------------------------
mafView *mafViewIsosurfaceCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
	mafViewIsosurfaceCompound *v = new mafViewIsosurfaceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
  v->m_Listener = Listener;
  v->m_Id = m_Id;

	for (int i = 0; i < m_PluggedChildViewList.size(); i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}

  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();

  return v;
}
//----------------------------------------------------------------------------
void mafViewIsosurfaceCompound::CreateGuiView()
{
	m_GuiView = new mafGUI(this);

	double range[2] = { VTK_DOUBLE_MIN, VTK_DOUBLE_MAX };
	m_SliderContourIso = m_GuiView->FloatExpandedSlider(ID_CONTOUR_VALUE_ISO, "ISO", &m_ContourValueIso, range[0], range[1]);

	m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, false);
	m_GuiView->Reparent(m_Win);
}

//-------------------------------------------------------------------------
mafGUI* mafViewIsosurfaceCompound::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = mafView::CreateGui();

	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();

	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewIsosurfaceCompound::OnEvent(mafEventBase *maf_event)
{
	switch (maf_event->GetId())
	{
	case ID_CONTOUR_VALUE_ISO:
	{
		UpdateISO();
	}
	break;

	default:
		Superclass::OnEvent(maf_event);
	}
}

//-------------------------------------------------------------------------
void mafViewIsosurfaceCompound::PackageView()
{
	m_ViewVolume = new mafViewVTK();
	m_ViewVolume->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface", MUTEX);

	PlugChildView(m_ViewVolume);
}
//----------------------------------------------------------------------------
void mafViewIsosurfaceCompound::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("mafVMEVolumeGray"))
	{
		m_CurrentVolume = mafVMEVolumeGray::SafeDownCast(vme);

		mafPipeIsosurface *pipe = mafPipeIsosurface::SafeDownCast(m_ChildViewList[0]->GetNodePipe(vme));

		if (pipe)
		{
			double sr[2];
			((mafVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetVTKData()->GetScalarRange(sr);
			m_ContourValueIso = pipe->GetContourValue();

			m_SliderContourIso->SetRange(sr[0], sr[1], m_ContourValueIso);
			m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, true);
			m_Gui->Update();
		}
	}

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafViewIsosurfaceCompound::UpdateISO()
{
	if(m_CurrentVolume)
	{
		mafPipeIsosurface *pipe = mafPipeIsosurface::SafeDownCast(m_ChildViewList[0]->GetNodePipe(m_CurrentVolume));

		if (pipe)
		{
			pipe->SetContourValue((float)m_ContourValueIso);
 
			CameraUpdate();
		}
	}
}

//----------------------------------------------------------------------------
void mafViewIsosurfaceCompound::VmeRemove(mafVME *vme)
{
	Superclass::VmeRemove(vme);
		
	if (m_CurrentVolume == vme)
	{
		m_CurrentVolume = NULL;
		UpdateISO();
	}
}
