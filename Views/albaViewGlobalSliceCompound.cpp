/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewGlobalSliceCompound
 Authors: Eleonora Mambrini
 
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

#include "albaViewGlobalSliceCompound.h"
#include "albaViewGlobalSlice.h"
#include "albaViewVTK.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaGUIViewWin.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "albaGUIFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:


enum SUBVIEW_ID
{
  ID_VIEW_GLOBAL_SLICE = 0,
};

enum VIEW_WIDGET_ID
{
	ID_FIRST = MINID,
	ID_LAST
};
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewGlobalSliceCompound);


//----------------------------------------------------------------------------
albaViewGlobalSliceCompound::albaViewGlobalSliceCompound( wxString label, int num_row, int num_col)
: albaViewCompoundWindowing(label,num_row,num_col)

{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
}
//----------------------------------------------------------------------------
albaViewGlobalSliceCompound::~albaViewGlobalSliceCompound()

{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
albaView *albaViewGlobalSliceCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)

{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewGlobalSliceCompound *v = new albaViewGlobalSliceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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

void albaViewGlobalSliceCompound::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_RANGE_MODIFIED:
	{
		
			double low, hi;
			m_LutSlider->GetSubRange(&low, &hi);

			for (int i = 0; i < m_LUTs.size(); i++)
				m_LUTs[i]->SetTableRange(low, hi);
			
			CameraUpdate();
	}
	break;
	case ID_LUT_CHOOSER:
	{
		double *sr;
		sr = m_ColorLUT->GetRange();
		
		for (int i = 0; i < m_LUTs.size(); i++)
			m_LUTs[i]->SetTableRange(sr[0], sr[1]);

		m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);

		CameraUpdate();
	}
	break;
	default:
		Superclass::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void albaViewGlobalSliceCompound::VmeShow(albaVME *vme, bool show)
{
	for (int i = 0; i < this->GetNumberOfSubView(); i++)
		m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("albaVMEVolumeGray") || vme->IsA("albaVMELabeledVolume"))
	{
		if (show)
		{
			// Check if VME is already in m_VisibleVMEs
			bool vmeFound = false;
			for (int i = 0; i < m_VisibleVMEs.size(); i++)
			{
				if (m_VisibleVMEs[i] == vme)
				{
					vmeFound = true;
					break;
				}
			}

			// Add VME and its LUT if not already present
			if (!vmeFound)
			{
				m_VisibleVMEs.push_back(vme);
				m_LUTs.push_back(albaVMEVolumeGray::SafeDownCast(vme)->GetMaterial()->m_ColorLut);

				if (m_LUTs.size() == 1)
				{
					m_LutSlider->SetRange(m_LUTs[0]->GetTableRange());
					m_LutSlider->SetSubRange(m_LUTs[0]->GetTableRange());
				}
				else
				{
					double low, hi;
					m_LutSlider->GetSubRange(&low, &hi);
					m_LUTs[m_LUTs.size()-1]->SetTableRange(low,hi);
				}

			}

			m_LutSlider->Enable(true);
		}
		else
		{
			// Remove VME and corresponding LUT from vectors
			for (int i = 0; i < m_VisibleVMEs.size(); i++)
			{
				if (m_VisibleVMEs[i] == vme)
				{
					m_VisibleVMEs.erase(m_VisibleVMEs.begin() + i);
					m_LUTs.erase(m_LUTs.begin() + i);
					break;
				}
			}

			m_LutSlider->Enable(m_LUTs.size()>0);
		}

		UpdateLutSlider();
	}

	CameraUpdate();
}

//-------------------------------------------------------------------------
void albaViewGlobalSliceCompound::VmeSelect(albaVME *node, bool select)
{
	Superclass::VmeSelect(node, select);

	if(node->IsA("albaVMEVolumeGray") || node->IsA("albaVMELabeledVolume"))
	{
		m_ColorLUT = select ? albaVMEVolumeGray::SafeDownCast(node)->GetMaterial()->m_ColorLut : NULL;

		m_LutWidget->SetLut(m_ColorLUT);
		m_LutWidget->Enable(m_ColorLUT != NULL);
	}
}

//-------------------------------------------------------------------------
albaGUI* albaViewGlobalSliceCompound::CreateGui()
{
	assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	m_Gui->AddGui(((albaViewGlobalSlice*)m_ChildViewList[ID_VIEW_GLOBAL_SLICE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider(0);
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}

void albaViewGlobalSliceCompound::UpdateLutSlider()
{
	double min = VTK_DOUBLE_MAX, max = VTK_DOUBLE_MIN;

	for(int i = 0; i < m_VisibleVMEs.size(); i++)
	{
		double *range = m_VisibleVMEs[i]->GetOutput()->GetVTKData()->GetScalarRange();
		if (range[0] < min) min = range[0];
		if (range[1] > max) max = range[1];
	}

	double subRange[2];
	m_LutSlider->GetSubRange(subRange);

	if (subRange[0] < min) 
		subRange[0] = min;
	if (subRange[1] > max) 
		subRange[1] = max;

	m_LutSlider->SetRange(min, max);
	m_LutSlider->SetSubRange(subRange);
}

//-------------------------------------------------------------------------
void albaViewGlobalSliceCompound::PackageView()
{
	m_ViewGlobalSlice = new albaViewGlobalSlice("",CAMERA_OS_P);
	m_ViewGlobalSlice->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeOrthoSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeOrthoSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMEAnalog", "albaPipeBox", NON_VISIBLE);
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");

	PlugChildView(m_ViewGlobalSlice);
}

