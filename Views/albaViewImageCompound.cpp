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
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkDataSet.h"
#include "albaGUIPicButton.h"
#include "vtkViewport.h"
#include "albaGUILutPreset.h"
#include "vtkRendererCollection.h"
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
	m_CanSpin = false;

	m_Ruler = NULL;
	m_Renderer = NULL;

	m_RulerButton = NULL;
	m_ShowRulerButton = true;
	m_ShowRuler = false;

	m_ReverseLUTButton = NULL;
	m_ShowReverseLUTButton = true;
	m_IsLutReversed = false;
	m_CurrentLUTPreset = 4;

	m_ShowInterpolationButton = true;
}
//----------------------------------------------------------------------------
albaViewImageCompound::~albaViewImageCompound()
//----------------------------------------------------------------------------
{
	if (m_ReverseLUTButton)
		delete m_ReverseLUTButton;

	if (m_Ruler)
	{
		m_Renderer->RemoveActor2D(m_Ruler);
		albaDEL(m_Ruler);
	}

	delete m_RulerButton;

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
	v->m_CanSpin = m_CanSpin;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewImageCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new albaGUI(this);

  wxBoxSizer *mainVertSizer = new wxBoxSizer(wxHORIZONTAL);

  m_LutSlider = new albaGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500, 24);
  m_LutSlider->SetMinSize(wxSize(500, 24));

  mainVertSizer->Add(m_LutSlider, wxEXPAND);

  if (m_ShowReverseLUTButton)
  {
	  m_ReverseLUTButton = new albaGUIPicButton(m_GuiView, "REVERSE_LUT_ICON", ID_REVERSE_LUT, this);
	  m_ReverseLUTButton->SetToolTip("LUT Reverse");
	  m_ReverseLUTButton->SetListener(this);

	  mainVertSizer->Add(m_ReverseLUTButton);
		mainVertSizer->AddSpacer(2);
  }

	if (m_ShowInterpolationButton)
	{
		m_InterpolateButton = new albaGUIPicButton(m_GuiView, "INTERPOLATE_ICON", ID_INTERPOLATE, this);
		m_InterpolateButton->SetToolTip("Enable/Disable Interpolation");
		m_InterpolateButton->SetListener(this);

		mainVertSizer->Add(m_InterpolateButton);
		mainVertSizer->AddSpacer(2);
	}

  if (m_ShowRulerButton)
  {
	  m_RulerButton = new albaGUIPicButton(m_GuiView, "SHOW_RULER_ICON", ID_VIEW_RULER, this);
	  m_RulerButton->SetToolTip("Show Ruler");
	  m_RulerButton->SetListener(this);

	  mainVertSizer->Add(m_RulerButton);
		mainVertSizer->AddSpacer(1);
  }

  m_GuiView->Add(mainVertSizer, 1, wxEXPAND);
  m_GuiView->Reparent(m_Win);

  EnableWidgets(false);
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
				CameraUpdate();
			}
			break;

		case ID_REVERSE_LUT:
		{			
			LutReverse();
		}
		break;

		case ID_VIEW_RULER:
		{
			ShowRuler(!m_ShowRuler);
		}
		break;

		case ID_INTERPOLATE:
		{
			ChangeInterpolation();
		}
		break;
    default:
      Superclass::OnEvent(alba_event);
  }
}

//-------------------------------------------------------------------------
void albaViewImageCompound::LutReverse()
{
	if (m_ColorLUT)
	{
		m_CurrentLUTPreset = m_IsLutReversed ? 4 : 20; // gray or reverted gray

		lutPreset(m_CurrentLUTPreset, m_ColorLUT);

		m_IsLutReversed = !m_IsLutReversed;

		CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaViewImageCompound::ShowRuler(bool show)
{
	m_ShowRuler = show;

	if (!m_Ruler)
	{
		SetRendererByView();

		// 		m_RulerButton->SetBitmap("HIDE_RULER_ICON");
		// 		m_RulerButton->SetToolTip("Show Ruler");

		m_Ruler = vtkALBASimpleRulerActor2D::New();
		m_Ruler->SetColor(0.5, 1.0, 1.0);
		m_Ruler->SetLabelAxesVisibility(false);
		m_Ruler->SetLegend("mm/tick");
		m_Ruler->CenterAxesOnScreen(false);
		m_Ruler->ShowFixedTick(true);

		m_Renderer->AddActor2D((vtkActor2D *)m_Ruler);

		m_Ruler->SetVisibility(show);
		m_Renderer->GetRenderWindow()->Render();
	}
	else
	{
		// 		m_RulerButton->SetBitmap("SHOW_RULER_ICON");
		// 		m_RulerButton->SetToolTip("Hide Ruler");

		m_Ruler->SetVisibility(show);
		m_Renderer->GetRenderWindow()->Render();
	}

	if (!m_ShowRuler)
	{
		m_RulerButton->SetToolTip("Show Ruler");
	}
	else
	{
		m_RulerButton->SetToolTip("Hide Ruler");
	}
}
//-------------------------------------------------------------------------
albaGUI* albaViewImageCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_Gui->AddGui(((albaViewImage*)m_ChildViewList[ID_VIEW_IMAGE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER, "lut", m_ColorLUT);
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
	//avoid double show (with camera/lut reset) of the current image
	if (vme == m_CurrentImage && show)
		return;

	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(vme, show);

	if (vme->IsA("albaVMEImage"))
	{
		if (show)
		{
			m_CurrentImage = (albaVMEImage *)vme;
			albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(vme);
			//when show is false the color lut must be NULL because the image will be removed from the view
			m_ColorLUT = pipe && show ? pipe->GetLUT() : NULL;
			
			lutPreset(m_CurrentLUTPreset, m_ColorLUT);

			UpdateWindowing(show && pipe && pipe->IsGrayImage());
		}
		else
		{
			m_CurrentImage = NULL;
			m_ColorLUT = NULL;
			UpdateWindowing(false);
		}
	}
	
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewImageCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER, enable);

  m_LutSlider->Enable(enable);

  if (m_GuiView)
  {
	  m_GuiView->Enable(ID_REVERSE_LUT, enable);
	  //m_Gui->Enable(ID_VIEW_RULER, enable);
  }
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
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void albaViewImageCompound::SetRendererByView()
{
	vtkRenderer *newRenderer = NULL;

	vtkRendererCollection *rc;
	rc = this->GetRWI()->GetRenderWindow()->GetRenderers();

	// Searching for a frontal renderer on render collection
	if (rc)
	{
		rc->InitTraversal();
		vtkRenderer *ren;
		while (ren = rc->GetNextItem())
			if (ren->GetLayer() == 1)//Frontal Render
			{
				newRenderer = ren;
				break;
			}
	}

	m_Renderer = newRenderer;
}

//----------------------------------------------------------------------------
void albaViewImageCompound::ChangeInterpolation()
{
	albaPipeImage3D* pipe = (albaPipeImage3D*)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(m_CurrentImage);

	if (pipe)
		pipe->SetInterpolation(!pipe->GetInterpolation());
}
