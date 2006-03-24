/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi2DMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-03-24 11:48:47 $
  Version:   $Revision: 1.14 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmi2DMeter.h"
#include "mafDecl.h"
#include "mmdMouse.h"
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

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmi2DMeter)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmi2DMeter::mmi2DMeter() 
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
  m_PlotActor->SetTitle("Density vs. Length (mm)");
  m_PlotActor->SetXTitle("mm");
  m_PlotActor->SetYTitle("Dens.");
  vtkTextProperty* tprop = m_PlotActor->GetTitleTextProperty();
  tprop->SetColor(0.02,0.06,0.62);
  tprop->SetFontFamilyToArial();
  tprop->ItalicOff();
  tprop->BoldOff();
  tprop->SetFontSize(12);
  m_PlotActor->SetPlotColor(0,.8,.3,.3);

  // Histogram dialog
  int width = 400;
  int height = 300;
  int x_init,y_init;
  x_init = mafGetFrame()->GetPosition().x;
  y_init = mafGetFrame()->GetPosition().y;
  m_HistogramDialog = new wxDialog(mafGetFrame(),-1,"Histogram",wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
  m_HistogramRWI = new mafRWI(mafGetFrame());
  m_HistogramRWI->SetListener(this);
  m_HistogramRWI->m_RenFront->AddActor2D(m_PlotActor);
  m_HistogramRWI->m_RenFront->SetBackground(1,1,1);
  m_HistogramRWI->SetSize(0,0,width,height);
  m_HistogramRWI->m_RwiBase->Reparent(m_HistogramDialog);
  m_HistogramRWI->m_RwiBase->Show(true);

  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_HistogramRWI->m_RwiBase,1, wxEXPAND);
  m_HistogramDialog->SetSizer(sizer);
  m_HistogramDialog->SetAutoLayout(TRUE);
  sizer->Fit(m_HistogramDialog);
  
  m_HistogramDialog->SetSize(x_init,y_init,width,height);
	m_HistogramDialog->Show(FALSE);

  m_CurrentRenderer  = NULL;
  m_LastRenderer     = NULL;

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
  m_MeasureType = DISTANCE_BETWEEN_POINTS;

  m_Distance = 0;
  m_Angle = 0;
  m_ExceptionFlag = false;
}
//----------------------------------------------------------------------------
mmi2DMeter::~mmi2DMeter() 
//----------------------------------------------------------------------------
{
  RemoveMeter();
  m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
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
    m_RendererVector[i]->RemoveActor2D(m_TextVector[i]);
    vtkDEL(m_TextVector[i]);
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
  m_TextVector.clear();
  m_LineSourceVector1.clear();
  m_LineMapperVector1.clear();
  m_LineActorVector1.clear();
  m_LineSourceVector2.clear();
  m_LineMapperVector2.clear();
  m_LineActorVector2.clear();
  m_RendererVector.clear();

  cppDEL(m_HistogramRWI);
  cppDEL(m_HistogramDialog);
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnLeftButtonDown(mafEventInteraction *e)
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
  mmdMouse  *mouse  = mmdMouse::SafeDownCast(device);
  if (m_Mouse == NULL)
  {
    m_Mouse = mouse;
  }
  m_CurrentRenderer = m_Mouse->GetRenderer();
  m_ParallelView = m_CurrentRenderer->GetActiveCamera()->GetParallelProjection() != 0;
  if (m_ParallelView)
  {
    OnButtonDown(e);  
    if (m_GenerateHistogram)
    {
      mafView *v = m_Mouse->GetView();
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
      if (m_GenerateHistogram)
      {
        mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());
        mmdMouse  *mouse  = mmdMouse::SafeDownCast(device);
        if (m_Mouse == NULL)
        {
          m_Mouse = mouse;
        }
        double pos_2d[2];
        e->Get2DPosition(pos_2d);
        mafView *v = m_Mouse->GetView();
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
	
  if (!m_DraggingMouse) return;
  if((m_DraggingLeft || m_DraggingLine) && m_ParallelView)
  {
    e->Get2DPosition(pos_2d);
    DrawMeasureTool(pos_2d[0], pos_2d[1]);
  }
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
 	m_DraggingMouse = true;
}
//----------------------------------------------------------------------------
void mmi2DMeter::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingMouse = false;
}
//----------------------------------------------------------------------------
void mmi2DMeter::DrawMeasureTool(double x, double y)
//----------------------------------------------------------------------------
{
	static long counter = 0;
	static double dx, dy, dz;
  
	if (m_CurrentRenderer == NULL)	{return;}

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
    
    m_TextVector.push_back(NULL);
    m_TextVector[m_TextVector.size()-1] = vtkTextActor::New();
    // initialization
    m_Distance = 0.0;
    m_Angle = 0.0;
    m_TextVector[m_TextVector.size()-1]->SetInput("");
    m_TextVector[m_TextVector.size()-1]->GetPositionCoordinate()->SetCoordinateSystemToWorld();

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
			m_EndMeasure = true;
		}
		else
		{
			counter++;
		}
		if(m_MeasureType == DISTANCE_BETWEEN_LINES || m_MeasureType == ANGLE_BETWEEN_POINTS) 
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
		else
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
  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  { 
    double tmp_pos[3];
    m_Line->GetPoint2(tmp_pos);
    m_TextVector[m_TextVector.size()-1]->SetInput(wxString::Format("%.2f" , m_Distance));
    m_TextVector[m_TextVector.size()-1]->SetPosition(tmp_pos[0],tmp_pos[1]);
    m_CurrentRenderer->AddActor2D(m_TextVector[m_TextVector.size()-1]);
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
   
    m_TextVector[m_TextVector.size()-1]->SetInput(wxString::Format("%.2f", m_Distance));
    if(m_Distance > 0.15)
    {
      m_TextVector[m_TextVector.size()-1]->SetPosition( ((tmp_pos1_1[0] + tmp_pos2_1[0])/2 + (tmp_pos1_2[0] + tmp_pos2_2[0])/2)/2, ((tmp_pos1_1[1] + tmp_pos2_1[1])/2 + (tmp_pos1_2[1] + tmp_pos2_2[1])/2)/2);  
    }
    else
    {
      m_TextVector[m_TextVector.size()-1]->SetPosition(tmp_pos2_1[0] ,tmp_pos2_1[1]);
    }
    m_CurrentRenderer->AddActor2D(m_TextVector[m_TextVector.size()-1]);
  }
  else if(m_MeasureType == ANGLE_BETWEEN_LINES && counter == 3)
  { 
    //calculate the length to avoid zero length message of CalculateMeasure
    double v2[3];
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

    if(vtkMath::Norm(v2))
    {
      CalculateMeasure();
      m_TextVector[m_TextVector.size()-1]->SetInput(wxString::Format("%.2f°", m_Angle));
      m_TextVector[m_TextVector.size()-1]->SetPosition((tmp_pos1_1[0] + tmp_pos2_1[0] + tmp_pos1_2[0] + tmp_pos2_2[0])/4 ,
                                                       (tmp_pos1_1[1] + tmp_pos2_1[1] + tmp_pos1_2[1] + tmp_pos2_2[1])/4 );
      m_CurrentRenderer->AddActor2D(m_TextVector[m_TextVector.size()-1]);
    }
  }
  else if(m_MeasureType == ANGLE_BETWEEN_POINTS && counter == 3)
  {
    CalculateMeasure();

    double tmp_pos2_1[3];
    double tmp_pos2_2[3];
    m_Line->GetPoint2(tmp_pos2_1);
    m_Line2->GetPoint2(tmp_pos2_2);

    m_TextVector[m_TextVector.size()-1]->SetInput(wxString::Format("%.2f°", m_Angle));
    m_TextVector[m_TextVector.size()-1]->SetPosition(tmp_pos2_2[0], tmp_pos2_2[1]);
    m_CurrentRenderer->AddActor2D(m_TextVector[m_TextVector.size()-1]);
  }

	if(m_EndMeasure)
	{
		counter = 0;
    CalculateMeasure();
    m_RendererVector.push_back(m_CurrentRenderer);
    if(m_ExceptionFlag)
    {
      m_TextVector[m_TextVector.size()-1]->SetInput("Error\n");
      wxMessageBox("Impossible to measure the angle.\n Both the lines must have length > 0!");
    }  
    m_CurrentRenderer->AddActor2D(m_TextVector[m_TextVector.size()-1]);

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
    m_LineMapperVector1[m_LineMapperVector1.size()-1]->SetInput(m_LineSourceVector1[m_LineMapperVector1.size()-1]->GetOutput());

    m_LineActorVector1.push_back(NULL);
    m_LineActorVector1[m_LineActorVector1.size()-1] = vtkActor2D::New();
    m_LineActorVector1[m_LineActorVector1.size()-1]->SetMapper(m_LineMapperVector1[m_LineMapperVector1.size()-1]);
    m_LineActorVector1[m_LineActorVector1.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);
    m_CurrentRenderer->AddActor2D(m_LineActorVector1[m_LineActorVector1.size()-1]);

    // persistent LINE2
    double tmpPt2[3];
    m_LineSourceVector2.push_back(NULL);
    m_LineSourceVector2[m_LineSourceVector2.size()-1] = vtkLineSource::New();
    m_Line2->GetPoint1(tmpPt2);
    if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
    }
    m_LineSourceVector2[m_LineSourceVector2.size()-1]->SetPoint1(tmpPt2);
    m_Line2->GetPoint2(tmpPt2);
    if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      tmpPt2[0] = tmpPt[0];
      tmpPt2[1] = tmpPt[1];
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
    m_LineActorVector2[m_LineActorVector2.size()-1]->GetProperty()->SetColor(0.0,1.0,0.0);
    m_CurrentRenderer->AddActor2D(m_LineActorVector2[m_LineActorVector2.size()-1]);

    //measure , measure type vectors
    if(m_MeasureType == DISTANCE_BETWEEN_LINES || m_MeasureType == DISTANCE_BETWEEN_POINTS)
    {
      m_Measure.push_back(m_Distance);
      m_FlagMeasureType.push_back(ID_RESULT_MEASURE);
    }
    else
    {
      m_Measure.push_back(m_Angle);
      m_FlagMeasureType.push_back(ID_RESULT_ANGLE);
    }
    
    //delete temporary measure
    m_CurrentRenderer->RemoveActor2D(m_LineActor);
    m_CurrentRenderer->RemoveActor2D(m_LineActor2);
	}
  
  m_ExceptionFlag = false;
	m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmi2DMeter::CalculateMeasure()
//----------------------------------------------------------------------------
{
  double p1_1[3],p2_1[3],p1_2[3],p2_2[3];

  m_Line->GetPoint1(p1_1);
  m_Line->GetPoint2(p2_1);
  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  {
    m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }

  m_Line2->GetPoint1(p1_2);
  m_Line2->GetPoint2(p2_2);
  if(m_MeasureType == DISTANCE_BETWEEN_LINES)
  {
    m_Distance = sqrt(vtkLine::DistanceToLine(p1_2,p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }

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
      //wxMessageBox("Is not possible to measure the angle. Both the lines must have length > 0!","Warning");
      angle = 0;
      m_ExceptionFlag = true;
    }
    
    angle *= vtkMath::RadiansToDegrees();
    if(angle >= 90.0 && m_MeasureType == ANGLE_BETWEEN_LINES) 
      angle = 180.0 - angle; 

    m_Angle = angle;
    mafEventMacro(mafEvent(this,ID_RESULT_ANGLE,angle));
    return;
  } 
}
//----------------------------------------------------------------------------
void mmi2DMeter::CreateHistogram()
//----------------------------------------------------------------------------
{
  if (m_ProbedVME != NULL)
  {
    vtkDataSet *probed_data = m_ProbedVME->GetOutput()->GetVTKData();
    probed_data->Update();

    m_PlotActor->SetXRange(0,m_Distance);
    m_PlotActor->SetPlotCoordinate(0,m_Distance);

    m_ProbingLine->SetResolution((int)m_Distance);
    m_ProbingLine->Update();

    vtkMAFSmartPointer<vtkProbeFilter> prober;
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
void mmi2DMeter::RemoveMeter()
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
void mmi2DMeter::GenerateHistogram(bool generate)
//----------------------------------------------------------------------------
{
  m_GenerateHistogram = generate;
  if (m_GenerateHistogram)
  {
    m_PlotActor->RemoveAllInputs();
    m_HistogramRWI->m_RwiBase->Render();
    RemoveMeter();
    SetMeasureTypeToDistanceBetweenPoints();
  }
  m_HistogramDialog->Show(m_GenerateHistogram);
}
//----------------------------------------------------------------------------
void mmi2DMeter::UndoMeasure()
//----------------------------------------------------------------------------
{
  if(m_RendererVector.size() != 0)
  {
    m_RendererVector[m_RendererVector.size()-1]->RemoveActor2D(m_TextVector[m_TextVector.size()-1]);
    vtkDEL(m_TextVector[m_TextVector.size()-1]);
    m_TextVector.pop_back();

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

    if(m_FlagMeasureType[m_FlagMeasureType.size()-1] == ID_RESULT_MEASURE)
    {
      m_Distance = m_Measure[m_Measure.size()-1];
    }
    else if (m_FlagMeasureType[m_FlagMeasureType.size()-1] == ID_RESULT_ANGLE)
    {
      m_Angle = m_Measure[m_Measure.size()-1];
    }
    
    if(m_Measure.size() == 0)
    {
      mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,0.0));
    }
    else
    {
      mafEventMacro(mafEvent(this,m_FlagMeasureType[m_FlagMeasureType.size()-1],m_Measure[m_Measure.size()-1]));
    }
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mmi2DMeter::SetMeasureType(int t)
//----------------------------------------------------------------------------
{
  switch(t) 
  {
    case DISTANCE_BETWEEN_LINES:
      m_MeasureType = DISTANCE_BETWEEN_LINES;
      GenerateHistogramOff();
  	break;
    case ANGLE_BETWEEN_LINES:
      m_MeasureType = ANGLE_BETWEEN_LINES;
      GenerateHistogramOff();
    break;
	  case ANGLE_BETWEEN_POINTS:
		  m_MeasureType = ANGLE_BETWEEN_POINTS;
		  GenerateHistogramOff();
		break;
    default:
      m_MeasureType = DISTANCE_BETWEEN_POINTS;
  }
}
