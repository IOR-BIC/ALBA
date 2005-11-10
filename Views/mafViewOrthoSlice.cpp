/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-10 15:44:08 $
  Version:   $Revision: 1.5 $
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

#include "mafViewOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeVolumeStructuredSlice.h"
#include "mmgGui.h"
#include "mmgFloatSlider.h"
#include "mmgLutSlider.h"
#include "mafVMEVolume.h"

#include "vtkDataSet.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewOrthoSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewOrthoSlice::mafViewOrthoSlice(wxString label, int num_row, int num_col, bool external)
: mafViewCompound(label, 2, 2, external)
//----------------------------------------------------------------------------
{
  m_Luts = NULL;
  for (int v=0;v<4;v++)
  {
    m_Views[v] = NULL;
  }
  m_SliderX = NULL;
  m_SliderY = NULL;
  m_SliderZ = NULL;
  m_CurrentVolume = NULL;
  m_Origin[0] = m_Origin[1] = m_Origin[2] = 0.0;
}
//----------------------------------------------------------------------------
mafViewOrthoSlice::~mafViewOrthoSlice()
//----------------------------------------------------------------------------
{
  for (int v=0;v<4;v++)
  {
    cppDEL(m_Views[v]);
  }
}
//----------------------------------------------------------------------------
mafView *mafViewOrthoSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewOrthoSlice *v = new mafViewOrthoSlice(m_Label, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PluggedChildViewList = m_PluggedChildViewList;
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  if (show && node->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
    double sr[2];
    vtkDataSet *data = m_CurrentVolume->GetOutput()->GetVTKData();
    data->Update();
    data->GetScalarRange(sr);
    m_Luts->SetRange((long)sr[0],(long)sr[1]);
    m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
    if (m_SliderX)
    {
      double b[6];
      data->GetBounds(b);
      m_Origin[0] = (b[0]+b[1])*.5;
      m_Origin[1] = (b[2]+b[3])*.5;
      m_Origin[2] = (b[4]+b[5])*.5;
      m_SliderX->SetRange(b[0],b[1],m_Origin[0]);
      m_SliderY->SetRange(b[2],b[3],m_Origin[1]);
      m_SliderZ->SetRange(b[4],b[5],m_Origin[2]);
      m_Gui->Update();
    }
  }
  if (!show && node->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = NULL;
  }
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  m_Luts = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  EnableWidgets(m_CurrentVolume != NULL);
  m_Luts->SetListener(this);
  m_GuiView->Add(m_Luts);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
    case ID_ORTHO_SLICE_X:
    case ID_ORTHO_SLICE_Y:
    case ID_ORTHO_SLICE_Z:
    {
      for(int i=0; i<4; i++)
      {
        mafPipeVolumeStructuredSlice *p = mafPipeVolumeStructuredSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
        p->SetSlice(m_Origin);
      }
      CameraUpdate();
    }
  	break;
    case ID_RANGE_MODIFIED:
    {
      if(((mafViewSlice *)m_ChildViewList[0])->VolumeIsVisible())
      {
        int low, hi;
        m_Luts->GetSubRange(&low,&hi);
        for(int i=0; i<4; i++) 
          ((mafViewSlice *)m_ChildViewList[i])->SetLutRange(low, hi);
        CameraUpdate();
      }
    }
    break;
    default:
      mafViewCompound::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mmgGui* mafViewOrthoSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Label("OS GUI", true);
  m_SliderX = m_Gui->FloatSlider(ID_ORTHO_SLICE_X, "x", &m_Origin[0],MINDOUBLE,MAXDOUBLE);
  m_SliderY = m_Gui->FloatSlider(ID_ORTHO_SLICE_Y, "y", &m_Origin[1],MINDOUBLE,MAXDOUBLE);
  m_SliderZ = m_Gui->FloatSlider(ID_ORTHO_SLICE_Z, "z", &m_Origin[2],MINDOUBLE,MAXDOUBLE);
  EnableWidgets(m_CurrentVolume != NULL);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[] = {CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z, CAMERA_OS_P};
  for(int v=0; v<4; v++)
  {
    m_Views[v] = new mafViewSlice("Slice view", cam_pos[v]);
    m_Views[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeStructuredSlice");
  }
  PlugChildView(m_Views[SLICE_ORTHO]);
  PlugChildView(m_Views[SLICE_X]);
  PlugChildView(m_Views[SLICE_Y]);
  PlugChildView(m_Views[SLICE_Z]);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_ORTHO_SLICE_X,enable);
    m_Gui->Enable(ID_ORTHO_SLICE_Y,enable);
    m_Gui->Enable(ID_ORTHO_SLICE_Z,enable);
  }
  m_Luts->Enable(enable);
}
