/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewImageCompound
 Authors: Daniele Giunchi
 
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

#include "albaViewImageCompound.h"
#include "albaViewImage.h"
#include "albaViewVTK.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaPipeImage3D.h"
#include "albaGUIViewWin.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMEImage.h"
#include "albaGUIFloatSlider.h"
#include "albaVMEOutputImage.h"
#include "vtkImageData.h"
#include "vtkTexture.h"

#include "vtkLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_IMAGE = 0,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewImageCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewImageCompound::albaViewImageCompound( wxString label, int num_row, int num_col)
: albaViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
albaViewImageCompound::~albaViewImageCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
albaView *albaViewImageCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewImageCompound *v = new albaViewImageCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
void albaViewImageCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new albaGUI(this);
  
  m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets(false);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewImageCompound::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(m_CurrentImage)
				{
					double low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					CameraUpdate();
				}
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
//-------------------------------------------------------------------------
albaGUI* albaViewImageCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	m_Gui->AddGui(((albaViewImage*)m_ChildViewList[ID_VIEW_IMAGE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaViewImageCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewImage = new albaViewImage("View Image",CAMERA_CT,false,false,false);
	m_ViewImage->PlugVisualPipe("albaVMEVolumeGray","albaPipeBox",NON_VISIBLE);
  m_ViewImage->PlugVisualPipe("albaVMESurface","albaPipeSurface",NON_VISIBLE);
	
	PlugChildView(m_ViewImage);
}
//----------------------------------------------------------------------------
void albaViewImageCompound::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("albaVMEImage"))
	{
		m_CurrentImage=albaVMEImage::SafeDownCast(vme);
		albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(vme);
		//when show is false the color lut must be NULL because the image will be removed from the view
		m_ColorLUT = pipe && show ? pipe->GetLUT() : NULL;
		UpdateWindowing(show && pipe && pipe->IsGrayImage());
	}
	
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewImageCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}

//----------------------------------------------------------------------------
void albaViewImageCompound::UpdateWindowing(bool enable)
//----------------------------------------------------------------------------
{
	m_LutWidget->SetLut(m_ColorLUT);
	EnableWidgets(enable);

	if(enable && m_CurrentImage)
	{
		double sr[2];
		m_CurrentImage->GetOutput()->GetVTKData()->GetScalarRange(sr);
		m_ColorLUT->SetRange(sr);
      
		m_LutWidget->Enable(true);
		m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	}
	else
	{
		m_LutSlider->SetRange(-100,100);
		m_LutSlider->SetSubRange(-100,100);
	}
}

void albaViewImageCompound::VmeRemove(albaVME *vme)
{
	Superclass::VmeRemove(vme);
		
	if (m_CurrentImage == vme)
	{
		m_CurrentImage = NULL;
		m_ColorLUT = NULL;
		UpdateWindowing(false);
	}

}
