/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCompoundWindowing
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

#include "albaViewCompoundWindowing.h"
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
#include "albaPipeImage3D.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEVolumeGray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaGUIFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
albaViewCompoundWindowing::albaViewCompoundWindowing( wxString label, int num_row, int num_col)
: albaViewCompound(label,num_row,num_col)
{
	m_LutSlider       = NULL;
	m_ColorLUT        = NULL;
}
//----------------------------------------------------------------------------
albaViewCompoundWindowing::~albaViewCompoundWindowing()
{
	cppDEL(m_LutSlider);
}

//----------------------------------------------------------------------------
void albaViewCompoundWindowing::CreateGuiView()
{
	m_GuiView = new albaGUI(this);
  
  m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  
	UpdateWindowing(false,NULL);
  
	m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewCompoundWindowing::OnEvent(albaEventBase *alba_event)
{
  switch(alba_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
					double low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					CameraUpdate();
			}
			break;
		case ID_LUT_CHOOSER:
			{
				double *sr;
				sr = m_ColorLUT->GetRange();
				m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
			}
			break;
    default:
			Superclass::OnEvent(alba_event);
  }
}

//----------------------------------------------------------------------------
void albaViewCompoundWindowing::VmeShow(albaVME *vme, bool show)
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);
  
	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
bool albaViewCompoundWindowing::ActivateWindowing(albaVME *vme)
{
  bool conditions     = false;
  bool nodeHasPipe    = false;
  
  if(vme->GetOutput()->IsA("albaVMEOutputVolume")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {
      
      if(m_ChildViewList[i]->GetNodePipe(vme)) {
        nodeHasPipe = true;
      }
      conditions = (conditions && nodeHasPipe);
    }
  }

  else if(vme->IsA("albaVMEImage")){
    
    conditions = true;

    for(int i=0; i<m_NumOfChildView; i++) {

      albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[i]->GetNodePipe(vme);
      conditions = (conditions && (pipe && pipe->IsGrayImage()));
    }
  }

  return conditions;
}

//----------------------------------------------------------------------------
void albaViewCompoundWindowing::UpdateWindowing(bool enable, albaVME *vme)
{
	if (enable)
	{
		double *dataRange;
		dataRange=vme->GetOutput()->GetVTKData()->GetScalarRange();

		if (albaVMEVolumeGray::SafeDownCast(vme))
			SetLutAndSlider(((albaVMEVolumeGray *)vme)->GetMaterial()->m_ColorLut,dataRange);
		else if (albaVMEImage::SafeDownCast(vme))
			SetLutAndSlider(((albaVMEImage *)vme)->GetMaterial()->m_ColorLut,dataRange);
	}
	else
	{
		SetLutAndSlider(NULL,NULL);
	}
}

//----------------------------------------------------------------------------
void albaViewCompoundWindowing::SetLutAndSlider(vtkLookupTable *lut, double *dataRange)
{
	m_ColorLUT = lut;

	if (m_LutSlider)
	{
		if (m_ColorLUT)
		{
			m_LutSlider->SetRange(dataRange);
			double *sr = m_ColorLUT->GetTableRange();
			m_LutSlider->SetSubRange(sr);
		}
		else
		{
			m_LutSlider->SetRange(0, 1);
			m_LutSlider->SetSubRange(0, 1);
		}
		m_LutSlider->Enable(m_ColorLUT != NULL);
	}
}

