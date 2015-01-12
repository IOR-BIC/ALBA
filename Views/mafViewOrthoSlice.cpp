/*=========================================================================

 Program: MAF2Medical
 Module: mafViewOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi, Grazia Di Cosmo
 
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

#include "mafViewOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice_BES.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUI.h"
#include "mafGUIFloatSlider.h"
#include "mafGUILutSlider.h"
#include "mafEventInteraction.h"
#include "mafEventSource.h"
#include "mafNodeIterator.h"

#include "mmaVolumeMaterial.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafIndent.h"
#include "mafGizmoSlice.h"
#include "mafVMEGizmo.h"
#include "mafPipeSurfaceSlice.h"
#include "medVisualPipeSlicerSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipePolyline.h"
#include "mafDeviceButtonsPadMouse.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCamera.h"
#include "mafPipeMeshSlice.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewOrthoSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
mafViewOrthoSlice::mafViewOrthoSlice(wxString label)
: mafViewCompound(label, 2, 2)
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

	m_AllSurface=0;
	m_Border=1;
  m_PolylineRadiusSize=1;

  m_CanPlugVisualPipes=true;

  // Added by Losi 11.25.2009
  m_EnableGPU=FALSE;
  m_TrilinearInterpolationOn = TRUE;

}
//----------------------------------------------------------------------------
mafViewOrthoSlice::~mafViewOrthoSlice()
//----------------------------------------------------------------------------
{  
  m_VMElist.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewOrthoSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewOrthoSlice *v = new mafViewOrthoSlice(m_Label);
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
void mafViewOrthoSlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	wxWindowDisabler wait1;
	wxBusyCursor wait2;

  // Disable Visual pipes plug at run time
  m_CanPlugVisualPipes=false;

  // Detect selected vme pos
  int pos=-1;
  for (int i=0; i<m_VMElist.size(); i++)
    if(node==m_VMElist[i])
      pos=i;

  // if i want to show a vme that is not in list i add it to the list
  if (show && pos == -1)
    m_VMElist.push_back(node);
  // else if i want to un-show a vme i remove it only if is in the list
  else if (!show && pos>=0)
    m_VMElist.erase(m_VMElist.begin()+pos);
  


  // Enable perspective View for every VME
  m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
  // Disable ChildView XN, YN and ZN when no Volume is selected
  if (m_CurrentVolume != NULL)
 	  for(int j=1; j<m_NumOfChildView; j++) 
		{
      m_ChildViewList[j]->VmeShow(node, show);
			if(show && j==YN_VIEW && mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(node)))
				mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(node))->SetFlipNormalOff();
		}
  
	if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
	{
    for(int j=1; j<m_NumOfChildView; j++)
      m_ChildViewList[j]->VmeShow(node, show);

		if (show)
		{
      // Create Ortho Stuff
      CreateOrthoslicesAndGizmos(node);
      
      // Definig radius of polylines at the equivalent side of the medium voxel
      double bounds[6], edges[3], vol, nPoints;
      vtkDataSet *volOutput;
      volOutput=((mafVME *)node)->GetOutput()->GetVTKData();
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
      DestroyOrthoSlicesAndGizmos();
    }

    // When one volume is selected/unselected we enable/disable ChildViews for all vme selected
    for (int i=0;i<m_VMElist.size();i++)
      for(int j=1; j<m_NumOfChildView; j++)
      {
        m_ChildViewList[j]->VmeShow(m_VMElist[i], show);
				if(show && j==YN_VIEW && mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(m_VMElist[i])))
					 mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(m_VMElist[i]))->SetFlipNormalOff();
        ApplyViewSettings(m_VMElist[i]);
      }
	}
  else if (show)
    ApplyViewSettings(node);

	//CameraUpdate();
	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    // Disable ChildViews
    for(int j=1; j<m_NumOfChildView; j++) 
      m_ChildViewList[j]->VmeShow(node, false);
    DestroyOrthoSlicesAndGizmos();
    EnableWidgets(false);
  }
  // Remove node from list
  int pos=-1;
  for (int i=0; i<m_VMElist.size(); i++)
    if(node==m_VMElist[i])
      pos=i;
  if (pos>=0)
    m_VMElist.erase(m_VMElist.begin()+pos);

  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mafGUI(this);
  m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
  /*    case ID_SIDE_ORTHO:
      {          
        if (m_Side == 1)
        {
          ((mafViewSlice *)m_ChildViewList[CHILD_XN_VIEW])->CameraSet(CAMERA_RX_RIGHT);
          ((mafViewSlice*)m_ChildViewList[CHILD_XN_VIEW])->CameraUpdate();
        }
        else
        {
          ((mafViewSlice *)m_ChildViewList[CHILD_XN_VIEW])->CameraSet(CAMERA_RX_LEFT);
          ((mafViewSlice*)m_ChildViewList[CHILD_XN_VIEW])->CameraUpdate();
        }
      }
      break;*/
			case ID_BORDER_CHANGE:
			{
				OnEventSetThickness();
			}
			break;
			case ID_ALL_SURFACE:
			{
				if(m_AllSurface)
				{
					mafNode* node=GetSceneGraph()->GetSelectedVme();
					mafVME* vme=(mafVME*)node;
					mafNode* root=vme->GetRoot();
					SetThicknessForAllSurfaceSlices(root);
				}
			}
			break;
      case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
        currentVolumeMaterial->UpdateFromTables();
        for(int i=0; i<m_NumOfChildView; i++)
        {
          mafPipeVolumeSlice_BES *p = (mafPipeVolumeSlice_BES *)((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
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
        if(((mafViewSlice *)m_ChildViewList[0])->VolumeIsVisible())
        {
          double low, hi;
          m_LutSlider->GetSubRange(&low,&hi);
          m_ColorLUT->SetTableRange(low,hi);
          mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
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
        double pos[3];
        vtkPoints *p = (vtkPoints *)e->GetVtkObj();
        if(p == NULL) return;
        p->GetPoint(0,pos);
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
      // Added by Losi 11.25.2009
      case ID_ENABLE_GPU:
        {
          if (m_CurrentVolume)
          {
            for(int i=0; i<m_NumOfChildView; i++)
            {
              mafPipeVolumeSlice_BES *p = NULL;
              p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetEnableGPU(m_EnableGPU);
              }
            }
            this->CameraUpdate();
          }
        }
        break;
      case ID_TRILINEAR_INTERPOLATION:
        {
          if (m_CurrentVolume)
          {
            for(int i=0; i<m_NumOfChildView; i++)
            {
              mafPipeVolumeSlice_BES *p = NULL;
              p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
              }
            }
            this->CameraUpdate();
          }
        }
        break;
      default:
        mafViewCompound::OnEvent(maf_event);
    }
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewOrthoSlice::CreateGui()
//-------------------------------------------------------------------------
{
  wxString layout_choices[3] = {"default","layout 1","layout 2"};

  assert(m_Gui == NULL);

  mafView::CreateGui();

  m_Gui->Combo(ID_LAYOUT_CHOOSER,"layout",&m_LayoutConfiguration,3,layout_choices);
  m_Gui->Divider();
  m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  m_Gui->Divider(2);

  wxString sidesName[2];
  sidesName[0] = "left";
  sidesName[1] = "right";
  // m_Gui->Radio(ID_SIDE_ORTHO, "side", &m_Side, 2, sidesName, 2);

	m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap,1);

  m_Gui->Button(ID_RESET_SLICES,"reset slices","");
  m_Gui->Divider();

	m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
	m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);

  EnableWidgets(m_CurrentVolume != NULL);
  for(int i=1; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->GetGui();
  }

  // Added by Losi 11.25.2009
  if (m_CurrentVolume)
  {
    for (int i=0; i<m_NumOfChildView; i++)
    {
      mafPipeVolumeSlice_BES *p = NULL;
      p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
      if (p)
      {
        p->SetEnableGPU(m_EnableGPU);
        p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
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
void mafViewOrthoSlice::PlugVisualPipeInSliceViews(mafString vme_type, mafString pipe_type, long visibility)
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
    mafLogMessage("OthoSlice Error: You cannot plug visual pipes after visualization operation");
  }
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::PlugVisualPipeInPerspective(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
      if (m_CanPlugVisualPipes)
      {
         if (m_Views[PERSPECTIVE_VIEW]!=NULL)
            m_Views[PERSPECTIVE_VIEW]->PlugVisualPipe(vme_type, pipe_type ,MUTEX);

      }
      else
      {
        mafLogMessage("OthoSlice Error: You cannot plug visual pipes after visualization operation");
      }
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[4] = {CAMERA_OS_P, CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z};
  
  wxString viewName[4] = {"perspective","camera x","camera y","camera z"};

	bool TICKs[4]={false,false,true,true};
  for(int v=PERSPECTIVE_VIEW; v<VIEWS_NUMBER; v++)
  {
    m_Views[v] = new mafViewSlice(viewName[v], cam_pos[v],false,false,false,0,TICKs[v]);
    m_Views[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice_BES", MUTEX);    
    m_Views[v]->PlugVisualPipe("medVMELabeledVolume", "mafPipeVolumeSlice_BES", MUTEX);
    m_Views[v]->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeVolumeSlice_BES", MUTEX);   //BES: 3.11.2009
		m_Views[v]->PlugVisualPipe("mafVMEImage", "mafPipeBox", NON_VISIBLE);
    m_Views[v]->PlugVisualPipe("medVMESegmentationVolume", "mafPipeVolumeSlice_BES", MUTEX);
    // plug surface slice visual pipe in not perspective views
    if (v != PERSPECTIVE_VIEW)
    {
      m_Views[v]->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice");
      m_Views[v]->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice",MUTEX);
      m_Views[v]->PlugVisualPipe("mafVMEPolyline", "mafPipePolylineSlice");
      m_Views[v]->PlugVisualPipe("mafVMEPolylineSpline", "mafPipePolylineSlice");
      m_Views[v]->PlugVisualPipe("mafVMEMeter", "mafPipePolyline");
			m_Views[v]->PlugVisualPipe("medVMEMuscleWrapper", "mafPipeSurfaceSlice",MUTEX);
    }
    else
    {
      m_Views[v]->PlugVisualPipe("mafVMESurface", "mafPipeSurface",MUTEX);
			m_Views[v]->PlugVisualPipe("medVMEMuscleWrapper", "mafPipeSurface",MUTEX);
    }
		
  }
  PlugChildView(m_Views[PERSPECTIVE_VIEW]);
  PlugChildView(m_Views[ZN_VIEW]);
  PlugChildView(m_Views[XN_VIEW]);
  PlugChildView(m_Views[YN_VIEW]);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::EnableWidgets(bool enable)
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
void mafViewOrthoSlice::GizmoCreate()
//----------------------------------------------------------------------------
{
  if( m_Gizmo[0] || m_Gizmo[1] || m_Gizmo[2] ) GizmoDelete();

	if(m_CurrentVolume)
	{
		int gizmoId;
		double colors[]    = {1,0,0,  0,1,0,  0,0,1};
		double direction[] = {mafGizmoSlice::GIZMO_SLICE_X,mafGizmoSlice::GIZMO_SLICE_Y,mafGizmoSlice::GIZMO_SLICE_Z};

		// creates the gizmos
		for(gizmoId=GIZMO_XN; gizmoId<GIZMOS_NUMBER; gizmoId++) 
		{
			double sliceOrigin[3];
			mafPipeVolumeSlice_BES *p = NULL;
			p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[0]))->GetNodePipe(m_CurrentVolume));
      double normal[3];
			p->GetSlice(sliceOrigin,normal);

			m_Gizmo[gizmoId] = new mafGizmoSlice(m_CurrentVolume, this);
			m_Gizmo[gizmoId]->CreateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId], sliceOrigin[gizmoId]);
			m_Gizmo[gizmoId]->SetColor(&colors[gizmoId*3]);
			m_Gizmo[gizmoId]->SetGizmoMovingModalityToBound();
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


}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::GizmoDelete()
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
void mafViewOrthoSlice::SetSlicePosition(long activeGizmoId, vtkPoints *p)
//----------------------------------------------------------------------------
{
  // gizmos update correctly in every views so this method is needed to update slice also
  /*  */
  
  // always update the perspective view

  mafVME *g[3];
  double pos[3], orient[3];

  p->GetPoint(0,m_GizmoHandlePosition);
  
  switch(activeGizmoId)
  {
    case (GIZMO_XN)	:
    {
      // update the X normal child view
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
    }
    break;
   

    case (GIZMO_YN)	:
    {
      // update the Y normal child view
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);    
    }
    break;

    case (GIZMO_ZN)	:
    {
      // update the Z normal child view
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
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
  ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_PERSPECTIVE_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
  

  this->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEventSetThickness()
//----------------------------------------------------------------------------
{
	if(m_AllSurface)
	{
		mafNode* node=this->GetSceneGraph()->GetSelectedVme();
		mafVME* vme=(mafVME*)node;
		mafNode* root=vme->GetRoot();
		SetThicknessForAllSurfaceSlices(root);
	}
	else
	{
		mafNode *node=this->GetSceneGraph()->GetSelectedVme();
		mafSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);

		if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}

		if(medVisualPipeSlicerSlice *pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(medVisualPipeSlicerSlice *pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if(medVisualPipeSlicerSlice *pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
	}
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafViewOrthoSlice" << '\t' << this << std::endl;
  
  //print components view information

  for(int v=PERSPECTIVE_VIEW; v<VIEWS_NUMBER; v++)
  {
    m_ChildViewList[v]->Print(os, 1);
  }
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::CreateOrthoslicesAndGizmos( mafNode * node )
//-------------------------------------------------------------------------
{
  if (node == NULL)
  {
    mafLogMessage("node = NULL");
    return;
  }

  m_CurrentVolume = mafVME::SafeDownCast(node);
  if (m_CurrentVolume == NULL)
  {
    mafLogMessage("current volume = NULL");
    return;
  }

  double colorsX[]    = {1,0,0};
  double colorsY[]    = {0,1,0};
  double colorsZ[]    = {0,0,1};
	mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetMaterial();
	double sr[2],vtkDataCenter[3];
	vtkDataSet *vtkData = m_CurrentVolume->GetOutput()->GetVTKData();
	vtkData->Update();
	vtkData->GetCenter(vtkDataCenter);
	vtkData->GetCenter(m_GizmoHandlePosition);
	vtkData->GetScalarRange(sr);
	m_ColorLUT = currentVolumeMaterial->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentVolumeMaterial->m_TableRange[0],(long)currentVolumeMaterial->m_TableRange[1]);
	for(int i=0; i<m_NumOfChildView; i++)
	{
		mafPipeVolumeSlice_BES *p = (mafPipeVolumeSlice_BES *)((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
    p->SetEnableGPU(m_EnableGPU);
    p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
		p->SetColorLookupTable(m_ColorLUT);
	}
	((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
  ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_XN_VIEW]))->SetTextColor(colorsX);
	((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
  ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_YN_VIEW]))->SetTextColor(colorsY);
	((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetSliceLocalOrigin(m_GizmoHandlePosition);
  ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CHILD_ZN_VIEW]))->SetTextColor(colorsZ);
	GizmoCreate();
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::DestroyOrthoSlicesAndGizmos()
//-------------------------------------------------------------------------
{
	// Destroy Ortho Stuff
  if (m_CurrentVolume == NULL)
  {
    mafLogMessage("current volume = NULL");
    return;
  }
	m_CurrentVolume->GetEventSource()->RemoveObserver(this);
	m_CurrentVolume = NULL;
	GizmoDelete();
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::ResetSlicesPosition( mafNode *node )
//-------------------------------------------------------------------------
{
  // workaround... :(
  // maybe we need some mechanism to execute view code from op?
  this->VmeShow(node, false);
  this->VmeShow(node, true);
  CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::SetThicknessForAllSurfaceSlices(mafNode *root)
//----------------------------------------------------------------------------
{
	mafNodeIterator *iter = root->NewIterator();
	for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputSurface")) //if(node->IsA("mafVMESurface"))
		{
			if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
			if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
			if(mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
			{
				pipe->SetThickness(m_Border);
			}
		}
	}
	iter->Delete();
}
//----------------------------------------------------------------------------
bool mafViewOrthoSlice::IsPickedSliceView()
//----------------------------------------------------------------------------
{
  mafRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsMAFType(mafViewSlice))
      {
        if(((mafViewSlice *)m_ChildViewList[i])->GetRWI()==rwi && ((mafViewSlice *)m_ChildViewList[i])->GetRWI()->GetCamera()->GetParallelProjection())
          return true;
      }
      else if (m_ChildViewList[i]->IsMAFType(mafViewCompound))
      {
        if(((mafViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
          return false;
      }
      else if (((mafViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return false;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::ApplyViewSettings(mafNode *node)
//----------------------------------------------------------------------------
{
  if(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputPolyline"))  
  {
    for (int i=CHILD_ZN_VIEW;i<=CHILD_YN_VIEW;i++)
    {
      mafPipePolylineSlice *pipeSlice = mafPipePolylineSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[i]))->GetNodePipe(node));
      if(pipeSlice) 
      {
        if(!node->IsA("mafVMEMeter"))
          pipeSlice->SetRadius(m_PolylineRadiusSize);
        pipeSlice->FillOn(); 
      }
    }
  }
}