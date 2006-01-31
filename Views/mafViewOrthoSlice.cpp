/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-31 15:31:21 $
  Version:   $Revision: 1.18 $
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
#include "mmgLutPreset.h"
#include "mmgHistogramWidget.h"
#include "mmgGui.h"
#include "mmgFloatSlider.h"
#include "mmgLutSlider.h"
#include "mafVMEVolume.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewOrthoSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewOrthoSlice::mafViewOrthoSlice(wxString label, bool external)
: mafViewCompound(label, 2, 2, external)
//----------------------------------------------------------------------------
{
  m_Luts = NULL;
  m_Histogram = NULL;
  for (int v=0;v<4;v++)
  {
    m_Views[v] = NULL;
  }
  m_SliderX = NULL;
  m_SliderY = NULL;
  m_SliderZ = NULL;
  m_ColorLUT= NULL;
  m_CurrentVolume = NULL;
  m_Origin[0] = m_Origin[1] = m_Origin[2] = 0.0;
  m_ScaleFactor = 0.05;
  m_LogScaleFlag= 0;
  m_LogConstant = 10.0;
  m_Autoscale = true;
}
//----------------------------------------------------------------------------
mafViewOrthoSlice::~mafViewOrthoSlice()
//----------------------------------------------------------------------------
{
  vtkDEL(m_ColorLUT);
}
//----------------------------------------------------------------------------
mafView *mafViewOrthoSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewOrthoSlice *v = new mafViewOrthoSlice(m_Label, m_ExternalFlag);
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
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      double sr[2],center[3];
      vtkDataSet *data = m_CurrentVolume->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);
      m_Luts->SetRange((long)sr[0],(long)sr[1]);
      m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);
      for(int i=0; i<m_NumOfChildView; i++)
      {
        mafPipeVolumeSlice *p = (mafPipeVolumeSlice *)((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
        p->SetColorLookupTable(m_ColorLUT);
      }
      if (m_SliderX)
      {
        double b[6];
        m_CurrentVolume->GetOutput()->GetVMEBounds(b);
        m_Origin[0] = (b[0]+b[1])*.5;
        m_Origin[1] = (b[2]+b[3])*.5;
        m_Origin[2] = (b[4]+b[5])*.5;
        m_SliderX->SetRange(b[0],b[1],m_Origin[0]);
        m_SliderY->SetRange(b[2],b[3],m_Origin[1]);
        m_SliderZ->SetRange(b[4],b[5],m_Origin[2]);
        m_Gui->Update();
      }
      vtkImageData *vol = vtkImageData::SafeDownCast(data);
      if (vol)
      {
        m_Histogram->SetData(vol);
        m_ScaleFactor = m_Histogram->GetScaleFactor();
        m_LogConstant = m_Histogram->GetLogScaleConstant();
        m_Gui->Update();
      }
    }
    else
    {
      m_CurrentVolume = NULL;
      lutPreset(4,m_ColorLUT);
    }
  }
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  m_Histogram = new mmgHistogramWidget(m_GuiView,-1,wxPoint(0,0),wxSize(500,100));
  m_Histogram->SetListener(this);
  
  m_Luts = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  EnableWidgets(m_CurrentVolume != NULL);
  m_Luts->SetListener(this);
  m_GuiView->Add(m_Histogram);
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
      for(int i=0; i<m_NumOfChildView; i++)
      {
        ((mafViewSlice *)m_ChildViewList[i])->SetSlice(m_Origin);
      }
      CameraUpdate();
    }
  	break;
    case ID_HISTOGRAM_SCALE_FACTOR:
      m_Histogram->SetScaleFactor(m_ScaleFactor);
    break;
    case ID_HISTOGRAM_LOGSCALE:
      m_Gui->Enable(ID_HISTOGRAM_LOGFACTOR,m_LogScaleFlag != 0);
      m_Histogram->LogarithmicScale(m_LogScaleFlag != 0);
    break;
    case ID_AUTOSCALE_HISTOGRAM:
      m_Histogram->AutoscaleHistogram(m_Autoscale != 0);
      m_Gui->Enable(ID_HISTOGRAM_SCALE_FACTOR,m_Autoscale == 0);
    break;
    case ID_HISTOGRAM_LOGFACTOR:
      m_Histogram->SetLogScaleConstant(m_LogConstant);
    break;
    case ID_LUT_CHOOSER:
    {
      for(int i=0; i<m_NumOfChildView; i++)
      {
        mafPipeVolumeSlice *p = (mafPipeVolumeSlice *)((mafViewSlice *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume);
        p->SetColorLookupTable(m_ColorLUT);
      }
      double *sr;
      sr = m_ColorLUT->GetRange();
      m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
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
  wxString layout_choices[3] = {"default","layout 1","layout 2"};

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Combo(ID_LAYOUT_CHOOSER,"layout",&m_LayoutConfiguration,3,layout_choices);
  m_Gui->Divider();
  m_SliderX = m_Gui->FloatSlider(ID_ORTHO_SLICE_X, "x", &m_Origin[0],MINDOUBLE,MAXDOUBLE);
  m_SliderY = m_Gui->FloatSlider(ID_ORTHO_SLICE_Y, "y", &m_Origin[1],MINDOUBLE,MAXDOUBLE);
  m_SliderZ = m_Gui->FloatSlider(ID_ORTHO_SLICE_Z, "z", &m_Origin[2],MINDOUBLE,MAXDOUBLE);

  if (m_ColorLUT == NULL)
  {
    vtkNEW(m_ColorLUT);
    m_ColorLUT->Build();
    lutPreset(4,m_ColorLUT);
  }
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_AUTOSCALE_HISTOGRAM,"autoscale", &m_Autoscale);
  m_Gui->Double(ID_HISTOGRAM_SCALE_FACTOR,"scale",&m_ScaleFactor,0.0000000001,MAXDOUBLE,-1);
  m_Gui->Bool(ID_HISTOGRAM_LOGSCALE,"log",&m_LogScaleFlag,0,"Enable/Disable log scale for histogram");
  m_Gui->Double(ID_HISTOGRAM_LOGFACTOR,"log constant",&m_LogConstant,0.0000000001,MAXDOUBLE,-1,"multiplicative factor for log scale histogram");
  m_Gui->Divider(2);

  EnableWidgets(m_CurrentVolume != NULL);
  for(int i=1; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->GetGui();
  }
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[4] = {CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z, CAMERA_OS_P};
  for(int v=0; v<4; v++)
  {
    m_Views[v] = new mafViewSlice("Slice view", cam_pos[v]);
    m_Views[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice", MUTEX);
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
    m_Gui->Enable(ID_LUT_CHOOSER,enable);
    m_Gui->Enable(ID_HISTOGRAM_SCALE_FACTOR,enable && m_Autoscale == 0);
    m_Gui->Enable(ID_HISTOGRAM_LOGSCALE,enable);
    m_Gui->Enable(ID_AUTOSCALE_HISTOGRAM,enable);
    m_Gui->Enable(ID_HISTOGRAM_LOGFACTOR,m_LogScaleFlag != 0);
  }
  m_Luts->Enable(enable);
  m_Histogram->Enable(enable);
  m_Histogram->Show(enable);
}
