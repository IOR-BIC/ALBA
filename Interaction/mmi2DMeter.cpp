/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi2DMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-18 13:45:44 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmi2DMeter.h"
#include "mafDecl.h"
#include "mmdMouse.h"

#include "mafVME.h"
#include "mafEventInteraction.h"

#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmi2DMeter)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmi2DMeter::mmi2DMeter() 
//----------------------------------------------------------------------------
{
  Coordinate = vtkCoordinate::New();
  Coordinate->SetCoordinateSystemToNormalizedViewport();

  Line        = vtkLineSource::New();
  LineMapper  = vtkPolyDataMapper2D::New();
  LineActor   = vtkActor2D::New();
  Line2       = vtkLineSource::New();
  LineMapper2 = vtkPolyDataMapper2D::New();
  LineActor2  = vtkActor2D::New();

  CurrentRenderer  = NULL;
  LastRenderer     = NULL;

  Line->SetPoint1(0,0,0);
  Line->SetPoint2(0.5,0.5,0);
  Line->Update();
  LineMapper->SetInput(Line->GetOutput());
  LineMapper->SetTransformCoordinate(Coordinate);
  LineActor->SetMapper(LineMapper);
  LineActor->GetProperty()->SetColor(1.0,0.0,0.0);

  Line2->SetPoint1(0,0,0);
  Line2->SetPoint2(0.5,0.5,0);
  Line2->Update();
  LineMapper2->SetInput(Line2->GetOutput());
  LineMapper2->SetTransformCoordinate(Coordinate);
  LineActor2->SetMapper(LineMapper2);
  LineActor2->GetProperty()->SetColor(1.0,0.0,0.0);

	m_dragging_line = false;
  EndMeasure  = false;
  MeasureType = DISTANCE_BETWEEN_POINTS;
}
//---------------------------------------------------------------------
mmi2DMeter::~mmi2DMeter() 
//----------------------------------------------------------------------------
{
  vtkDEL(Line);
  vtkDEL(LineMapper);
  vtkDEL(LineActor);
  vtkDEL(Line2);
  vtkDEL(LineMapper2);
  vtkDEL(LineActor2);
  vtkDEL(Coordinate);
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2];
  e->Get2DPosition(pos_2d);
  
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());
  mmdMouse  *mouse  = mmdMouse::SafeDownCast(device);

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  if(EndMeasure)
  { 
    RemoveMeter();
  }
  
  CurrentRenderer = mouse->GetRenderer();
  
  OnButtonDown(e);  

  DrawMeasureTool(pos_2d[0], pos_2d[1]);

  // register the last renderer
  LastRenderer = CurrentRenderer;   
  m_dragging_left = true;
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnMiddleButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e); 
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnRightButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonDown(e); 
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnLeftButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  double pos_2d[2] = {0,0};
  
  m_dragging_left = false;
	m_dragging_line = false;
  OnButtonUp(e);

  DrawMeasureTool(pos_2d[0], pos_2d[1]);
  if(EndMeasure)
    CalculateMeasure();
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnMiddleButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnRightButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnLeftButtonUp(e);
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnMove(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
	double pos_2d[2];
	
  if (!m_dragging) return;

  if(m_dragging_left || m_dragging_line)
  {
    e->Get2DPosition(pos_2d);
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
  }
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
 	m_dragging = true;
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_dragging = false;
}
//----------------------------------------------------------------------------
void mmi2DMeter::DrawMeasureTool(double x, double y)
//----------------------------------------------------------------------------
{
  static long counter = 0;
	static double dx, dy, dz;

  if (CurrentRenderer == NULL)
  {
    return;
  }

  CurrentRenderer->SetDisplayPoint(x,y,0);
  CurrentRenderer->DisplayToView();
  CurrentRenderer->ViewportToNormalizedViewport(x,y);

  // no point has yet been picked
  if(counter == 0)
  {
    EndMeasure = false;
    CurrentRenderer->RemoveActor2D(LineActor2);
    Line->SetPoint1(x,y,0);
    Line->SetPoint2(x,y,0);
    Line->Update();
    CurrentRenderer->AddActor2D(LineActor);
    counter++;
  }
  // first point has been picked and the second one is being dragged
  else if(counter == 1 && m_dragging_left)
  {
    Line->SetPoint2(x,y,0);
    Line->Update();
  }
  // finished dragging the second point
  else if(counter == 1)
  {
    if(MeasureType == DISTANCE_BETWEEN_POINTS)
      EndMeasure = true;
    else
      counter++;

		if(MeasureType == DISTANCE_BETWEEN_LINES) 
      m_dragging_line = true;
  }
  else if (counter == 2 && (CurrentRenderer != LastRenderer))
  {
    // remove the first line
    LastRenderer->RemoveActor2D(LineActor);
    LastRenderer->GetRenderWindow()->Render(); 
    // reset the counter
    counter = 0;   
  } 
  // add the  second line for the distance between lines mode
  else if(counter == 2 && m_dragging_line)
  {
    assert(MeasureType = DISTANCE_BETWEEN_LINES);
    // add the second line 
		double tmpPt[3];
		Line->GetPoint1(tmpPt);
    Line2->SetPoint1(tmpPt);
		dx=tmpPt[0];
		dy=tmpPt[1];
		dz=tmpPt[2];
		Line->GetPoint2(tmpPt);
    Line2->SetPoint2(tmpPt);
		dx-=tmpPt[0];
		dy-=tmpPt[1];
		dz-=tmpPt[2];
    Line2->Update();
    CurrentRenderer->AddActor2D(LineActor2);
    counter++;
  }
  // add the  second line for the angle between lines mode
  else if(counter == 2 && !m_dragging_left)
  { 
    assert(MeasureType == ANGLE_BETWEEN_LINES);
    Line2->SetPoint1(x,y,0);
    Line2->SetPoint2(x,y,0);
    Line2->Update();
    CurrentRenderer->AddActor2D(LineActor2);
    counter++; 
  }
  else if(counter == 3 && MeasureType == DISTANCE_BETWEEN_LINES && m_dragging_line)
  {
    Line2->SetPoint2(x,y,0);
    Line2->SetPoint1(x+dx,y+dy,dz);
    Line2->Update();
  }
  else if(counter == 3 && MeasureType == DISTANCE_BETWEEN_LINES)
  {
    EndMeasure = true;
  }
  else if(counter == 3 && MeasureType == ANGLE_BETWEEN_LINES && m_dragging_left)
  {
    Line2->SetPoint2(x,y,0);
    Line2->Update();
  }
  else if(counter == 3 && MeasureType == ANGLE_BETWEEN_LINES)
  {
    EndMeasure = true;
  }

  if(EndMeasure)
  {
    counter = 0; 
  }

  CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmi2DMeter::CalculateMeasure()
//----------------------------------------------------------------------------
{
  double p1_1[3],p2_1[3],p1_2[3],p2_2[3],vx,vy,vz,d;

  Line->GetPoint1(p1_1);
  Line->GetPoint2(p2_1);

  vx = p1_1[0];
  vy = p1_1[1];
  vz = p1_1[2];
  CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  CurrentRenderer->ViewToWorld(vx,vy,vz);
  p1_1[0] = vx;
  p1_1[1] = vy;
  p1_1[2] = vz;

  vx = p2_1[0];
  vy = p2_1[1];
  vz = p2_1[2];
  CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  CurrentRenderer->ViewToWorld(vx,vy,vz);
  p2_1[0] = vx;
  p2_1[1] = vy;
  p2_1[2] = vz;

  if(MeasureType == DISTANCE_BETWEEN_POINTS)
  {
    d = sqrt(vtkMath::Distance2BetweenPoints(p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,d));
    return;
  }

  Line2->GetPoint1(p1_2);
  Line2->GetPoint2(p2_2);

  vx = p1_2[0];
  vy = p1_2[1];
  vz = p1_2[2];
  CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  CurrentRenderer->ViewToWorld(vx,vy,vz);
  p1_2[0] = vx;
  p1_2[1] = vy;
  p1_2[2] = vz;

  vx = p2_2[0];
  vy = p2_2[1];
  vz = p2_2[2];
  CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  CurrentRenderer->ViewToWorld(vx,vy,vz);
  p2_2[0] = vx;
  p2_2[1] = vy;
  p2_2[2] = vz;

  if(MeasureType == DISTANCE_BETWEEN_LINES)
  {
    d = sqrt(vtkLine::DistanceToLine(p1_2,p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,d));
    return;
  }

  if(MeasureType == ANGLE_BETWEEN_LINES)
  {
    float angle, v1[3], vn1, v2[3], vn2, s;
    v1[0] = fabs(p2_1[0] - p1_1[0]);
	  v1[1] = fabs(p2_1[1] - p1_1[1]);
	  v1[2] = fabs(p2_1[2] - p1_1[2]);
    v2[0] = fabs(p2_2[0] - p1_2[0]);
	  v2[1] = fabs(p2_2[1] - p1_2[1]);
	  v2[2] = fabs(p2_2[2] - p1_2[2]);

    vn1 = vtkMath::Norm(v1);
    vn2 = vtkMath::Norm(v2);
    s = vtkMath::Dot(v1,v2);

    if(vn1 != 0 && vn2 != 0)
      angle = acos(s / (vn1 * vn2));
    else
    {
      wxMessageBox("Is not possible to measure the angle. Both the lines must have length > 0!","Warning");
      angle = 0;
    }
    
    angle *= vtkMath::RadiansToDegrees();
    angle = fabs(angle);
    mafEventMacro(mafEvent(this,ID_RESULT_ANGLE,angle));
    return;
  } 
}

//----------------------------------------------------------------------------
void mmi2DMeter::RemoveMeter()
//----------------------------------------------------------------------------
{
  //if the current render window is not null remove the two actors 
  if (CurrentRenderer == NULL) return;
  CurrentRenderer->RemoveActor2D(LineActor);
  CurrentRenderer->RemoveActor2D(LineActor2);
  CurrentRenderer->GetRenderWindow()->Render();
}
