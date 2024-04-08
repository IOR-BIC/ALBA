/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeter
 Authors: Marco Petrone, Paolo Quadrani
 
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

#include "albaVMEMeter.h"
#include "albaVMEOutputMeter.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "albaTransform.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include "albaDataPipeCustom.h"
#include "mmuIdFactory.h"
#include "albaGUI.h"
#include "albaAbsMatrixPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaRWI.h"
#include "albaGUIDialogPreview.h"

#include "vtkALBADataPipe.h"
#include "vtkMath.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"
#include "vtkProbeFilter.h"
#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"

#include <assert.h>

ALBA_ID_IMP(albaVMEMeter::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEMeter::albaVMEMeter()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  m_Angle         = 0.0;
  
  m_StartVmeName  = "";
  m_EndVme1Name   = "";
  m_EndVme2Name   = "";
  m_ProbeVmeName   = "";
  
  albaNEW(m_Transform);
  albaVMEOutputMeter *output = albaVMEOutputMeter::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  vtkNEW(m_LineSource);
  vtkNEW(m_LineSource2);
  vtkNEW(m_Goniometer);
  vtkNEW(m_PolyData);
  

  m_Goniometer->AddInputConnection(m_LineSource->GetOutputPort());
  m_Goniometer->AddInputConnection(m_LineSource2->GetOutputPort());

  m_PolyData->DeepCopy(m_Goniometer->GetOutput());

  albaNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_PolyData);

  // histogram
  // Probing tool
  vtkNEW(m_ProbingLine);
  m_ProbingLine->SetResolution(512);

  m_ProbedVME = NULL;

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

  m_HistogramDialog = NULL;
  m_HistogramRWI    = NULL;
  
  m_GenerateHistogram = 0;
}
//-------------------------------------------------------------------------
albaVMEMeter::~albaVMEMeter()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
  vtkDEL(m_Goniometer);
  albaDEL(m_TmpTransform);
  vtkDEL(m_PolyData);
  SetOutput(NULL);

  if(m_HistogramRWI)
    m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
  vtkDEL(m_PlotActor);
  vtkDEL(m_ProbingLine);
  cppDEL(m_HistogramDialog);
}
//-------------------------------------------------------------------------
int albaVMEMeter::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a)==ALBA_OK)
	{
		albaVMEMeter *meter = albaVMEMeter::SafeDownCast(a);
		m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

		albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
		if (dpipe)
		{
			dpipe->SetInput(m_Goniometer->GetOutput());
			m_Goniometer->Update();
		}
		return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEMeter::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((albaVMEMeter *)vme)->m_Transform->GetMatrix() && \
          GetLink("StartVME") == ((albaVMEMeter *)vme)->GetLink("StartVME") && \
          GetLink("EndVME1") == ((albaVMEMeter *)vme)->GetLink("EndVME1") && \
          GetLink("EndVME2") == ((albaVMEMeter *)vme)->GetLink("EndVME2") && \
          GetLink("PlottedVME") == ((albaVMEMeter *)vme)->GetLink("EndVME2");
  }
  return ret;
}
//-------------------------------------------------------------------------
int albaVMEMeter::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();

    return ALBA_OK;
  }

  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEMeter::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}
//-------------------------------------------------------------------------
albaVMEOutputPolyline *albaVMEMeter::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (albaVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool albaVMEMeter::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void albaVMEMeter::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void albaVMEMeter::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void albaVMEMeter::InternalUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  UpdateLinks();

  if (GetMeterMode() == albaVMEMeter::POINT_DISTANCE)
  {
    albaVME *start_vme = GetStartVME();
    albaVME *end_vme   = GetEnd1VME();

    bool start_ok = true, end_ok = true;

    double orientation[3];

    if (start_vme && end_vme)
    {
      start_vme->GetOutput()->Update();  
      start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);

      end_vme->GetOutput()->Update();  
      end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
    }
    else
    {
      start_ok = false;
      end_ok   = false;
    }

    if (start_ok && end_ok)
    {
      // compute distance between points
      m_Distance = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint, m_EndPoint));

      if(GetMeterMeasureType() == albaVMEMeter::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()

      // compute end point in local coordinate system
      double local_end[3];
      m_TmpTransform->TransformPoint(m_EndPoint,local_end);

      m_LineSource2->SetPoint1(local_start[0],local_start[1],local_start[2]);
      m_LineSource2->SetPoint2(local_start[0],local_start[1],local_start[2]);
      m_LineSource2->Update();
      m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
      m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
      m_LineSource->Update();
      m_Goniometer->Modified();

      GenerateHistogram(m_GenerateHistogram);
    }
    else
      m_Distance = -1;

    GetOutput()->Update();
    InvokeEvent(this,VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == albaVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == albaVMEMeter::LINE_DISTANCE)
  {
    albaVME *start_vme = GetStartVME();
    albaVME *end1_vme  = GetEnd1VME();
    albaVME *end2_vme  = GetEnd2VME();

    bool start_ok = true, end1_ok = true, end2_ok = true;
    double orientation[3];

    if (start_vme && end1_vme && end2_vme)
    {
      
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
      
        end1_vme->GetOutput()->Update();  
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
      
        end2_vme->GetOutput()->Update();  
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation);
    }
    else
    {
      start_ok = false;
      end1_ok  = false;
      end2_ok  = false;
    }

    if (start_ok && end1_ok && end2_ok)
    {
      double start[3],p1[3],p2[3],p3[3],t;

      start[0] = m_StartPoint[0];
      start[1] = m_StartPoint[1];
      start[2] = m_StartPoint[2];

      p1[0] = m_EndPoint[0];
      p1[1] = m_EndPoint[1];
      p1[2] = m_EndPoint[2];

      p2[0] = m_EndPoint2[0];
      p2[1] = m_EndPoint2[1];
      p2[2] = m_EndPoint2[2];

      vtkLine::DistanceToLine(start,p1,p2,t,p3);

      // compute distance between start and closest point
      m_Distance = sqrt(vtkMath::Distance2BetweenPoints(start,p3));

      if(GetMeterMeasureType() == albaVMEMeter::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(start,local_start);

      // compute end point in local coordinate system
      double local_closest[3];
      m_TmpTransform->TransformPoint(p3,local_closest);

      double local_p1[3];
      m_TmpTransform->TransformPoint(p1,local_p1);

      double local_p2[3];
      m_TmpTransform->TransformPoint(p2,local_p2);

      m_LineSource->SetPoint1(local_start);
      m_LineSource->SetPoint2(local_closest);
      m_LineSource->Update();

      m_LineSource2->SetPoint1(local_p1);
      m_LineSource2->SetPoint2(local_p2);
      m_LineSource2->Update();

      m_Goniometer->Modified();
    }
    else
      m_Distance = -1;

    GetOutput()->Update();
    InvokeEvent(this,VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == albaVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == albaVMEMeter::LINE_ANGLE)
  {
    albaVME *start_vme = GetStartVME();
    albaVME *end1_vme  = GetEnd1VME();
    albaVME *end2_vme  = GetEnd2VME();

    double orientation[3];

    bool start_ok = true, end1_ok = true, end2_ok = true;
    if (start_vme && end1_vme && end2_vme)
    {
      start_vme->GetOutput()->Update();  
      start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);

      end1_vme->GetOutput()->Update();  
      end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);

			end2_vme->GetOutput()->Update();  
      end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation);
    }
    else
    {
      start_ok = false;
      end1_ok  = false;
      end2_ok  = false;
    }
    if (start_ok && end1_ok && end2_ok)
    {
      double start[3],p1[3],p2[3], v1[3], v2[3], vn1, vn2, s;

      start[0] = m_StartPoint[0];
      start[1] = m_StartPoint[1];
      start[2] = m_StartPoint[2];
      p1[0] = m_EndPoint[0];
      p1[1] = m_EndPoint[1];
      p1[2] = m_EndPoint[2];
      p2[0] = m_EndPoint2[0];
      p2[1] = m_EndPoint2[1];
      p2[2] = m_EndPoint2[2];
      v1[0] = p1[0] - start[0];
      v1[1] = p1[1] - start[1];
      v1[2] = p1[2] - start[2];
      v2[0] = p2[0] - start[0];
      v2[1] = p2[1] - start[1];
      v2[2] = p2[2] - start[2];
      vn1 = vtkMath::Norm(v1);
      vn2 = vtkMath::Norm(v2);
      s = vtkMath::Dot(v1,v2);
      if(vn1 != 0 && vn2 != 0)
      {
        m_Angle = vtkMath::DegreesFromRadians(acos(s / (vn1 * vn2)));
        if(GetMeterMeasureType() == albaVMEMeter::RELATIVE_MEASURE)
          m_Angle -= GetMeterAttributes()->m_InitMeasure;
      }
      else
        m_Angle = 0;

      // compute points in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(start,local_start);

      double local_end1[3];
      m_TmpTransform->TransformPoint(p1,local_end1);
      double local_end2[3];
      m_TmpTransform->TransformPoint(p2,local_end2);

      m_LineSource->SetPoint1(local_start);
      m_LineSource->SetPoint2(local_end1);
      m_LineSource->Update();

      m_LineSource2->SetPoint1(local_start);
      m_LineSource2->SetPoint2(local_end2);
      m_LineSource2->Update();

      m_Goniometer->Modified();
    }
    else
      m_Angle = 0;

    GetOutput()->Update();
    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == albaVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Angle > 0 && m_Angle >= threshold)
      InvokeEvent(this, LENGTH_THRESHOLD_EVENT);
  }

  m_Goniometer->Update();
  vtkPolyData *polydata = m_Goniometer->GetOutput();
  int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
	vtkIdType pointId[2];
  vtkALBASmartPointer<vtkCellArray> cellArray;
  for(int i = 0; i< num;i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  m_PolyData->SetPoints(m_Goniometer->GetOutput()->GetPoints());
  m_PolyData->SetLines(cellArray);

}
//-----------------------------------------------------------------------
int albaVMEMeter::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEMeter::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMEMeter::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** albaVMEMeter::GetIcon() 
{
 #include "albaVMEMeter.xpm"
 return albaVMEMeter_xpm;
}
//-------------------------------------------------------------------------
mmaMeter *albaVMEMeter::GetMeterAttributes()
//-------------------------------------------------------------------------
{
  mmaMeter *meter_attributes = (mmaMeter *)GetAttribute("MeterAttributes");
  if (meter_attributes == NULL)
  {
    meter_attributes = mmaMeter::New();
    SetAttribute("MeterAttributes", meter_attributes);
  }
  return meter_attributes;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetMeterMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *albaVMEMeter::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double albaVMEMeter::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double albaVMEMeter::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int albaVMEMeter::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double albaVMEMeter::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double albaVMEMeter::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
albaGUI* albaVMEMeter::CreateGui()
//-------------------------------------------------------------------------
{
  int num_mode = 3;
  const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};

  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Combo(ID_METER_MODE,_("Mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_("Choose the meter mode"));
  m_Gui->Divider();

  UpdateLinks();
  
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End 1"), _("Select the end vme for point distance"));
  m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,_("End 2"), _("Select the vme representing \nthe point for line distance"));

  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
    m_Gui->Enable(ID_END2_METER_LINK,false);

  m_Gui->Bool(ID_PLOT_PROFILE,_("Plot profile"),&m_GenerateHistogram);
  m_Gui->Enable(ID_PLOT_PROFILE,GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

  m_Gui->Button(ID_PLOTTED_VME_LINK,&m_ProbeVmeName,_("Probed"), _("Select the vme that will be plotted"));
  m_Gui->Enable(ID_PLOTTED_VME_LINK, GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

	m_Gui->Divider();

  InternalUpdate();
  GetPolylineOutput()->Update();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEMeter::UpdateLinks()
//-------------------------------------------------------------------------
{
  albaID sub_id = -1;
  albaVME *start_vme = GetStartVME();
  albaVME *end_vme1 = GetEnd1VME();
  albaVME *end_vme2 = GetEnd2VME();
  albaVME *probedVme = GetPlottedVME();

  m_StartVmeName = start_vme ? start_vme->GetName() : _("none");
  m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");
  m_EndVme2Name = end_vme2 ? end_vme2->GetName() : _("none");

  m_ProbedVME = albaVMEVolumeGray::SafeDownCast(probedVme);
  m_ProbeVmeName = probedVme ? probedVme->GetName() : _("none");
}
//-------------------------------------------------------------------------
void albaVMEMeter::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_START_METER_LINK:
      case ID_END1_METER_LINK:
      case ID_END2_METER_LINK:
      {
        albaID button_id = e->GetId();
        albaString title = _("Choose meter vme link");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMEMeter::VMEAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_START_METER_LINK)
          {
            SetLink("StartVME", n);
            m_StartVmeName = n->GetName();
          }
          else if (button_id == ID_END1_METER_LINK)
          {
            SetLink("EndVME1", n);
            m_EndVme1Name = n->GetName();
          }
          else
          {
            SetLink("EndVME2", n);
            m_EndVme2Name = n->GetName();
          }
          m_Gui->Update();
        }
				GetLogicManager()->CameraUpdate();
      }
      break;
      case ID_PLOTTED_VME_LINK:
        {
          albaID button_id = e->GetId();
          albaString title = _("Choose meter vme link");
          e->SetId(VME_CHOOSE);
          e->SetPointer(&albaVMEMeter::VolumeAccept);
          e->SetString(&title);
          ForwardUpEvent(e);
          albaVME *n = e->GetVme();
          if (n != NULL)
          {
            SetLink("PlottedVME",n);
            m_ProbedVME = albaVMEVolumeGray::SafeDownCast(n);
            m_ProbeVmeName = n->GetName();
            CreateHistogram();
          }
          m_Gui->Update();
          
        }
        break;
	  case ID_METER_MODE:
	  {
		  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
		  {  
        m_Gui->Enable(ID_END2_METER_LINK,false);
		  }
		  else if(GetMeterAttributes()->m_MeterMode ==  LINE_DISTANCE)
		  { 
			  m_Gui->Enable(ID_END2_METER_LINK,true);
		  }
		  else if(GetMeterAttributes()->m_MeterMode ==  LINE_ANGLE)
		  {       
			  m_Gui->Enable(ID_END2_METER_LINK,true);
		  }
      m_Gui->Enable(ID_PLOT_PROFILE,GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);
      m_Gui->Enable(ID_PLOTTED_VME_LINK, GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);
      this->Modified();
			GetLogicManager()->CameraUpdate();
	  }
	  break;
    case ID_PLOT_PROFILE:
      {
        // Histogram dialog
        if(m_HistogramDialog == NULL)
        {
          int width = 400;
          int height = 300;
          int x_init,y_init;
          x_init = albaGetFrame()->GetPosition().x;
          y_init = albaGetFrame()->GetPosition().y;
          m_HistogramDialog = new albaGUIDialogPreview(_("Histogram Dialog"), albaCLOSEWINDOW | albaUSERWI);
          m_HistogramRWI = m_HistogramDialog->GetRWI();
          m_HistogramRWI->SetListener(this);
          m_HistogramRWI->m_RenFront->AddActor2D(m_PlotActor);
          m_HistogramRWI->m_RenFront->SetBackground(1,1,1);
          m_HistogramRWI->SetSize(0,0,width,height);

          m_HistogramDialog->SetSize(x_init,y_init,width,height);
          m_HistogramDialog->Show(FALSE);
        }
        
        GenerateHistogram(m_GenerateHistogram);
      }
      break;
      default:
        albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
void albaVMEMeter::SetMeterLink(const char *link_name, albaVME *n)
//-------------------------------------------------------------------------
{
  SetLink(link_name, n);
}
//-------------------------------------------------------------------------
albaVME *albaVMEMeter::GetStartVME()
//-------------------------------------------------------------------------
{
  return GetLink("StartVME");
}
//-------------------------------------------------------------------------
albaVME *albaVMEMeter::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return GetLink("EndVME1");
}
//-------------------------------------------------------------------------
albaVME *albaVMEMeter::GetEnd2VME()
//-------------------------------------------------------------------------
{
  return GetLink("EndVME2");
}
//-------------------------------------------------------------------------
albaVME *albaVMEMeter::GetPlottedVME()
//-------------------------------------------------------------------------
{
  return GetLink("PlottedVME");
}
//----------------------------------------------------------------------------
void albaVMEMeter::GenerateHistogram(int generate)
//----------------------------------------------------------------------------
{
  if(m_HistogramDialog)
  {
    m_GenerateHistogram = generate;
    if (m_GenerateHistogram)
    {
      CreateHistogram();
      m_HistogramRWI->m_RwiBase->Render();
    }
    m_HistogramDialog->Show(m_GenerateHistogram != 0);
  }
}
//----------------------------------------------------------------------------
void albaVMEMeter::CreateHistogram()
//----------------------------------------------------------------------------
{
  if (m_ProbedVME != NULL)
  {
    vtkDataSet *probed_data = m_ProbedVME->GetOutput()->GetVTKData();
		    
    m_PlotActor->SetXRange(0,m_Distance);
    double srY[2];
    m_ProbedVME->GetOutput()->GetVTKData()->GetScalarRange(srY);
    m_PlotActor->SetYRange(srY);
    m_PlotActor->SetPlotCoordinate(0,m_Distance);

    double b[6];
    m_ProbedVME->GetOutput()->GetBounds(b);

    m_ProbingLine->SetPoint1(m_StartPoint);
    m_ProbingLine->SetPoint2(m_EndPoint);
    m_ProbingLine->SetResolution((int)m_Distance);
    m_ProbingLine->Update();

    vtkALBASmartPointer<vtkProbeFilter> prober;
    prober->SetInputConnection(m_ProbingLine->GetOutputPort());
    prober->SetSourceData(probed_data);
    prober->Update();

    m_PlotActor->RemoveAllDataSetInputConnections();

    vtkPolyData *probimg_result = prober->GetPolyDataOutput();
    m_PlotActor->AddDataSetInput(probimg_result);
    if(m_HistogramRWI) m_HistogramRWI->m_RwiBase->Render();
  }
}
