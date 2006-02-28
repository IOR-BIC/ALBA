/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgHistogramWidget.cpp,v $
Language:  C++
Date:      $Date: 2006-02-28 14:47:12 $
Version:   $Revision: 1.5 $
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
#include "mmdButtonsPad.h"
#include "mmdMouse.h"
#include "mafEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkImageAccumulate.h"
#include "vtkImageLogarithmicScale.h"
#include "vtkImageChangeInformation.h"
#include "vtkGlyph3D.h"
#include "vtkLineSource.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmgHistogramWidget::HISTOGRAM_UPDATED)

//----------------------------------------------------------------------------
mmgHistogramWidget::mmgHistogramWidget(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)
//----------------------------------------------------------------------------
{
	m_Listener    = NULL;
  m_PlotActor   = NULL;
  m_Accumulate  = NULL;
  m_LogScale    = NULL;
  m_Glyph       = NULL;
  m_TextMapper  = NULL;
  m_TextActor   = NULL;

  m_ScaleFactor     = 1.0;
  m_NumberOfBins    = 1;
  m_HisctogramValue = 0;
  m_LogScaleConstant= 10.0;
  m_DragStart       = 0;
  
  m_AutoscaleHistogram = true;
  m_LogHistogramFlag   = false;
  m_Dragging           = false;
  
  vtkNEW(m_TextMapper);
  m_TextMapper->SetInput("");
  m_TextMapper->GetTextProperty()->AntiAliasingOff();
  m_TextMapper->GetTextProperty()->SetFontFamily(VTK_TIMES);
  m_TextMapper->GetTextProperty()->SetColor(0,0,0.8);
  m_TextMapper->GetTextProperty()->SetLineOffset(0.5);
  m_TextMapper->GetTextProperty()->SetLineSpacing(1.5);
  m_TextMapper->GetTextProperty()->SetJustificationToRight();
  m_TextMapper->GetTextProperty()->SetVerticalJustificationToTop();

  vtkNEW(m_TextActor);
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  m_TextActor->SetPosition(1, 1);

  // Create Histogram pipeline
  vtkNEW(m_Accumulate);

  vtkNEW(m_LogScale);
  m_LogScale->SetConstant(m_LogScaleConstant);

  vtkNEW(m_ChangeInfo);
  m_ChangeInfo->SetOutputOrigin(0,0,0);

  vtkMAFSmartPointer<vtkLineSource> line;
  line->SetPoint1(0,0,0);
  line->SetPoint2(0,1,0);
  line->Update();
  vtkNEW(m_Glyph);
  m_Glyph->SetSource(line->GetOutput());
  m_Glyph->SetScaleModeToScaleByScalar();
  m_Glyph->OrientOn();
  vtkMAFSmartPointer<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedDisplay();

  vtkMAFSmartPointer<vtkPolyDataMapper2D> mapper2d;
  mapper2d->SetInput(m_Glyph->GetOutput());
  mapper2d->SetTransformCoordinate(coordinate);

  vtkNEW(m_PlotActor);
  m_PlotActor->SetMapper(mapper2d);

  m_HistogramRWI = new mafRWI(mafGetFrame());
  m_HistogramRWI->SetListener(this);
  m_HistogramRWI->m_RenFront->AddActor2D(m_PlotActor);
  m_HistogramRWI->m_RenFront->SetBackground(1,1,1);
  m_HistogramRWI->SetSize(pos.x,pos.y,size.GetWidth(),size.GetHeight());
  m_HistogramRWI->m_RwiBase->Reparent(this);
  m_HistogramRWI->m_RwiBase->SetListener(this);
  m_HistogramRWI->m_RwiBase->Show(true);

  m_HistogramRWI->m_RenFront->AddActor(m_TextActor);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_HistogramRWI->m_RwiBase,1, wxEXPAND);

  SetSizer(sizer);
  SetAutoLayout(TRUE);
  sizer->Fit(this);

  SetSize(pos.x,pos.y,size.GetWidth(),size.GetHeight());
}
//----------------------------------------------------------------------------
mmgHistogramWidget::~mmgHistogramWidget() 
//----------------------------------------------------------------------------
{
  m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
  m_HistogramRWI->m_RenFront->RemoveActor(m_TextActor);
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
  vtkDEL(m_Accumulate);
  vtkDEL(m_LogScale);
  vtkDEL(m_ChangeInfo);
  vtkDEL(m_Glyph);
  vtkDEL(m_PlotActor);
  cppDEL(m_HistogramRWI);
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::OnEvent( mafEventBase *event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
/*    switch(e->GetId())
    {
      default:
        e->Log();
      break; 
    }*/
  }
  else if (mafEventInteraction *ei = mafEventInteraction::SafeDownCast(event))
  {
    if (ei->GetId() == mmdMouse::MOUSE_2D_MOVE)
    {
      int x_size, idx;
      double pos[2];
      ei->Get2DPosition(pos);
      if (m_Dragging)
      {
        int dy = pos[1]-m_DragStart;
        m_ScaleFactor *= ( 1 + dy/100.0 );
        SetScaleFactor(m_ScaleFactor);
      }
      else
      {
        x_size = GetSize().GetWidth();
        idx = (pos[0]/x_size) * m_NumberOfBins;
        float line_width = 0.5 + x_size / (float)m_NumberOfBins;
        m_PlotActor->GetProperty()->SetLineWidth(line_width);
        m_HisctogramValue = m_Accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(idx);
        m_TextMapper->SetInput(mafString(m_HisctogramValue).GetCStr());
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
void mmgHistogramWidget::SetData(vtkImageData *data)
//----------------------------------------------------------------------------
{
  m_Data = data;
  UpdateHistogram();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::UpdateHistogram()
//----------------------------------------------------------------------------
{
  if (m_Data == NULL)
  {
    return;
  }
  double sr[2];
  m_Data->Update();
  m_Data->GetScalarRange(sr);
  double srw = sr[1]-sr[0];

  if (m_Data->GetScalarType() == VTK_CHAR || m_Data->GetScalarType() == VTK_UNSIGNED_CHAR )
  {
    m_NumberOfBins = srw; 
  }
  else if ( m_Data->GetScalarType() >= VTK_SHORT && m_Data->GetScalarType() <= VTK_UNSIGNED_INT )
  {
    m_NumberOfBins = srw;
    int i=1;
    while(m_NumberOfBins > 500 )
    {
       m_NumberOfBins = srw / i++;
    }
  }
  else 
  {
    m_NumberOfBins = ( srw > 500 ) ? 500 : srw ; 
  }

  m_Accumulate->SetInput(m_Data);
  m_Accumulate->SetComponentOrigin(sr[0],0,0);  
  m_Accumulate->SetComponentExtent(0,m_NumberOfBins,0,0,0,0);
  m_Accumulate->SetComponentSpacing(srw/m_NumberOfBins,0,0);
  m_Accumulate->Update();

  double acc_min[3], acc_max[3];
  m_Accumulate->GetMin(acc_min);
  m_Accumulate->GetMax(acc_max);

  if (m_AutoscaleHistogram)
  {
    m_ScaleFactor = .05 / m_Accumulate->GetMean()[0];
  }

  m_ChangeInfo->SetInput(m_Accumulate->GetOutput());
  m_ChangeInfo->SetOutputSpacing(1.0/m_NumberOfBins,1,1);
  m_ChangeInfo->Update();

  m_LogScale->SetConstant(m_LogScaleConstant);
  m_LogScale->SetInput(m_ChangeInfo->GetOutput());
  m_LogScale->Update();

  if (m_LogHistogramFlag)
  {
    if (m_AutoscaleHistogram)
    {
      m_ScaleFactor = .5 / log(1 + m_Accumulate->GetMean()[0]);
    }
    m_Glyph->SetInput(m_LogScale->GetOutput());
  }
  else
  {
    m_Glyph->SetInput(m_ChangeInfo->GetOutput());
  }
  m_Glyph->SetScaleFactor(m_ScaleFactor);
  m_Glyph->Modified();
  m_Glyph->Update();

  float line_width = m_HistogramRWI->m_RenFront->GetSize()[0] / (float)(m_NumberOfBins - 1);
  m_PlotActor->GetProperty()->SetLineWidth(line_width);
  m_HistogramRWI->m_RwiBase->Render();
  if (m_Listener)
  {
    mafEventInteraction e(this,mmgHistogramWidget::HISTOGRAM_UPDATED);
    m_Listener->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetScaleFactor(double factor)
//----------------------------------------------------------------------------
{
  m_ScaleFactor = factor;
  UpdateHistogram();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::LogarithmicScale(bool enable)
//----------------------------------------------------------------------------
{
  m_LogHistogramFlag = enable;
  UpdateHistogram();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::SetLogScaleConstant(double c)
//----------------------------------------------------------------------------
{
  m_LogScaleConstant = c;
  UpdateHistogram();
}
//----------------------------------------------------------------------------
void mmgHistogramWidget::AutoscaleHistogram(bool autoscale)
//----------------------------------------------------------------------------
{
  m_AutoscaleHistogram = autoscale;
  UpdateHistogram();
}
