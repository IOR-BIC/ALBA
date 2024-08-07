/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaInteractor2DAngle.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaInteractor2DAngle.h"
#include "albaDecl.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaRWI.h"
#include "albaView.h"
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

#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkRendererCollection.h"



//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor2DAngle)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaInteractor2DAngle::albaInteractor2DAngle() 
//----------------------------------------------------------------------------
{
  m_TestMode = false;
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

	
  m_DraggingLine  = false;
  m_DraggingLeft  = false;
  m_EndMeasure    = false;
  m_ParallelView  = false;
  m_DisableUndoAndOkCancel = false;
  m_Clockwise = false;
	m_RegisterMeasure = false;
  m_MeasureType = ANGLE_BETWEEN_POINTS;

  m_Color[0] = 0.0;
  m_Color[1] = 1.0;
  m_Color[2] = 0.0;

  
  m_Angle = 0;
  m_AbsoluteAngle = 0;
}
//----------------------------------------------------------------------------
albaInteractor2DAngle::~albaInteractor2DAngle() 
//----------------------------------------------------------------------------
{
  RemoveMeter();
  

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

  

}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnLeftButtonDown(albaEventInteraction *e)
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
    OnButtonDown(e);  
    
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
    // register the last renderer
    m_LastRenderer = m_CurrentRenderer;
    m_DraggingLeft = true;
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnMiddleButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e); 
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnRightButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonDown(e); 
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnLeftButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2] = {-1,-1};

  m_DraggingLeft = false;
	m_DraggingLine = false;
  OnButtonUp(e);

  if(m_ParallelView)
  {
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
    if(m_EndMeasure)
    {
      CalculateMeasure();
			m_RegisterMeasure = false;
    }
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnMiddleButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnRightButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnMove(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
	double pos_2d[2];
	
  if (!m_DraggingMouse) return;
  if((m_DraggingLeft || m_DraggingLine) && m_ParallelView)
  {
    e->Get2DPosition(pos_2d);
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
 	m_DraggingMouse = true;
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::OnButtonUp(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingMouse = false;
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::DrawMeasureTool(double x, double y)
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
    
    m_Angle = 0.0;

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

		
	}
	// finished dragging the second point
	else if(counter == 1)
	{
		
		counter++;
		
		if(m_MeasureType == ANGLE_BETWEEN_POINTS) 
			m_DraggingLine = true;
	}
	else if (counter == 2 && (m_CurrentRenderer != m_LastRenderer))
	{
		// remove the first line
		m_LastRenderer->RemoveActor2D(m_LineActor);
		m_LastRenderer->GetRenderWindow()->Render(); 
		// reset the counter
		counter = 0;   
	} 
	// add the  second line for the distance between lines mode
	else if(counter == 2 && m_DraggingLine)
	{
		if(m_MeasureType == ANGLE_BETWEEN_POINTS) 
		{
			double tmpPt[3];
			m_Line->GetPoint1(tmpPt);
			m_Line2->SetPoint1(tmpPt);
			m_Line2->SetPoint2(p);
			m_Line2->Update();
			m_CurrentRenderer->AddActor2D(m_LineActor2);
			counter++;
		}
	}
	// add the  second line for the angle between lines mode
	else if(counter == 2 && !m_DraggingLeft)
	{ 
		assert(m_MeasureType == ANGLE_BETWEEN_LINES);
		m_Line2->SetPoint1(p);
		m_Line2->SetPoint2(p);
		m_Line2->Update();
		m_CurrentRenderer->AddActor2D(m_LineActor2);
		counter++; 
	}
	else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_POINTS && m_DraggingLeft)
	{
		double tmpPt[3];
		m_Line->GetPoint1(tmpPt);
		m_Line2->SetPoint1(tmpPt);
		m_Line2->SetPoint2(p);
		m_Line2->Update();
	}
	else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_POINTS)
	{
		m_EndMeasure = true;
	}
	else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_LINES && m_DraggingLeft)
	{
		m_Line2->SetPoint2(p);
		m_Line2->Update();
	}
	else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_LINES)
	{
		m_EndMeasure = true;
	}

  // text in Render Window (if series for writing text)
  if(m_MeasureType == ANGLE_BETWEEN_LINES && counter == 3)
  { 
    //calculate the length to avoid zero length message of CalculateMeasure
    double v2[3] , v1[3];
    double tmp_pos1_1[3];
    double tmp_pos2_1[3];
    double tmp_pos1_2[3];
    double tmp_pos2_2[3];
    m_Line->GetPoint1(tmp_pos1_1);
    m_Line->GetPoint2(tmp_pos2_1);
    m_Line2->GetPoint1(tmp_pos1_2);
    m_Line2->GetPoint2(tmp_pos2_2);
    v2[0] = tmp_pos2_2[0] - tmp_pos1_2[0];
    v2[1] = tmp_pos2_2[1] - tmp_pos1_2[1];
    v2[2] = tmp_pos2_2[2] - tmp_pos1_2[2];

    v1[0] = tmp_pos2_1[0] - tmp_pos1_1[0];
    v1[1] = tmp_pos2_1[1] - tmp_pos1_1[1];
    v1[2] = tmp_pos2_1[2] - tmp_pos1_1[2];

    double tmp_pos[3];
    tmp_pos[0] = (tmp_pos1_1[0] + tmp_pos2_1[0] + tmp_pos1_2[0] + tmp_pos2_2[0])/4; 
    tmp_pos[1] = (tmp_pos1_1[1] + tmp_pos2_1[1] + tmp_pos1_2[1] + tmp_pos2_2[1])/4;
    tmp_pos[2] = (tmp_pos1_1[2] + tmp_pos2_1[2] + tmp_pos1_2[2] + tmp_pos2_2[2])/4;
    if(vtkMath::Norm(v2) && vtkMath::Norm(v1))
    {
      CalculateMeasure();
      albaString as;
      as = albaString::Format(_("%.2f"), m_Angle);
      m_MeterVector[m_MeterVector.size()-1]->SetText(as);
      m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
      m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);
    }
    else
    {
      m_MeterVector[m_MeterVector.size()-1]->SetText("*Error*");
      m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
      m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);
    }
  }
  else if(m_MeasureType == ANGLE_BETWEEN_POINTS && counter == 3)
  {
    CalculateMeasure();

    double tmp_pos2_1[3];
    double tmp_pos2_2[3];
    m_Line->GetPoint2(tmp_pos2_1);
    m_Line2->GetPoint2(tmp_pos2_2);

    albaString as;
    as = albaString::Format(_("%.2f deg"), m_Angle);
    m_MeterVector[m_MeterVector.size()-1]->SetText(as);
    m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos2_2);
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

    m_LineMapperVector1.push_back(NULL);
    m_LineMapperVector1[m_LineMapperVector1.size()-1] = vtkPolyDataMapper2D::New();
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetTransformCoordinate(m_Coordinate);
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetInput(m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetOutput());

    m_LineActorVector1.push_back(NULL);
    m_LineActorVector1[m_LineActorVector1.size()-1] = vtkActor2D::New();
    m_LineActorVector1[m_LineActorVector1.size()-1]->SetMapper(m_LineMapperVector1[m_LineMapperVector1.size()-1]);
    m_LineActorVector1[m_LineActorVector1.size()-1]->GetProperty()->SetColor(m_Color);
    m_CurrentRenderer->AddActor2D(m_LineActorVector1[m_LineActorVector1.size()-1]);

    // persistent LINE2
    double tmpPt2[3];
    m_LineSourceVector2.push_back(NULL);
    m_LineSourceVector2[m_LineSourceVector2.size()-1] = vtkLineSource::New();
    m_Line2->GetPoint1(tmpPt2);
    
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint1(tmpPt2);
    m_Line2->GetPoint2(tmpPt2);
    
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint2(tmpPt2);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->Update();

    m_LineMapperVector2.push_back(NULL);
    m_LineMapperVector2[m_LineMapperVector2.size()-1] = vtkPolyDataMapper2D::New();
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetTransformCoordinate(m_Coordinate);
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetInput(m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetOutput());

    m_LineActorVector2.push_back(NULL);
    m_LineActorVector2[m_LineActorVector2.size()-1] = vtkActor2D::New();
    m_LineActorVector2[m_LineActorVector2.size()-1]->SetMapper(m_LineMapperVector2[m_LineMapperVector2.size()-1]);
    m_LineActorVector2[m_LineActorVector2.size()-1]->GetProperty()->SetColor(m_Color);
    m_CurrentRenderer->AddActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);

    //measure , measure type vectors
    
    m_Measure.push_back(m_Angle);
    m_FlagMeasureType.push_back(ID_RESULT_ANGLE);
    
		m_RegisterMeasure = true;
    
    //delete temporary measure
    m_CurrentRenderer->RemoveActor2D(m_LineActor);
    m_CurrentRenderer->RemoveActor2D(m_LineActor2);
	}
  
  
	m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::CalculateMeasure()
//----------------------------------------------------------------------------
{
  double p1_1[3],p2_1[3],p1_2[3],p2_2[3];

  m_Line->GetPoint1(p1_1);
  m_Line->GetPoint2(p2_1);
	m_Line2->GetPoint1(p1_2);
	m_Line2->GetPoint2(p2_2);

  if(m_MeasureType == ANGLE_BETWEEN_LINES || m_MeasureType == ANGLE_BETWEEN_POINTS)
  {
    double angle, v1[3], vn1, v2[3], vn2, s;
    v1[0] = p2_1[0] - p1_1[0];
    v1[1] = p2_1[1] - p1_1[1];
    v1[2] = p2_1[2] - p1_1[2];
    v2[0] = p2_2[0] - p1_2[0];
    v2[1] = p2_2[1] - p1_2[1];
    v2[2] = p2_2[2] - p1_2[2];

    vn1 = vtkMath::Norm(v1);
    vn2 = vtkMath::Norm(v2);
    s = vtkMath::Dot(v1,v2);

    if(vn1 != 0 && vn2 != 0)
      angle = acos(s / (vn1 * vn2));
    else
    {
      angle = 0;
      
    }
    
    angle *= vtkMath::RadiansToDegrees();
    m_AbsoluteAngle = angle;
    //if(angle >= 90.0 && m_MeasureType == ANGLE_BETWEEN_LINES) 
    //  angle = 180.0 - angle; 

    m_Angle = angle;
    albaEventMacro(albaEvent(this,ID_RESULT_ANGLE,angle));
    return;
  } 
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::RemoveMeter()
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
void albaInteractor2DAngle::UndoMeasure()
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

    
    if (m_FlagMeasureType.size()>0 && m_FlagMeasureType[m_FlagMeasureType.size()-1] == ID_RESULT_ANGLE)
    {
      m_Angle = m_Measure[m_Measure.size()-1];
    }
    if(m_Measure.size() == 0)
    {
      albaEventMacro(albaEvent(this,ID_RESULT_ANGLE,0.0));
    }
    else
    {
      albaEventMacro(albaEvent(this,m_FlagMeasureType[m_FlagMeasureType.size()-1],m_Measure[m_Measure.size()-1]));
    }
    albaEventMacro(albaEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::SetMeasureType(int t)
//----------------------------------------------------------------------------
{
  switch(t) 
  {
    case ANGLE_BETWEEN_LINES:
      m_MeasureType = ANGLE_BETWEEN_LINES;
      
    break;
	  case ANGLE_BETWEEN_POINTS:
		  m_MeasureType = ANGLE_BETWEEN_POINTS;
		  
		break;
    default:
      m_MeasureType = ANGLE_BETWEEN_POINTS;
  }
}
//----------------------------------------------------------------------------
bool albaInteractor2DAngle::IsDisableUndoAndOkCancel()
//----------------------------------------------------------------------------
{
  return m_DisableUndoAndOkCancel;
}

//----------------------------------------------------------------------------
void albaInteractor2DAngle::SetManualAngle(double manualAngle)
//----------------------------------------------------------------------------
{
  if (m_MeasureType == ANGLE_BETWEEN_POINTS || m_MeasureType == ANGLE_BETWEEN_LINES )
  {
    if (manualAngle <= 0) return;

    double dirProj[3]; // opposite direction of view plane normal
		double viewUp[3]; // camera view UP
    if (!m_RendererVector.empty()) {
		  m_RendererVector[m_RendererVector.size()-1]->GetActiveCamera()->GetViewPlaneNormal(dirProj);
		  m_RendererVector[m_RendererVector.size()-1]->GetActiveCamera()->GetViewUp(viewUp);
    }

		double res[3]; //perpendicular axes
		double *u = viewUp;
		double *v = dirProj;
		res[0] = u[1] * v[2] - u[2] * v[1];
		res[1] = u[2] * v[0] - u[0] * v[2];
		res[2] = u[0] * v[1] - u[1] * v[0];
	  
  

    //redraw the  line from the first point to the new point,
    //calculated after insert angle in the field.
    double tmp1[3] = {0,0,0}; 
    double tmp2[3] = {0,0,0};

    double tmp3[3] = {0,0,0};
    double tmp4[3] = {0,0,0};

    m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint1(tmp1);
    m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetPoint2(tmp2);

    m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetPoint1(tmp3);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetPoint2(tmp4);
		
		double tmp5[3] = {0,0,0};

    // projections along plane versors defined by the camera
    double tmp1P[3];
    tmp1P[0] = tmp1[0] * res[0] + tmp1[1]*res[1] + tmp1[2]*res[2];
    tmp1P[1] = tmp1[0] * viewUp[0] + tmp1[1]*viewUp[1] + tmp1[2]*viewUp[2];
    tmp1P[2] = tmp1[0] * dirProj[0] + tmp1[1]*dirProj[1] + tmp1[2]*dirProj[2];

    
    double tmp2P[3];
    tmp2P[0] = tmp2[0] * res[0] + tmp2[1]*res[1] + tmp2[2]*res[2];
    tmp2P[1] = tmp2[0] * viewUp[0] + tmp2[1]*viewUp[1] + tmp2[2]*viewUp[2];
    tmp2P[2] = tmp2[0] * dirProj[0] + tmp2[1]*dirProj[1] + tmp2[2]*dirProj[2];

    
    double tmp3P[3];
    tmp3P[0] = tmp3[0] * res[0] + tmp3[1]*res[1] + tmp3[2]*res[2];
    tmp3P[1] = tmp3[0] * viewUp[0] + tmp3[1]*viewUp[1] + tmp3[2]*viewUp[2];
    tmp3P[2] = tmp3[0] * dirProj[0] + tmp3[1]*dirProj[1] + tmp3[2]*dirProj[2];

    
    double tmp4P[3];
    tmp4P[0] = tmp4[0] * res[0] + tmp4[1]*res[1] + tmp4[2]*res[2];
    tmp4P[1] = tmp4[0] * viewUp[0] + tmp4[1]*viewUp[1] + tmp4[2]*viewUp[2];
    tmp4P[2] = tmp4[0] * dirProj[0] + tmp4[1]*dirProj[1] + tmp4[2]*dirProj[2];

    tmp1[0] = tmp1P[0];
    tmp1[1] = tmp1P[1];
    tmp1[2] = tmp1P[2];

    tmp2[0] = tmp2P[0];
    tmp2[1] = tmp2P[1];
    tmp2[2] = tmp2P[2];

    tmp3[0] = tmp3P[0];
    tmp3[1] = tmp3P[1];
    tmp3[2] = tmp3P[2];

    tmp4[0] = tmp4P[0];
    tmp4[1] = tmp4P[1];
    tmp4[2] = tmp4P[2];


    tmp5[0] = tmp4[0];
    tmp5[1] = tmp4[1];
    tmp5[2] = tmp4[2];

 
	  bool particularCase = false; 
  
	  //particularCase
    if(tmp2[0] == tmp1[0])
    {
			particularCase = true;
      double m2;
      if(tmp4[0] != tmp3[0])
      {
        m2 = (tmp4[1] - tmp3[1]) / (tmp4[0] - tmp3[0]);
      }
      else
        m2 = 1; 

      if(tmp2[1] > tmp1[1] && tmp3[0] < tmp4[0]) m_Clockwise = true;
      else if (tmp2[1] > tmp1[1] && tmp3[0] > tmp4[0]) m_Clockwise = false;
      else if (tmp2[1] < tmp1[1] && tmp3[0] > tmp4[0]) m_Clockwise = true;
      else if (tmp2[1] < tmp1[1] && tmp3[0] < tmp4[0]) m_Clockwise = false;
        

      double deltaY = 0, deltaX = 0;
      double line2Length = 0;
      line2Length= sqrt(vtkMath::Distance2BetweenPoints(tmp3,tmp4));
      deltaY = line2Length * sin(manualAngle/vtkMath::RadiansToDegrees());
      deltaX = line2Length * cos(manualAngle/vtkMath::RadiansToDegrees());

      deltaY = abs(deltaY);
      deltaX = abs(deltaX);

      if(tmp2[1] > tmp1[1])
      {
        if(manualAngle <= 90 && m_Clockwise == true)
        {
          tmp5[0] = tmp3[0] + deltaY;
          tmp5[1] = tmp3[1] + deltaX;
        }
        else if (manualAngle > 90 && m_Clockwise == true)
        {
          tmp5[0] = tmp3[0] + deltaY;
          tmp5[1] = tmp3[1] - deltaX;
        }
        else if(manualAngle <= 90 && m_Clockwise == false)
        {
          tmp5[0] = tmp3[0] - deltaY;
          tmp5[1] = tmp3[1] + deltaX;
        }
        else if (manualAngle > 90 && m_Clockwise == false)
        {
          tmp5[0] = tmp3[0] - deltaY;
          tmp5[1] = tmp3[1] - deltaX;
        }
      }
      else
      {
        if(manualAngle <= 90 && m_Clockwise == true)
        {
          tmp5[0] = tmp3[0] - deltaY;
          tmp5[1] = tmp3[1] - deltaX;
        }
        else if (manualAngle > 90 && m_Clockwise == true)
        {
          tmp5[0] = tmp3[0] - deltaY;
          tmp5[1] = tmp3[1] + deltaX;
        }
        else if(manualAngle <= 90 && m_Clockwise == false)
        {
          tmp5[0] = tmp3[0] + deltaY;
          tmp5[1] = tmp3[1] - deltaX;
        }
        else if (manualAngle > 90 && m_Clockwise == false)
        {
          tmp5[0] = tmp3[0] + deltaY;
          tmp5[1] = tmp3[1] + deltaX;
        }
      }
    }
 
  if(tmp4[0] == tmp3[0] && tmp2[0] != tmp1[0])
  {
    double m1;
    m1 = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
    
    if(tmp2[0] > tmp1[0] && tmp4[1] > tmp3[1])
    {
      m_Clockwise = false;
      tmp4[0] = tmp4[0] + tmp4[0]/100;
    }
    else if (tmp2[0] > tmp1[0] && tmp4[1] < tmp3[1])
    {
      m_Clockwise = true;
      tmp4[0] = tmp4[0] + tmp4[0]/100;
    }
    else if (tmp2[0] < tmp1[0] && tmp4[1] > tmp3[1])
    {
      m_Clockwise = true;
      tmp4[0] = tmp4[0] - tmp4[0]/100;
    }
    else if (tmp2[0] < tmp1[0] && tmp4[1] < tmp3[1])
    {
      m_Clockwise = false;
      tmp4[0] = tmp4[0] - tmp4[0]/100;
    }
  }
  if(particularCase == false)
	{
    double m1;
    m1 = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
 
    if(tmp2[0] > tmp1[0] && tmp2[1] >= tmp1[1])
    {
      // first quadrant
      double m;
      double mLine2;
      mLine2 = (tmp4[1] - tmp3[1])/(tmp4[0] - tmp3[0]);
      
      if(tmp4[0] > tmp3[0] && tmp4[1] >= tmp3[1])
      {
        if(mLine2 < m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] > tmp3[0] && tmp4[1] < tmp3[1])
      {
        m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );
        m_Clockwise = true;
      }

      if(tmp4[0] <= tmp3[0] && tmp4[1] < tmp3[1])
      {
        if(mLine2 > m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] < tmp3[0] && tmp4[1] >= tmp3[1])
      {
        m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );
        m_Clockwise = false;
      }

      double angle;
      angle = atan(m);

      double deltaY = 0, deltaX = 0;
      double line2Length = 0;
      line2Length= sqrt(vtkMath::Distance2BetweenPoints(tmp3,tmp4));
      deltaY = line2Length * sin(angle);
      deltaX = line2Length * cos(angle);
      
      
      double m1QUad;
      m1QUad = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
   
      double angle1QUad;
      angle1QUad = atan(m1QUad);
      angle1QUad *= vtkMath::RadiansToDegrees();
      angle1QUad = 90 - angle1QUad;

      double searchAngleQuadrant;
      searchAngleQuadrant = manualAngle + angle1QUad;
      if(m_Clockwise == false)
        searchAngleQuadrant = manualAngle + 90 - angle1QUad;
      
     if(searchAngleQuadrant <= 180)
     {
       if(searchAngleQuadrant > 90 && m_Clockwise == false)
       {
        tmp5[0] = tmp3[0] - deltaX;
        tmp5[1] = tmp3[1] - deltaY;
       }
       else
       {
        tmp5[0] = tmp3[0] + deltaX;
        tmp5[1] = tmp3[1] + deltaY;
       }
     }
     else
     {
       tmp5[0] = tmp3[0] - deltaX;
       tmp5[1] = tmp3[1] - deltaY;
     }
    }
    else if(tmp2[0] >= tmp1[0] && tmp2[1] < tmp1[1])
    {
      // second quadrant
      double m;
      double mLine2;
      mLine2 = (tmp4[1] - tmp3[1])/(tmp4[0] - tmp3[0]);
      
      if(tmp4[0] > tmp3[0] && tmp4[1] >= tmp3[1])
      {
        m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
        m_Clockwise = false;
      }

      if(tmp4[0] > tmp3[0] && tmp4[1] < tmp3[1])
      {
       if(mLine2 < m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] <= tmp3[0] && tmp4[1] < tmp3[1])
      {
        m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
        m_Clockwise = true;
      }

      if(tmp4[0] < tmp3[0] && tmp4[1] >= tmp3[1])
      {
        if(mLine2 > m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }
      double angle;
      angle = atan(-1/m);

      double deltaY = 0, deltaX = 0;
      double line2Length = 0;
      line2Length= sqrt(vtkMath::Distance2BetweenPoints(tmp3,tmp4));
      deltaY = line2Length * sin(angle);
      deltaX = line2Length * cos(angle);

      double m2QUad;
      m2QUad = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
     
      double angle2QUad;
      angle2QUad = atan(-1/m2QUad);
      angle2QUad *= vtkMath::RadiansToDegrees();
      angle2QUad = 90 - angle2QUad;

      double searchAngleQuadrant;
      searchAngleQuadrant = manualAngle + angle2QUad;
      if(m_Clockwise == false)
        searchAngleQuadrant = manualAngle + 90 - angle2QUad;

      if(searchAngleQuadrant <= 180)
      {
        if(searchAngleQuadrant > 90  && m_Clockwise == false)
        {       
          tmp5[0] = tmp3[0] - deltaY;
          tmp5[1] = tmp3[1] + deltaX;
        }
        else
        {
          tmp5[0] = tmp3[0] + deltaY;
          tmp5[1] = tmp3[1] - deltaX;
        }
      }
      else
      {
        tmp5[0] = tmp3[0] - deltaY;
        tmp5[1] = tmp3[1] + deltaX;
      }
    }
    else if(tmp2[0] < tmp1[0] && tmp2[1] <= tmp1[1])
    {
      // third quadrant
      double m;
      double mLine2;
      mLine2 = (tmp4[1] - tmp3[1])/(tmp4[0] - tmp3[0]);
      
      if(tmp4[0] > tmp3[0] && tmp4[1] >= tmp3[1])
      {
        if(mLine2 > m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] > tmp3[0] && tmp4[1] < tmp3[1])
      {
        m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );
        m_Clockwise = false;
      }

      if(tmp4[0] <= tmp3[0] && tmp4[1] < tmp3[1])
      {
        if(mLine2 < m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] < tmp3[0] && tmp4[1] >= tmp3[1])
      {
        
        m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );
        m_Clockwise = true;
      }
 
      double angle;
      angle = atan(m);

      double deltaY = 0, deltaX = 0;
      double line2Length = 0;
      line2Length= sqrt(vtkMath::Distance2BetweenPoints(tmp3,tmp4));
      deltaY = line2Length * sin(angle);
      deltaX = line2Length * cos(angle);


      double m3QUad;
      m3QUad = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
   
      double angle3QUad;
      angle3QUad = atan(m3QUad);
      angle3QUad *= vtkMath::RadiansToDegrees();
      angle3QUad = 90 - angle3QUad;

      double searchAngleQuadrant;
      searchAngleQuadrant = manualAngle + angle3QUad;

      if(m_Clockwise == false)
        searchAngleQuadrant = manualAngle + 90 - angle3QUad;

     if(searchAngleQuadrant <= 180)
     {
       if(searchAngleQuadrant > 90 && m_Clockwise == false)
       {
        tmp5[0] = tmp3[0] + deltaX;
        tmp5[1] = tmp3[1] + deltaY;
       }
       else
       {
        tmp5[0] = tmp3[0] - deltaX;
        tmp5[1] = tmp3[1] - deltaY;
       }
     }
     else
     {
       tmp5[0] = tmp3[0] + deltaX;
       tmp5[1] = tmp3[1] + deltaY;
     }
    }
    else if(tmp2[0] <= tmp1[0] && tmp2[1] > tmp1[1])
    {
      // fourth quadrant
      double m;
      double mLine2;
      mLine2 = (tmp4[1] - tmp3[1])/(tmp4[0] - tmp3[0]);
      
      if(tmp4[0] > tmp3[0] && tmp4[1] >= tmp3[1])
      {  
        m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
        m_Clockwise = true;
      }

      if(tmp4[0] > tmp3[0] && tmp4[1] < tmp3[1])
      {
       if(mLine2 > m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      if(tmp4[0] <= tmp3[0] && tmp4[1] < tmp3[1])
      {
        m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
        m_Clockwise = false;
      }

      if(tmp4[0] < tmp3[0] && tmp4[1] >= tmp3[1])
      {
        if(mLine2 < m1)
        {
          m = (-tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 +1 );  
          m_Clockwise = true;
        }
        else
        {
          m = -(tan(manualAngle/vtkMath::RadiansToDegrees()) + m1) / ( tan(manualAngle/vtkMath::RadiansToDegrees()) * m1 -1 );  
          m_Clockwise = false;
        }
      }

      double angle;
      angle = atan(-1/m);

      double deltaY = 0, deltaX = 0;
      double line2Length = 0;
      line2Length= sqrt(vtkMath::Distance2BetweenPoints(tmp3,tmp4));
      deltaY = line2Length * sin(angle);
      deltaX = line2Length * cos(angle);

      double m4QUad;
      m4QUad = (tmp2[1] - tmp1[1]) / (tmp2[0] - tmp1[0]);
     
      double angle4QUad;
      angle4QUad = atan(-1/m4QUad);
      angle4QUad *= vtkMath::RadiansToDegrees();
      angle4QUad = 90 - angle4QUad;

      double searchAngleQuadrant;
      searchAngleQuadrant = manualAngle + angle4QUad;

      if(m_Clockwise == false)
        searchAngleQuadrant = manualAngle + 90 - angle4QUad;

      if(searchAngleQuadrant <= 180)
      {
        if(searchAngleQuadrant > 90 && m_Clockwise == false)
       {
        tmp5[0] = tmp3[0] + deltaY;
        tmp5[1] = tmp3[1] - deltaX;
       }
       else
       {
        tmp5[0] = tmp3[0] - deltaY;
        tmp5[1] = tmp3[1] + deltaX;
       }
      }
      else
      {
        tmp5[0] = tmp3[0] + deltaY;
        tmp5[1] = tmp3[1] - deltaX;
      }
    }
    }//particular case
    double tmp5P[3];

    tmp5P[0] = tmp5[0] * res[0] + tmp5[1] * viewUp[0] + tmp5[2] * dirProj[0];
    tmp5P[1] = tmp5[0] * res[1] + tmp5[1] * viewUp[1] + tmp5[2] * dirProj[1];
    tmp5P[2] = tmp5[0] * res[2] + tmp5[1] * viewUp[2] + tmp5[2] * dirProj[2];

    tmp5[0] = tmp5P[0];
    tmp5[1] = tmp5P[1];
    tmp5[2] = tmp5P[2];

    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint2(tmp5);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->Update();

    albaString ds;
    albaString degree = "�";
    ds = albaString::Format(_("%.2f")  , manualAngle);
    ds.Append(degree);
    if (!m_TestMode)
    {
      m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
    }
    
    double tmp_pos[3];
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->GetPoint2(tmp_pos);
    if (!m_TestMode)
    {
      m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
    }
    
    m_Measure[m_Measure.size()-1] = manualAngle;   
  }
  if (!m_RendererVector.empty()) {
    m_RendererVector[m_RendererVector.size()-1]->GetRenderWindow()->Render();
    m_RendererVector[m_RendererVector.size()-1]->GetRenderWindow()->Render();
  }
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::SetLabel(albaString label)
//----------------------------------------------------------------------------
{
	m_MeterVector[m_MeterVector.size()-1]->SetText(label);
	m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::ShowOnlyLastMeasure( bool show )
  //----------------------------------------------------------------------------
{
  //No check last actor
  for (int i=0;i<m_LineActorVector1.size()-1;i++)
  {
    //Set opacity to 0.0 if actor should be show off otherwise set opacity to 1.0
    double opacity = show ? 0.0 : 1.0;
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
albaRWIBase * albaInteractor2DAngle::GetCurrentRwi()
  //----------------------------------------------------------------------------
{
  return m_CurrentRwi;
}
//----------------------------------------------------------------------------
void albaInteractor2DAngle::ShowAllMeasures( bool show )
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

void albaInteractor2DAngle::SetColor(double r,double g,double b)
{
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;
}