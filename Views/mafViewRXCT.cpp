/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCT.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-21 13:55:13 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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
#include "mmgLutPreset.h"
#include "mmgGui.h"
#include "mafVMEVolume.h"
#include "mafGizmoSlice.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewRXCT);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRXCT::mafViewRXCT(wxString label, bool external)
: mafViewCompound(label, 1, 3, external)
//----------------------------------------------------------------------------
{
  for (int v=0;v<2;v++)
  {
    m_ViewsRX[v] = NULL;
  }
  m_BorderColor[0][0] = 1; m_BorderColor[0][1] = 0; m_BorderColor[0][2] = 0;
  m_BorderColor[1][0] = 0; m_BorderColor[1][1] = 1; m_BorderColor[1][2] = 0;
  m_BorderColor[2][0] = 0; m_BorderColor[2][1] = 0; m_BorderColor[2][2] = 1;
  m_BorderColor[3][0] = 1; m_BorderColor[3][1] = 1; m_BorderColor[3][2] = 0;
  m_BorderColor[4][0] = 0; m_BorderColor[4][1] = 1; m_BorderColor[4][2] = 1;
  m_BorderColor[5][0] = 1; m_BorderColor[5][1] = 0; m_BorderColor[5][2] = 1;

  for(int j=0; j<6; j++) m_gizmo[j] = NULL;

  m_ViewCT = NULL;
  m_ColorLUT= NULL;
  m_CurrentVolume = NULL;
  m_LayoutConfiguration = LAYOUT_CUSTOM;
}
//----------------------------------------------------------------------------
mafViewRXCT::~mafViewRXCT()
//----------------------------------------------------------------------------
{
  GizmoDelete();
  vtkDEL(m_ColorLUT);
}
//----------------------------------------------------------------------------
mafView *mafViewRXCT::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewRXCT *v = new mafViewRXCT(m_Label, m_ExternalFlag);
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
  for(int i=0; i<2; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      double sr[2],center[3],b[6],step;
      vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetBounds(b);
      step = (b[5]-b[4])/7.0;
      data->GetScalarRange(sr);
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);
      for(int i=0; i<6; i++)
      {
        center[2] = b[5]-step*(i+1);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->InitializeSlice(center);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->VmeShow(node,show);
        ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->BorderCreate(m_BorderColor[i]);
        mafPipeVolumeSlice *p = NULL;
        p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetNodePipe(node));
        p->SetColorLookupTable(m_ColorLUT);
      }
      m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
//      GizmoCreate();
    }
    else
    {
      lutPreset(4,m_ColorLUT);
      m_ChildViewList[2]->VmeShow(node, show);
      m_CurrentVolume = NULL;
    }
  }
  else
  {
    m_ChildViewList[2]->VmeShow(node, show);
  }

  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewRXCT::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
    case ID_LUT_CHOOSER:
      for(int i=0; i<6; i++)
      {
        mafPipeVolumeSlice *p = NULL;
        p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
        p->SetColorLookupTable(m_ColorLUT);
      }
      CameraUpdate();
    break;
    default:
      mafViewCompound::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mmgGui* mafViewRXCT::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  if (m_ColorLUT == NULL)
  {
    vtkNEW(m_ColorLUT);
  }
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

  EnableWidgets(m_CurrentVolume != NULL);
  for(int i=1; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->GetGui();
  }
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewRXCT::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  for(int v=0; v<2; v++)
  {
    m_ViewsRX[v] = new mafViewRX("RX view", cam_pos[v]);
    m_ViewsRX[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
    PlugChildView(m_ViewsRX[v]);
  }
  m_ViewCT = new mafViewCompound("CT view",3);
  mafViewSlice *vs = new mafViewSlice("Slice view", CAMERA_CT);
  vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice",MUTEX);
  m_ViewCT->PlugChildView(vs);
  PlugChildView(m_ViewCT);
}
//----------------------------------------------------------------------------
void mafViewRXCT::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_CHOOSER,enable);
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
/*  for(int i=0; i<6; i++) 
  {
    m_gizmo[i] = new mafGizmoSlice(m_CurrentVolume, this);
    m_gizmo[i]->SetSlice(i,mafGizmoSlice::GIZMO_SLICE_Z);
    m_gizmo[i]->SetColor();

    m_ChildViewList[0]->VmeAdd(m_gizmo[i]);
    m_ChildViewList[1]->VmeAdd(m_gizmo[i]);
  }*/
}
//----------------------------------------------------------------------------
void mafViewRXCT::GizmoDelete()
//----------------------------------------------------------------------------
{
/*  for(int i=0; i<6; i++) 
  {
    if(m_gizmo[i])
    {
      mafEventMacro(mafEvent(this,VME_REMOVE,m_gizmo[i]));
      m_ChildViewList[0]->VmeRemove(m_gizmo[i]);
      m_ChildViewList[1]->VmeAdd(m_gizmo[i]);
      cppDEL(m_gizmo[i]);
    }
  }*/
}
