/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgHistogramWidget.cpp,v $
Language:  C++
Date:      $Date: 2007-03-14 17:09:08 $
Version:   $Revision: 1.12 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgHistogramWidget.h"
#include "mafDecl.h"
#include "mafRWI.h"
#include "mmgGui.h"

#include "mmdButtonsPad.h"
#include "mmdMouse.h"
#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkRenderer.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
mmgHistogramWidget::mmgHistogramWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:mmgPanel(parent,id,pos,size,style)
//----------------------------------------------------------------------------
{
	m_Listener    = NULL;
  m_Histogram   = NULL;
	m_HistogramData = NULL;
  m_Gui         = NULL;
  m_Data        = NULL;

  m_ScaleFactor     = 1.0;
  m_HisctogramValue = 0;
  m_LogScaleConstant= 10.0;
  m_DragStart       = 0;
  m_Representation  = 0;
  
  m_AutoscaleHistogram = true;
  m_LogHistogramFlag   = false;
  m_Dragging           = false;
  
  vtkNEW(m_Histogram);
  m_Histogram->SetColor(1,1,1);
  m_Histogram->SetHisctogramRepresentation(vtkHistogram::BAR_REPRESENTATION);

  m_HistogramRWI = new mafRWI(mafGetFrame());
  m_HistogramRWI->SetListener(this);
  m_HistogramRWI->m_RenFront->AddActor2D(m_Histogram);
  m_HistogramRWI->m_RenFront->SetBackground(0.28,0.28,0.28);
  m_HistogramRWI->SetSize(pos.x,pos.y,size.GetWidth(),size.GetHeight());
  ((wxWindow *)m_HistogramRWI->m_RwiBase)->SetSize(size.GetWidth(),size.GetHeight());
  m_HistogramRWI->m_RwiBase->SetMinSize(wxSize(size.GetWidth(),size.GetHeight()));
  m_HistogramRWI->m_RwiBase->Reparent(this);
  m_HistogramRWI->m_RwiBase->SetListener(this);
  m_HistogramRWI->m_RwiBase->Show(true);
/*
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_HistogramRWI->m_RwiBase,1, wxEXPAND);

  SetSizer(sizer);

  sizer->Layout();           // resize & fit the contents
  sizer->SetSizeHints(this); // resize the dialog accordingly 

  SetAutoLayout(TRUE);
  sizer->Fit(this);
*/
  CreateGui();
}
//----------------------------------------------------------------------------
mmgHistogramWidget::~mmgHistogramWidget() 
//----------------------------------------------------------------------------
{
  m_HistogramRWI->m_RenFront->RemoveActor(m_Histogram);
  vtkDEL(m_Histogram);
  cppDEL(m_HistogramRWI);
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::OnEvent( mafEventBase *event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
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
      default:
        e->Log();
      break; 
    }
    m_HistogramRWI->CameraUpdate();
  }
  else if (mafEventInteraction *ei = mafEventInteraction::SafeDownCast(event))
  {
    if (ei->GetId() == mmdMouse::MOUSE_2D_MOVE)
    {
      if(m_Histogram->GetInputData() == NULL) return;
      double pos[2];
      ei->Get2DPosition(pos);
      if (m_Dragging)
      {
        int dy = pos[1]-m_DragStart;
        m_ScaleFactor *= ( 1 + dy/100.0 );
        m_Gui->Update();
        m_Histogram->SetScaleFactor(m_ScaleFactor);
      }
      else
      {
        m_HisctogramValue = m_Histogram->GetHistogramValue(pos[0],pos[1]);
        m_Histogram->SetLabel(mafString(m_HisctogramValue).GetCStr());
      }
      m_HistogramRWI->CameraUpdate();
    }
    else if (ei->GetId() == mmdButtonsPad::BUTTON_DOWN)
    {
      if (ei->GetButton() == MAF_RIGHT_BUTTON)
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
    else if (ei->GetId() == mmdButtonsPad::BUTTON_UP)
    {
      if (ei->GetButton() == MAF_RIGHT_BUTTON)
      {
        m_Dragging = false;
      }
    }
  }
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetData(vtkDataArray *data)
//----------------------------------------------------------------------------
{
  m_Data = data;
  m_Histogram->SetInputData(m_Data);
  UpdateGui();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  m_Histogram->SetScaleFactor(factor);
  UpdateGui();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::LogarithmicScale(int enable)
//----------------------------------------------------------------------------
{
  m_Histogram->SetLogHistogram(enable);
  UpdateGui();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetLogScaleConstant(double c)
//----------------------------------------------------------------------------
{
  m_Histogram->SetLogScaleConstant(c);
  UpdateGui();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::AutoscaleHistogram(int autoscale)
//----------------------------------------------------------------------------
{
  m_Histogram->SetAutoscaleHistogram(autoscale);
  UpdateGui();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Show(true);

  wxString represent[3] = {"points", "lines", "bar"};
  m_Gui->Combo(ID_REPRESENTATION,"represent",&m_Representation,3,represent);
  m_Gui->Bool(ID_AUTOSCALE,"autoscale", &m_AutoscaleHistogram);
  m_Gui->Double(ID_SCALE_FACTOR,"scale",&m_ScaleFactor,1.0e-299,MAXDOUBLE,-1);
  m_Gui->Bool(ID_LOGSCALE,"log",&m_LogHistogramFlag,0,"Enable/Disable log scale for histogram");
  m_Gui->Double(ID_LOGFACTOR,"log constant",&m_LogScaleConstant,1.0e-299,MAXDOUBLE,-1,"multiplicative factor for log scale histogram");
	m_Gui->Divider();

  EnableWidgets(m_Data != NULL);
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::UpdateGui()
//----------------------------------------------------------------------------
{
  m_HistogramRWI->CameraUpdate();
  m_ScaleFactor         = m_Histogram->GetScaleFactor();
  m_LogScaleConstant    = m_Histogram->GetLogScaleConstant();
  m_LogHistogramFlag    = m_Histogram->GetLogHistogram();
  m_AutoscaleHistogram  = m_Histogram->GetAutoscaleHistogram();
  m_Representation      = m_Histogram->GetHisctogramRepresentation();
  m_Gui->Update();
  EnableWidgets(m_Data != NULL);
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetRepresentation(int represent)
//----------------------------------------------------------------------------
{
  switch(represent) 
  {
    case vtkHistogram::POINT_REPRESENTATION:
    case vtkHistogram::LINE_REPRESENTATION:
    case vtkHistogram::BAR_REPRESENTATION:
      m_Representation = represent;
  	break;
    default:
      m_Representation = vtkHistogram::POINT_REPRESENTATION;
  }
  m_Histogram->SetHisctogramRepresentation(m_Representation);
  m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(mmgHistogramWidget::ID_REPRESENTATION, enable);
  m_Gui->Enable(mmgHistogramWidget::ID_AUTOSCALE,enable);
  m_Gui->Enable(mmgHistogramWidget::ID_SCALE_FACTOR,enable && m_AutoscaleHistogram == 0);
  m_Gui->Enable(mmgHistogramWidget::ID_LOGSCALE,enable);
  m_Gui->Enable(mmgHistogramWidget::ID_LOGFACTOR,m_LogHistogramFlag != 0);
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetHistogramData(vtkImageData *histogram)
//----------------------------------------------------------------------------
{
	m_HistogramData = histogram;
}
