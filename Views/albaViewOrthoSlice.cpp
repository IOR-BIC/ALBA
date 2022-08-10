
/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi, Grazia Di Cosmo
 
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

#include "albaViewOrthoSlice.h"
#include "albaViewSlice.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutPreset.h"
#include "albaGUI.h"
#include "albaGUIFloatSlider.h"
#include "albaGUILutSlider.h"
#include "albaEventInteraction.h"
#include "albaVMEIterator.h"

#include "mmaVolumeMaterial.h"
#include "albaVMESurface.h"
#include "albaVMEVolume.h"
#include "albaVMEOutputVolume.h"
#include "albaIndent.h"
#include "albaGizmoSlice.h"
#include "albaVMEGizmo.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVisualPipeSlicerSlice.h"
#include "albaPipePolylineSlice.h"
#include "albaPipePolyline.h"
#include "albaDeviceButtonsPadMouse.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCamera.h"
#include "albaPipeMeshSlice.h"
#include "albaTransform.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewOrthoSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
albaViewOrthoSlice::albaViewOrthoSlice(wxString label)
: albaViewCompound(label, 2, 2)
//----------------------------------------------------------------------------
{
  m_LutSlider = NULL;
  m_LutWidget = NULL;
  for (int v=0;v<4;v++)
  {
    m_Views[v] = NULL;
  }
  m_ColorLUT= NULL;
  m_CurrentVolume = NULL;
  m_GizmoHandlePosition[0] = m_GizmoHandlePosition[1] = m_GizmoHandlePosition[2] = 0.0;

  for(int j=0; j<3; j++) 
  {
    m_Gizmo[j] = NULL;
  }

  m_Side = 0;
	m_Snap = 0;

	m_AllSurface=true;
	m_Border=1;
  m_PolylineRadiusSize=1;

  m_CanPlugVisualPipes=true;

  m_TrilinearInterpolationOn = TRUE;

	m_Root = NULL;

}
//----------------------------------------------------------------------------
albaViewOrthoSlice::~albaViewOrthoSlice()
//----------------------------------------------------------------------------
{  
  m_VMElist.clear();
}
//----------------------------------------------------------------------------
albaView *albaViewOrthoSlice::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewOrthoSlice *v = new albaViewOrthoSlice(m_Label);
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
void albaViewOrthoSlice::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	wxWindowDisabler wait1;
	wxBusyCursor wait2;

  // Disable Visual pipes plug at run time
  m_CanPlugVisualPipes=false;

	m_Root = vme->GetRoot();

  // Detect selected vme pos
  int pos=-1;
  for (int i=0; i<m_VMElist.size(); i++)
    if(vme==m_VMElist[i])
      pos=i;

  // if i want to show a vme that is not in list i add it to the list
	if (show && pos == -1)
		m_VMElist.push_back(vme);
	// else if i want to un-show a vme i remove it only if is in the list
	else if (!show && pos >= 0)
		m_VMElist.erase(m_VMElist.begin() + pos);
	// else the show status is unchanged just return
	else
		return;

	//first element showed -> create gizmo
	if (show && m_VMElist.size() == 1)
		CreateGizmo();
	
	if (m_VMElist.size() == 0)
		DestroyGizmo();

  // Enable perspective View for every VME
  m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
  // Disable ChildView XN, YN and ZN when no Volume is selected
  
	bool isVolume = vme->GetOutput()->IsA("albaVMEOutputVolume");
	if (isVolume)
	{
    for(int j=1; j<m_NumOfChildView; j++)
      m_ChildViewList[j]->VmeShow(vme, show);

		if (show)
		{
			m_CurrentVolume = vme;
      // Create Ortho Stuff
      CreateOrthoslicesAndGizmos(vme);
      
      // Defining radius of polylines at the equivalent side of the medium voxel
      double bounds[6], edges[3], vol, nPoints;
      vtkDataSet *volOutput;
      volOutput=vme->GetOutput()->GetVTKData();
      volOutput->GetBounds(bounds);
      nPoints=volOutput->GetNumberOfPoints();
      edges[0]=bounds[1]-bounds[0];
      edges[1]=bounds[3]-bounds[2];
      edges[2]=bounds[5]-bounds[4];
      vol=edges[0]*edges[1]*edges[2];
      m_PolylineRadiusSize=pow(vol/nPoints,1.0/3.0)/2.0;
		}
		else
		{
			m_CurrentVolume = NULL;
      DestroyOrthoSlicesAndGizmos();
    }

    // When one volume is selected/unselected we enable/disable ChildViews for all vme selected
		for (int i = 0; i < m_VMElist.size(); i++)
		{
			albaVME *vme = m_VMElist[i];

			for (int j = 1; j < m_NumOfChildView; j++)
			{
				m_ChildViewList[j]->VmeShow(vme, show);
				if (show)
					ApplySliceSetting(j, vme);

			}
			ApplyViewSettings(vme);
		}
	}
	else
	{
		for (int j = 1; j < m_NumOfChildView; j++)
		{
			m_ChildViewList[j]->VmeShow(vme, show);
			if (show)
				ApplySliceSetting(j, vme);
		}
		ApplyViewSettings(vme);
	}

	UpdateGizmoBounds(show); 

	//CameraUpdate();
	EnableWidgets(m_CurrentVolume != NULL);

	if (isVolume)
		CameraReset();
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::ApplySliceSetting(int view, albaVME * vme)
{
	albaPipe* nodePipe = m_ChildViewList[view]->GetNodePipe(vme);
	albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(nodePipe);
	albaPipeSurfaceSlice* surfacePipe = albaPipeSurfaceSlice::SafeDownCast(nodePipe);

	if (surfacePipe)
		surfacePipe->SetThickness(m_Border);

	if (meshPipe)
		meshPipe->SetThickness(m_Border);
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && vme == m_CurrentVolume) 
  {
    // Disable ChildViews
    for(int j=1; j<m_NumOfChildView; j++) 
      m_ChildViewList[j]->VmeShow(vme, false);
    DestroyOrthoSlicesAndGizmos();
    EnableWidgets(false);
  }
  // Remove node from list
  int pos=-1;
  for (int i=0; i<m_VMElist.size(); i++)
    if(vme==m_VMElist[i])
      pos=i;
  if (pos>=0)
    m_VMElist.erase(m_VMElist.begin()+pos);

  Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new albaGUI(this);

  m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void albaViewOrthoSlice::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
  		case ID_BORDER_CHANGE:
			{
				OnEventSetThickness();
			}
			break;
			case ID_ALL_SURFACE:
			{
				if(m_AllSurface)
				{
					albaVME* vme=GetSceneGraph()->GetSelectedVme();
					SetThicknessForAllSurfaceSlices(m_Root);
				}
			}
			break;
      case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *currentVolumeMaterial = ((albaVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
        currentVolumeMaterial->UpdateFromTables();
        for(int i=0; i<m_NumOfChildView; i++)
        {
          albaPipeVolumeOrthoSlice *p = (albaPipeVolumeOrthoSlice *)((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
          p->SetColorLookupTable(m_ColorLUT);
        }
        double *sr;
        sr = m_ColorLUT->GetRange();
        m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
        CameraUpdate();
      }
      break;
      case ID_RANGE_MODIFIED:
      {
        if(((albaViewSlice *)m_ChildViewList[0])->VolumeIsVisible())
        {
          double low, hi;
          m_LutSlider->GetSubRange(&low,&hi);
          m_ColorLUT->SetTableRange(low,hi);
          mmaVolumeMaterial *currentVolumeMaterial = ((albaVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
          currentVolumeMaterial->UpdateFromTables();
          CameraUpdate();
        }
      }
      break;
      case MOUSE_UP:
      case MOUSE_MOVE:
      {
        // get the gizmo that is being moved
        long gizmoId = e->GetArg();
        vtkPoints *p = (vtkPoints *)e->GetVtkObj();
        if(p == NULL) return;
        this->SetSlicePosition(gizmoId, p);
      }
      break;
			case ID_SNAP:
			{
				if(this->m_CurrentVolume==NULL && m_Snap)
				{
					wxMessageBox("You can't switch to snap modality!");
					m_Snap=0;
					m_Gui->Update();
				}
				else
				{
					for(int i=GIZMO_XN; i<GIZMOS_NUMBER; i++)
					{
						if(m_Snap==1)
							m_Gizmo[i]->SetGizmoMovingModalityToSnap();
						else
							m_Gizmo[i]->SetGizmoMovingModalityToBound();
					}
				}
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
            for(int i=0; i<m_NumOfChildView; i++)
            {
              albaPipeVolumeOrthoSlice *p = NULL;
              p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetInterpolation(m_TrilinearInterpolationOn);
              }
            }
            this->CameraUpdate();
          }
        }
        break;
      default:
        albaViewCompound::OnEvent(alba_event);
    }
  }
}
//-------------------------------------------------------------------------
albaGUI* albaViewOrthoSlice::CreateGui()
//-------------------------------------------------------------------------
{
  wxString layout_choices[3] = {"Default","Layout 1","Layout 2"};

  assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

  m_Gui->Combo(ID_LAYOUT_CHOOSER,"Layout",&m_LayoutConfiguration,3,layout_choices);
  m_Gui->Divider();
  m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
  m_Gui->Divider(2);

	wxString sidesName[2]{ "Left", "Right" };
  // m_Gui->Radio(ID_SIDE_ORTHO, "side", &m_Side, 2, sidesName, 2);

	m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap,1);

  m_Gui->Button(ID_RESET_SLICES,"Reset slices","");
  m_Gui->Divider();

	m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
	m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);

  EnableWidgets(m_CurrentVolume != NULL);
  for(int i=1; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->GetGui();
  }

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
  m_Gui->Bool(ID_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1);

  m_Gui->Divider();
  return m_Gui;
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::PlugVisualPipeInSliceViews(albaString vme_type, albaString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  if (m_CanPlugVisualPipes)
  {
    for(int v=PERSPECTIVE_VIEW; v<VIEWS_NUMBER; v++)
      if (v != PERSPECTIVE_VIEW && m_Views[v]!=NULL)
        m_Views[v]->PlugVisualPipe(vme_type, pipe_type,MUTEX);
       
  }
  else
  {
    albaLogMessage("OthoSlice Error: You cannot plug visual pipes after visualization operation");
  }
}

//----------------------------------------------------------------------------
char ** albaViewOrthoSlice::GetIcon()
{
#include "pic/VIEW_ORTHO.xpm"
	return VIEW_ORTHO_xpm;
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::PlugVisualPipeInPerspective(albaString vme_type, albaString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
      if (m_CanPlugVisualPipes)
      {
         if (m_Views[PERSPECTIVE_VIEW]!=NULL)
            m_Views[PERSPECTIVE_VIEW]->PlugVisualPipe(vme_type, pipe_type ,MUTEX);

      }
      else
      {
        albaLogMessage("OthoSlice Error: You cannot plug visual pipes after visualization operation");
      }
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[4] = {CAMERA_OS_P, CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z};
  
  wxString viewName[4] = {"perspective","camera x","camera y","camera z"};

	bool TICKs[4]={false,false,true,true};
  for(int v=PERSPECTIVE_VIEW; v<VIEWS_NUMBER; v++)
  {
    m_Views[v] = new albaViewSlice(viewName[v], cam_pos[v],false,false,false,0,TICKs[v]);
    m_Views[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeOrthoSlice", MUTEX);    
    m_Views[v]->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeOrthoSlice", MUTEX);
		m_Views[v]->PlugVisualPipe("albaVMEImage", "albaPipeBox", NON_VISIBLE);
    m_Views[v]->PlugVisualPipe("albaVMESegmentationVolume", "albaPipeVolumeOrthoSlice", MUTEX);
    // plug surface slice visual pipe in not perspective views
    if (v != PERSPECTIVE_VIEW)
    {
      m_Views[v]->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
      m_Views[v]->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("albaVMEPolyline", "albaPipePolylineSlice");
      m_Views[v]->PlugVisualPipe("albaVMEPolylineSpline", "albaPipePolylineSlice");
      m_Views[v]->PlugVisualPipe("albaVMEMeter", "albaPipePolyline");
			m_Views[v]->PlugVisualPipe("medVMEMuscleWrapper", "albaPipeSurfaceSlice",MUTEX);
			m_Views[v]->PlugVisualPipe("albaVMEPointCloud", "albaPipeBox", NON_VISIBLE);
			m_Views[v]->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");

    }
    else
    {
      m_Views[v]->PlugVisualPipe("albaVMESurface", "albaPipeSurface",MUTEX);
			m_Views[v]->PlugVisualPipe("medVMEMuscleWrapper", "albaPipeSurface",MUTEX);
    }
  }
  PlugChildView(m_Views[PERSPECTIVE_VIEW]);
  PlugChildView(m_Views[ZN_VIEW]);
  PlugChildView(m_Views[XN_VIEW]);
  PlugChildView(m_Views[YN_VIEW]);
}
//----------------------------------------------------------------------------
void albaViewOrthoSlice::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_CHOOSER,enable);
    m_Gui->Enable(ID_RESET_SLICES, enable);
  }
  m_LutSlider->Enable(enable);

}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::CreateGizmo()
{
	if (m_Gizmo[0] || m_Gizmo[1] || m_Gizmo[2]) DestroyGizmo();
	double direction[] = { albaGizmoSlice::GIZMO_SLICE_X,albaGizmoSlice::GIZMO_SLICE_Y,albaGizmoSlice::GIZMO_SLICE_Z };

	// creates the gizmos
	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		double sliceOrigin[3] = { 0, 0, 0 };

		if (m_CurrentVolume)
		{
			albaPipeVolumeOrthoSlice *p = NULL;
			p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[0]))->GetNodePipe(m_CurrentVolume));
			double normal[3];
			p->GetSlice(sliceOrigin, normal);
		}

		m_Gizmo[gizmoId] = new albaGizmoSlice(m_Root, this);
		m_Gizmo[gizmoId]->UpdateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId], sliceOrigin[gizmoId]);
	}

	GizmoShow();
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::GizmoShow()
{
	if (m_Gizmo[0] == NULL)
		return;

	double colors[] = { 1,0,0,  0,1,0,  0,0,1 };
	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_Gizmo[gizmoId]->SetGizmoMovingModalityToBound();
		m_Gizmo[gizmoId]->SetColor(&colors[gizmoId * 3]);
	}
	// put them in the right views:
	// perspective view
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);

	// ZN view
	m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
	m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);

	// YN view
	m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
	m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);

	// ZN view
	m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);
	m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::DestroyGizmo()
//----------------------------------------------------------------------------
{
  // set gizmos visibility to false
  // perspective view
	for(int i=0; i<3; i++)
	{
		if(!m_Gizmo[i]) return;
	}
  m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
  m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);
  m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);

  // ZN view
  m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
  m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);

  // YN view
  m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
  m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);

  // XN view
  m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);
  m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);
  for(int i=0; i<3; i++)
  {
    cppDEL(m_Gizmo[i]);
  }
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::SetSlicePosition(long activeGizmoId, vtkPoints *p)
//----------------------------------------------------------------------------
{
  // gizmos update correctly in every views so this method is needed to update slice also
  /*  */
  
  // always update the perspective view

  albaVME *g[3];
  double pos[3], orient[3];

  p->GetPoint(0,m_GizmoHandlePosition);
  

  switch(activeGizmoId)
  {
    case (GIZMO_XN)	:
    {
      // update the X normal child view
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetSlice(m_GizmoHandlePosition);
    }
    break;
    case (GIZMO_YN)	:
    {
      // update the Y normal child view
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetSlice(m_GizmoHandlePosition);    
    }
    break;
    case (GIZMO_ZN)	:
    {
      // update the Z normal child view
      ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetSlice(m_GizmoHandlePosition);
    }
    break;
  }

  for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
  {
    // get the moved gizmo pose
    if (gizmoId == activeGizmoId) continue;

    g[gizmoId] = this->m_Gizmo[gizmoId]->GetOutput();
    g[gizmoId]->GetOutput()->GetPose(pos,orient);
    m_GizmoHandlePosition[gizmoId] = (double)pos[gizmoId];
  }

  // always update the child perspective view
  ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_PERSPECTIVE_VIEW]))->SetSlice(m_GizmoHandlePosition);
  

  this->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewOrthoSlice::OnEventSetThickness()
//----------------------------------------------------------------------------
{
	if(m_AllSurface)
	{
		albaVME* vme=this->GetSceneGraph()->GetSelectedVme();
		SetThicknessForAllSurfaceSlices(m_Root);
	}
	else
	{
		albaVME *node=this->GetSceneGraph()->GetSelectedVme();
		albaSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);

		if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}

		if(albaVisualPipeSlicerSlice *pipe = albaVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(albaVisualPipeSlicerSlice *pipe = albaVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(albaVisualPipeSlicerSlice *pipe = albaVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
	}
}
//-------------------------------------------------------------------------
void albaViewOrthoSlice::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  os << indent << "albaViewOrthoSlice" << '\t' << this << std::endl;
  
  //print components view information

  for(int v=PERSPECTIVE_VIEW; v<VIEWS_NUMBER; v++)
  {
    m_ChildViewList[v]->Print(os, 1);
  }
}
//-------------------------------------------------------------------------
void albaViewOrthoSlice::CreateOrthoslicesAndGizmos(albaVME *vme)
//-------------------------------------------------------------------------
{

	double colorsX[]    = {1,0,0};
  double colorsY[]    = {0,1,0};
  double colorsZ[]    = {0,0,1};
	mmaVolumeMaterial *currentVolumeMaterial = ((albaVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
	double sr[2],vtkDataCenter[3];
	vtkDataSet *vtkData = m_CurrentVolume->GetOutput()->GetVTKData();
	vtkData->Update();
	vtkData->GetCenter(vtkDataCenter);
	vtkData->GetCenter(m_GizmoHandlePosition);
	vtkData->GetScalarRange(sr);
	m_ColorLUT = currentVolumeMaterial->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentVolumeMaterial->GetTableRange()[0],(long)currentVolumeMaterial->GetTableRange()[1]);
	for(int i=0; i<m_NumOfChildView; i++)
	{
		albaPipeVolumeOrthoSlice *p = (albaPipeVolumeOrthoSlice *)((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
    p->SetInterpolation(m_TrilinearInterpolationOn);
		p->SetColorLookupTable(m_ColorLUT);
	}

	albaMatrix rot;

	albaTransform::CopyRotation(*m_CurrentVolume->GetOutput()->GetAbsMatrix(), rot);

	double xNormal[4] = { 1, 0, 0, 1 };
	double yNormal[4] = { 0, 1, 0, 1 };
	double zNormal[4] = { 0, 0, 1, 1 };
	double normal[4];

	rot.MultiplyPoint(xNormal,normal);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetSlice(m_GizmoHandlePosition,normal);
  ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetTextColor(colorsX);

	rot.MultiplyPoint(yNormal, normal);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetSlice(m_GizmoHandlePosition, normal);
  ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetTextColor(colorsY);

	rot.MultiplyPoint(zNormal, normal);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetSlice(m_GizmoHandlePosition, normal);
  ((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetTextColor(colorsZ);

	// creates the gizmos
	double direction[] = { albaGizmoSlice::GIZMO_SLICE_X,albaGizmoSlice::GIZMO_SLICE_Y,albaGizmoSlice::GIZMO_SLICE_Z };

	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		double sliceOrigin[3] = { 0, 0, 0 };

		if (m_CurrentVolume)
		{
			albaPipeVolumeOrthoSlice *p = NULL;
			p = albaPipeVolumeOrthoSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[0]))->GetNodePipe(m_CurrentVolume));
			double normal[3];
			p->GetSlice(sliceOrigin, normal);
		}

		m_Gizmo[gizmoId]->SetInput(m_CurrentVolume);
		m_Gizmo[gizmoId]->UpdateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId], sliceOrigin[gizmoId]);
	}
	GizmoShow();
}
//-------------------------------------------------------------------------
void albaViewOrthoSlice::DestroyOrthoSlicesAndGizmos()
//-------------------------------------------------------------------------
{
	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
		if(m_Gizmo[gizmoId])
			m_Gizmo[gizmoId]->SetInput(m_Root);

	GizmoShow();
}
//-------------------------------------------------------------------------
void albaViewOrthoSlice::ResetSlicesPosition(albaVME *vme)
//---------------------	----------------------------------------------------
{
  // workaround... :(
  // maybe we need some mechanism to execute view code from op?
  this->VmeShow(vme, false);
  this->VmeShow(vme, true);
  CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewOrthoSlice::SetThicknessForAllSurfaceSlices(albaVME *root)
//----------------------------------------------------------------------------
{
	albaVMEIterator *iter = root->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		if (node->GetOutput()->IsA("albaVMEOutputSurface")) //if(node->IsA("albaVMESurface"))
		{
			if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
			if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
			if(albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
		}
	}
	iter->Delete();
}
//----------------------------------------------------------------------------
bool albaViewOrthoSlice::IsPickedSliceView()
//----------------------------------------------------------------------------
{
  albaRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsALBAType(albaViewSlice))
      {
        if(((albaViewSlice *)m_ChildViewList[i])->GetRWI()==rwi && ((albaViewSlice *)m_ChildViewList[i])->GetRWI()->GetCamera()->GetParallelProjection())
          return true;
      }
      else if (m_ChildViewList[i]->IsALBAType(albaViewCompound))
      {
        if(((albaViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
          return false;
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
void albaViewOrthoSlice::ApplyViewSettings(albaVME *vme)
//----------------------------------------------------------------------------
{
  if(vme->GetOutput()->IsA("albaVMEOutputPolyline"))  
  {
    for (int i=CHILD_ZN_VIEW;i<=CHILD_YN_VIEW;i++)
    {
      albaPipePolylineSlice *pipeSlice = albaPipePolylineSlice::SafeDownCast(((albaViewSlice *)((albaViewCompound *)m_ChildViewList[i]))->GetNodePipe(vme));
      if(pipeSlice) 
      {
        if(!vme->IsA("albaVMEMeter"))
          pipeSlice->SetRadius(m_PolylineRadiusSize);
        pipeSlice->FillOn(); 
      }
    }
  }
}

//----------------------------------------------------------------------------
void albaViewOrthoSlice::UpdateGizmoBounds(bool show)
{
	//if exists a volume or there are no Gizmos we don't need to update bounds.
	if (m_CurrentVolume || m_Gizmo[0] == NULL)
		return;

	double minVert[3] = { VTK_DOUBLE_MAX,VTK_DOUBLE_MAX,VTK_DOUBLE_MAX }, maxVert[3] = { VTK_DOUBLE_MIN,VTK_DOUBLE_MIN,VTK_DOUBLE_MIN };

	for (int i = 0; i < m_VMElist.size(); i++)
	{
		albaVMEOutput * vmeOutput = m_VMElist[i]->GetOutput();
		albaMatrix mtr = *vmeOutput->GetAbsMatrix();
		double *localBounds = vmeOutput->GetVTKData()->GetBounds();
	
		double locMinVert[4] = { localBounds[0],localBounds[2],localBounds[4],1 }, locMaxVert[4] = { localBounds[1],localBounds[3],localBounds[5],1};
		double globMinVert[4], globMaxVert[4];

		mtr.MultiplyPoint(locMinVert, globMinVert);
		mtr.MultiplyPoint(locMaxVert, globMaxVert);

		for (int i = 0; i < 3; i++)
		{
			minVert[i] = MIN(minVert[i], globMinVert[i]);
			maxVert[i] = MAX(maxVert[i], globMaxVert[i]);
		}
	}

	double gizmoBounds[6] = { minVert[0],maxVert[0],minVert[1],maxVert[1],minVert[2],maxVert[2] };


	double direction[] = { albaGizmoSlice::GIZMO_SLICE_X,albaGizmoSlice::GIZMO_SLICE_Y,albaGizmoSlice::GIZMO_SLICE_Z };


	bool firstVMEshowed = show && m_VMElist.size() == 1;
	if(!firstVMEshowed)
		m_Gizmo[0]->GetPosition(m_GizmoHandlePosition);

	if (firstVMEshowed || m_GizmoHandlePosition[0]<gizmoBounds[0] || m_GizmoHandlePosition[0]>gizmoBounds[1])
		m_GizmoHandlePosition[0] = (gizmoBounds[0] + gizmoBounds[1]) / 2;

	if (firstVMEshowed || m_GizmoHandlePosition[1]<gizmoBounds[2] || m_GizmoHandlePosition[1]>gizmoBounds[3])
		m_GizmoHandlePosition[1] = (gizmoBounds[2] + gizmoBounds[3]) / 2.0;
	
	if (firstVMEshowed ||m_GizmoHandlePosition[2]<gizmoBounds[4] || m_GizmoHandlePosition[4]>gizmoBounds[4])
		m_GizmoHandlePosition[2] = (gizmoBounds[4] + gizmoBounds[5]) / 2.0;

	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		m_Gizmo[gizmoId]->SetBounds(gizmoBounds);
		m_Gizmo[gizmoId]->UpdateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId], m_GizmoHandlePosition[gizmoId]);
	}

	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetSlice(m_GizmoHandlePosition);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetSlice(m_GizmoHandlePosition);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetSlice(m_GizmoHandlePosition);
	((albaViewSlice *)((albaViewCompound *)m_ChildViewList[CHILD_PERSPECTIVE_VIEW]))->SetSlice(m_GizmoHandlePosition);

	if (firstVMEshowed)
		CameraReset();
	else
		CameraUpdate();
}
