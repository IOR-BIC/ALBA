/*=========================================================================

 Program: MAF2
 Module: mafViewRXCT
 Authors: Stefano Perticoni , Paolo Quadrani
 
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

#include "mafViewRXCT.h"
#include "mafViewVTK.h"
#include "mafViewRX.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice_BES.h"
#include "mafPipeSurfaceSlice.h"
#include "mafNodeIterator.h"
#include "mafGUILutPreset.h"
#include "mafGUI.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutSlider.h"
#include "mafGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafVisualPipeSlicerSlice.h"
#include "mafDeviceButtonsPadMouse.h"

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
mafCxxTypeMacro(mafViewRXCT);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRXCT::mafViewRXCT(wxString label)
: mafViewCompound(label, 1, 3)
//----------------------------------------------------------------------------
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
  m_Lut = NULL;

  m_RightOrLeft=1;
  m_MoveAllSlices = 0; 
  m_Snap=0;
  m_CurrentSurface.clear();
  m_AllSurface=0;
  m_Border=1;

  // Added by Losi 11.25.2009
  m_EnableGPU=FALSE;
  m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
mafViewRXCT::~mafViewRXCT()
//----------------------------------------------------------------------------
{
  m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
  m_ViewCTCompound = NULL;
  m_CurrentSurface.clear();

  for (int i = RX_FRONT_VIEW;i < VIEWS_NUMBER;i++)
  {
    cppDEL(m_LutSliders[i]);
    //vtkDEL(m_vtkLUT[i]);
  }
}
//----------------------------------------------------------------------------
mafView *mafViewRXCT::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewRXCT *v = new mafViewRXCT(m_Label);
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
void mafViewRXCT::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<CT_COMPOUND_VIEW; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  //if (node->IsMAFType(mafVMEVolume))
  if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
      double center[3],b[CT_CHILD_VIEWS_NUMBER],step;
    
      // set the range for every slider widget
      mafVME *volumeVme = mafVME::SafeDownCast(node);
      mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(volumeVme->GetOutput());
      for (int childID = RX_FRONT_VIEW; childID < CT_COMPOUND_VIEW; childID++)
      {
        double advLow,advHigh;
        double range[2];
        if(volumeOutput->GetMaterial())
        {
          
          ((mafViewRX *)m_ChildViewList[childID])->GetLutRange(range); //range of projected
          double volTableRange[2];
          vtkLookupTable *cl = volumeOutput->GetMaterial()->m_ColorLut;
          
          double volRange[2];
          volumeOutput->GetVTKData()->GetScalarRange(volRange);

          if (volumeOutput->GetMaterial()->m_TableRange[1] < volumeOutput->GetMaterial()->m_TableRange[0]) 
          {
            volTableRange[0] = volRange[0];
            volTableRange[1] = volRange[1];
          }
          else
          {
            volTableRange[0] = volumeOutput->GetMaterial()->m_TableRange[0];
            volTableRange[1] = volumeOutput->GetMaterial()->m_TableRange[1];
          }
          

          

          double proportionalConstant = ((range[1] - range[0])/(volRange[1] - volRange[0]));
          double inverseProportionalConstant = 1.0 / proportionalConstant;
          
          //advLow = range[0] + ((range[1] - range[0])/(volTableRange[1] - volTableRange[0])) * (range[0] - volTableRange[0]);
          //advHigh = range[1] + ((range[1] - range[0])/(volTableRange[1] - volTableRange[0])) * (range[1] - volTableRange[1]);
          advLow = proportionalConstant * (volTableRange[0] - volRange[0] + inverseProportionalConstant * range[0]);
          advHigh = proportionalConstant * (volTableRange[1] - volRange[1] + inverseProportionalConstant * range[1]);

          ((mafViewRX *)m_ChildViewList[childID])->SetLutRange(advLow,advHigh);
        }
        else
        {
          ((mafViewRX *)m_ChildViewList[childID])->GetLutRange(range);
          advLow = range[0];
          advHigh = range[1];
        }

        m_LutSliders[childID]->SetRange(range[0],range[1]);
        m_LutSliders[childID]->SetSubRange(advLow,advHigh);
      
        // create a lookup table for each RX view
        /*vtkNEW(m_vtkLUT[childID]);
        if(volumeOutput->GetMaterial()->m_ColorLut)
        {
          m_vtkLUT[childID]->DeepCopy(volumeOutput->GetMaterial()->m_ColorLut);
          m_vtkLUT[childID]->SetRange(advLow,advHigh);
          m_vtkLUT[childID]->Build();
        }
        else
        {
          m_vtkLUT[childID]->SetRange(advLow,advHigh);
          m_vtkLUT[childID]->Build();
          lutPreset(4,m_vtkLUT[childID]);
        }*/
        
        
       
        ((mafViewRX *)m_ChildViewList[childID])->SetLutRange(advLow,advHigh);

      }

      double sr[CT_COMPOUND_VIEW];

      // get the VTK volume
      vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);
      double totalSR[2];
      totalSR[0] = sr[0];
      totalSR[1] = sr[1];

      if(volumeVme)
      { 
        if(volumeOutput->GetMaterial())
        {
          if (volumeOutput->GetMaterial()->m_TableRange[1] > volumeOutput->GetMaterial()->m_TableRange[0]) 
          {
            sr[0] = volumeOutput->GetMaterial()->m_TableRange[0];
            sr[1] = volumeOutput->GetMaterial()->m_TableRange[1];
          }
        }
      }
      
      // set the slider for the CT compound view
      m_LutSliders[CT_COMPOUND_VIEW]->SetRange(totalSR[0],totalSR[1]);
      m_LutSliders[CT_COMPOUND_VIEW]->SetSubRange(sr[0],sr[1]);
      
      // create a lookup table for CT views
      
      if(volumeOutput->GetMaterial()->m_ColorLut)
      {
        /*m_vtkLUT[CT_COMPOUND_VIEW] = volumeOutput->GetMaterial()->m_ColorLut;
        m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(sr);
        m_vtkLUT[CT_COMPOUND_VIEW]->Build();*/
        
        m_Lut = volumeOutput->GetMaterial()->m_ColorLut;
        m_Lut->SetRange(sr);
        m_Lut->Build();
      }
      

      // gather data to initialize CT slices
      data->GetBounds(b);
      step = (b[5]-b[4])/7.0;
      for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
      {
        center[2] = b[5]-step*(i+1);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->InitializeSlice(center);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetTextColor(m_BorderColor[i]);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->VmeShow(node,show);

        mafPipeVolumeSlice_BES *p = NULL;
        // set pipe lookup table
        p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(node));

        p->SetEnableGPU(m_EnableGPU);
        p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);

        //p->SetColorLookupTable(m_vtkLUT[CT_COMPOUND_VIEW]);
        p->SetColorLookupTable(m_Lut);
        m_Pos[i] = b[5]-step*(i+1);
      }
      m_CurrentVolume = mafVME::SafeDownCast(node);
      GizmoCreate();

      //BEGIN cycle for remove old surface and redraw the right slice
      
      mafNodeIterator *iter = node->GetRoot()->NewIterator();
      for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
      {
        if(node->IsA("mafVMESurface"))
        {
          mafPipe *p=(m_ChildViewList[RX_FRONT_VIEW])->GetNodePipe(node);
          if(p)
          {
            this->VmeShow(node,false);
            this->VmeShow(node,true);
          }
        } 
      }
      iter->Delete();
      //END cycle for remove old surface and redraw the rigth slice
    }
    else
    {
      m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(node, show);
      m_CurrentVolume = NULL;
      GizmoDelete();
    }

  }
  else if (node->IsMAFType(mafVMESurface)||node->IsMAFType(mafVMESurfaceParametric))
  {
    // showing a surface with the volume present already
    if (show && m_CurrentVolume)
    {
      // create the slice in every CT views
      mafNode *node_selected = this->GetSceneGraph()->GetSelectedVme();
      ((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->VmeShow(node,show);
      for(int j=0;j<CT_CHILD_VIEWS_NUMBER;j++)
      {
        int i=0;
        while (j!=m_Sort[i]) i++;
        double pos[3]={0.0,0.0,m_Pos[m_Sort[i]]};
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetSliceLocalOrigin(pos);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->CameraUpdate();
      }
      mafPipe *p=((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(node);
  
      if (node_selected==node)
      {
        m_Gui->Enable(ID_ALL_SURFACE,true);
        m_Gui->Enable(ID_BORDER_CHANGE,true);
        m_Gui->Enable(ID_ADJUST_SLICES,true);
        if (p)
        {
          double old_thickness=((mafPipeSurfaceSlice *)p)->GetThickness();
          m_Border=old_thickness;
          m_Gui->Update();
        }
        else
          m_Border=1;
      }

    }//if (show)
    else if (!show)
    {
      // hide the surface
      m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(node, show);
      mafNode *node_selected = this->GetSceneGraph()->GetSelectedVme();
      if (node_selected==node)
      {
        m_Gui->Enable(ID_ALL_SURFACE,false);
        m_Gui->Enable(ID_BORDER_CHANGE,false);
        m_Gui->Enable(ID_ADJUST_SLICES,false);
      }
      for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
      {
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->UpdateSurfacesList(node);
      }
    }//else if(show)
  }
  else
  {
    m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(node, show);
  }

  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewRXCT::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    GizmoDelete();
  }
  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEventRangeModified(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // is the volume visible?
  if(((mafViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
  {
    double low, hi;

    // from which lut slider the event is coming?
    if (maf_event->GetSender() == m_LutSliders[RX_FRONT_VIEW])
    {
      m_LutSliders[RX_FRONT_VIEW]->GetSubRange(&low,&hi);
      ((mafViewRX *)m_ChildViewList[RX_FRONT_VIEW])->SetLutRange(low,hi);
    }
    else if (maf_event->GetSender() == m_LutSliders[RX_SIDE_VIEW])
    {
      m_LutSliders[RX_SIDE_VIEW]->GetSubRange(&low,&hi);
      ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->SetLutRange(low,hi);
    }
    else if (maf_event->GetSender() == m_LutSliders[CT_COMPOUND_VIEW])
    {
      m_LutSliders[CT_COMPOUND_VIEW]->GetSubRange(&low,&hi);
      //m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(low,hi);
      m_Lut->SetRange(low,hi);
      for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
      {
        mafPipeVolumeSlice_BES *p = NULL;
        p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
        //p->SetColorLookupTable(m_vtkLUT[CT_COMPOUND_VIEW]);
        p->SetColorLookupTable(m_Lut);
      }
    }

    CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEventSnapModality()
//----------------------------------------------------------------------------
{
  if(this->m_CurrentVolume==NULL && m_Snap)
  {
    wxMessageBox("You can't switch to snap modality!");
    m_Snap=0;
    m_Gui->Update();
  }
  else
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
void mafViewRXCT::OnEventSortSlices()
//----------------------------------------------------------------------------
{
  mafNode* node=GetSceneGraph()->GetSelectedVme();
  mafPipe *p=((mafViewRX *)m_ChildViewList[0])->GetNodePipe(node);
  if(((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
    mafLogMessage("SURFACE NOT SELECTED");
  else  if (node->IsMAFType(mafVMESurface))
  {
    double center[3],b[6],step;
    mafVMESurface *surface=(mafVMESurface*)node;
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
        m_GizmoSlice[currChildCTView]->CreateGizmoSliceInLocalPositionOnAxis(currChildCTView,mafGizmoSlice::GIZMO_SLICE_Z,center[2]);
        m_Pos[currChildCTView]=center[2];
        m_Sort[currChildCTView]=currChildCTView;
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetSliceLocalOrigin(center);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetTextColor(m_BorderColor[currChildCTView]);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->UpdateText();
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->BorderCreate(m_BorderColor[currChildCTView]);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->CameraUpdate();
      }
    }
    m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
    m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
  }
	else if (node->IsMAFType(mafVMESurfaceParametric))
	{
		double center[3],b[6],step;
		mafVMESurfaceParametric *surface=(mafVMESurfaceParametric*)node;
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
				m_GizmoSlice[currChildCTView]->CreateGizmoSliceInLocalPositionOnAxis(currChildCTView,mafGizmoSlice::GIZMO_SLICE_Z,center[2]);
				m_Pos[currChildCTView]=center[2];
				m_Sort[currChildCTView]=currChildCTView;
				((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetSliceLocalOrigin(center);
				((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->SetTextColor(m_BorderColor[currChildCTView]);
				((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->UpdateText();
				((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->BorderCreate(m_BorderColor[currChildCTView]);
				((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currChildCTView))->CameraUpdate();
			}
		}
		m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
		m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEventSetThickness()
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
    mafPipe *p=((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
    ((mafPipeSurfaceSlice *)p)->SetThickness(m_Border);

    if(mafVisualPipeSlicerSlice *pipe = mafVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[RX_FRONT_VIEW]->GetNodePipe(node)))
    {
      pipe->SetThickness(m_Border);
    }

    if(mafVisualPipeSlicerSlice *pipe = mafVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[RX_SIDE_VIEW]->GetNodePipe(node)))
    {
      pipe->SetThickness(m_Border);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEventMouseMove( mafEvent *e )
//----------------------------------------------------------------------------
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
    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(movingSliceId))->GetSlice(oldSliceLocalOrigin);
    delta[0] = newSliceLocalOrigin[0] - oldSliceLocalOrigin[0];
    delta[1] = newSliceLocalOrigin[1] - oldSliceLocalOrigin[1];
    delta[2] = newSliceLocalOrigin[2] - oldSliceLocalOrigin[2];

    for (int currSubView = 0; currSubView<((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetNumberOfSubView(); currSubView++)
    {
      m_CurrentVolume->GetOutput()->GetVMEBounds(b);

      int i=0;
      while (currSubView!=m_Sort[i]) i++;

      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->GetSlice(oldSliceLocalOrigin);
      newSliceLocalOrigin[0] = oldSliceLocalOrigin[0] + delta[0];
      newSliceLocalOrigin[1] = oldSliceLocalOrigin[1] + delta[1];
      newSliceLocalOrigin[2] = oldSliceLocalOrigin[2] + delta[2];
      newSliceLocalOrigin[2] = newSliceLocalOrigin[2] > b[5] ? b[5] : newSliceLocalOrigin[2];
      newSliceLocalOrigin[2] = newSliceLocalOrigin[2] < b[4] ? b[4] : newSliceLocalOrigin[2];
      m_GizmoSlice[currSubView]->CreateGizmoSliceInLocalPositionOnAxis(currSubView,mafGizmoSlice::GIZMO_SLICE_Z,newSliceLocalOrigin[2]);

      m_Pos[currSubView]=newSliceLocalOrigin[2];

      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->SetSliceLocalOrigin(newSliceLocalOrigin);
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(currSubView))->CameraUpdate();
    }
  }
  else
  {
    // move a single slice: this needs reordering
    m_Pos[movingSliceId]=newSliceLocalOrigin[2];
    SortSlices();
    int i=0;
    while (movingSliceId != m_Sort[i]) i++;

    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetSliceLocalOrigin(newSliceLocalOrigin);
    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->CameraUpdate();
  }
  m_ChildViewList[RX_FRONT_VIEW]->CameraUpdate();
  m_ChildViewList[RX_SIDE_VIEW]->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    { 
      // events from the slider
      case ID_RANGE_MODIFIED:
      {
        OnEventRangeModified(maf_event);
      }
      break;

	  case ID_HELP:
	  {
		mafEvent helpEvent;
		helpEvent.SetSender(this);
		mafString viewLabel = this->m_Label;
		helpEvent.SetString(&viewLabel);
		helpEvent.SetId(OPEN_HELP_PAGE);
		mafEventMacro(helpEvent);
	  }
	  break;

      case ID_SNAP:
      {
        OnEventSnapModality();
      }
      case ID_RIGHT_OR_LEFT:
        {
          if (m_RightOrLeft==0)
          {
            ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_RIGHT);
          }
          else
            ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_LEFT);
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
      // Added by Losi 11.25.2009
      case ID_ENABLE_GPU:
        {
          if (m_CurrentVolume)
          {
            for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
            {
              mafPipeVolumeSlice_BES *p = NULL;
              p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetEnableGPU(m_EnableGPU);
              }
            }
            this->CameraUpdate();
          }

          for (int i=0;i<CT_CHILD_VIEWS_NUMBER;i++)
          {
          	((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetEnableGPU(m_EnableGPU);
          }
        }
        break;
      case ID_TRILINEAR_INTERPOLATION:
        {
          if (m_CurrentVolume)
          {
            for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
            {
              mafPipeVolumeSlice_BES *p = NULL;
              p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
              if (p)
              {
                p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
              }
            }
            mafPipeVolumeSlice_BES *p = NULL;
            p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[RX_SIDE_VIEW]))->GetNodePipe(m_CurrentVolume));
            if (p)
            {
              p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
            }
            p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[RX_FRONT_VIEW]))->GetNodePipe(m_CurrentVolume));
            if (p)
            {
              p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
            }
            this->CameraUpdate();
          }

          for (int i=0;i<CT_CHILD_VIEWS_NUMBER;i++)
          {
            ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW]))->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
          }
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

      default:
      mafViewCompound::OnEvent(maf_event);
    }
  }
  else
  {
    mafViewCompound::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewRXCT::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, "Help","");	
  }

  wxString m_Choices[2];
  m_Choices[0]="Right";
  m_Choices[1]="Left";
  m_Gui->Radio(ID_RIGHT_OR_LEFT,"Side",&m_RightOrLeft,2,m_Choices);

  m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap,1);

  m_Gui->Bool(ID_MOVE_ALL_SLICES,"Move all",&m_MoveAllSlices);

  m_Gui->Button(ID_ADJUST_SLICES,"Adjust Slices");

  m_Gui->Divider(1);

  m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);

  mafNode* node=this->GetSceneGraph()->GetSelectedVme();
  if (node->IsA("mafVMESurface")||node->IsA("mafVMESurfaceParametric")||node->IsA("mafVMESlicer"))
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
		((mafViewRX *)m_ChildViewList[i]->GetGui());
	
	for(int i=0;i<=CT_CHILD_VIEWS_NUMBER;i++)
		(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetGui());

  m_Gui->Button(ID_RESET_SLICES,"reset slices","");

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
void mafViewRXCT::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mafGUI(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  // create three windowing widgets
  for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
  {
    m_LutSliders[i] = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
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
void mafViewRXCT::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  for(int v=RX_FRONT_VIEW; v<CT_COMPOUND_VIEW; v++)
  {
    // create to the child view
    m_ViewsRX[v] = new mafViewRX("RX child view", cam_pos[v]);
    m_ViewsRX[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeProjected",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("mafVMESlicer", "mafVisualPipeSlicerSlice",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeVolumeProjected",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("mafVMESegmentationVolume", "mafPipeVolumeProjected",MUTEX);
    
    PlugChildView(m_ViewsRX[v]);
  }

  m_ViewCTCompound = new mafViewCompound("CT view",3,2);
  mafViewSlice *vs = new mafViewSlice("Slice view", CAMERA_CT);
  vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice_BES",MUTEX);
  vs->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeSlice_BES",MUTEX);
  vs->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMEPolyline", "mafPipePolylineSlice",MUTEX);
  vs->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice",MUTEX);
  vs->PlugVisualPipe("mafVMESlicer", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMEMeter", "mafPipePolylineSlice",MUTEX);
  vs->PlugVisualPipe("mafVMEWrappedMeter", "mafPipePolylineSlice",MUTEX);
  vs->PlugVisualPipe("mafVMEVolumeLarge", "mafPipeVolumeSlice_BES",MUTEX);
  vs->PlugVisualPipe("mafVMESegmentationVolume", "mafPipeVolumeSlice_BES",MUTEX);
 
  m_ViewCTCompound->PlugChildView(vs);
  PlugChildView(m_ViewCTCompound);
}
//----------------------------------------------------------------------------
void mafViewRXCT::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_WIDGET,enable);
    m_Gui->Enable(ID_RESET_SLICES, enable);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::LayoutSubViewCustom(int width, int height)
//----------------------------------------------------------------------------
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
  ((mafViewCompound *)m_ChildViewList[i-1])->OnSize(event);
  //((mafViewCompound *)m_ChildViewList[i-1])->OnLayout();

  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
  {
    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderUpdate();
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::MaximizeSubView(int subview_id, bool maximize)
//----------------------------------------------------------------------------
{
  mafViewCompound::MaximizeSubView(subview_id, maximize);
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
    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderUpdate();
  }

  m_GuiView->Update();
  
}
//----------------------------------------------------------------------------
void mafViewRXCT::GizmoCreate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++) 
  {
    double slice[3],normal[3];
    mafPipeVolumeSlice_BES *p = NULL;
    p = mafPipeVolumeSlice_BES::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
    p->GetSlice(slice,normal);
    m_GizmoSlice[i] = new mafGizmoSlice(m_CurrentVolume, this);
    m_GizmoSlice[i]->CreateGizmoSliceInLocalPositionOnAxis(i,mafGizmoSlice::GIZMO_SLICE_Z,slice[2]);
    m_GizmoSlice[i]->SetColor(m_BorderColor[i]);
    ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderCreate(m_BorderColor[i]);

    m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(), true);
    m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(), true);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::GizmoDelete()
//----------------------------------------------------------------------------
{
  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
  {
    if(m_GizmoSlice[i])
    {
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->BorderDelete();
      m_ChildViewList[RX_FRONT_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(),false);
      m_ChildViewList[RX_SIDE_VIEW]->VmeShow(m_GizmoSlice[i]->GetOutput(),false);
      cppDEL(m_GizmoSlice[i]);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewRXCT::SortSlices()
//----------------------------------------------------------------------------
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
      OldPos=((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->GetSlice();
      OldPos[2]=m_Pos[m_Sort[j]];
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->SetSliceLocalOrigin(OldPos);
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->SetTextColor(m_BorderColor[m_Sort[j]]);
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->UpdateText();
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->BorderCreate(m_BorderColor[m_Sort[j]]);
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(j))->CameraUpdate();
    }
  }

}

//----------------------------------------------------------------------------
void mafViewRXCT::SetThicknessForAllSurfaceSlices(mafNode *root)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = root->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    if(node->IsA("mafVMESurface"))
    {
      mafPipe *p=((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(node);
      if(p)
        ((mafPipeSurfaceSlice *)p)->SetThickness(m_Border);
    }
  }
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafViewRXCT::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
	for(int i=0; i<m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeSelect(node, select);

	if(m_Gui)
	{
		mafPipe *p=((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(0))->GetNodePipe(node);
		if((node->IsA("mafVMESurface")||node->IsA("mafVMESurfaceParametric")||node->IsA("mafVMESlicer"))&&select&&p)
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
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void mafViewRXCT::BoundsValidate(double *pos)
//----------------------------------------------------------------------------
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
void mafViewRXCT::ResetSlicesPosition( mafNode *node )
//----------------------------------------------------------------------------
{
  // workaround... :(
  // maybe we need some mechanism to execute view code from op?
  this->VmeShow(node, false);
  this->VmeShow(node, true);
  CameraUpdate();
}
//----------------------------------------------------------------------------
bool mafViewRXCT::IsPickedSliceView()
//----------------------------------------------------------------------------
{
  mafRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsMAFType(mafViewSlice))
      {
        if(((mafViewSlice *)m_ChildViewList[i])->GetRWI()==rwi)
          return true;
      }
      else if (m_ChildViewList[i]->IsMAFType(mafViewCompound))
      {
        if(((mafViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
          return true;
      }
      else if (((mafViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return false;
      }
    }
  }
  return false;
}
//-------------------------------------------------------------------------
void mafViewRXCT::OnSize(wxSizeEvent &size_event)
//-------------------------------------------------------------------------
{
  mafViewCompound::OnSize(size_event);
}