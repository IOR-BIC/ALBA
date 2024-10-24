/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DDistance.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Daniele Giunchi - Roberto Mucci
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaInteractor2DDistance.h"
#include "albaDecl.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaRWI.h"
#include "albaView.h"
#include "albaGUIDialogPreview.h"
#include "albaVME.h"
#include "albaEventInteraction.h"

#include "vtkALBASmartPointer.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkProbeFilter.h"
#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkImageImport.h"
#include "vtkImageAccumulate.h"
#include "vtkALBATextActorMeter.h"

#include "vtkRendererCollection.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DDistance)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaInteractor2DDistance::albaInteractor2DDistance(bool testMode /* = false */) 
//----------------------------------------------------------------------------
{
  m_TestMode = testMode;

  m_HistogramRWI = NULL;
  m_HistogramDialog = NULL;

  m_Coordinate = vtkCoordinate::New();
  m_Coordinate->SetCoordinateSystemToWorld();

  // Measure tools
  vtkNEW(m_Line);
  vtkNEW(m_LineMapper);
  vtkNEW(m_LineActor);
  vtkNEW(m_Line2);
  vtkNEW(m_LineMapper2);
  vtkNEW(m_LineActor2);

  // Probing tool
  vtkNEW(m_ProbingLine);
  m_ProbingLine->SetResolution(512);
  
  m_ProbedVME = NULL;
  m_Mouse     = NULL;

  albaString plot_title = _("Density vs. Length (mm)");
  albaString plot_titleX = "mm";
  albaString plot_titleY = _("Dens.");
  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotActor->GetProperty()->SetLineWidth(2);
  m_PlotActor->SetPosition(0.03,0.03);
  m_PlotActor->SetPosition2(0.9,0.9);
  m_PlotActor->SetLabelFormat("%g");
  m_PlotActor->SetXRange(0,300);
  m_PlotActor->SetPlotCoordinate(0,300);
  m_PlotActor->SetNumberOfXLabels(10);
  m_PlotActor->SetXValuesToIndex();
  m_PlotActor->SetTitle(plot_title);
  m_PlotActor->SetXTitle(plot_titleX);
  m_PlotActor->SetYTitle(plot_titleY);
  vtkTextProperty* tprop = m_PlotActor->GetTitleTextProperty();
  tprop->SetColor(0.02,0.06,0.62);
  tprop->SetFontFamilyToArial();
  tprop->ItalicOff();
  tprop->BoldOff();
  tprop->SetFontSize(12);
  m_PlotActor->SetPlotColor(0,.8,.3,.3);

  if (!m_TestMode)
  { // Histogram dialog
	  int width = 400;
	  int height = 300;
	  int x_init,y_init;
	  x_init = albaGetFrame()->GetPosition().x;
	  y_init = albaGetFrame()->GetPosition().y;
	  m_HistogramDialog = new albaGUIDialogPreview(_("Histogram"), albaCLOSEWINDOW | albaUSERWI);
	  m_HistogramRWI = m_HistogramDialog->GetRWI();
	  m_HistogramRWI->SetListener(this);
	  m_HistogramRWI->m_RenFront->AddActor2D(m_PlotActor);
	  m_HistogramRWI->m_RenFront->SetBackground(1,1,1);
	  m_HistogramRWI->SetSize(0,0,width,height);
	  
	  m_HistogramDialog->SetSize(x_init,y_init,width,height);
		m_HistogramDialog->Show(false);
	}

  m_CurrentRwi = NULL;
  m_CurrentRenderer  = NULL;
  m_LastRenderer     = NULL;
  m_PreviousRenderer   = NULL;

  m_Line->SetPoint1(0,0,0);
  m_Line->SetPoint2(0.5,0.5,0);
  m_Line->Update();
  m_LineMapper->SetInput(m_Line->GetOutput());
  m_LineMapper->SetTransformCoordinate(m_Coordinate);
  m_LineActor->SetMapper(m_LineMapper);
  m_LineActor->GetProperty()->SetColor(1.0,0.0,0.0);

  m_Line2->SetPoint1(0,0,0);
  m_Line2->SetPoint2(0.5,0.5,0);
  m_Line2->Update();
  m_LineMapper2->SetInput(m_Line2->GetOutput());
  m_LineMapper2->SetTransformCoordinate(m_Coordinate);
  m_LineActor2->SetMapper(m_LineMapper2);
  m_LineActor2->GetProperty()->SetColor(1.0,0.0,0.0);

	m_GenerateHistogram = false;
  m_DraggingLine  = false;
  m_DraggingLeft  = false;
  m_EndMeasure    = false;
  m_ParallelView  = false;
  m_DisableUndoAndOkCancel = false;
  m_Clockwise = false;
	m_RegisterMeasure = false;
  m_MeasureType = DISTANCE_BETWEEN_POINTS;

  m_Distance = 0;

  m_Color[0] = 0.0;
  m_Color[1] = 1.0;
  m_Color[2] = 0.0;
  
}
//----------------------------------------------------------------------------
albaInteractor2DDistance::~albaInteractor2DDistance() 
//----------------------------------------------------------------------------
{
  RemoveMeter();
  if (m_HistogramRWI)
  {
  	m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
  }
  vtkDEL(m_PlotActor);
  vtkDEL(m_ProbingLine);
  vtkDEL(m_Line);
  vtkDEL(m_LineMapper);
  vtkDEL(m_LineActor);
  vtkDEL(m_Line2);
  vtkDEL(m_LineMapper2);
  vtkDEL(m_LineActor2);
  vtkDEL(m_Coordinate);

  int num = m_RendererVector.size();
  for (int i=0; i<num; i++) 
  {
    m_RendererVector[i]->RemoveActor2D(m_MeterVector[i]);
    vtkDEL(m_MeterVector[i]);
    m_RendererVector[i]->RemoveActor2D(m_LineActorVector1[i]);
    vtkDEL(m_LineActorVector1[i]);
    vtkDEL(m_LineMapperVector1[i]);
    vtkDEL(m_LineSourceVector1[i]);
/*  CONE
		m_RendererVector[i]->RemoveActor2D(m_ConeActorVector[i]);
		vtkDEL(m_ConeActorVector[i]);
		vtkDEL(m_ConeMapperVector[i]);
		vtkDEL(m_ConeSourceVector[i]);
*/
    m_RendererVector[i]->RemoveActor2D(m_LineActorVector2[i]);
    vtkDEL(m_LineActorVector2[i]);
    vtkDEL(m_LineMapperVector2[i]);
    vtkDEL(m_LineSourceVector2[i]);
    m_RendererVector[i]->GetRenderWindow()->Render();
  }
  m_MeterVector.clear();
  m_LineSourceVector1.clear();
  m_LineMapperVector1.clear();
  m_LineActorVector1.clear();

  m_LineSourceVector2.clear();
  m_LineMapperVector2.clear();
  m_LineActorVector2.clear();
  m_RendererVector.clear();
  m_Measure.clear();
  m_FlagMeasureType.clear();

  cppDEL(m_HistogramDialog);
}

//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnLeftButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2];
  e->Get2DPosition(pos_2d);
  
  albaEventMacro(albaEvent(this, CAMERA_UPDATE));
  if(m_EndMeasure)
  { 
    RemoveMeter();
  }
  
  albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
  albaDeviceButtonsPadMouse  *mouse  = albaDeviceButtonsPadMouse::SafeDownCast(device);
  if (m_Mouse == NULL)
  {
    m_Mouse = mouse;
  }
  m_CurrentRwi = m_Mouse->GetRWI();
  m_CurrentRenderer = m_Mouse->GetRenderer();

  if (m_CurrentRenderer->GetLayer() != 1)//Frontal Render
  {
	  vtkRendererCollection *rc = m_Mouse->GetRenderer()->GetRenderWindow()->GetRenderers();;
	  if(rc)
	  {
	    rc->InitTraversal();
	    
	    vtkRenderer *ren = NULL;
	    do 
	    {
		    ren = rc->GetNextItem();
		    if(ren)
		    {
		      if (ren->GetLayer() == 1)//Frontal Render
		      {
		        m_CurrentRenderer = ren;
	          break;
	        }
		    }
	    } while (ren);
	  }
  }

  m_ParallelView = m_CurrentRenderer->GetActiveCamera()->GetParallelProjection() != 0;
  if (m_ParallelView)
  {
    OnButtonDown2D(e);  
    if (m_GenerateHistogram)
    {
      albaView *v = m_Mouse->GetView();
      if(v->Pick(pos_2d[0],pos_2d[1]))
      {
        v->GetPickedPosition(m_PickedPoint);
        m_ProbingLine->SetPoint1(m_PickedPoint);
        m_ProbingLine->SetPoint2(m_PickedPoint);
        m_ProbingLine->Update();
        m_ProbedVME = v->GetPickedVme();
      }
    }
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
    // register the last renderer
    m_LastRenderer = m_CurrentRenderer;
    m_DraggingLeft = true;
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnMiddleButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e); 
}

//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnLeftButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2] = {-1,-1};

  m_DraggingLeft = false;
	m_DraggingLine = false;
  OnButtonUp(e);

  if(m_ParallelView)
  {
    e->Get2DPosition(pos_2d);
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
    if(m_EndMeasure)
    {
      CalculateMeasure();
			m_RegisterMeasure = false;
      if (m_GenerateHistogram)
      {
        albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
        albaDeviceButtonsPadMouse  *mouse  = albaDeviceButtonsPadMouse::SafeDownCast(device);
        if (m_Mouse == NULL)
        {
          m_Mouse = mouse;
        }
        double pos_2d[2];
        e->Get2DPosition(pos_2d);
        albaView *v = m_Mouse->GetView();
        if(v->Pick(pos_2d[0],pos_2d[1]))
        {
          v->GetPickedPosition(m_PickedPoint);
          m_ProbingLine->SetPoint2(m_PickedPoint);
          m_ProbingLine->Update();
          m_ProbedVME = v->GetPickedVme();
        }
        CreateHistogram();
      }
    }
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnMiddleButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}

//------------------------------------------------------------------------------
void albaInteractor2DDistance::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());

  albaID id=event->GetId();
  albaID channel=event->GetChannel();

  Superclass::OnEvent(event);
}

//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnMove(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (!m_DraggingMouse) return;
  if((m_DraggingLeft || m_DraggingLine) && m_ParallelView)
  {
    double pos_2d[2];
    e->Get2DPosition(pos_2d);
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnButtonDown2D(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
 	m_DraggingMouse = true;
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::OnButtonUp(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingMouse = false;
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::DrawMeasureTool(double x, double y)
//----------------------------------------------------------------------------
{
	static long counter = 0;
	static double dx, dy, dz;
  
  m_CurrentRwi = m_Mouse->GetRWI();
  m_CurrentRenderer = m_Mouse->GetRenderer();
  if (m_CurrentRenderer->GetLayer() != 1)//Frontal Render
  {
    vtkRendererCollection *rc = m_Mouse->GetRenderer()->GetRenderWindow()->GetRenderers();;
    if(rc)
    {
      rc->InitTraversal();

      vtkRenderer *ren = NULL;
      do 
      {
        ren = rc->GetNextItem();
        if(ren)
        {
          if (ren->GetLayer() == 1)//Frontal Render
          {
            m_CurrentRenderer = ren;
            break;
          }
        }
      } while (ren);
    }
  }

	if (m_CurrentRenderer == NULL || (m_DisableUndoAndOkCancel && counter == 2 && m_CurrentRenderer != m_PreviousRenderer ))	{return;}

	double wp[4], p[3];
  m_CurrentRenderer->SetDisplayPoint(x,y,0);
	m_CurrentRenderer->DisplayToWorld();
	m_CurrentRenderer->GetWorldPoint(wp);
  p[0] = wp[0];
  p[1] = wp[1];
  p[2] = wp[2];
  
	// no point has yet been picked
	if(counter == 0)
	{
		m_EndMeasure = false;
    
    m_MeterVector.push_back(NULL);
    m_MeterVector[m_MeterVector.size()-1] = vtkALBATextActorMeter::New();
    // initialization
    m_Distance = 0.0;
		m_CurrentRenderer->RemoveActor2D(m_LineActor2);
		m_Line->SetPoint1(p);
		m_Line->SetPoint2(p);
		m_Line->Update();
		m_CurrentRenderer->AddActor2D(m_LineActor);
		counter++;
	}
	// first point has been picked and the second one is being dragged
	else if(counter == 1 && m_DraggingLeft)
	{
		m_Line->SetPoint2(p);
		m_Line->Update();

		if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
		{
			CalculateMeasure();
		}
	}
	// finished dragging the second point
	else if(counter == 1)
	{
		if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
		{
			//m_EndMeasure = true;
      m_PreviousRenderer = m_CurrentRenderer;
      m_Line2->SetPoint1(p);
      m_Line2->SetPoint2(p);
      m_Line2->Update();
      m_CurrentRenderer->AddActor2D(m_LineActor2);
      m_EndMeasure = true;
      counter++;

      if (counter < 2)
      {
        double tmp_pos[3];
        m_Line->GetPoint2(tmp_pos);
        albaString ds;
        ds = albaString::Format(_("%.2f") , m_Distance);
        m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
        m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
        m_DisableUndoAndOkCancel = true;
      }
      else
      {
        double tmp_pos[3];
        m_Line2->GetPoint2(tmp_pos);
        albaString ds;
        ds = albaString::Format(_("%.2f") , m_Distance);
        m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
        m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
        m_DisableUndoAndOkCancel = false;
      }

      m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);

      m_LastRenderer->GetRenderWindow()->Render(); 
		}
		else
		{
			counter++;
		}
		if(m_MeasureType == DISTANCE_BETWEEN_LINES) 
			m_DraggingLine = true;
	}
	else if (counter == 2 && (m_CurrentRenderer != m_LastRenderer) && m_MeasureType != DISTANCE_BETWEEN_POINTS)
	{
		// remove the first line
		m_LastRenderer->RemoveActor2D(m_LineActor);
		m_LastRenderer->GetRenderWindow()->Render(); 
		// reset the counter
		counter = 0;   
	} 
	// add the  second line for the distance between lines mode
	else if(counter == 2 && m_MeasureType == DISTANCE_BETWEEN_POINTS)
	{
// 		m_Line2->SetPoint1(p);
// 		m_Line2->SetPoint2(p);
// 		m_Line2->Update();
// 		m_CurrentRenderer->AddActor2D(m_LineActor2);
// 		counter++;
	}
	else if(counter == 2 && m_DraggingLine)
	{
		assert(m_MeasureType = DISTANCE_BETWEEN_LINES);
		// add the second line 
		double tmpPt[3];
		m_Line->GetPoint1(tmpPt);
		m_Line2->SetPoint1(tmpPt);
		dx = tmpPt[0];
		dy = tmpPt[1];
		dz = tmpPt[2];
		m_Line->GetPoint2(tmpPt);
		m_Line2->SetPoint2(tmpPt);
		dx -= tmpPt[0];
		dy -= tmpPt[1];
		dz -= tmpPt[2];
		m_Line2->Update();
		m_CurrentRenderer->AddActor2D(m_LineActor2);
		counter++;
	}
	// add the  second line for the angle between lines mode
	else if(counter == 3 && m_MeasureType == DISTANCE_BETWEEN_POINTS && m_DraggingLeft)
	{
		m_Line2->SetPoint1(p);
		m_Line2->SetPoint2(p);
		m_Line2->Update();
	}
	else if(counter == 2 && m_MeasureType == DISTANCE_BETWEEN_POINTS)
	{
		m_EndMeasure = true;
	}
	else if(counter == 3 && m_MeasureType == DISTANCE_BETWEEN_LINES && m_DraggingLine)
	{
		m_Line2->SetPoint2(p);
		m_Line2->SetPoint1(p[0] + dx, p[1] + dy, p[2] + dz);
		m_Line2->Update();
	}
	else if(counter == 3 && m_MeasureType == DISTANCE_BETWEEN_LINES)
	{
		m_EndMeasure = true;
	}

  // text in Render Window (if series for writing text)
  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  {    
//     if (counter < 2)
//     {
//       double tmp_pos[3];
//       m_Line->GetPoint2(tmp_pos);
//       albaString ds;
//       ds = albaString::Format(_("%.2f") , m_Distance);
//       m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
//       m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
//       m_DisableUndoAndOkCancel = true;
//     }
//     else
//     {
//       double tmp_pos[3];
//       m_Line2->GetPoint2(tmp_pos);
//       albaString ds;
//       ds = albaString::Format(_("%.2f") , m_Distance);
//       m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
//       m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
//       m_DisableUndoAndOkCancel = false;
//     }
// 
//     m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);
  }
  else if (m_MeasureType == DISTANCE_BETWEEN_LINES && counter > 2)
  {
    CalculateMeasure();
    double tmp_pos1_1[3];
    double tmp_pos2_1[3];
    double tmp_pos1_2[3];
    double tmp_pos2_2[3];
    m_Line->GetPoint1(tmp_pos1_1);
    m_Line->GetPoint2(tmp_pos2_1);
    m_Line2->GetPoint1(tmp_pos1_2);
    m_Line2->GetPoint2(tmp_pos2_2);
   
    double tmp_pos[3];
    tmp_pos[0] =  ((tmp_pos1_1[0] + tmp_pos2_1[0])/2 + (tmp_pos1_2[0] + tmp_pos2_2[0])/2)/2;
    tmp_pos[1] =  ((tmp_pos1_1[1] + tmp_pos2_1[1])/2 + (tmp_pos1_2[1] + tmp_pos2_2[1])/2)/2;
    tmp_pos[2] =  ((tmp_pos1_1[2] + tmp_pos2_1[2])/2 + (tmp_pos1_2[2] + tmp_pos2_2[2])/2)/2;
    albaString ds;
    ds = albaString::Format(_("%.2f"), m_Distance);
    m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
    if(m_Distance > 0.15)
    {
      m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);  
    }
    else
    {
      m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos2_1);
    }
    m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);
  }

  if(m_EndMeasure)
	{
	  counter = 0;
    CalculateMeasure();
    m_RendererVector.push_back(m_CurrentRenderer);

    if(albaString::Equals("*Error*",m_MeterVector[m_MeterVector.size()-1]->GetText()))
    {
      wxMessageBox(_("Impossible to measure the angle.\n Both the lines must have length > 0!"));
    }  
    
// persistent LINE1
    double tmpPt[3];
    m_LineSourceVector1.push_back(NULL);
    m_LineSourceVector1[m_LineSourceVector1.size()-1] = vtkLineSource::New();
    m_Line->GetPoint1(tmpPt);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->SetPoint1(tmpPt);
    m_Line->GetPoint2(tmpPt);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->SetPoint2(tmpPt);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->Update();

		/*
		vtkALBASmartPointer<vtkAppendPolyData> polyAppend;
		if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
		{
			vtkALBASmartPointer<vtkSphereSource> sphere; 
			sphere->SetCenter(tmpPt);
			sphere->SetRadius(m_Distance/100.0);
			sphere->SetThetaResolution(8);
			sphere->SetPhiResolution(8);
			sphere->Update();

			polyAppend->AddInput(m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetOutput());
			polyAppend->AddInput(sphere->GetOutput());
			polyAppend->Update();
		}
		*/

    m_LineMapperVector1.push_back(NULL);
    m_LineMapperVector1[m_LineMapperVector1.size()-1] = vtkPolyDataMapper2D::New();
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetTransformCoordinate(m_Coordinate);
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetInput(m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetOutput());

    m_LineActorVector1.push_back(NULL);
    m_LineActorVector1[m_LineActorVector1.size()-1] = vtkActor2D::New();
    m_LineActorVector1[m_LineActorVector1.size()-1]->SetMapper(m_LineMapperVector1[m_LineMapperVector1.size()-1]);
    m_LineActorVector1[m_LineActorVector1.size()-1]->GetProperty()->SetColor(m_Color);
    m_CurrentRenderer->AddActor2D(m_LineActorVector1[m_LineActorVector1.size()-1]);

		// glyph to emulate a arrow
		
		/* CONE	
			double tmpPtCS1[3];
			double tmpPtCS2[3];
			m_ConeSourceVector.push_back(NULL);
			m_ConeSourceVector[m_ConeSourceVector.size()-1] = vtkConeSource::New();
			m_Line->GetPoint1(tmpPtCS1);
			m_Line->GetPoint2(tmpPtCS2);
			double distanceCS12 = 0;
			distanceCS12 = sqrt(vtkMath::Distance2BetweenPoints(tmpPtCS1,tmpPtCS2));
			m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetCenter(tmpPtCS2);
			m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetRadius(distanceCS12/30.0);
      m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetHeight(distanceCS12/20.0);
      m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetResolution(8);
      double direction[3];
      direction[0] = tmpPtCS2[0] - tmpPtCS1[0];
      direction[1] = tmpPtCS2[1] - tmpPtCS1[1];
      direction[2] = tmpPtCS2[2] - tmpPtCS1[2];
      m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetDirection(direction);
			m_ConeSourceVector[m_ConeSourceVector.size()-1]->Update();
			

			m_ConeMapperVector.push_back(NULL);
			m_ConeMapperVector[m_ConeMapperVector.size()-1] = vtkPolyDataMapper2D::New();
			m_ConeMapperVector[m_ConeMapperVector.size()-1]->SetTransformCoordinate(m_Coordinate);
			m_ConeMapperVector[m_ConeMapperVector.size()-1]->SetInput(m_ConeSourceVector[m_ConeSourceVector.size()-1]->GetOutput());

			m_ConeActorVector.push_back(NULL);
			m_ConeActorVector[m_ConeActorVector.size()-1] = vtkActor2D::New();
			m_ConeActorVector[m_ConeActorVector.size()-1]->SetMapper(m_ConeMapperVector[m_ConeMapperVector.size()-1]);
			m_ConeActorVector[m_ConeActorVector.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);

		if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
		{
			m_CurrentRenderer->AddActor2D(m_ConeActorVector[m_ConeActorVector.size()-1]);
		}

    */
    // persistent LINE2
    double tmpPt2[3];
    m_LineSourceVector2.push_back(NULL);
    m_LineSourceVector2[m_LineSourceVector2.size()-1] = vtkLineSource::New();
    m_Line2->GetPoint1(tmpPt2);
    if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
      tmpPt2[2] = tmpPt[2];
    }
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint1(tmpPt2);
    m_Line2->GetPoint2(tmpPt2);
    if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
      tmpPt2[2] = tmpPt[2];
    }
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint2(tmpPt2);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->Update();

    m_LineMapperVector2.push_back(NULL);
    m_LineMapperVector2[m_LineMapperVector2.size()-1] = vtkPolyDataMapper2D::New();
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetTransformCoordinate(m_Coordinate);
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetInput(m_LineSourceVector2[m_LineMapperVector2.size()-1]->GetOutput());

    m_LineActorVector2.push_back(NULL);
    m_LineActorVector2[m_LineActorVector2.size()-1] = vtkActor2D::New();
    m_LineActorVector2[m_LineActorVector2.size()-1]->SetMapper(m_LineMapperVector2[m_LineMapperVector2.size()-1]);
    m_LineActorVector2[m_LineActorVector2.size()-1]->GetProperty()->SetColor(m_Color);
    m_CurrentRenderer->AddActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);

    //measure , measure type vectors
    if(m_MeasureType == DISTANCE_BETWEEN_LINES || m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      m_Measure.push_back(m_Distance);
      m_FlagMeasureType.push_back(ID_RESULT_MEASURE);
    }
    
		m_RegisterMeasure = true;

    // remove temporary measure
    m_CurrentRenderer->RemoveActor2D(m_LineActor);
    m_CurrentRenderer->RemoveActor2D(m_LineActor2);
	}
  
	m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::CalculateMeasure()
//----------------------------------------------------------------------------
{
  double p1_1[3],p2_1[3],p1_2[3],p2_2[3];

  m_Line->GetPoint1(p1_1);
  m_Line->GetPoint2(p2_1);
  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  {
    m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p1_1,p2_1));
    albaEventMacro(albaEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }

  m_Line2->GetPoint1(p1_2);
  m_Line2->GetPoint2(p2_2);
  if(m_MeasureType == DISTANCE_BETWEEN_LINES)
  {
    m_Distance = sqrt(vtkLine::DistanceToLine(p1_2,p1_1,p2_1));
    albaEventMacro(albaEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::CreateHistogram()
//----------------------------------------------------------------------------
{
  if (m_ProbedVME != NULL)
  {
    vtkDataSet *probed_data = m_ProbedVME->GetOutput()->GetVTKData();
    probed_data->Update();

    m_PlotActor->SetXRange(0,m_Distance);
    m_PlotActor->SetPlotCoordinate(0,m_Distance);

    double tmp1[3];
    if(m_LineSourceVector1.size() == 0)
      m_Line->GetPoint1(tmp1);
    else
      m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint1(tmp1);

    double tmp2[3];
    if(m_LineSourceVector1.size() == 0)
      m_Line->GetPoint2(tmp2);
    else
      m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint2(tmp2);

    double b[6];
    m_ProbedVME->GetOutput()->GetBounds(b);
    
    if(tmp1[0] < b[0]) tmp1[0] = b[0];
    else if (tmp1[0] > b[1]) tmp1[0] = b[1];

    if(tmp1[1] < b[2]) tmp1[1] = b[2];
    else if (tmp1[1] > b[3]) tmp1[1] = b[3];

    if(tmp1[2] < b[4]) tmp1[2] = b[4];
    else if (tmp1[2] > b[5]) tmp1[2] = b[5];

    if(tmp2[0] < b[0]) tmp2[0] = b[0];
    else if (tmp2[0] > b[1]) tmp2[0] = b[1];

    if(tmp2[1] < b[2]) tmp2[1] = b[2];
    else if (tmp2[1] > b[3]) tmp2[1] = b[3];

    if(tmp2[2] < b[4]) tmp2[2] = b[4];
    else if (tmp2[2] > b[5]) tmp2[2] = b[5];

    m_ProbingLine->SetPoint1(tmp1);
    m_ProbingLine->SetPoint2(tmp2);
    m_ProbingLine->SetResolution((int)m_Distance);
    m_ProbingLine->Update();

    vtkALBASmartPointer<vtkProbeFilter> prober;
    prober->SetInput(m_ProbingLine->GetOutput());
    prober->SetSource(probed_data);
    prober->Update();

    m_PlotActor->RemoveAllInputs();

    vtkPolyData *probimg_result = prober->GetPolyDataOutput();
    m_PlotActor->AddInput(probimg_result);
    m_HistogramRWI->m_RwiBase->Render();
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::RemoveMeter()
//----------------------------------------------------------------------------
{
  //if the current render window is not null remove the two actors 
  if (m_CurrentRenderer == NULL || m_Mouse->GetRenderer() == NULL) 
    return;
  m_CurrentRenderer->RemoveActor2D(m_LineActor);
  m_CurrentRenderer->RemoveActor2D(m_LineActor2);
  m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::GenerateHistogram(bool generate)
//----------------------------------------------------------------------------
{
  m_GenerateHistogram = generate;
  if (m_GenerateHistogram)
  {
    m_PlotActor->RemoveAllInputs();
    if (m_HistogramRWI)
    {
    	m_HistogramRWI->m_RwiBase->Render();
    }
    RemoveMeter();
    SetMeasureTypeToDistanceBetweenPoints();
   
  }

  if (m_HistogramDialog)
  {
  	m_HistogramDialog->Show(m_GenerateHistogram);
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::UndoMeasure()
//----------------------------------------------------------------------------
{
  if(m_RendererVector.size() != 0)
  {
    m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_MeterVector[m_MeterVector.size()-1]);
    vtkDEL(m_MeterVector[m_MeterVector.size()-1]);
    m_MeterVector.pop_back();

    m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_LineActorVector1[m_LineActorVector1.size()-1]);
    vtkDEL(m_LineActorVector1[m_LineActorVector1.size()-1]);
    m_LineActorVector1.pop_back();
    vtkDEL(m_LineMapperVector1[m_LineMapperVector1.size()-1]);
    m_LineMapperVector1.pop_back();
    vtkDEL(m_LineSourceVector1[m_LineSourceVector1.size()-1]);
    m_LineSourceVector1.pop_back();

/* CONE
		m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_ConeActorVector[m_ConeActorVector.size()-1]);
		vtkDEL(m_ConeActorVector[m_ConeActorVector.size()-1]);
		m_ConeActorVector.pop_back();
		vtkDEL(m_ConeMapperVector[m_ConeMapperVector.size()-1]);
		m_ConeMapperVector.pop_back();
		vtkDEL(m_ConeSourceVector[m_ConeSourceVector.size()-1]);
		m_ConeSourceVector.pop_back();
*/
    m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);
    vtkDEL(m_LineActorVector2[m_LineActorVector2.size()-1]);
    m_LineActorVector2.pop_back();
    vtkDEL(m_LineMapperVector2[m_LineMapperVector2.size()-1]);
    m_LineMapperVector2.pop_back();
    vtkDEL(m_LineSourceVector2[m_LineSourceVector2.size()-1]);
    m_LineSourceVector2.pop_back();

    m_RendererVector[m_RendererVector.size()-1]->GetRenderWindow()->Render();
    m_RendererVector.pop_back();
      
    //gui
    m_Measure.pop_back();
    m_FlagMeasureType.pop_back();

    if(m_FlagMeasureType.size()>0 && m_FlagMeasureType[m_FlagMeasureType.size()-1] == ID_RESULT_MEASURE)
    {
      m_Distance = m_Measure[m_Measure.size()-1];
    }
    if(m_LineActorVector1.size() != 0)
      CreateHistogram();
    else
      GenerateHistogram(m_GenerateHistogram);
    if(m_Measure.size() == 0)
    {
      albaEventMacro(albaEvent(this,ID_RESULT_MEASURE,0.0));
    }
    else
    {
      albaEventMacro(albaEvent(this,m_FlagMeasureType[m_FlagMeasureType.size()-1],m_Measure[m_Measure.size()-1]));
    }
    albaEventMacro(albaEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::SetMeasureType(int t)
//----------------------------------------------------------------------------
{
  switch(t) 
  {
    case DISTANCE_BETWEEN_LINES:
      m_MeasureType = DISTANCE_BETWEEN_LINES;
      GenerateHistogramOff();
  	break;
    default:
      m_MeasureType = DISTANCE_BETWEEN_POINTS;
  }
}
//----------------------------------------------------------------------------
bool albaInteractor2DDistance::IsDisableUndoAndOkCancel()
//----------------------------------------------------------------------------
{
  return m_DisableUndoAndOkCancel;
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::SetManualDistance(double manualDistance)
//----------------------------------------------------------------------------
{
  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  {
    if (manualDistance <= 0 || m_Measure[m_Measure.size()-1] == 0) return;
    //redraw the distance line from the first point to the new point,
    //calculated after insert distance in the field.
    double tmp1[3] = {0,0,0}; 
    double tmp2[3] = {0,0,0};

    m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint1(tmp1);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint2(tmp2);

    double tmp3[3] = {0,0,0};
    tmp3[0] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[0] - tmp1[0]) + tmp1[0];
		tmp3[1] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[1] - tmp1[1]) + tmp1[1];
		tmp3[2] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[2] - tmp1[2]) + tmp1[2];

    m_LineSourceVector1[m_LineSourceVector1.size()-1]->SetPoint2(tmp3);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->Update();

    /* CONE
		m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetCenter(tmp3);
		m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetRadius(manualDistance/30.0);
    m_ConeSourceVector[m_ConeSourceVector.size()-1]->SetHeight(manualDistance/20.0);
		m_ConeSourceVector[m_ConeSourceVector.size()-1]->Update();
    */


		/*
		vtkALBASmartPointer<vtkAppendPolyData> polyAppend;
		if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
		{
		vtkALBASmartPointer<vtkSphereSource> sphere; 
		sphere->SetCenter(tmpPt);
		sphere->SetRadius(m_Distance/100.0);
		sphere->SetThetaResolution(8);
		sphere->SetPhiResolution(8);
		sphere->Update();

		polyAppend->AddInput(m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetOutput());
		polyAppend->AddInput(sphere->GetOutput());
		polyAppend->Update();
		}
		*/

    albaString ds;
    ds = albaString::Format(_("%.2f") , manualDistance);
    if (!m_TestMode)
    {
    	m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
    }

    m_Measure[m_Measure.size()-1] = manualDistance;
  }
  else if (m_MeasureType == DISTANCE_BETWEEN_LINES)
  {
    if (manualDistance <= 0 || m_Measure[m_Measure.size()-1] == 0) return;
    
    double tmp1[3] = {0,0,0}; 
    double tmp2[3] = {0,0,0};

		double tmp3[3] = {0,0,0};
		double tmp4[3] = {0,0,0};

    m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint1(tmp1);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetPoint1(tmp2);

		m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint2(tmp3);
		m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetPoint2(tmp4);
    
    double tmp5[3] = {0,0,0};
		double tmp6[3] = {0,0,0};

		tmp5[0] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[0] - tmp1[0]) + tmp1[0];
		tmp5[1] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[1] - tmp1[1]) + tmp1[1];
		tmp5[2] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp2[2] - tmp1[2]) + tmp1[2];

		tmp6[0] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp4[0] - tmp3[0]) + tmp3[0];
		tmp6[1] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp4[1] - tmp3[1]) + tmp3[1];
		tmp6[2] = (manualDistance/m_Measure[m_Measure.size()-1]) * (tmp4[2] - tmp3[2]) + tmp3[2];

    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint1(tmp5);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint2(tmp6);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->Update();

    albaString ds;
    ds = albaString::Format(_("%.2f") , manualDistance);
    if (!m_TestMode)
    {
    	m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
    }

    m_Measure[m_Measure.size()-1] = manualDistance;
  }

  if (m_CurrentRenderer)
  {
  	m_CurrentRenderer->GetRenderWindow()->Render();
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::SetLabel(albaString label)
//----------------------------------------------------------------------------
{
	m_MeterVector[m_MeterVector.size()-1]->SetText(label);
	if (m_CurrentRenderer)
	{
		m_CurrentRenderer->GetRenderWindow()->Render();
	}
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::ShowOnlyLastMeasure( bool show )
//----------------------------------------------------------------------------
{
  //No check last actor
  for (int i=0;i<m_LineActorVector1.size()-1;i++)
  {
    if (m_LineActorVector1[i])
    {
      if (!show)
      {
        m_RendererVector[i]->AddActor2D(m_LineActorVector1[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_LineActorVector1[i]);
      }
    }

    if (m_LineActorVector2[i])
    {
      if (!show)
      {
        m_RendererVector[i]->AddActor2D(m_LineActorVector2[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_LineActorVector2[i]);
      }
    }

    if (m_MeterVector[i])
    {
      if (!show)
      {
        m_RendererVector[i]->AddActor2D(m_MeterVector[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_MeterVector[i]);
      }
    }
  }

  m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
albaRWIBase * albaInteractor2DDistance::GetCurrentRwi()
//----------------------------------------------------------------------------
{
  return m_CurrentRwi;
}
//----------------------------------------------------------------------------
void albaInteractor2DDistance::ShowAllMeasures( bool show )
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_LineActorVector1.size();i++)
  {
    if (m_LineActorVector1[i])
    {
      if (show)
      {
        m_RendererVector[i]->AddActor2D(m_LineActorVector1[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_LineActorVector1[i]);
      }
    }

    if (m_LineActorVector2[i])
    {
      if (show)
      {
        m_RendererVector[i]->AddActor2D(m_LineActorVector2[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_LineActorVector2[i]);
      }
    }

    if (m_MeterVector[i])
    {
      if (show)
      {
        m_RendererVector[i]->AddActor2D(m_MeterVector[i]);
      }
      else
      {
        m_RendererVector[i]->RemoveActor2D(m_MeterVector[i]);
      }
    }
  }

  if (m_CurrentRenderer)
  {
    m_CurrentRenderer->GetRenderWindow()->Render();
  }
}

//----------------------------------------------------------------------------
void albaInteractor2DDistance::SetColor(double r,double g,double b)
{
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
}

//----------------------------------------------------------------------------
albaInteractor2DDistance* albaInteractor2DDistance::NewTestInstance()
//----------------------------------------------------------------------------
{
  return new albaInteractor2DDistance(true); 
}