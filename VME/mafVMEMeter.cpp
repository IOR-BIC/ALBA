/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-09-28 23:11:46 $
  Version:   $Revision: 1.18 $
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
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "mafEventSource.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mmuIdFactory.h"
#include "mmgGui.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"

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
  
  m_MeterAttributes = NULL;
  m_VMEAccept = new mafVMEAccept();

  mafNEW(m_Transform);
  mafVMEOutputMeter *output = mafVMEOutputMeter::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  vtkNEW(m_LineSource);
  vtkNEW(m_LineSource2);
  vtkNEW(m_Goniometer);

  m_Goniometer->AddInput(m_LineSource->GetOutput());
  m_Goniometer->AddInput(m_LineSource2->GetOutput());

  vtkNEW(m_TmpTransform);

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  
  dpipe->SetInput(m_LineSource->GetOutput());
}
//-------------------------------------------------------------------------
mafVMEMeter::~mafVMEMeter()
//-------------------------------------------------------------------------
{
  cppDEL(m_VMEAccept);
  // mafDEL(m_MeterAttributes);  //modified by Marco. 29-9-2005 Attributes are destroyd automatically!
  mafDEL(m_Transform);
  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
  vtkDEL(m_Goniometer);
  vtkDEL(m_TmpTransform);
  SetOutput(NULL);
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
    m_Transform->SetMatrix(meter->m_Transform->GetMatrix());
    
    //modified by Marco. 29-9-2005 Mhmm... Poco pulito: se qualcuno risettasse questo attributo il metro 
    //non se ne accorgerebbe fino al prossimo reload... meglio chiedere sempre l'attributo al nodo...
    m_MeterAttributes = meter->GetMeterAttributes();  
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
          GetLink("EndVME2") == ((mafVMEMeter *)vme)->GetLink("EndVME2");
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
  m_MeterAttributes->m_ThresholdEvent = GetGenerateEvent();
  m_MeterAttributes->m_DeltaPercent   = GetDeltaPercent();
  m_MeterAttributes->m_InitMeasure    = GetInitMeasure();

  double threshold = m_MeterAttributes->m_InitMeasure * (1 + m_MeterAttributes->m_DeltaPercent / 100.0);

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
        start_ok = start_landmark->GetLandmarkVisibility();

      if(start_vme->IsA("mflVMELandmarkCloud") && GetStartVMELandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)start_vme)->GetLandmark(GetStartVMELandmarkId(),StartPoint,-1);
        m_TmpTransform->SetMatrix(start_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(StartPoint,StartPoint);
      }
      else*/
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end_landmark = mflVMELandmark::SafeDownCast(end_vme))
        end_ok = end_landmark->GetLandmarkVisibility();

      if(end_vme->IsA("mflVMELandmarkCloud") && GetEndVME1LandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)end_vme)->GetLandmark(GetEndVME1LandmarkId(),EndPoint,-1);
        m_TmpTransform->SetMatrix(end_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(EndPoint,EndPoint);
      }
      else*/
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

      if(GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE)
        m_Distance -= m_MeterAttributes->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
      //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

      // compute end point in local coordinate system
      double local_end[3];
      m_TmpTransform->TransformPoint(m_EndPoint,local_end);

      m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
      m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
      m_LineSource->Update();
    }
    else
      m_Distance = -1;

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && m_MeterAttributes->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
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
        start_ok = start_landmark->GetLandmarkVisibility();

      if(start_vme->IsA("mflVMELandmarkCloud") && GetStartVMELandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)start_vme)->GetLandmark(GetStartVMELandmarkId(),StartPoint,-1);
        m_TmpTransform->SetMatrix(start_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(StartPoint,StartPoint);
      }
      else*/
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();

      if(end1_vme->IsA("mflVMELandmarkCloud") && GetEndVME1LandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)end1_vme)->GetLandmark(GetEndVME1LandmarkId(),EndPoint,-1);
        m_TmpTransform->SetMatrix(end1_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(EndPoint,EndPoint);
      }
      else*/
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();

      if(end2_vme->IsA("mflVMELandmarkCloud") && GetEndVME2LandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)end2_vme)->GetLandmark(GetEndVME2LandmarkId(),EndPoint2,-1);
        m_TmpTransform->SetMatrix(end2_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(EndPoint2,EndPoint2);
      }
      else*/
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

      if(GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE)
        m_Distance -= m_MeterAttributes->m_InitMeasure;

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
    }
    else
      m_Distance = -1;

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && m_MeterAttributes->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
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
        start_ok = start_landmark->GetLandmarkVisibility();

      if(start_vme->IsA("mflVMELandmarkCloud") && GetStartVMELandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)start_vme)->GetLandmark(GetStartVMELandmarkId(),StartPoint,-1);
        m_TmpTransform->SetMatrix(start_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(StartPoint,StartPoint);
      }
      else*/
        start_vme->GetOutput()->GetAbsPose(m_StartPoint,orientation);

      /*if(mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();

      if(end1_vme->IsA("mflVMELandmarkCloud") && GetEndVME1LandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)end1_vme)->GetLandmark(GetEndVME1LandmarkId(),EndPoint,-1);
        m_TmpTransform->SetMatrix(end1_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(EndPoint,EndPoint);
      }
      else*/
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint,orientation);

      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();

      if(end2_vme->IsA("mflVMELandmarkCloud") && GetEndVME2LandmarkId() != -1)
      {
        ((mflVMELandmarkCloud *)end2_vme)->GetLandmark(GetEndVME2LandmarkId(),EndPoint2,-1);
        m_TmpTransform->SetMatrix(end2_vme->GetAbsPose());
        m_TmpTransform->TransformPoint(EndPoint2,EndPoint2);
      }
      else*/
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2,orientation);
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
          m_Angle -= m_MeterAttributes->m_InitMeasure;
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
    }
    else
      m_Angle = 0;

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE && m_MeterAttributes->m_ThresholdEvent > 0 && m_Angle > 0 && m_Angle >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }
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

  m_Transform->GetMatrix().Print(os,indent.GetNextIndent());
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
  if (m_MeterAttributes == NULL)
  {
    m_MeterAttributes = (mmaMeter *)GetAttribute("MeterAttributes");
    if (m_MeterAttributes == NULL)
    {
      mafNEW(m_MeterAttributes);
      SetAttribute("MeterAttributes", m_MeterAttributes);
      mafDEL(m_MeterAttributes);  //modified by Marco. 29-9-2005 Materials are kept alive by the Node
                                  // it cannot be sestroyed later since when loading MSF file the 
                                  // attribute is created by the mafNode class itself.
    }
  }
  return m_MeterAttributes;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterMode()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_MeterMode;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_DistanceRange[0] = min;
  m_MeterAttributes->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *mafVMEMeter::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_DistanceRange;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_ColorMode;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_MeasureType;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_Representation = representation;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_Representation;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_Capping = capping;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetMeterCapping()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_Capping;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetInitMeasure()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_InitMeasure;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double mafVMEMeter::GetMeterRadius()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_TubeRadius;
}
//-------------------------------------------------------------------------
void mafVMEMeter::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  m_MeterAttributes->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int mafVMEMeter::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  GetMeterAttributes();
  return m_MeterAttributes->m_DeltaPercent;
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
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  mafVME *start_vme = GetStartVME();
  m_StartVmeName = start_vme ? start_vme->GetName() : "none";
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,"Start", "Select the start vme for the meter");

  mafVME *end_vme1   = GetEnd1VME();
  m_EndVme1Name = end_vme1 ? end_vme1->GetName() : "none";
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,"End 1", "Select the end vme for point distance");

  mafVME *end_vme2   = GetEnd2VME();
  m_EndVme2Name = end_vme2 ? end_vme2->GetName() : "none";
  m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,"End 2", "Select the vme representing \nthe point for line distance");

  return m_Gui;
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
        mafString title = "Choose meter vme link";
        e->SetId(VME_CHOOSE);
        e->SetArg((long)m_VMEAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
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
          //InternalUpdate();
        }
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
