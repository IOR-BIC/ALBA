/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi2DMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-09 11:35:31 $
  Version:   $Revision: 1.9 $
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
  m_Coordinate->SetCoordinateSystemToNormalizedViewport();

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
}
//---------------------------------------------------------------------
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
  m_ParallelView = m_CurrentRenderer->GetActiveCamera()->GetParallelProjection();
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
  double pos_2d[2] = {0,0};
  
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

  if (m_CurrentRenderer == NULL)
  {
    return;
  }

  m_CurrentRenderer->SetDisplayPoint(x,y,0);
  m_CurrentRenderer->DisplayToView();
  m_CurrentRenderer->ViewportToNormalizedViewport(x,y);

  // no point has yet been picked
  if(counter == 0)
  {
    m_EndMeasure = false;
    m_CurrentRenderer->RemoveActor2D(m_LineActor2);
    m_Line->SetPoint1(x,y,0);
    m_Line->SetPoint2(x,y,0);
    m_Line->Update();
    m_CurrentRenderer->AddActor2D(m_LineActor);
    counter++;
  }
  // first point has been picked and the second one is being dragged
  else if(counter == 1 && m_DraggingLeft)
  {
    m_Line->SetPoint2(x,y,0);
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

		if(m_MeasureType == DISTANCE_BETWEEN_LINES) 
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
    assert(m_MeasureType = DISTANCE_BETWEEN_LINES);
    // add the second line 
		double tmpPt[3];
		m_Line->GetPoint1(tmpPt);
    m_Line2->SetPoint1(tmpPt);
		dx=tmpPt[0];
		dy=tmpPt[1];
		dz=tmpPt[2];
		m_Line->GetPoint2(tmpPt);
    m_Line2->SetPoint2(tmpPt);
		dx-=tmpPt[0];
		dy-=tmpPt[1];
		dz-=tmpPt[2];
    m_Line2->Update();
    m_CurrentRenderer->AddActor2D(m_LineActor2);
    counter++;
  }
  // add the  second line for the angle between lines mode
  else if(counter == 2 && !m_DraggingLeft)
  { 
    assert(m_MeasureType == ANGLE_BETWEEN_LINES);
    m_Line2->SetPoint1(x,y,0);
    m_Line2->SetPoint2(x,y,0);
    m_Line2->Update();
    m_CurrentRenderer->AddActor2D(m_LineActor2);
    counter++; 
  }
  else if(counter == 3 && m_MeasureType == DISTANCE_BETWEEN_LINES && m_DraggingLine)
  {
    m_Line2->SetPoint2(x,y,0);
    m_Line2->SetPoint1(x+dx,y+dy,dz);
    m_Line2->Update();
  }
  else if(counter == 3 && m_MeasureType == DISTANCE_BETWEEN_LINES)
  {
    m_EndMeasure = true;
  }
  else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_LINES && m_DraggingLeft)
  {
    m_Line2->SetPoint2(x,y,0);
    m_Line2->Update();
  }
  else if(counter == 3 && m_MeasureType == ANGLE_BETWEEN_LINES)
  {
    m_EndMeasure = true;
  }

  if(m_EndMeasure)
  {
    counter = 0; 
  }

  m_CurrentRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mmi2DMeter::CalculateMeasure()
//----------------------------------------------------------------------------
{
  double p1_1[3],p2_1[3],p1_2[3],p2_2[3],vx,vy,vz;

  m_Line->GetPoint1(p1_1);
  m_Line->GetPoint2(p2_1);

  vx = p1_1[0];
  vy = p1_1[1];
  vz = p1_1[2];
  m_CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  m_CurrentRenderer->ViewToWorld(vx,vy,vz);
  p1_1[0] = vx;
  p1_1[1] = vy;
  p1_1[2] = vz;

  vx = p2_1[0];
  vy = p2_1[1];
  vz = p2_1[2];
  m_CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  m_CurrentRenderer->ViewToWorld(vx,vy,vz);
  p2_1[0] = vx;
  p2_1[1] = vy;
  p2_1[2] = vz;

  if(m_MeasureType == DISTANCE_BETWEEN_POINTS)
  {
    m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }

  m_Line2->GetPoint1(p1_2);
  m_Line2->GetPoint2(p2_2);

  vx = p1_2[0];
  vy = p1_2[1];
  vz = p1_2[2];
  m_CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  m_CurrentRenderer->ViewToWorld(vx,vy,vz);
  p1_2[0] = vx;
  p1_2[1] = vy;
  p1_2[2] = vz;

  vx = p2_2[0];
  vy = p2_2[1];
  vz = p2_2[2];
  m_CurrentRenderer->NormalizedViewportToView(vx,vy,vz);
  m_CurrentRenderer->ViewToWorld(vx,vy,vz);
  p2_2[0] = vx;
  p2_2[1] = vy;
  p2_2[2] = vz;

  if(m_MeasureType == DISTANCE_BETWEEN_LINES)
  {
    m_Distance = sqrt(vtkLine::DistanceToLine(p1_2,p1_1,p2_1));
    mafEventMacro(mafEvent(this,ID_RESULT_MEASURE,m_Distance));
    return;
  }

  if(m_MeasureType == ANGLE_BETWEEN_LINES)
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
      wxMessageBox("Is not possible to measure the angle. Both the lines must have length > 0!","Warning");
      angle = 0;
    }
    
    angle *= vtkMath::RadiansToDegrees();
    if(angle >= 90.0) 
      angle = 180.0 - angle; 
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
  /*  
    vtkMAFSmartPointer<vtkImageImport> importer;
    importer->SetWholeExtent(1,512,1,1,1,1); 
    importer->SetDataExtentToWholeExtent(); 
    importer->SetDataOrigin(0,0,0);
    importer->SetDataSpacing(1,1,1);
    importer->SetDataScalarType(probimg_result->GetPointData()->GetScalars()->GetDataType());
    importer->SetImportVoidPointer(probimg_result->GetPointData()->GetScalars()->GetVoidPointer(0));

    vtkMAFSmartPointer<vtkImageAccumulate> accumulate;
    accumulate->SetInput(importer->GetOutput());
    accumulate->Update();
*/
    m_PlotActor->AddInput(probimg_result);
    //m_PlotActor->AddInput((vtkDataSet *)accumulate->GetOutput());
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
    default:
      m_MeasureType = DISTANCE_BETWEEN_POINTS;
  }
}
