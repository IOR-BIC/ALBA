/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCT.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-28 14:11:06 $
  Version:   $Revision: 1.26 $
  Authors:   Stefano Perticoni , Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafNodeIterator.h"
#include "mmgLutPreset.h"
#include "mmgGui.h"
#include "mmgLutSwatch.h"
#include "mmgLutSlider.h"
#include "mafGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"

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
  m_vtkLUT[RX_FRONT_VIEW] = m_vtkLUT[RX_SIDE_VIEW] = m_vtkLUT[CT_COMPOUND_VIEW] = NULL;

  m_RightOrLeft=1;
  m_MoveAllSlices = 0; 
  m_Snap=1;
  m_CurrentSurface.clear();
  m_AllSurface=0;
  m_Border=1;

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
    vtkDEL(m_vtkLUT[i]);
  }
}
//----------------------------------------------------------------------------
mafView *mafViewRXCT::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
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

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      double center[3],b[CT_CHILD_VIEWS_NUMBER],step;
    
      // set the range for every slider widget
      for (int childID = RX_FRONT_VIEW; childID < CT_COMPOUND_VIEW; childID++)
      {
        double minMax[2];
        ((mafViewRX *)(m_ChildViewList[childID]))->GetLutRange(minMax);

        m_LutSliders[childID]->SetRange(minMax[0],minMax[1]);
        m_LutSliders[childID]->SetSubRange(minMax[0],minMax[1]);
      
        // create a lookup table for each RX view
        vtkNEW(m_vtkLUT[childID]);
        m_vtkLUT[childID]->SetRange(minMax);
        m_vtkLUT[childID]->Build();
        lutPreset(4,m_vtkLUT[childID]);
        m_vtkLUT[childID]->SetRange(minMax);
        m_vtkLUT[childID]->Build();
        lutPreset(4,m_vtkLUT[childID]);

        ((mafViewRX *)m_ChildViewList[childID])->SetLutRange(minMax[0],minMax[1]);

      }

      double sr[CT_COMPOUND_VIEW];

      // get the VTK volume
      vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);


      // set the slider for the CT compound view
      m_LutSliders[CT_COMPOUND_VIEW]->SetRange(sr[0],sr[1]);
      m_LutSliders[CT_COMPOUND_VIEW]->SetSubRange(sr[0],sr[1]);
      
      // create a lookup table for CT views
      vtkNEW(m_vtkLUT[CT_COMPOUND_VIEW]);
      m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(sr);
      m_vtkLUT[CT_COMPOUND_VIEW]->Build();
      lutPreset(4,m_vtkLUT[CT_COMPOUND_VIEW]);
      m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(sr);
      m_vtkLUT[CT_COMPOUND_VIEW]->Build();
      lutPreset(4,m_vtkLUT[CT_COMPOUND_VIEW]);

      // gather data to initialize CT slices
      data->GetBounds(b);
      step = (b[5]-b[4])/7.0;
      for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
      {
        center[2] = b[5]-step*(i+1);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->InitializeSlice(center);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->SetTextColor(m_BorderColor[i]);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->VmeShow(node,show);

        mafPipeVolumeSlice *p = NULL;
        // set pipe lookup table
        p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(node));
        p->SetColorLookupTable(m_vtkLUT[CT_COMPOUND_VIEW]);
        m_Pos[i] = b[5]-step*(i+1);
      }
      m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      GizmoCreate();

      //BEGIN cycle for remove old surface and redraw the rigth slice
      
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
      //END cycle for remove old surface and redraw the rigth slice
    }
    else
    {
      m_ChildViewList[CT_COMPOUND_VIEW]->VmeShow(node, show);
      m_CurrentVolume = NULL;
      GizmoDelete();
    }

  }
  else if (node->IsMAFType(mafVMESurface))
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
  }
  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEventRangeModified(mafEventBase *maf_event)
{
  // is the volume visible?
  if(((mafViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
  {
    int low, hi;

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
      m_vtkLUT[CT_COMPOUND_VIEW]->SetRange(low,hi);
      for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++)
      {
        mafPipeVolumeSlice *p = NULL;
        p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
        p->SetColorLookupTable(m_vtkLUT[CT_COMPOUND_VIEW]);
      }
    }

    CameraUpdate();
  }
}
void mafViewRXCT::OnEventSnapModality()
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
void mafViewRXCT::OnEventSortSlices()
{
  mafNode* node=GetSceneGraph()->GetSelectedVme();
  mafPipe *p=((mafViewRX *)m_ChildViewList[0])->GetNodePipe(node);
  if (node->IsMAFType(mafVMEVolume))
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
}
void mafViewRXCT::OnEventSetThickness()
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
  }
}
void mafViewRXCT::OnEventMouseMove( mafEvent *e )
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
mmgGui* mafViewRXCT::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  wxString m_Choices[2];
  m_Choices[0]="Right";
  m_Choices[1]="Left";
  m_Gui->Radio(ID_RIGHT_OR_LEFT,"Side",&m_RightOrLeft,2,m_Choices);

  m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap);

  m_Gui->Bool(ID_MOVE_ALL_SLICES,"Move all",&m_MoveAllSlices);

  m_Gui->Button(ID_ADJUST_SLICES,"Adjust Slices");

  m_Gui->Divider(1);

  m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);

  mafNode* node=this->GetSceneGraph()->GetSelectedVme();
  if (node->IsA("mafVMESurface"))
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

  EnableWidgets(m_CurrentVolume != NULL);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewRXCT::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  // create three windowing widgets
  for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
  {
    m_LutSliders[i] = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
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
    
    PlugChildView(m_ViewsRX[v]);
  }

  m_ViewCTCompound = new mafViewCompound("CT view",3,2);
  mafViewSlice *vs = new mafViewSlice("Slice view", CAMERA_CT);
  vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice",MUTEX);
  vs->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice",MUTEX);
  vs->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice",MUTEX);
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
  ((mafViewCompound *)m_ChildViewList[i-1])->OnLayout();
}
//----------------------------------------------------------------------------
void mafViewRXCT::GizmoCreate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<CT_CHILD_VIEWS_NUMBER; i++) 
  {
    double slice[3];
    mafPipeVolumeSlice *p = NULL;
    p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
    p->GetSliceOrigin(slice);
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
		if(node->IsA("mafVMESurface")&&select&&p)
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