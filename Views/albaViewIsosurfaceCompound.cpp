/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewIsosurfaceCompound
 Authors: Nicola Vanella
 
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

#include "albaViewIsosurfaceCompound.h"

#include "albaGUI.h"
#include "albaGUIFloatSlider.h"
#include "albaGUIValidator.h"
#include "albaPipeIsosurface.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaViewVTK.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewIsosurfaceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewIsosurfaceCompound::albaViewIsosurfaceCompound( wxString label, int num_row, int num_col)
: albaViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_ViewVolume = NULL;
	m_SliderContourIso = NULL;
	
	m_ContourValueIso = 0;
}
//----------------------------------------------------------------------------
albaViewIsosurfaceCompound::~albaViewIsosurfaceCompound()
//----------------------------------------------------------------------------
{
	cppDEL(m_SliderContourIso);
}

//----------------------------------------------------------------------------
albaView *albaViewIsosurfaceCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
	albaViewIsosurfaceCompound *v = new albaViewIsosurfaceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
void albaViewIsosurfaceCompound::CreateGuiView()
{
	m_GuiView = new albaGUI(this);

	double range[2] = { VTK_DOUBLE_MIN, VTK_DOUBLE_MAX };
	m_SliderContourIso = m_GuiView->FloatExpandedSlider(ID_CONTOUR_VALUE_ISO, "ISO", &m_ContourValueIso, range[0], range[1]);

	m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, false);
	m_GuiView->Reparent(m_Win);
}

//-------------------------------------------------------------------------
albaGUI* albaViewIsosurfaceCompound::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewIsosurfaceCompound::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_CONTOUR_VALUE_ISO:
	{
		UpdateISO();
	}
	break;

	default:
		Superclass::OnEvent(alba_event);
	}
}

//-------------------------------------------------------------------------
void albaViewIsosurfaceCompound::PackageView()
{
	m_ViewVolume = new albaViewVTK();
	m_ViewVolume->PlugVisualPipe("albaVMEVolumeGray", "albaPipeIsosurface", MUTEX);

	PlugChildView(m_ViewVolume);
}
//----------------------------------------------------------------------------
void albaViewIsosurfaceCompound::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("albaVMEVolumeGray"))
	{
		m_CurrentVolume = albaVMEVolumeGray::SafeDownCast(vme);

		albaPipeIsosurface *pipe = albaPipeIsosurface::SafeDownCast(m_ChildViewList[0]->GetNodePipe(vme));

		if (pipe)
		{
			double sr[2];
			((albaVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetVTKData()->GetScalarRange(sr);
			m_ContourValueIso = pipe->GetContourValue();

			m_SliderContourIso->SetRange(sr[0], sr[1], m_ContourValueIso);
			m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, true);
			m_Gui->Update();
		}
	}
	m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, show);
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void albaViewIsosurfaceCompound::UpdateISO()
{
	if(m_CurrentVolume)
	{
		albaPipeIsosurface *pipe = albaPipeIsosurface::SafeDownCast(m_ChildViewList[0]->GetNodePipe(m_CurrentVolume));

		if (pipe)
		{
			pipe->SetContourValue(m_ContourValueIso);
 
			CameraUpdate();
		}
	}
}

//----------------------------------------------------------------------------
void albaViewIsosurfaceCompound::VmeRemove(albaVME *vme)
{
	Superclass::VmeRemove(vme);
		
	if (m_CurrentVolume == vme)
	{
		m_CurrentVolume = NULL;
		UpdateISO();
	}

	m_GuiView->Enable(ID_CONTOUR_VALUE_ISO, false);
}
