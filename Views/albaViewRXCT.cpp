/*=========================================================================
Program:   Alba
Module:    albaViewRXCT.cpp
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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaViewRXCT.h"
#include "albaViewVTK.h"
#include "albaViewRX.h"
#include "albaViewSlice.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEIterator.h"
#include "albaGUILutPreset.h"
#include "albaGUI.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutSlider.h"
#include "albaGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaVisualPipeSlicerSlice.h"
#include "albaDeviceButtonsPadMouse.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

const int CT_CHILD_VIEWS_NUMBER  = 6;

enum RXCT_SUBVIEW_ID
{
  RX_FRONT_VIEW = 0,
  RX_SIDE_VIEW,
  CT_COMPOUND_VIEW,
  VIEWS_NUMBER,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewRXCT);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewRXCT::albaViewRXCT(wxString label)
: albaViewCompound(label, 1, 3)
{
  m_BorderColor[0][0] = 1; m_BorderColor[0][1] = 0; m_BorderColor[0][2] = 0;
  m_BorderColor[1][0] = 0; m_BorderColor[1][1] = 1; m_BorderColor[1][2] = 0;
  m_BorderColor[2][0] = 0; m_BorderColor[2][1] = 0; m_BorderColor[2][2] = 1;
  m_BorderColor[3][0] = 1; m_BorderColor[3][1] = 1; m_BorderColor[3][2] = 0;
  m_BorderColor[4][0] = 0; m_BorderColor[4][1] = 1; m_BorderColor[4][2] = 1;
  m_BorderColor[5][0] = 1; m_BorderColor[5][1] = 0; m_BorderColor[5][2] = 1;

  for(int j=0; j<CT_CHILD_VIEWS_NUMBER; j++) 
  {
    m_GizmoSlice[j] = NULL;
    m_Pos[j]=0;
    m_Sort[j]=j;
  }

  m_LutWidget = NULL;
  m_CurrentVolume = NULL;
  m_LayoutConfiguration = LAYOUT_CUSTOM;

  m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
  m_ViewCTCompound    = NULL;
  
  m_LutSliders[RX_FRONT_VIEW] = m_LutSliders[RX_SIDE_VIEW] = m_LutSliders[CT_COMPOUND_VIEW] = NULL;
  //m_vtkLUT[RX_FRONT_VIEW] = m_vtkLUT[RX_SIDE_VIEW] = m_vtkLUT[CT_COMPOUND_VIEW] = NULL;
  m_ColorLUT = NULL;

  m_RightOrLeft=1;
	m_Side = 0; // All
  m_MoveAllSlices = 0; 
  m_Snap=0;
  m_CurrentSurface.clear();
  m_AllSurface=0;
  m_Border=1;

	m_IsSubViewMaximized = false;

  m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
albaViewRXCT::~albaViewRXCT()
{
	m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
	m_ViewCTCompound = NULL;
	m_CurrentSurface.clear();

	for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
	{
		cppDEL(m_LutSliders[i]);
		//vtkDEL(m_vtkLUT[i]);
	}
}

//----------------------------------------------------------------------------
albaView *albaViewRXCT::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewRXCT *v = new albaViewRXCT(m_Label);
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
void albaViewRXCT::PackageView()
{
	int cam_pos[2] = { CAMERA_RX_FRONT, CAMERA_RX_LEFT };
	for (int v = RX_FRONT_VIEW; v < CT_COMPOUND_VIEW; v++)
	{
		// create to the child view
		m_ViewsRX[v] = new albaViewRX("RX child view", cam_pos[v]);
		m_ViewsRX[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeProjected", MUTEX);
		m_ViewsRX[v]->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeProjected", MUTEX);
		m_ViewsRX[v]->PlugVisualPipe("albaVMESlicer", "albaVisualPipeSlicerSlice", MUTEX);
		m_ViewsRX[v]->PlugVisualPipe("albaVMESegmentationVolume", "albaPipeVolumeProjected", MUTEX);

		PlugChildView(m_ViewsRX[v]);
	}

	m_ViewCTCompound = new albaViewCompound("CT view", 3, 2);
	albaViewSlice *vs = new albaViewSlice("Slice view", CAMERA_OS_Z);
	vs->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeOrthoSlice", MUTEX);
	vs->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeOrthoSlice", MUTEX);
	vs->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice", MUTEX);
	vs->PlugVisualPipe("albaVMEPolyline", "albaPipePolylineSlice", MUTEX);
	vs->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice", MUTEX);
	vs->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice", MUTEX);
	vs->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice", MUTEX);
	vs->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice", MUTEX);
	vs->PlugVisualPipe("albaVMESlicer", "albaPipeSurfaceSlice", MUTEX);
	vs->PlugVisualPipe("albaVMEMeter", "albaPipePolylineSlice", MUTEX);
	vs->PlugVisualPipe("albaVMEWrappedMeter", "albaPipePolylineSlice", MUTEX);
	vs->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");
	vs->PlugVisualPipe("albaVMESegmentationVolume", "albaPipeVolumeOrthoSlice", MUTEX);
	vs->SetCanSpin(false);

	m_ViewCTCompound->PlugChildView(vs);
	PlugChildView(m_ViewCTCompound);
}

//----------------------------------------------------------------------------
void albaViewRXCT::VmeShow(albaVME *vme, bool show)
{
	bool isVmeShowed = m_ChildViewList[0]->IsVmeShowed(vme);

	for (int i = 0; i < CT_COMPOUND_VIEW; i++)
		m_ChildViewList[i]->VmeShow(vme, show);

	//if (node->IsALBAType(albaVMEVolume))
	if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
	{
		if (show && !isVmeShowed)
		{
			double center[3], b[CT_CHILD_VIEWS_NUMBER], step;

			// set the range for every slider widget
			albaVMEOutputVolume *volumeOutput = albaVMEOutputVolume::SafeDownCast(vme->GetOutput());
			for (int childID = RX_FRONT_VIEW; childID < CT_COMPOUND_VIEW; childID++)
			{
				double advLow, advHigh;
				double range[2];
				if (volumeOutput->GetMaterial())
				{

					((albaViewRX *)m_ChildViewList[childID])->GetLutRange(range); //range of projected
					double volTableRange[2];
					vtkLookupTable *cl = volumeOutput->GetMaterial()->m_ColorLut;

					double volRange[2];
					volumeOutput->GetVTKData()->GetScalarRange(volRange);

					const double * tableRange = volumeOutput->GetMaterial()->GetTableRange();
					if (tableRange[1] < tableRange[0])
					{
						volTableRange[0] = volRange[0];
						volTableRange[1] = volRange[1];
					}
					else
					{
						volTableRange[0] = tableRange[0];
						volTableRange[1] = tableRange[1];
					}

					double proportionalConstant = ((range[1] - range[0]) / (volRange[1] - volRange[0]));
					double inverseProportionalConstant = 1.0 / proportionalConstant;

					//advLow = range[0] + ((range[1] - range[0])/(volTableRange[1] - volTableRange[0])) * (range[0] - volTableRange[0]);
					//advHigh = range[1] + ((range[1] - range[0])/(volTableRange[1] - volTableRange[0])) * (range[1] - volTableRange[1]);
					advLow = proportionalConstant * (volTableRange[0] - volRange[0] + inverseProportionalConstant * range[0]);
					advHigh = proportionalConstant * (volTableRange[1] - volRange[1] + inverseProportionalConstant * range[1]);

					((albaViewRX *)m_ChildViewList[childID])->SetLutRange(advLow, advHigh);
				}
				else
				{
					((albaViewRX *)m_ChildViewList[childID])->GetLutRange(range);
					advLow = range[0];
					advHigh = range[1];
				}

				m_LutSliders[childID]->SetRange(range[0], range[1]);
				m_LutSliders[childID]->SetSubRange(advLow, advHigh);

				((albaViewRX *)m_ChildViewList[childID])->SetLutRange(advLow, advHigh);
			}

			double sr[CT_COMPOUND_VIEW];

			// get the VTK volume
			vtkDataSet *data = vme->GetOutput()->GetVTKData();
			data->Update();
			data->GetCenter(center);
			data->GetScalarRange(sr);
			double totalSR[2];
			totalSR[0] = sr[0];
			totalSR[1] = sr[1];

			if (volumeOutput->GetMaterial())
			{
				const double * tableRange = volumeOutput->GetMaterial()->GetTableRange();

				if (tableRange[1] > tableRange[0])
				{
					sr[0] = tableRange[0];
					sr[1] = tableRange[1];
				}
			}

			// set the slider for the CT compound view
			m_LutSliders[CT_COMPOUND_VIEW]->SetRange(totalSR[0], totalSR[1]);
			m_LutSliders[CT_COMPOUND_VIEW]->SetSubRange(sr[0], sr[1]);

			// create a lookup table for CT views

			if (volumeOutput->GetMaterial()->m_ColorLut)
			{
				m_ColorLUT = volumeOutput->GetMaterial()->m_ColorLut;
				m_ColorLUT->SetRange(sr);
				m_ColorLUT->Build();
			}
			
			// gather data to initialize CT slices
			data->GetBounds(b);
			step = (b[5] - b[4]) / 7.0;
			for (int i = 0; i < CT_CHILD_VIEWS_NUMBER; i++)
			{
				center[2] = b[5] - step*(i + 1);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->InitializeSlice(center);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetTextColor(m_BorderColor[i]);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->VmeShow(vme, show);

				albaPipeVolumeOrthoSlice *p = NULL;
				// set pipe lookup table
				p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(vme));

				p->SetInterpolation(m_TrilinearInterpolationOn);

				//p->SetColorLookupTable(m_vtkLUT[CT_COMPOUND_VIEW]);
				p->SetColorLookupTable(m_ColorLUT);
				m_Pos[i] = b[5] - step*(i + 1);
			}
			m_CurrentVolume = vme;
			GizmoCreate();

			//BEGIN cycle for remove old surface and redraw the right slice

			albaVMEIterator *iter = vme->GetRoot()->NewIterator();
			for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				albaVMESurface *surface = albaVMESurface::SafeDownCast(vme);
				if (surface)
				{
					albaPipe *p = (m_ChildViewList[RX_FRONT_VIEW])->GetNodePipe(node);
					if (p)
					{
						this->VmeShow(surface, false);
						this->VmeShow(surface, true);
					}
				}
			}
			iter->Delete();
			//END cycle for remove old surface and redraw the right slice
		}
		else if (!show)
		{
			m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(vme, show);
			m_CurrentVolume = NULL;
			GizmoDelete();
		}
	}
	else
	{
		m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(vme, show);

		// showing a surface with the volume present already
		if (show && m_CurrentVolume)
		{
			// create the slice in every CT views
			albaVME *vmeSelected = this->GetSceneGraph()->GetSelectedVme();
			if (vmeSelected == vme)
			{
				m_Gui->Enable(ID_ALL_SURFACE, true);
				m_Gui->Enable(ID_BORDER_CHANGE, true);
				m_Gui->Enable(ID_ADJUST_SLICES, true);

				albaPipe *pipe = ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(vme);
				if (pipe)
				{
					m_Border = ((albaPipeSurfaceSlice *)pipe)->GetThickness();
				}
				else
					m_Border = 1;
			}
		}
		else if (!show)
		{
			albaVME *vmeSelected = this->GetSceneGraph()->GetSelectedVme();
			if (vmeSelected == vme)
			{
				m_Gui->Enable(ID_ALL_SURFACE, false);
				m_Gui->Enable(ID_BORDER_CHANGE, false);
				m_Gui->Enable(ID_ADJUST_SLICES, false);
			}
		}
	}

	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void albaViewRXCT::VmeRemove(albaVME *vme)
{
  if (m_CurrentVolume && vme == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    GizmoDelete();
  }
	Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewRXCT::VmeSelect(albaVME *vme, bool select)
{
	for (int i = 0; i < m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeSelect(vme, select);

	if (m_Gui)
	{
		albaPipe *p = ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(vme);
		if ((vme->IsA("albaVMESurface") || vme->IsA("albaVMESurfaceParametric") || vme->IsA("albaVMESlicer")) && select&&p)
		{
			m_Gui->Enable(ID_ALL_SURFACE, true);
			m_Gui->Enable(ID_BORDER_CHANGE, true);
			m_Gui->Enable(ID_ADJUST_SLICES, true);
		}
		else
		{
			m_Gui->Enable(ID_ALL_SURFACE, false);
			m_Gui->Enable(ID_BORDER_CHANGE, false);
			m_Gui->Enable(ID_ADJUST_SLICES, false);
		}
		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void albaViewRXCT::OnEventRangeModified(albaEventBase *alba_event)
{
  // is the volume visible?
  if(((albaViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
  {
    double low, hi;

    // from which lut slider the event is coming?
    if (alba_event->GetSender() == m_LutSliders[RX_FRONT_VIEW])
    {
      m_LutSliders[RX_FRONT_VIEW]->GetSubRange(&low,&hi);
      ((albaViewRX *)m_ChildViewList[RX_FRONT_VIEW])->SetLutRange(low,hi);
    }
    else if (alba_event->GetSender() == m_LutSliders[RX_SIDE_VIEW])
    {
      m_LutSliders[RX_SIDE_VIEW]->GetSubRange(&low,&hi);
      ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->SetLutRange(low,hi);
    }
    else if (alba_event->GetSender() == m_LutSliders[CT_COMPOUND_VIEW])
    {
      m_LutSliders[CT_COMPOUND_VIEW]->GetSubRange(&low,&hi);
      //m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(low,hi);
      m_ColorLUT->SetTableRange(low,hi);
    }

    CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::OnEventSnapModality()
{
  if(this->m_CurrentVolume!=NULL)
  {
    for(int i=0; i<6; i++)
    {
      if(m_Snap==1)
        m_GizmoSlice[i]->SetGizmoMovingModalityToSnap();
      else
        m_GizmoSlice[i]->SetGizmoMovingModalityToBound();
    }
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::OnEventSortSlices()
{
  albaVME* node=GetSceneGraph()->GetSelectedVme();
  albaPipe *p=((albaViewRX *)m_ChildViewList[0])->GetNodePipe(node);
  if(node->GetOutput()->IsA("albaVMEOutputVolume"))
    albaLogMessage("SURFACE NOT SELECTED");
  else  if (node->IsALBAType(albaVMESurface))
  {
    double center[3],b[6],step;
    albaVMESurface *surface=(albaVMESurface*)node;
    surface->GetOutput()->GetBounds(b);
    step = (b[5]-b[4])/7.0;
    center[0]=0;
    center[1]=0;
    for (int currChildCTView=0; currChildCTView < CT_CHILD_VIEWS_NUMBER; currChildCTView++)
    {
      if(m_GizmoSlice[currChildCTView])
      {
        center[2] = b[5]-step*(currChildCTView+1);
        center[2] = center[2] > b[5] ? b[5] : center[2];
        center[2] = center[2] < b[4] ? b[4] : center[2];
        m_GizmoSlice[currChildCTView]->UpdateGizmoSliceInLocalPositionOnAxis(currChildCTView,albaGizmoSlice::GIZMO_SLICE_Z,center[2]);
        m_Pos[currChildCTView]=center[2];
        m_Sort[currChildCTView]=currChildCTView;
        ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetSlice(center);
        ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetTextColor(m_BorderColor[currChildCTView]);
        ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->UpdateText();
        ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->BorderCreate(m_BorderColor[currChildCTView]);
        ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->CameraUpdate();
      }
    }
    m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
    m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
  }
	else if (node->IsALBAType(albaVMESurfaceParametric))
	{
		double center[3],b[6],step;
		albaVMESurfaceParametric *surface=(albaVMESurfaceParametric*)node;
		surface->GetOutput()->GetBounds(b);
		step = (b[5]-b[4])/7.0;
		center[0]=0;
		center[1]=0;
		for (int currChildCTView=0; currChildCTView < CT_CHILD_VIEWS_NUMBER; currChildCTView++)
		{
			if(m_GizmoSlice[currChildCTView])
			{
				center[2] = b[5]-step*(currChildCTView+1);
				center[2] = center[2] > b[5] ? b[5] : center[2];
				center[2] = center[2] < b[4] ? b[4] : center[2];
				m_GizmoSlice[currChildCTView]->UpdateGizmoSliceInLocalPositionOnAxis(currChildCTView,albaGizmoSlice::GIZMO_SLICE_Z,center[2]);
				m_Pos[currChildCTView]=center[2];
				m_Sort[currChildCTView]=currChildCTView;
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetSlice(center);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetTextColor(m_BorderColor[currChildCTView]);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->UpdateText();
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->BorderCreate(m_BorderColor[currChildCTView]);
				((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->CameraUpdate();
			}
		}
		m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
		m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaViewRXCT::OnEventSetThickness()
{
  if(m_AllSurface)
  {
    albaVME *vme=this->GetSceneGraph()->GetSelectedVme();
		albaVME *root=vme->GetRoot();
    SetThicknessForAllSurfaceSlices(root);
  }
  else
  {
    albaVME *node=this->GetSceneGraph()->GetSelectedVme();
    albaSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);
    albaPipe *p=((albaViewSlice *)((albaViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
    ((albaPipeSurfaceSlice *)p)->SetThickness(m_Border);

    if(albaVisualPipeSlicerSlice *pipe = albaVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[RX_FRONT_VIEW]->GetNodePipe(node)))
    {
      pipe->SetThickness(m_Border);
    }

    if(albaVisualPipeSlicerSlice *pipe = albaVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[RX_SIDE_VIEW]->GetNodePipe(node)))
    {
      pipe->SetThickness(m_Border);
    }
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::OnEventMouseMove( albaEvent *e )
{
  long movingSliceId;
  movingSliceId = e->GetArg();

  double newSliceLocalOrigin[3];
  vtkPoints *p = (vtkPoints *)e->GetVtkObj();
  if(p == NULL) {
    return;
  }
  p->GetPoint(0,newSliceLocalOrigin);
	BoundsValidate(newSliceLocalOrigin);
  if (m_MoveAllSlices)
  {
    double oldSliceLocalOrigin[3], delta[3], b[CT_CHILD_VIEWS_NUMBER];
    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(movingSliceId))->GetSlice(oldSliceLocalOrigin);
    delta[0] = newSliceLocalOrigin[0] - oldSliceLocalOrigin[0];
    delta[1] = newSliceLocalOrigin[1] - oldSliceLocalOrigin[1];
    delta[2] = newSliceLocalOrigin[2] - oldSliceLocalOrigin[2];

    for (int currSubView = 0; currSubView<((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetNumberOfSubView(); currSubView++)
    {
      m_CurrentVolume->GetOutput()->GetVMEBounds(b);

      int i=0;
      while (currSubView!=m_Sort[i]) i++;

      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->GetSlice(oldSliceLocalOrigin);
      newSliceLocalOrigin[0] = oldSliceLocalOrigin[0] + delta[0];
      newSliceLocalOrigin[1] = oldSliceLocalOrigin[1] + delta[1];
      newSliceLocalOrigin[2] = oldSliceLocalOrigin[2] + delta[2];
      newSliceLocalOrigin[2] = newSliceLocalOrigin[2] > b[5] ? b[5] : newSliceLocalOrigin[2];
      newSliceLocalOrigin[2] = newSliceLocalOrigin[2] < b[4] ? b[4] : newSliceLocalOrigin[2];
      m_GizmoSlice[currSubView]->UpdateGizmoSliceInLocalPositionOnAxis(currSubView,albaGizmoSlice::GIZMO_SLICE_Z,newSliceLocalOrigin[2]);

      m_Pos[currSubView]=newSliceLocalOrigin[2];

      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->SetSlice(newSliceLocalOrigin);
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->CameraUpdate();
    }
  }
  else
  {
    // move a single slice: this needs reordering
    m_Pos[movingSliceId]=newSliceLocalOrigin[2];
    SortSlices();
    int i=0;
    while (movingSliceId != m_Sort[i]) i++;

    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetSlice(newSliceLocalOrigin);
    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->CameraUpdate();
  }
  m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
  m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewRXCT::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(alba_event->GetId()) 
    { 
      // events from the slider
      case ID_RANGE_MODIFIED:
      {
        OnEventRangeModified(alba_event);
      }
      break;

      case ID_SNAP:
      {
        OnEventSnapModality();
      }
			break;

			case ID_SIDE:
			{
				if (m_Side == 0)
				((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->ShowSideVolume(ALL);
				else if (m_Side == 1)
				((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->ShowSideVolume(SIDE_LEFT);
				else if (m_Side == 2)
				((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->ShowSideVolume(SIDE_RIGHT);
			}
			break;
      
			case ID_RIGHT_OR_LEFT:
        {
          if (m_RightOrLeft==0)
          {
            ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_RIGHT);
          }
          else
            ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_LEFT);
        }
      break;

      case MOUSE_UP:
      case MOUSE_MOVE:
      {
        OnEventMouseMove(e);
      }
      break;

      case ID_ADJUST_SLICES:
        {
          OnEventSortSlices();
        }
        break;
        
      case ID_BORDER_CHANGE:
      {
        OnEventSetThickness();
      }
      break;

      case ID_RESET_SLICES:
        {
          assert(m_CurrentVolume);
          this->ResetSlicesPosition(m_CurrentVolume);
        }
        break;
      
      case ID_TRILINEAR_INTERPOLATION:
        {
          if (m_CurrentVolume)
          {
            for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
            {
              albaPipeVolumeOrthoSlice *p = NULL;
              p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetInterpolation(m_TrilinearInterpolationOn);
              }
            }
            albaPipeVolumeOrthoSlice *p = NULL;
            p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[RX_SIDE_VIEW]))->GetNodePipe(m_CurrentVolume));
            if (p)
            {
              p->SetInterpolation(m_TrilinearInterpolationOn);
            }
            p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[RX_FRONT_VIEW]))->GetNodePipe(m_CurrentVolume));
            if (p)
            {
              p->SetInterpolation(m_TrilinearInterpolationOn);
            }
            this->CameraUpdate();
          }

          for (int i=0;i<CT_CHILD_VIEWS_NUMBER;i++)
          {
            ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW]))->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
          }
        }
        break;
      case ID_ALL_SURFACE:
      {
        if(m_AllSurface)
        {
          albaVME *vme=GetSceneGraph()->GetSelectedVme();
          albaVME* root=vme->GetRoot();
          SetThicknessForAllSurfaceSlices(root);
        }
      }

      default:
      albaViewCompound::OnEvent(alba_event);
    }
  }
  else
  {
    albaViewCompound::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
void albaViewRXCT::OnSize(wxSizeEvent &size_event)
{
	albaViewCompound::OnSize(size_event);

	if (m_IsSubViewMaximized)
		for (int i = 0; i < CT_CHILD_VIEWS_NUMBER; i++)
		{
			((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetWindowSize(size_event.GetSize().GetX(), size_event.GetSize().GetY());
			((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderUpdate();
		}
}

//-------------------------------------------------------------------------
albaGUI* albaViewRXCT::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	wxString povChoices[2]{ "Right","Left" };
	m_Gui->Radio(ID_RIGHT_OR_LEFT, "POV", &m_RightOrLeft, 2, povChoices, 2, "Point Of View");

	wxString sideChoices[3]{ "All", "Left", "Right" };
	m_Gui->Radio(ID_SIDE, "Side", &m_Side, 3, sideChoices, 1, "Side");

  m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap,1);

  m_Gui->Bool(ID_MOVE_ALL_SLICES,"Move all",&m_MoveAllSlices,1);

  m_Gui->Button(ID_ADJUST_SLICES,"Adjust Slices");

  m_Gui->Divider(1);

  m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);

  albaVME* node=this->GetSceneGraph()->GetSelectedVme();
  if (node->IsA("albaVMESurface")||node->IsA("albaVMESurfaceParametric")||node->IsA("albaVMESlicer"))
  {
    m_Gui->Enable(ID_ALL_SURFACE,true);
    m_Gui->Enable(ID_BORDER_CHANGE,true);
    m_Gui->Enable(ID_ADJUST_SLICES,true);
  }
  else
  {
    m_Gui->Enable(ID_ALL_SURFACE,false);
    m_Gui->Enable(ID_BORDER_CHANGE,false);
    m_Gui->Enable(ID_ADJUST_SLICES,false);
  }

	for(int i=RX_FRONT_VIEW;i<=RX_SIDE_VIEW;i++)
		((albaViewRX *)m_ChildViewList[i]->GetGui());
	
	for(int i=0;i<=CT_CHILD_VIEWS_NUMBER;i++)
		(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetGui());

  m_Gui->Button(ID_RESET_SLICES,"Reset slices","");

  if (m_CurrentVolume)
  {
    for (int i=0; i<m_NumOfChildView; i++)
    {
      albaPipeVolumeOrthoSlice *p = NULL;
      p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
      if (p)
      {
        p->SetInterpolation(m_TrilinearInterpolationOn);
      }
    }
  }
  m_Gui->Divider(1);
  //m_Gui->Bool(ID_ENABLE_GPU,"Enable GPU",&m_EnableGPU,1);
  m_Gui->Bool(ID_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1);

  m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewRXCT::CreateGuiView()
{
  m_GuiView = new albaGUI(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  // create three windowing widgets
  for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
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
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewRXCT::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_LUT_WIDGET, enable);
		m_Gui->Enable(ID_RESET_SLICES, enable);

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void albaViewRXCT::LayoutSubView(int width, int height)
{
  // this implement the Fixed SubViews Layout
  int border = 2;
  int x_pos, c, i;

  int step_width  = (width-border) / 3;
  i = 0;
  for (c = 0; c < m_NumOfChildView; c++)
  {
    x_pos = c*(step_width + border);
    m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
    i++;
  }
  wxSize sizeToSend = m_ChildViewList[i-1]->GetWindow()->GetSize();
  wxSizeEvent event(sizeToSend);
  ((albaViewCompound *)m_ChildViewList[i-1])->OnSize(event);
  //((albaViewCompound *)m_ChildViewList[i-1])->OnLayout();

  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
  {
    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderUpdate();
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::MaximizeSubView(int subview_id, bool maximize)
{
	m_IsSubViewMaximized = maximize;

  albaViewCompound::MaximizeSubView(subview_id, maximize);
  for(int v=RX_FRONT_VIEW; v<VIEWS_NUMBER; v++)
  {
    if(v == subview_id || !maximize)
    {
       m_LutSliders[v]->Enable(true);
    }
    else
    {
       m_LutSliders[v]->Enable(false);
    }
  }

  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
  {
    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderUpdate();
  }

  m_GuiView->Update();
  
}

//----------------------------------------------------------------------------
void albaViewRXCT::GizmoCreate()
{
  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++) 
  {
    double slice[3],normal[3];
    albaPipeVolumeOrthoSlice *p = NULL;
    p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
    p->GetSlice(slice,normal);
    m_GizmoSlice[i] = new albaGizmoSlice(m_CurrentVolume, this);
    m_GizmoSlice[i]->CreateGizmoSliceInLocalPositionOnAxis(i,albaGizmoSlice::GIZMO_SLICE_Z,slice[2]);
    m_GizmoSlice[i]->SetColor(m_BorderColor[i]);
    ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderCreate(m_BorderColor[i]);

    m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(), true);
    m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(), true);
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::GizmoDelete()
{
  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
  {
    if(m_GizmoSlice[i])
    {
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderDelete();
      m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(),false);
      m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(),false);
      cppDEL(m_GizmoSlice[i]);
    }
  }
}
//----------------------------------------------------------------------------
void albaViewRXCT::SortSlices()
{
  bool modified = false;
  int i,j,t;
  //check if a ct view should change posistion
  for(j=0; j<CT_CHILD_VIEWS_NUMBER; j++)
  {
    for(i=j; i<CT_CHILD_VIEWS_NUMBER; i++)
    {
      if( m_Pos[m_Sort[j]] < m_Pos[m_Sort[i]])
      {
        t = m_Sort[j];
        m_Sort[j] = m_Sort[i];
        m_Sort[i] = t;
        modified=true; 
      }
    }
  }	

  if (modified)
  {
    double *OldPos;
    for(j=0;j<CT_CHILD_VIEWS_NUMBER; j++)
    {
      OldPos=((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->GetSliceOrigin();
      OldPos[2]=m_Pos[m_Sort[j]];
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->SetSlice(OldPos);
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->SetTextColor(m_BorderColor[m_Sort[j]]);
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->UpdateText();
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->BorderCreate(m_BorderColor[m_Sort[j]]);
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->CameraUpdate();
    }
  }

}

//----------------------------------------------------------------------------
void albaViewRXCT::SetThicknessForAllSurfaceSlices(albaVME *root)
{
  albaVMEIterator *iter = root->NewIterator();
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    if(node->IsA("albaVMESurface"))
    {
      albaPipe *p=((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(node);
      if(p)
        ((albaPipeSurfaceSlice *)p)->SetThickness(m_Border);
    }
  }
  iter->Delete();
}

//----------------------------------------------------------------------------
void albaViewRXCT::BoundsValidate(double *pos)
{
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
void albaViewRXCT::ResetSlicesPosition(albaVME *vme)
{
  // workaround... :(
  // maybe we need some mechanism to execute view code from op?
  this->VmeShow(vme, false);
  this->VmeShow(vme, true);
  CameraUpdate();
}
//----------------------------------------------------------------------------
bool albaViewRXCT::IsPickedSliceView()
{
  albaRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsALBAType(albaViewSlice))
      {
        if(((albaViewSlice *)m_ChildViewList[i])->GetRWI()==rwi)
          return true;
      }
      else if (m_ChildViewList[i]->IsALBAType(albaViewCompound))
      {
        if(((albaViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
          return true;
      }
      else if (((albaViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return false;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
char ** albaViewRXCT::GetIcon()
{
#include "pic/VIEW_RXCT.xpm"
	return VIEW_RXCT_xpm;
}