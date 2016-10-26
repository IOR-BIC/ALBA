/*=========================================================================

 Program: MAF2
 Module: mafViewSliceBlendRX
 Authors: Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
 
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
{
//   m_ViewsRX = NULL;
//   m_ViewSliceBlend = NULL;
    
  cppDEL(m_LutSliders);
  vtkDEL(m_VtkLUT);
}
//----------------------------------------------------------------------------
mafView *mafViewSliceBlendRX::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceBlendRX *v = new mafViewSliceBlendRX(m_Label);
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
void mafViewSliceBlendRX::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->VmeShow(vme, show);
  }

  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
      double minMax[2];
      ((mafViewRX*)m_ChildViewList[RX_VIEW])->GetLutRange(minMax);
      //set new values for lut slider
      m_LutSliders->SetRange(minMax[0],minMax[1]);
      m_LutSliders->SetSubRange(minMax[0],minMax[1]);
      // create a lookup table for each RX view
      vtkNEW(m_VtkLUT);
      m_VtkLUT->SetRange(minMax);
      m_VtkLUT->Build();
      lutPreset(4,m_VtkLUT);
      m_VtkLUT->SetRange(minMax);
      m_VtkLUT->Build();
      lutPreset(4,m_VtkLUT);

      ((mafViewRX*)m_ChildViewList[RX_VIEW])->SetLutRange(minMax[0],minMax[1]);
      
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    { 
      // events from the slider
      case ID_RANGE_MODIFIED:
      {
        // is the volume visible?
        if(((mafViewRX*)m_ChildViewList[RX_VIEW])->VolumeIsVisible())
        {
          double low, hi;

          //Get new values of LUT
          m_LutSliders->GetSubRange(&low,&hi);
          //Set new values of LUT
          ((mafViewRX*)m_ChildViewList[RX_VIEW])->SetLutRange(low,hi);
          
          CameraUpdate();
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
//-------------------------------------------------------------------------
mafGUI* mafViewSliceBlendRX::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  m_Gui->Label(_("Blend View"),true);
  m_BlendGui = m_ChildViewList[BLEND_VIEW]->GetGui();
  // m_BlendGui->SetListener(this);
  m_Gui->AddGui(m_ChildViewList[BLEND_VIEW]->GetGui());
  m_Gui->FitInside();
  m_Gui->Update();
  m_Gui->Divider(1);
  m_Gui->Divider(1);
  
  //Enable/disable gui componets depending from volume
  EnableWidgets(m_CurrentVolume != NULL);
	for(int i=0;i<m_NumOfChildView;i++)
  {
    //Generate gui for the subviews
		((mafViewRX*)m_ChildViewList[i])->GetGui();
  }
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mafGUI(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  m_LutSliders = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
  //EnableWidgets(m_CurrentVolume != NULL);
  m_LutSliders->SetListener(this);
  m_LutSliders->SetMinSize(wxSize(500,24));
  lutsSizer->Add(m_LutSliders,wxALIGN_CENTER|wxRIGHT);
  //Addd lut slider to the gui view
  m_GuiView->Add(lutsSizer);
  m_GuiView->Reparent(m_Win);
  m_GuiView->FitGui();
  m_GuiView->Update();
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  // create to the child view
  //Create a mafViewRX
  m_ViewsRX = new mafViewRX("RX child view", cam_pos[0]);
  m_ViewsRX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
  m_ViewsRX->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeProjected",MUTEX);
  PlugChildView(m_ViewsRX);
  //Create a mafViewSliceBlend
  m_ViewSliceBlend = new mafViewSliceBlend("Blend",CAMERA_BLEND);
  m_ViewSliceBlend->PlugVisualPipe("mafVMEVolumeGray","mafPipeVolumeSliceBlend",MUTEX);

  PlugChildView(m_ViewSliceBlend);
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_WIDGET,enable);
  }
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::LayoutSubViewCustom(int width, int height)
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
{
  // Retrieve mafPipeVolumeSliceBlend to obtain slices positions
  mafPipeVolumeSliceBlend *p = mafPipeVolumeSliceBlend::SafeDownCast(m_ChildViewList[BLEND_VIEW]->GetNodePipe(m_CurrentVolume));

  if (p == NULL)
  {
    //No volume is visualized
    return;
  }

  double pos0[3];
  p->GetSliceOrigin(GIZMO_0,pos0);
  double pos1[3];
  p->GetSliceOrigin(GIZMO_1,pos1);
  //Create Gizmo0
  m_GizmoSlice[GIZMO_0] = new mafGizmoSlice(m_CurrentVolume, this);
  m_GizmoSlice[GIZMO_0]->CreateGizmoSliceInLocalPositionOnAxis(GIZMO_0,mafGizmoSlice::GIZMO_SLICE_Z,pos0[2]);
  m_GizmoSlice[GIZMO_0]->SetColor(m_BorderColor[GIZMO_0]);

  //Create Gizmo1
  m_GizmoSlice[GIZMO_1] = new mafGizmoSlice(m_CurrentVolume, this);
  m_GizmoSlice[GIZMO_1]->CreateGizmoSliceInLocalPositionOnAxis(GIZMO_1,mafGizmoSlice::GIZMO_SLICE_Z,pos1[2]);
  m_GizmoSlice[GIZMO_1]->SetColor(m_BorderColor[GIZMO_1]);

  //Show On Gizmos
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_0]->GetOutput(), true);
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_1]->GetOutput(), true);
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::GizmoDelete()
//----------------------------------------------------------------------------
{
  //Show Off Gizmos
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_0]->GetOutput(),false);
  m_ChildViewList[RX_VIEW]->VmeShow(m_GizmoSlice[GIZMO_1]->GetOutput(),false);
  cppDEL(m_GizmoSlice[GIZMO_0]);
  cppDEL(m_GizmoSlice[GIZMO_1]);
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::OnEventMouseMove( mafEvent *e )
//----------------------------------------------------------------------------
{
  long movingSliceId;
  movingSliceId = e->GetArg();

  double newSliceLocalOrigin[3];
  vtkPoints *p = (vtkPoints *)e->GetVtkObj();
  if(p == NULL) {
    return;
  }
  //Get point picked
  p->GetPoint(0,newSliceLocalOrigin);

  //Validate slices origins and adjust them
  BoundsValidate(newSliceLocalOrigin);
  
	((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->SetSlice(movingSliceId,newSliceLocalOrigin);
	((mafViewSliceBlend *)m_ChildViewList[BLEND_VIEW])->CameraUpdate();

  m_ChildViewList[RX_VIEW]->CameraUpdate();
  m_ChildViewList[BLEND_VIEW]->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewSliceBlendRX::BoundsValidate(double *pos)
//----------------------------------------------------------------------------
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