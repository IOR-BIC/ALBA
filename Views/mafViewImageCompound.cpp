/*=========================================================================

 Program: MAF2
 Module: mafViewImageCompound
 Authors: Daniele Giunchi
 
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

#include "mafViewImageCompound.h"
#include "mafViewImage.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipeImage3D.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafVMEImage.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEOutputImage.h"
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
mafCxxTypeMacro(mafViewImageCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewImageCompound::mafViewImageCompound( wxString label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
mafViewImageCompound::~mafViewImageCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView *mafViewImageCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewImageCompound *v = new mafViewImageCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
void mafViewImageCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);
  
  m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets(false);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(m_CurrentImage)
				{
					double low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					GetLogicManager()->CameraUpdate();
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
      Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewImageCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

	m_Gui->AddGui(((mafViewImage*)m_ChildViewList[ID_VIEW_IMAGE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewImageCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewImage = new mafViewImage("View Image",CAMERA_CT,false,false,false);
	m_ViewImage->PlugVisualPipe("mafVMEVolumeGray","mafPipeBox",NON_VISIBLE);
  m_ViewImage->PlugVisualPipe("mafVMESurface","mafPipeSurface",NON_VISIBLE);
	
	PlugChildView(m_ViewImage);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("mafVMEImage"))
	{
		m_CurrentImage=mafVMEImage::SafeDownCast(vme);
		mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(vme);
		m_ColorLUT = pipe ? pipe->GetLUT() : NULL;
		UpdateWindowing(show && pipe && pipe->IsGrayImage());
	}
	
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewImageCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}

//----------------------------------------------------------------------------
void mafViewImageCompound::UpdateWindowing(bool enable)
//----------------------------------------------------------------------------
{
	if(enable && m_CurrentImage)
	{
		double sr[2];
		m_CurrentImage->GetOutput()->GetVTKData()->GetScalarRange(sr);
		m_ColorLUT->SetRange(sr);
      
    m_LutWidget->SetLut(m_ColorLUT);
		m_LutWidget->Enable(true);
		m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);

		EnableWidgets(enable);
	}
	else
	{
		EnableWidgets(enable);
		m_LutSlider->SetRange(-100,100);
		m_LutSlider->SetSubRange(-100,100);
	}
}

void mafViewImageCompound::VmeRemove(mafVME *vme)
{
	Superclass::VmeRemove(vme);
		
	if (m_CurrentImage == vme)
	{
		m_CurrentImage = NULL;
		m_ColorLUT = NULL;
		UpdateWindowing(false);
	}

}
