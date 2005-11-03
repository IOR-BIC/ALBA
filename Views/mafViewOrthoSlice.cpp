/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-03 09:00:02 $
  Version:   $Revision: 1.1 $
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
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mmgViewWin.h"
#include "mmgGui.h"
#include "mmgLutSlider.h"

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
}
//----------------------------------------------------------------------------
mafViewOrthoSlice::~mafViewOrthoSlice()
//----------------------------------------------------------------------------
{
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
void mafViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  m_Luts = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_Luts->SetListener(this);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
    case ID_ORTHO_CHILD_VIEW:
    {
    }
  	break;
    case ID_RANGE_MODIFIED:
    {
      int low, hi;
      m_Luts->GetSubRange(&low,&hi);
      //SetLutRange(low,hi);
      CameraUpdate();
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
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[] = {CAMERA_OS_P, CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z};
  for(int v=0; v<4; v++)
  {
    m_Views[v] = new mafViewVTK("Slice view", cam_pos[v]);
    m_Views[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
    PlugChildView(m_Views[v]);
  }
}

