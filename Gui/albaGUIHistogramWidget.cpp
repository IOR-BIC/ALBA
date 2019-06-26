/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIHistogramWidget
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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

#include "albaGUIHistogramWidget.h"
#include "albaDecl.h"
#include "albaRWI.h"
#include "albaGUI.h"
#include "albaGUIRangeSlider.h"
#include "albaGUILutSlider.h"

#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"
#include "vtkLineSource.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"

ALBA_ID_IMP(albaGUIHistogramWidget::RANGE_MODIFIED);

//----------------------------------------------------------------------------
albaGUIHistogramWidget::albaGUIHistogramWidget(wxWindow* parent, wxWindowID id /* = -1 */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxSize */,long style /* = wxTAB_TRAVERSAL */, bool showThresholds /* = false */)
:albaGUIPanel(parent,id,pos,size,style)
//----------------------------------------------------------------------------
{
	m_Listener    = NULL;
  m_Histogram   = NULL;
	m_HistogramData = NULL;
  m_Gui         = NULL;
  m_Data        = NULL;
  m_Lut         = NULL;
  m_Slider      = NULL;
  m_SliderThresholds = NULL;

  m_SelectedRange[0] = 0.0;
  m_SelectedRange[1] = 1.0;
  m_ScaleFactor     = 1.0;
  m_HisctogramValue = 0;
  m_LogScaleConstant= 10.0;
  m_DragStart       = 0;
  m_Representation  = 1;

  m_LowerThreshold = 0;
  m_UpperThreshold = 1;
  
  m_AutoscaleHistogram = true;
  m_LogHistogramFlag   = false;
  m_Dragging           = false;
  m_ShowText           = true;
  //////////////////////////////////////////////////////////////////////////

  vtkNEW(m_Histogram);
  m_Histogram->SetColor(1,1,1);
  m_Histogram->SetHisctogramRepresentation(vtkALBAHistogram::LINE_REPRESENTATION);

  wxBoxSizer *sizerV = new wxBoxSizer(wxVERTICAL);

  m_HistogramRWI = new albaRWI(albaGetFrame());
  m_HistogramRWI->SetListener(this);
  m_HistogramRWI->m_RenFront->AddActor2D(m_Histogram);
  //m_HistogramRWI->m_RenFront->AddActor2D(actor);
  m_HistogramRWI->m_RenFront->SetBackground(0.28,0.28,0.28);
  m_HistogramRWI->SetSize(pos.x,pos.y,size.GetWidth(),size.GetHeight());
  ((wxWindow *)m_HistogramRWI->m_RwiBase)->SetSize(size.GetWidth(),size.GetHeight());
  ((wxWindow *)m_HistogramRWI->m_RwiBase)->SetMinSize(wxSize(size.GetWidth(),size.GetHeight()));
  //m_HistogramRWI->m_RwiBase->SetMinSize(wxSize(size.GetWidth(),size.GetHeight()));
  m_HistogramRWI->m_RwiBase->Reparent(this);
  m_HistogramRWI->m_RwiBase->SetListener(this);
  m_HistogramRWI->m_RwiBase->Show(true);

  if (showThresholds)
  {
	  wxBoxSizer *sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	  m_SliderThresholds = new albaGUILutSlider(this,ID_SLIDER_THRESHOLD ,wxPoint(0,0),wxSize(10,24), 0, "Thresholds");
	  m_SliderThresholds->SetListener(this);
	  m_SliderThresholds->SetSize(5,24);
	  m_SliderThresholds->SetMinSize(wxSize(5,24));
	  m_SliderThresholds->SetRange(m_LowerThreshold,m_UpperThreshold);
	  m_SliderThresholds->SetSubRange(m_LowerThreshold,m_UpperThreshold);
	  sizerH1->Add(m_SliderThresholds,wxEXPAND);
	  sizerV->Add(sizerH1, 0,wxEXPAND);

    m_Histogram->ShowLinesOn();
  }

  wxStaticText *foo_l = new wxStaticText(this,-1, "");
  wxStaticText *foo_r = new wxStaticText(this,-1, "");

  wxBoxSizer *sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerH2->Add(foo_l, 0, wxLEFT, 0);
  sizerH2->Add(m_HistogramRWI->m_RwiBase, 1, wxEXPAND, 0);
  sizerH2->Add(foo_r, 0, wxLEFT, 0);
  sizerV->Add(sizerH2, 1,wxEXPAND);

  SetSizer(sizerV);

  sizerV->Layout();           // resize & fit the contents
  sizerV->SetSizeHints(this); // resize the dialog accordingly 

  SetAutoLayout(TRUE);
  sizerV->Fit(this);

//  CreateGui();

  SetMinSize(size);
}
//----------------------------------------------------------------------------
albaGUIHistogramWidget::~albaGUIHistogramWidget() 
//----------------------------------------------------------------------------
{
  m_HistogramRWI->m_RenFront->RemoveActor(m_Histogram);
  cppDEL(m_Slider);
  vtkDEL(m_Histogram);
  cppDEL(m_HistogramRWI);
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::UpdateLines(int min,int max)
//----------------------------------------------------------------------------
{
  m_LowerThreshold=min;
  m_UpperThreshold=max;
  m_Histogram->UpdateLines(min,max);
  m_HistogramRWI->CameraUpdate();
}
//----------------------------------------------------------------------------
albaGUI *albaGUIHistogramWidget::GetGui()
//----------------------------------------------------------------------------
{
  if (m_Gui == NULL)
  {
    CreateGui();
  }
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::GetThresholds(double *lower, double *upper)
//----------------------------------------------------------------------------
{
  *lower=m_LowerThreshold;
  *upper=m_UpperThreshold;
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::CreateGui()
//----------------------------------------------------------------------------
{
  wxString represent[3] = {"points", "lines", "bar"};
  m_Gui = new albaGUI(this);
  m_Gui->Show(true);

  if (m_Lut != NULL)
  {
    m_Slider = new albaGUIRangeSlider(this, ID_RANGE_SLICER, wxDefaultPosition, wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30), wxNO_BORDER);
    m_Slider->SetListener(this);
    m_Slider->EnableCenterWidget(false);
    m_Slider->EnableBoundaryHandles(false);
    m_Data->GetRange(m_SelectedRange);
    m_Slider->SetRange(m_SelectedRange);
//    m_Slider->SetValue(0, m_SelectedRange[0]);
//    m_Slider->SetValue(2, m_SelectedRange[1]);
    m_Lut->GetTableRange(m_SelectedRange);
    m_Slider->SetValue(0, m_SelectedRange[0]);
    m_Slider->SetValue(2, m_SelectedRange[1]);
    m_Slider->SetSize(wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30));
    m_Slider->SetMinSize(wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30));
    m_Slider->Update();
    m_Gui->Add(m_Slider);
  }
  m_Gui->Combo(ID_REPRESENTATION,"represent",&m_Representation,3,represent);
  m_Gui->Bool(ID_AUTOSCALE,"autoscale", &m_AutoscaleHistogram);
  m_Gui->Double(ID_SCALE_FACTOR,"scale",&m_ScaleFactor,1.0e-299,MAXDOUBLE,-1);
  m_Gui->Bool(ID_LOGSCALE,"log",&m_LogHistogramFlag,0,"Enable/Disable log scale for histogram");
  m_Gui->Double(ID_LOGFACTOR,"log constant",&m_LogScaleConstant,1.0e-299,MAXDOUBLE,-1,"multiplicative factor for log scale histogram");
  m_Gui->Button(ID_RESET, "reset");
  m_Gui->Divider();

  EnableWidgets(m_Data != NULL);
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::OnEvent( albaEventBase *event )
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
    case ID_RANGE_MODIFIED:
      {
        if (m_SliderThresholds)
          m_SliderThresholds->GetSubRange(&m_LowerThreshold,&m_UpperThreshold);
        m_Histogram->UpdateLines(m_LowerThreshold,m_UpperThreshold);
      }
      break;
      case ID_REPRESENTATION:
        SetRepresentation(m_Representation);
      break;
      case ID_AUTOSCALE:
        AutoscaleHistogram(m_AutoscaleHistogram);
      break;
      case ID_SCALE_FACTOR:
        SetScaleFactor(m_ScaleFactor);
      break;
      case ID_LOGSCALE:
        LogarithmicScale(m_LogHistogramFlag);
      break;
      case ID_LOGFACTOR:
        SetLogScaleConstant(m_LogScaleConstant);
      break;
      case ID_RESET:
        ResetHistogram();
      break;
      case ID_RANGE_SLICER:
        m_SelectedRange[0] = m_Slider->GetValue(0);
        m_SelectedRange[1] = m_Slider->GetValue(2);
        m_Lut->SetTableRange(m_SelectedRange);
        albaEventMacro(albaEvent(this,albaGUIHistogramWidget::RANGE_MODIFIED));
      break;
      default:
        e->Log();
      break; 
    }
    m_HistogramRWI->CameraUpdate();
  }
  else if (albaEventInteraction *ei = albaEventInteraction::SafeDownCast(event))
  {
    if (ei->GetId() == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
    {
      if(m_Histogram->GetInputData() == NULL) return;
      double pos[2];
      ei->Get2DPosition(pos);
      if (m_Dragging)
      {
        int dy = pos[1]-m_DragStart;
        m_ScaleFactor *= ( 1 + dy/100.0 );
        if(m_Gui)
        {
          m_Gui->Update();
        }
        m_Histogram->SetScaleFactor(m_ScaleFactor);
        m_DragStart=pos[1];
      }
      else
      {
        if (m_ShowText)
        {
        m_HisctogramValue = m_Histogram->GetHistogramValue(pos[0],pos[1]);
        m_Histogram->SetLabel(albaString(m_HisctogramValue).GetCStr());
        }
      }
      m_HistogramRWI->CameraUpdate();
    }
    else if (ei->GetId() == albaDeviceButtonsPad::GetButtonDownId())
    {
      if (ei->GetButton() == ALBA_LEFT_BUTTON && ei->GetModifier(ALBA_CTRL_KEY))
      {
//         if(m_Histogram->GetInputData() == NULL) return;
//         double pos[2];
//         ei->Get2DPosition(pos);
//         m_HisctogramValue = m_Histogram->GetHistogramValue(pos[0],pos[1]);
//         double scalar = m_Histogram->GetScalarValue(pos[0],pos[1]);
// 
//         m_HistogramRWI->CameraUpdate();

        albaEventMacro(*event);
      }
      else if (ei->GetButton() == ALBA_RIGHT_BUTTON)
      {
        double pos[2];
        m_Dragging = true;
        ei->Get2DPosition(pos);
        m_DragStart = (int)pos[1];
        if(m_AutoscaleHistogram)
        {
          AutoscaleHistogramOff();
        }
      }
    }
    else if (ei->GetId() == albaDeviceButtonsPad::GetButtonUpId())
    {
      if (ei->GetButton() == ALBA_RIGHT_BUTTON)
      {
        m_Dragging = false;
      }
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::UpdateGui()
//----------------------------------------------------------------------------
{
  m_HistogramRWI->CameraUpdate();
  m_ScaleFactor         = m_Histogram->GetScaleFactor();
  m_LogScaleConstant    = m_Histogram->GetLogScaleConstant();
  m_LogHistogramFlag    = m_Histogram->GetLogHistogram();
  m_AutoscaleHistogram  = m_Histogram->GetAutoscaleHistogram();
  m_Representation      = m_Histogram->GetHisctogramRepresentation();
  if (m_Gui != NULL)
  {
    m_Gui->Update();
    EnableWidgets(m_Data != NULL);
  }
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetData(vtkDataArray *data)
//----------------------------------------------------------------------------
{
  m_Data = data;
  m_Histogram->SetInputData(m_Data);
  double sr[2],subR[2];
  m_Data->GetRange(sr);
  if (m_Lut)
  {
    m_Lut->GetTableRange(subR);
    m_LowerThreshold = subR[0];
    m_UpperThreshold = subR[1];
  }
  else
  {
    m_LowerThreshold = sr[0];
    m_UpperThreshold = sr[1];
  }

  if (m_SliderThresholds != NULL)
  {
	  m_SliderThresholds->SetRange(sr[0],sr[1]);
	  m_SliderThresholds->SetSubRange(m_LowerThreshold,m_UpperThreshold);
  }
  m_Histogram->UpdateLines(m_LowerThreshold,m_UpperThreshold);
  UpdateGui();
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_Lut=lut;
  double sr[2],subR[2];
  if(m_Data)
    m_Data->GetRange(sr);
  else 
    lut->GetTableRange(sr);
  lut->GetTableRange(subR);
  m_LowerThreshold = subR[0];
  m_UpperThreshold = subR[1];
  if (m_SliderThresholds != NULL)
  {
	  m_SliderThresholds->SetRange(sr[0],sr[1]);
	  m_SliderThresholds->SetSubRange(m_LowerThreshold,m_UpperThreshold);
  }
  m_Histogram->UpdateLines(m_LowerThreshold,m_UpperThreshold);
  UpdateGui();
}


//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  m_Histogram->SetScaleFactor(factor);
  UpdateGui();
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::LogarithmicScale(int enable)
//----------------------------------------------------------------------------
{
  m_Histogram->SetLogHistogram(enable);
  UpdateGui();
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetLogScaleConstant(double c)
//----------------------------------------------------------------------------
{
  m_Histogram->SetLogScaleConstant(c);
  UpdateGui();
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::AutoscaleHistogram(int autoscale)
//----------------------------------------------------------------------------
{
  m_Histogram->SetAutoscaleHistogram(autoscale);
  UpdateGui();
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetRepresentation(int represent)
//----------------------------------------------------------------------------
{
  switch(represent) 
  {
    case vtkALBAHistogram::POINT_REPRESENTATION:
    case vtkALBAHistogram::LINE_REPRESENTATION:
    case vtkALBAHistogram::BAR_REPRESENTATION:
      m_Representation = represent;
  	break;
    default:
      m_Representation = vtkALBAHistogram::POINT_REPRESENTATION;
  }
  m_Histogram->SetHisctogramRepresentation(m_Representation);
  if (m_Gui != NULL)
  {
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(albaGUIHistogramWidget::ID_REPRESENTATION, enable);
  m_Gui->Enable(albaGUIHistogramWidget::ID_AUTOSCALE,enable);
  m_Gui->Enable(albaGUIHistogramWidget::ID_SCALE_FACTOR,enable && m_AutoscaleHistogram == 0);
  m_Gui->Enable(albaGUIHistogramWidget::ID_LOGSCALE,enable);
  m_Gui->Enable(albaGUIHistogramWidget::ID_LOGFACTOR,m_LogHistogramFlag != 0);
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::ShowLines(int value)
//----------------------------------------------------------------------------
{
  if (value)
    m_Histogram->ShowLinesOn();
  else
    m_Histogram->ShowLinesOff();
}


//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetHistogramData(vtkImageData *histogram)
//----------------------------------------------------------------------------
{
	m_HistogramData = histogram;
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::ResetHistogram()
//----------------------------------------------------------------------------
{
  AutoscaleHistogram(0);
  LogarithmicScale(0);
  SetLogScaleConstant(10);
  SetRepresentation(vtkALBAHistogram::LINE_REPRESENTATION);
  AutoscaleHistogram(1);
  if(m_Slider != NULL && m_Lut != NULL)
  {
    m_SelectedRange[0] = m_Slider->GetRange()[0];
    m_SelectedRange[1] = m_Slider->GetRange()[1];
    m_Slider->SetValue(0, m_SelectedRange[0]);
    m_Slider->SetValue(2, m_SelectedRange[1]);
    m_Lut->SetTableRange(m_SelectedRange);
    albaEventMacro(albaEvent(this,albaGUIHistogramWidget::RANGE_MODIFIED));
  }

  UpdateGui();
}
//----------------------------------------------------------------------------
double albaGUIHistogramWidget::GetHistogramScalarValue(int x, int y)
//----------------------------------------------------------------------------
{
  return m_Histogram->GetScalarValue(x,y);
}
//----------------------------------------------------------------------------
long int albaGUIHistogramWidget::GetHistogramValue(int x, int y)
//----------------------------------------------------------------------------
{
  return m_Histogram->GetHistogramValue(x,y);
}
