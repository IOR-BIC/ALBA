/*=========================================================================
Program:   Alba
Module:    mafViewSliceBlendRX.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewSliceBlendRX.h"
#include "mafViewRX.h"
#include "mafViewSlice.h"
#include "mafPipeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafGUILutPreset.h"
#include "mafGUI.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutSlider.h"
#include "mafGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"
#include "mafViewSliceBlend.h"
#include "mafPipeVolumeSliceBlend.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum RX_SUBVIEW_ID
{
  RX_VIEW = 0,
  BLEND_VIEW,
  VIEWS_NUMBER,
};
enum GIZMO_ID
{
  GIZMO_0 = 0,
  GIZMO_1,
};
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSliceBlendRX);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSliceBlendRX::mafViewSliceBlendRX(wxString label)
: mafViewCompound(label, 1, 2)
{
  //default values
  m_LutWidget = NULL;
  m_CurrentVolume = NULL;
  m_LayoutConfiguration = LAYOUT_CUSTOM;
  m_ViewsRX = NULL;
  m_ViewSliceBlend = NULL;

  m_LutSliders = NULL;
  m_VtkLUT = NULL ;
  m_BlendGui = NULL;

	m_BorderColor[0][0] = 1.0; m_BorderColor[0][1] = 0.0; m_BorderColor[0][2] = 1.0;
  m_BorderColor[1][0] = 1.0; m_BorderColor[1][1] = 0.65; m_BorderColor[1][2] = 1.0;
}
//----------------------------------------------------------------------------
mafViewSliceBlendRX::~mafViewSliceBlendRX()
{
  cppDEL(m_LutSliders);
  vtkDEL(m_VtkLUT);
}

//----------------------------------------------------------------------------
mafView *mafViewSliceBlendRX::Copy(mafObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	mafViewSliceBlendRX *v = new mafViewSliceBlendRX(m_Label);
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
void mafViewSliceBlendRX::VmeShow(mafVME *vme, bool show)
{
  for(int i=0; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->VmeShow(vme, show);
  }

  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
			// Get Lut minMax from Views 
			((mafViewSliceBlend*)m_ChildViewList[BLEND_VIEW])->GetLutRange(m_LutBlendMinMax);
			((mafViewRX*)m_ChildViewList[RX_VIEW])->GetLutRange(m_LutRxMinMax);

      //set new values for lut slider
      m_LutSliders->SetRange(m_LutBlendMinMax[0],m_LutBlendMinMax[1]);
      m_LutSliders->SetSubRange(m_LutBlendMinMax[0],m_LutBlendMinMax[1]);
      // create a lookup table for each RX view
      vtkNEW(m_VtkLUT);
      m_VtkLUT->SetRange(m_LutBlendMinMax);
      m_VtkLUT->Build();
      lutPreset(4,m_VtkLUT);
     
      ((mafViewRX*)m_ChildViewList[RX_VIEW])->SetLutRange(m_LutRxMinMax[0], m_LutRxMinMax[1]);
			((mafViewSliceBlend*)m_ChildViewList[BLEND_VIEW])->SetLutRange(m_LutBlendMinMax[0], m_LutBlendMinMax[1]);

      m_CurrentVolume = vme;

      //Create Gizmos for the volume visualized
      GizmoCreate();
    }
    else
    {
      // Remove Gizmos for the volume showed off
      m_CurrentVolume = NULL;
      GizmoDelete();
    }
  } 
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::VmeRemove(mafVME *vme)
{
  //If node is a volume delete gizmos
  if (m_CurrentVolume && vme == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    GizmoDelete();
  }

  Superclass::VmeRemove(vme);
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (maf_event->GetId())
		{
		case ID_SLICE_POSITION:
		{
			BoundsValidate(m_SliceTop_Position);
			BoundsValidate(m_SliceBottom_Position);

			((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->SetSlice(0, m_SliceTop_Position);
			((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->SetSlice(1, m_SliceBottom_Position);

			m_GizmoSlice[GIZMO_0]->UpdateGizmoSliceInLocalPositionOnAxis(GIZMO_0, mafGizmoSlice::GIZMO_SLICE_Z, m_SliceTop_Position[2]);
			m_GizmoSlice[GIZMO_1]->UpdateGizmoSliceInLocalPositionOnAxis(GIZMO_1, mafGizmoSlice::GIZMO_SLICE_Z, m_SliceBottom_Position[2]);

			m_GizmoSliceHeight[0] = m_SliceTop_Position[2];
			m_GizmoSliceHeight[1] = m_SliceBottom_Position[2];

			((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->CameraUpdate();
			((mafViewRX *)m_ChildViewList[RX_VIEW])->CameraUpdate();

			m_Gui->Update();
		}
		break;
		// events from the slider
		case ID_RANGE_MODIFIED:
		{
			// is the volume visible?
			if (((mafViewRX*)m_ChildViewList[RX_VIEW])->VolumeIsVisible())
			{
				//Get new values of LUT
				double low, hi;
				m_LutSliders->GetSubRange(&low, &hi);

				SetLutRange(low, hi);
			}
		}
		case MOUSE_UP:
		case MOUSE_MOVE:
		{
			//Manage MOUSE events
			OnEventMouseMove(e);
		}
		break;
		break;
		default:
			//Other events
			mafViewCompound::OnEvent(maf_event);
		}
	}
	else
	{
		mafViewCompound::OnEvent(maf_event);
	}
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::OnEventMouseMove(mafEvent *e)
{
	vtkPoints *p = (vtkPoints *)e->GetVtkObj();

	if (p == NULL) 
		return;

	long gizmoId = e->GetArg();
	double newSliceLocalOrigin[3];
	
	//Get point picked
	p->GetPoint(0, newSliceLocalOrigin);

	//Validate slices origins and adjust them
	BoundsValidate(newSliceLocalOrigin);

	m_GizmoSliceHeight[gizmoId] = newSliceLocalOrigin[2];
	
	// Find the Gizmo slicer on top
	int topSlice = m_GizmoSliceHeight[gizmoId] > m_GizmoSliceHeight[!gizmoId];
		
	((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->SetSlice(topSlice, newSliceLocalOrigin);
	((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->CameraUpdate();

	if (topSlice)
	{
		m_SliceTop_Position[0] = newSliceLocalOrigin[0];
		m_SliceTop_Position[1] = newSliceLocalOrigin[1];
		m_SliceTop_Position[2] = newSliceLocalOrigin[2];
	}
	else
	{
		m_SliceBottom_Position[0] = newSliceLocalOrigin[0];
		m_SliceBottom_Position[1] = newSliceLocalOrigin[1];
		m_SliceBottom_Position[2] = newSliceLocalOrigin[2];
	}

	m_Gui->Update();

	m_ChildViewList[RX_VIEW]->CameraUpdate();
	m_ChildViewList[BLEND_VIEW]->CameraUpdate();
}


//-------------------------------------------------------------------------
mafGUI* mafViewSliceBlendRX::CreateGui()
{
  assert(m_Gui == NULL);
	m_Gui = mafView::CreateGui();
	
  m_Gui->Label(_("Blend View"),true);

  m_BlendGui = m_ChildViewList[BLEND_VIEW]->GetGui();
  m_Gui->AddGui(m_BlendGui);
	m_Gui->Divider(1);

// 	m_Gui->VectorN(ID_SLICE_POSITION, "Slice 1", m_Slice1_Position);
// 	m_Gui->VectorN(ID_SLICE_POSITION, "Slice 2", m_Slice2_Position);

	m_Gui->Double(ID_SLICE_POSITION, "Top:", m_SliceTop_Position+2);
	m_Gui->Double(ID_SLICE_POSITION, "Bottom:", m_SliceBottom_Position+2);
	m_Gui->Divider(1);

	m_Gui->FitInside();
  m_Gui->Update();
		  
  //Enable/disable Gui components depending from volume
  EnableWidgets(m_CurrentVolume != NULL);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::CreateGuiView()
{
  m_GuiView = new mafGUI(this);
	
  m_LutSliders = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
  //EnableWidgets(m_CurrentVolume != NULL);
  m_LutSliders->SetListener(this);
  m_LutSliders->SetMinSize(wxSize(500,24));

	wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);
  lutsSizer->Add(m_LutSliders,wxALIGN_CENTER|wxRIGHT);
  
	//Add lut slider to the gui view
  m_GuiView->Add(lutsSizer);
  m_GuiView->Reparent(m_Win);
  m_GuiView->FitGui();
  m_GuiView->Update();
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_LUT_WIDGET, enable);
		m_Gui->Enable(ID_SLICE_POSITION, enable && m_CurrentVolume);
	}
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::PackageView()
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};

  //Create a mafViewRX
  m_ViewsRX = new mafViewRX("RX child view", cam_pos[0]);
  m_ViewsRX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
  m_ViewsRX->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeProjected",MUTEX);
  PlugChildView(m_ViewsRX);

  //Create a mafViewSliceBlend
  m_ViewSliceBlend = new mafViewSliceBlend("Blend", CAMERA_OS_Z);
  m_ViewSliceBlend->PlugVisualPipe("mafVMEVolumeGray","mafPipeVolumeSliceBlend",MUTEX);
  PlugChildView(m_ViewSliceBlend);
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::LayoutSubView(int width, int height)
{
  // this implement the Fixed SubViews Layout
  int border = 2;
  int x_pos, c, i;

  int step_width  = (width-border) / 2;
  i = 0;
  for (c = 0; c < m_NumOfChildView; c++)
  {
    x_pos = c*(step_width + border);
    m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
    i++;
  }  
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::GizmoCreate()
{
  // Retrieve mafPipeVolumeSliceBlend to obtain slices positions
  mafPipeVolumeSliceBlend *p = mafPipeVolumeSliceBlend::SafeDownCast(m_ChildViewList[BLEND_VIEW]->GetNodePipe(m_CurrentVolume));

  if (p == NULL)
  {
    //No volume is visualized
    return;
  }

	//Create Gizmo0
	p->GetSliceOrigin(GIZMO_0, m_SliceBottom_Position);
	m_GizmoSlice[GIZMO_0] = new mafGizmoSlice(m_CurrentVolume, this);
  m_GizmoSlice[GIZMO_0]->CreateGizmoSliceInLocalPositionOnAxis(GIZMO_0,mafGizmoSlice::GIZMO_SLICE_Z, m_SliceBottom_Position[2]);
  m_GizmoSlice[GIZMO_0]->SetColor(m_BorderColor[GIZMO_0]);

  //Create Gizmo1
	p->GetSliceOrigin(GIZMO_1, m_SliceTop_Position );
	m_GizmoSlice[GIZMO_1] = new mafGizmoSlice(m_CurrentVolume, this);
  m_GizmoSlice[GIZMO_1]->CreateGizmoSliceInLocalPositionOnAxis(GIZMO_1,mafGizmoSlice::GIZMO_SLICE_Z, m_SliceTop_Position[2]);
  m_GizmoSlice[GIZMO_1]->SetColor(m_BorderColor[GIZMO_1]);

  //Show On Gizmos
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_0]->GetOutput(), true);
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_1]->GetOutput(), true);

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::GizmoDelete()
{
  //Show Off Gizmos
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_0]->GetOutput(),false);
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_1]->GetOutput(),false);
  cppDEL(m_GizmoSlice[GIZMO_0]);
  cppDEL(m_GizmoSlice[GIZMO_1]);
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::BoundsValidate(double *pos)
{
  //Check if a volume is present
  if(m_CurrentVolume)
  {
    double b[6];
    m_CurrentVolume->GetOutput()->GetVTKData()->GetBounds(b);
    for(int i=0;i<3;i++)
    {
      if(pos[i]<b[i*2])
        pos[i]=b[i*2];
      if(pos[i]>b[i*2+1])
        pos[i]=b[i*2+1];
    }
  }
}

//----------------------------------------------------------------------------
void mafViewSliceBlendRX::SetLutRange(double low, double hi)
{
	double deltaLutBlend = (m_LutBlendMinMax[1] - m_LutBlendMinMax[0]);
	double deltaLutRx = (m_LutRxMinMax[1] - m_LutRxMinMax[0]);

	double val1 = (((low - m_LutBlendMinMax[0]) / deltaLutBlend)* deltaLutRx) + m_LutRxMinMax[0];
	double val2 = (((hi - m_LutBlendMinMax[0]) / deltaLutBlend)*deltaLutRx) + m_LutRxMinMax[0];

	((mafViewRX*)m_ChildViewList[RX_VIEW])->SetLutRange(val1, val2);
	((mafViewSliceBlend*)m_ChildViewList[BLEND_VIEW])->SetLutRange(low, hi);

	CameraUpdate();
}