/*=========================================================================

 Program: MAF2
 Module: mafInteractor2DIndicator
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafInteractor2DIndicator.h"
#include "mafDecl.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafRWI.h"
#include "mafView.h"
#include "mafVME.h"
#include "mafEventInteraction.h"

#include "vtkMAFSmartPointer.h"
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
#include "vtkMAFTextActorMeter.h"

#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractor2DIndicator)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafInteractor2DIndicator::mafInteractor2DIndicator() 
//----------------------------------------------------------------------------
{
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

  m_CurrentRwi = NULL;
  m_CurrentRenderer  = NULL;
  m_LastRenderer     = NULL;
  m_PreviousRenderer   = NULL;

  m_Line->SetPoint1(0,0,0);
  m_Line->SetPoint2(0.5,0.5,0);
  m_Line->Update();
  m_LineMapper->SetInputConnection(m_Line->GetOutputPort());
  m_LineMapper->SetTransformCoordinate(m_Coordinate);
  m_LineActor->SetMapper(m_LineMapper);
  m_LineActor->GetProperty()->SetColor(1.0,0.0,0.0);

  m_Line2->SetPoint1(0,0,0);
  m_Line2->SetPoint2(0.5,0.5,0);
  m_Line2->Update();
  m_LineMapper2->SetInputConnection(m_Line2->GetOutputPort());
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
	

}
//----------------------------------------------------------------------------
mafInteractor2DIndicator::~mafInteractor2DIndicator() 
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

		m_RendererVector[i]->RemoveActor2D(m_ConeActorVector[i]);
		vtkDEL(m_ConeActorVector[i]);
		vtkDEL(m_ConeMapperVector[i]);
		vtkDEL(m_ConeSourceVector[i]);

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


	m_ConeSourceVector.clear();
	m_ConeMapperVector.clear();
	m_ConeActorVector.clear();


  m_LineSourceVector2.clear();
  m_LineMapperVector2.clear();
  m_LineActorVector2.clear();
  m_RendererVector.clear();
  

  
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2];
  e->Get2DPosition(pos_2d);
  
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  if(m_EndMeasure)
  { 
    RemoveMeter();
  }
  
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());
  mafDeviceButtonsPadMouse  *mouse  = mafDeviceButtonsPadMouse::SafeDownCast(device);
  if (m_Mouse == NULL)
  {
    m_Mouse = mouse;
  }
  m_CurrentRwi = m_Mouse->GetRWI();
  m_CurrentRenderer = m_Mouse->GetRenderer();
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
void mafInteractor2DIndicator::OnMiddleButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e); 
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnRightButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonDown(e); 
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnLeftButtonUp(mafEventInteraction *e)
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
      m_RegisterMeasure = false;
    }
  }
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnMiddleButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnRightButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnMove(mafEventInteraction *e) 
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
void mafInteractor2DIndicator::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
 	m_DraggingMouse = true;
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingMouse = false;
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::DrawMeasureTool(double x, double y)
//----------------------------------------------------------------------------
{
	static long counter = 0;
	static double dx, dy, dz;
  
  m_CurrentRwi =  m_Mouse->GetRWI();
  m_CurrentRenderer = m_Mouse->GetRenderer();
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
    m_MeterVector[m_MeterVector.size()-1] = vtkMAFTextActorMeter::New();
    // initialization

    

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
    //m_EndMeasure = true;
    m_PreviousRenderer = m_CurrentRenderer;
		counter++;
		
	}
	
	// add the  second line for the distance between lines mode
	else if(counter == 2)
	{
		m_Line2->SetPoint1(p);
		m_Line2->SetPoint2(p);
		m_Line2->Update();
		m_CurrentRenderer->AddActor2D(m_LineActor2);
		counter++;
	}
	else if(counter == 2 && m_DraggingLine)
	{
		counter++;	
	}
	// add the  second line for the angle between lines mode
	else if(counter == 3 && m_DraggingLeft)
	{
		m_Line2->SetPoint1(p);
		m_Line2->SetPoint2(p);
		m_Line2->Update();
	}
	else if(counter == 3)
	{
		m_EndMeasure = true;
	}
	
	
  // text in Render Window (if series for writing text)
     
    if (counter < 3)
		{
			double tmp_pos[3];
			m_Line->GetPoint2(tmp_pos);
			mafString ds;
      ds = wxString::Format(_("Label"));
		  m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
			m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
      m_DisableUndoAndOkCancel = true;
		}
		else
		{
			double tmp_pos[3];
			m_Line2->GetPoint2(tmp_pos);
			mafString ds;
      ds = wxString::Format(_("Label"));
		  m_MeterVector[m_MeterVector.size()-1]->SetText(ds);
			m_MeterVector[m_MeterVector.size()-1]->SetTextPosition(tmp_pos);
      m_DisableUndoAndOkCancel = false;
		}

    m_CurrentRenderer->AddActor2D(m_MeterVector[m_MeterVector.size()-1]);
  
  
  
	if(m_EndMeasure)
	{
	  counter = 0;
    
    m_RendererVector.push_back(m_CurrentRenderer);

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
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetInputConnection(m_LineSourceVector1[m_LineSourceVector1.size()-1]->GetOutputPort());

    m_LineActorVector1.push_back(NULL);
    m_LineActorVector1[m_LineActorVector1.size()-1] = vtkActor2D::New();
    m_LineActorVector1[m_LineActorVector1.size()-1]->SetMapper(m_LineMapperVector1[m_LineMapperVector1.size()-1]);
    m_LineActorVector1[m_LineActorVector1.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);
    m_CurrentRenderer->AddActor2D(m_LineActorVector1[m_LineActorVector1.size()-1]);

		// glyph to emulate a arrow
		
			
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
			m_ConeMapperVector[m_ConeMapperVector.size()-1]->SetInputConnection(m_ConeSourceVector[m_ConeSourceVector.size()-1]->GetOutputPort());

			m_ConeActorVector.push_back(NULL);
			m_ConeActorVector[m_ConeActorVector.size()-1] = vtkActor2D::New();
			m_ConeActorVector[m_ConeActorVector.size()-1]->SetMapper(m_ConeMapperVector[m_ConeMapperVector.size()-1]);
			m_ConeActorVector[m_ConeActorVector.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);

		
			m_CurrentRenderer->AddActor2D(m_ConeActorVector[m_ConeActorVector.size()-1]);
		
    // persistent LINE2
    double tmpPt2[3];
    m_LineSourceVector2.push_back(NULL);
    m_LineSourceVector2[m_LineSourceVector2.size()-1] = vtkLineSource::New();
    m_Line2->GetPoint1(tmpPt2);
    
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
      tmpPt2[2] = tmpPt[2];
    
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint1(tmpPt2);
    m_Line2->GetPoint2(tmpPt2);
    
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
      tmpPt2[2] = tmpPt[2];
    
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint2(tmpPt2);
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->Update();

    m_LineMapperVector2.push_back(NULL);
    m_LineMapperVector2[m_LineMapperVector2.size()-1] = vtkPolyDataMapper2D::New();
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetTransformCoordinate(m_Coordinate);
    m_LineMapperVector2[m_LineMapperVector2.size()-1]->SetInputConnection(m_LineSourceVector2[m_LineMapperVector2.size()-1]->GetOutputPort());

    m_LineActorVector2.push_back(NULL);
    m_LineActorVector2[m_LineActorVector2.size()-1] = vtkActor2D::New();
    m_LineActorVector2[m_LineActorVector2.size()-1]->SetMapper(m_LineMapperVector2[m_LineMapperVector2.size()-1]);
    m_LineActorVector2[m_LineActorVector2.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);
    m_CurrentRenderer->AddActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);

    m_RegisterMeasure = true;

    mafEventMacro(mafEvent(this,ID_RESULT_INDICATOR,true));
    //delete temporary measure
    m_CurrentRenderer->RemoveActor2D(m_LineActor);
    m_CurrentRenderer->RemoveActor2D(m_LineActor2);
	}
  
	m_CurrentRenderer->GetRenderWindow()->Render();

}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::RemoveMeter()
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
void mafInteractor2DIndicator::UndoMeasure()
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


		m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_ConeActorVector[m_ConeActorVector.size()-1]);
		vtkDEL(m_ConeActorVector[m_ConeActorVector.size()-1]);
		m_ConeActorVector.pop_back();
		vtkDEL(m_ConeMapperVector[m_ConeMapperVector.size()-1]);
		m_ConeMapperVector.pop_back();
		vtkDEL(m_ConeSourceVector[m_ConeSourceVector.size()-1]);
		m_ConeSourceVector.pop_back();

    m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);
    vtkDEL(m_LineActorVector2[m_LineActorVector2.size()-1]);
    m_LineActorVector2.pop_back();
    vtkDEL(m_LineMapperVector2[m_LineMapperVector2.size()-1]);
    m_LineMapperVector2.pop_back();
    vtkDEL(m_LineSourceVector2[m_LineSourceVector2.size()-1]);
    m_LineSourceVector2.pop_back();

    m_RendererVector[m_RendererVector.size()-1]->GetRenderWindow()->Render();
    m_RendererVector.pop_back();
      
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
bool mafInteractor2DIndicator::IsDisableUndoAndOkCancel()
//----------------------------------------------------------------------------
{
  return m_DisableUndoAndOkCancel;
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::SetLabel(mafString label)
//----------------------------------------------------------------------------
{
	m_MeterVector[m_MeterVector.size()-1]->SetText(label);
	m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::ShowOnlyLastMeasure( bool show )
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
mafRWIBase * mafInteractor2DIndicator::GetCurrentRwi()
  //----------------------------------------------------------------------------
{
  return m_CurrentRwi;
}
//----------------------------------------------------------------------------
void mafInteractor2DIndicator::ShowAllMeasures( bool show )
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
