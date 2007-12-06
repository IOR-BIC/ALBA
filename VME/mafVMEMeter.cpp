/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-06 09:30:32 $
  Version:   $Revision: 1.34 $
  Authors:   Marco Petrone, Paolo Quadrani
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

#include "mafVMEMeter.h"
#include "mafVMEOutputMeter.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "mafEventSource.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mmuIdFactory.h"
#include "mmgGui.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafRWI.h"
#include "mmgDialogPreview.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
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

MAF_ID_IMP(mafVMEMeter::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEMeter::mafVMEMeter()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  m_Angle         = 0.0;
  
  m_StartVmeName  = "";
  m_EndVme1Name   = "";
  m_EndVme2Name   = "";
  m_ProbeVmeName   = "";
  
  mafNEW(m_Transform);
  mafVMEOutputMeter *output = mafVMEOutputMeter::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  vtkNEW(m_LineSource);
  vtkNEW(m_LineSource2);
  vtkNEW(m_Goniometer);
  vtkNEW(m_PolyData);
  

  m_Goniometer->AddInput(m_LineSource->GetOutput());
  m_Goniometer->AddInput(m_LineSource2->GetOutput());

  m_PolyData->DeepCopy(m_Goniometer->GetOutput());

  mafNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_PolyData);

  // histogram
  // Probing tool
  vtkNEW(m_ProbingLine);
  m_ProbingLine->SetResolution(512);

  m_ProbedVME = NULL;

  mafString plot_title = _("Density vs. Length (mm)");
  mafString plot_titleX = "mm";
  mafString plot_titleY = _("Dens.");
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
mafVMEMeter::~mafVMEMeter()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
  vtkDEL(m_Goniometer);
  mafDEL(m_TmpTransform);
  vtkDEL(m_PolyData);
  SetOutput(NULL);

  if(m_HistogramRWI)
    m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
  vtkDEL(m_PlotActor);
  vtkDEL(m_ProbingLine);
  cppDEL(m_HistogramDialog);
}
//-------------------------------------------------------------------------
int mafVMEMeter::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEMeter *meter = mafVMEMeter::SafeDownCast(a);
    mafNode *linked_node = meter->GetLink("StartVME");
    if (linked_node)
    {
      this->SetLink("StartVME", linked_node);
    }
    linked_node = meter->GetLink("EndVME1");
    if (linked_node)
    {
      this->SetLink("EndVME1", linked_node);
    }
    linked_node = meter->GetLink("EndVME2");
    if (linked_node)
    {
      this->SetLink("EndVME2", linked_node);
    }
    if (linked_node)
    {
      this->SetLink("PlottedVME", linked_node);
    }
    m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Goniometer->GetOutput());
      m_Goniometer->Update();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEMeter::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEMeter *)vme)->m_Transform->GetMatrix() && \
          GetLink("StartVME") == ((mafVMEMeter *)vme)->GetLink("StartVME") && \
          GetLink("EndVME1") == ((mafVMEMeter *)vme)->GetLink("EndVME1") && \
          GetLink("EndVME2") == ((mafVMEMeter *)vme)->GetLink("EndVME2") && \
          GetLink("PlottedVME") == ((mafVMEMeter *)vme)->GetLink("EndVME2");
  }
  return ret;
}
//-------------------------------------------------------------------------
int mafVMEMeter::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();

    return MAF_OK;
  }

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEMeter::GetMaterial()
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
mafVMEOutputPolyline *mafVMEMeter::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool mafVMEMeter::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void mafVMEMeter::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void mafVMEMeter::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void mafVMEMeter::InternalUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  UpdateLinks();

  if (GetMeterMode() == mafVMEMeter::POINT_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end_vme   = GetEnd1VME();

    bool start_ok = true, end_ok = true;

    double orientation[3];

    if (start_vme && end_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("StartVME") != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId("StartVME"),m_StartPoint,-1);
        m_TmpTransform->SetMatrix(*start_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end_landmark = mflVMELandmark::SafeDownCast(end_vme))
        end_ok = end_landmark->GetLandmarkVisibility();*/

      if(end_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME1") != -1)
      {
        ((mafVMELandmarkCloud *)end_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_EndPoint,-1);
        m_TmpTransform->SetMatrix(*end_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end_vme->GetOutput()->Update();  
        end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
      }
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

      if(GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
      //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

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
    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == mafVMEMeter::LINE_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *end2_vme  = GetEnd2VME();

    bool start_ok = true, end1_ok = true, end2_ok = true;
    double orientation[3];

    if (start_vme && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("StartVME") != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId("StartVME"),m_StartPoint,-1);
        m_TmpTransform->SetMatrix(*start_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();*/

      if(end1_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME1") != -1)
      {
        ((mafVMELandmarkCloud *)end1_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_EndPoint,-1);
        m_TmpTransform->SetMatrix(*end1_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end1_vme->GetOutput()->Update();  
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();*/

      if(end2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME2") != -1)
      {
        ((mafVMELandmarkCloud *)end2_vme)->GetLandmark(GetLinkSubId("EndVME2"),m_EndPoint2,-1);
        m_TmpTransform->SetMatrix(*end2_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_EndPoint2,m_EndPoint2);
      }
      else
      {
        end2_vme->GetOutput()->Update();  
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation);
      }
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

      if(GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE)
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
    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      m_EventSource->InvokeEvent(this, LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == mafVMEMeter::LINE_ANGLE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *end2_vme  = GetEnd2VME();

    double orientation[3];

    bool start_ok = true, end1_ok = true, end2_ok = true;
    if (start_vme && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("StartVME") != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId("StartVME"),m_StartPoint,-1);
        m_TmpTransform->SetMatrix(*start_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
      }

      /*if(mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();*/

      if(end1_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME1") != -1)
      {
        ((mafVMELandmarkCloud *)end1_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_EndPoint,-1);
        m_TmpTransform->SetMatrix(*end1_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end1_vme->GetOutput()->Update();  
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
      }

      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();*/

      if(end2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME2") != -1)
      {
        ((mafVMELandmarkCloud *)end2_vme)->GetLandmark(GetLinkSubId("EndVME2"),m_EndPoint2,-1);
        m_TmpTransform->SetMatrix(*end2_vme->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(m_EndPoint2,m_EndPoint2);
      }
      else
      {
        end2_vme->GetOutput()->Update();  
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation);
      }
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
        m_Angle = acos(s / (vn1 * vn2)) * vtkMath::RadiansToDegrees();
        if(GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE)
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
    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Angle > 0 && m_Angle >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }

  m_Goniometer->Update();
  vtkPolyData *polydata = m_Goniometer->GetOutput();
  int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
  int pointId[2];
  vtkMAFSmartPointer<vtkCellArray> cellArray;
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
  m_PolyData->Update();

}
//-----------------------------------------------------------------------
int mafVMEMeter::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEMeter::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEMeter::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** mafVMEMeter::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "mafVMEMeter.xpm"
//  return mafVMEMeter_xpm;
//}
//-------------------------------------------------------------------------
mmaMeter *mafVMEMeter::GetMeterAttributes()
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
void mafVMEMeter::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *mafVMEMeter::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
mmgGui* mafVMEMeter::CreateGui()
//-------------------------------------------------------------------------
{
  int num_mode = 3;
  const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Combo(ID_METER_MODE,_("mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_("Choose the meter mode"));
  m_Gui->Divider();

  UpdateLinks();
  
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End 1"), _("Select the end vme for point distance"));
  m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,_("End 2"), _("Select the vme representing \nthe point for line distance"));

  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
    m_Gui->Enable(ID_END2_METER_LINK,false);

  m_Gui->Bool(ID_PLOT_PROFILE,_("plot profile"),&m_GenerateHistogram);
  m_Gui->Enable(ID_PLOT_PROFILE,GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

  m_Gui->Button(ID_PLOTTED_VME_LINK,&m_ProbeVmeName,_("Probed"), _("Select the vme that will be plotted"));
  m_Gui->Enable(ID_PLOTTED_VME_LINK, GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

	m_Gui->Divider();

  InternalUpdate();
  GetPolylineOutput()->Update();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEMeter::UpdateLinks()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;
  mafVME *start_vme = GetStartVME();
  mafVME *end_vme1 = GetEnd1VME();
  mafVME *end_vme2 = GetEnd2VME();
  mafVME *probedVme = GetPlottedVME();

  if (start_vme && start_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("StartVME");
    m_StartVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)start_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_StartVmeName = start_vme ? start_vme->GetName() : _("none");

  if (end_vme1 && end_vme1->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("EndVME1");
    m_EndVme1Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme1)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");

  if (end_vme2 && end_vme2->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("EndVME2");
    m_EndVme2Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme2)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_EndVme2Name = end_vme2 ? end_vme2->GetName() : _("none");

  m_ProbedVME = mafVMEVolumeGray::SafeDownCast(probedVme);
  m_ProbeVmeName = probedVme ? probedVme->GetName() : _("none");
}
//-------------------------------------------------------------------------
void mafVMEMeter::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_START_METER_LINK:
      case ID_END1_METER_LINK:
      case ID_END2_METER_LINK:
      {
        mafID button_id = e->GetId();
        mafString title = _("Choose meter vme link");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEMeter::VMEAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_START_METER_LINK)
          {
            SetMeterLink("StartVME", n);
            m_StartVmeName = n->GetName();
          }
          else if (button_id == ID_END1_METER_LINK)
          {
            SetMeterLink("EndVME1", n);
            m_EndVme1Name = n->GetName();
          }
          else
          {
            SetMeterLink("EndVME2", n);
            m_EndVme2Name = n->GetName();
          }
          m_Gui->Update();
          //InternalUpdate();
        }
      }
      break;
      case ID_PLOTTED_VME_LINK:
        {
          mafID button_id = e->GetId();
          mafString title = _("Choose meter vme link");
          e->SetId(VME_CHOOSE);
          e->SetArg((long)&mafVMEMeter::VolumeAccept);
          e->SetString(&title);
          ForwardUpEvent(e);
          mafNode *n = e->GetVme();
          if (n != NULL)
          {
            SetMeterLink("PlottedVME",n);
            m_ProbedVME = mafVMEVolumeGray::SafeDownCast(n);
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
      mafID button_id = e->GetId();
      e->SetId(CAMERA_UPDATE);
      ForwardUpEvent(e);
     
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
          x_init = mafGetFrame()->GetPosition().x;
          y_init = mafGetFrame()->GetPosition().y;
          m_HistogramDialog = new mmgDialogPreview(_("Histogram Dialog"), mafCLOSEWINDOW | mafUSERWI);
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
        mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{
  if (n->IsMAFType(mafVMELandmark))
  {
    SetLink(link_name,n->GetParent(),((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
  }
  else
    SetLink(link_name, n);
}
//-------------------------------------------------------------------------
mafVME *mafVMEMeter::GetStartVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("StartVME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMeter::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME1"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMeter::GetEnd2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME2"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMeter::GetPlottedVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("PlottedVME"));
}
//----------------------------------------------------------------------------
void mafVMEMeter::GenerateHistogram(int generate)
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
void mafVMEMeter::CreateHistogram()
//----------------------------------------------------------------------------
{
  if (m_ProbedVME != NULL)
  {
    vtkDataSet *probed_data = m_ProbedVME->GetOutput()->GetVTKData();
    probed_data->Update();

    vtkMAFSmartPointer<vtkTransform> transformStart;
    mafVME *start_vme = GetStartVME();
    mafMatrix *matrixStart = start_vme->GetOutput()->GetMatrix();
    double point1[3], rotStart[3];

    if (start_vme && start_vme->IsMAFType(mafVMELandmarkCloud))
    {
      int sub_id = GetLinkSubId("StartVME");
      ((mafVMELandmark *)((mafVMELandmarkCloud *)start_vme)->GetChild(sub_id))->GetOutput()->GetAbsPose(point1, rotStart);
      
    }
    else
    {
      point1[0] = m_StartPoint[0];
      point1[1] = m_StartPoint[1];
      point1[2] = m_StartPoint[2];
    }

    vtkMAFSmartPointer<vtkTransform> transformEnd;
   
    mafVME *end_vme1 = GetEnd1VME();
    mafMatrix *matrixEnd1 = end_vme1->GetOutput()->GetMatrix();

    double point2[3], rotEnd[3];
    if (end_vme1 && end_vme1->IsMAFType(mafVMELandmarkCloud))
    {
      int sub_id = GetLinkSubId("EndVME1");
      ((mafVMELandmark *)((mafVMELandmarkCloud *)end_vme1)->GetChild(sub_id))->GetOutput()->GetAbsPose(point2, rotEnd);

    }
    else
    {   
      
      point2[0] = m_EndPoint[0];
      point2[1] = m_EndPoint[1];
      point2[2] = m_EndPoint[2];
    }

    m_PlotActor->SetXRange(0,m_Distance);
    double srY[2];
    m_ProbedVME->GetOutput()->GetVTKData()->GetScalarRange(srY);
    m_PlotActor->SetYRange(srY);
    m_PlotActor->SetPlotCoordinate(0,m_Distance);

    double b[6];
    m_ProbedVME->GetOutput()->GetBounds(b);

    /*if(tmp1[0] < b[0]) tmp1[0] = b[0];
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
    else if (tmp2[2] > b[5]) tmp2[2] = b[5];*/

    m_ProbingLine->SetPoint1(point1);
    m_ProbingLine->SetPoint2(point2);
    m_ProbingLine->SetResolution((int)m_Distance);
    m_ProbingLine->Update();

    vtkMAFSmartPointer<vtkProbeFilter> prober;
    prober->SetInput(m_ProbingLine->GetOutput());
    prober->SetSource(probed_data);
    prober->Update();

    m_PlotActor->RemoveAllInputs();

    vtkPolyData *probimg_result = prober->GetPolyDataOutput();
    m_PlotActor->AddInput(probimg_result);
    if(m_HistogramRWI) m_HistogramRWI->m_RwiBase->Render();
  }
}