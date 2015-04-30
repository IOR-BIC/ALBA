/*=========================================================================

 Program: MAF2
 Module: mafVMEWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEWrappedMeter.h"
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
#include "mafGUI.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFSmartPointer.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"
#include "vtkOBBTree.h"
#include "vtkPoints.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLinearTransform.h"

#include <assert.h>

MAF_ID_IMP(mafVMEWrappedMeter::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEWrappedMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEWrappedMeter::mafVMEWrappedMeter()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  m_Angle         = 0.0;
	m_WrappedMode   = MANUAL_WRAP;
  m_WrapSide      = 0;
  m_WrapReverse   = 0;
  
  m_StartVmeName  = "";
  m_EndVme1Name   = "";
  m_EndVme2Name   = "";
  m_WrappedVmeName   = "";

  m_Gui = NULL;
  m_ListBox = NULL;
  
  mafNEW(m_Transform);
  mafVMEOutputWrappedMeter *output = mafVMEOutputWrappedMeter::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
  
  /*vtkNEW(m_PlaneSource);
  vtkNEW(m_PlaneCutter);
  vtkNEW(m_Cutter);
  vtkNEW(m_PlaneClip);
  vtkNEW(m_Clip);*/

  vtkNEW(m_LineSource);
  vtkNEW(m_LineSource2);
  //vtkNEW(m_LineSourceMiddle);
  vtkNEW(m_Goniometer);

  m_Goniometer->AddInput(m_LineSource->GetOutput());
  m_Goniometer->AddInput(m_LineSource2->GetOutput());

  mafNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_Goniometer->GetOutput());

}
//-------------------------------------------------------------------------
mafVMEWrappedMeter::~mafVMEWrappedMeter()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);

  /*vtkDEL(m_PlaneSource);
  vtkDEL(m_PlaneCutter);
  vtkDEL(m_Cutter);
  vtkDEL(m_PlaneClip);
  vtkDEL(m_Clip);*/

  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
//  vtkDEL(m_LineSourceMiddle);
  vtkDEL(m_Goniometer);
  mafDEL(m_TmpTransform);

	for(int i=0; i< m_MiddlePointList.size(); i++)
	{
		if(m_MiddlePointList[i]) delete m_MiddlePointList[i];
	}
	m_MiddlePointList.clear();

  for(int i=0; i< m_LineSourceList.size(); i++)
  {
    vtkDEL(m_LineSourceList[i]);
  }
  m_LineSourceList.clear();

	m_OrderMiddlePointsVMEList.clear();

  SetOutput(NULL);
}


//-------------------------------------------------------------------------
int mafVMEWrappedMeter::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a)==MAF_OK)
	{
		mafVMEWrappedMeter *meter = mafVMEWrappedMeter::SafeDownCast(a);
		m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

		mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
		if (dpipe)
		{
			dpipe->SetInput(m_Goniometer->GetOutput());
		}
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEWrappedMeter::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEWrappedMeter *)vme)->m_Transform->GetMatrix() && \
          GetLink("StartVME") == ((mafVMEWrappedMeter *)vme)->GetLink("StartVME") && \
          GetLink("EndVME1") == ((mafVMEWrappedMeter *)vme)->GetLink("EndVME1") && \
          GetLink("EndVME2") == ((mafVMEWrappedMeter *)vme)->GetLink("EndVME2");
  }
  return ret;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::InternalInitialize()
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
mmaMaterial *mafVMEWrappedMeter::GetMaterial()
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
mafVMEOutputWrappedMeter *mafVMEWrappedMeter::GetWrappedMeterOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputWrappedMeter *) GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool mafVMEWrappedMeter::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::InternalUpdate()
//-----------------------------------------------------------------------
{
  if(m_WrappedMode == MANUAL_WRAP)
  {
    InternalUpdateManual();
  }
  else if(m_WrappedMode == AUTOMATED_WRAP)
  {
    InternalUpdateAutomated();
  }
  else if(m_WrappedMode == IOR_AUTOMATED_WRAP)
  {
    InternalUpdateAutomatedIOR();
  }
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::InternalUpdateAutomatedIOR()
//-----------------------------------------------------------------------
{
  mafVME *start_vme = GetStartVME();
  mafVME *end_vme   = GetEnd1VME();
  mafVME *wrapped_vme   = GetWrappedVME();

  bool start_ok = true, end_ok = true;

  double orientation[3];
  m_Goniometer->RemoveAllInputs();


  if (start_vme && end_vme && wrapped_vme)
  { 
    if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("StartVME") != -1)
    {
      ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId("StartVME"),m_StartPoint,-1);
      m_TmpTransform->SetMatrix(*start_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
    }
    else
    {
      start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
    }

    if(wrapped_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("WrappedVME") != -1)
    {
      ((mafVMELandmarkCloud *)wrapped_vme)->GetLandmark(GetLinkSubId("WrappedVME"),m_WrappedVMECenter,-1);
      m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);
    }
    else
    {
      wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter, orientation);

      double translation[3];
      ((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);

      mafMatrix mat;
      mat.Identity();
      for(int i=0; i<3; i++)
        mat.SetElement(i,3,translation[i]);
      m_TmpTransform->SetMatrix(mat);
      m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);
    }

    if(end_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME1") != -1)
    {
      ((mafVMELandmarkCloud *)end_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_EndPoint,-1);
      m_TmpTransform->SetMatrix(*end_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
    }
    else
    {
      end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
    }
  }
  else
  {
    start_ok = false;
    end_ok   = false;
  }

  m_WrappedTangent1[0] =  m_WrappedTangent1[1] = m_WrappedTangent1[2] = 0.0;
  m_WrappedTangent2[0] =  m_WrappedTangent2[1] = m_WrappedTangent2[2] = 0.0;


  if (start_ok && end_ok)
  {
    // compute distance between points
    m_Distance = 0.0;

    // compute start point in local coordinate system
    double local_start[3];
    m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
    m_TmpTransform->Invert();
    m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
    //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

    // compute end point in local coordinate system
    double local_end[3];
    m_TmpTransform->TransformPoint(m_EndPoint,local_end);

    // compute end point in local coordinate system
    double local_wrapped_center[3];
    m_TmpTransform->TransformPoint(m_WrappedVMECenter,local_wrapped_center);

    // use list of tangent point (raw)
    double pointTangent1[3];
    double pointTangent2[3];

    //control if the point are allineated
    double vec1[3], vec2[3], vec3[3];
    vec1[0] = local_start[0] - local_end[0];
    vec1[1] = local_start[1] - local_end[1];
    vec1[2] = local_start[2] - local_end[2];

    vec2[0] = local_start[0] - local_wrapped_center[0];
    vec2[1] = local_start[1] - local_wrapped_center[1];
    vec2[2] = local_start[2] - local_wrapped_center[2];

    vec3[0] = local_end[0] - local_wrapped_center[0];
    vec3[1] = local_end[1] - local_wrapped_center[1];
    vec3[2] = local_end[2] - local_wrapped_center[2];

    bool aligned = false;
    double vectorProduct[3];
    vtkMath::Cross(vec1,vec2, vectorProduct); 
    if(vectorProduct[0] == 0.0 && vectorProduct[1] == 0.0 && vectorProduct[2] == 0.0)
      aligned = true;

    // create ordered list of tangent point (2) real algorithm
    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
    transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform());
    transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)wrapped_vme)->GetOutput()->GetVTKData());
    transformFirstDataInput->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
    transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
    transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
    transformFirstData->Update(); 

    // here REAL ALGORITHM //////////////////////////////
    vtkMAFSmartPointer<vtkOBBTree> locator;
    locator->SetDataSet(transformFirstData->GetOutput());
    locator->SetGlobalWarningDisplay(0);
    locator->BuildLocator();

    //Control if Start or End point is inside vtk data (surface)
    if(locator->InsideOrOutside(local_start) <= 0 || locator->InsideOrOutside(local_end) <= 0) 
    {
      AvoidWrapping(local_start,local_end);

      return;
    }

    vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
    vtkMAFSmartPointer<vtkPoints> pointsIntersection1;
    vtkMAFSmartPointer<vtkPoints> pointsIntersection2;

    //control if there is an intersection
    locator->IntersectWithLine(local_start, local_end, temporaryIntersection, NULL);

    //  code to control if exist an intersection between the line draw from start point to end point and 
    //  the vtk data (surface)
    int nControl = temporaryIntersection->GetNumberOfPoints();
    if(aligned == true)
    {
      AvoidWrapping(local_start,local_end);

      return;
    }

    //WRAPPED METER REFACTOR // 2008 09 15

    //local_wrapped_center -> center of wrapped surface
    //need a vector that is the normal of the plane

    vtkMatrix4x4 *mat = ((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix();
    mafMatrix matrix;
    matrix.SetVTKMatrix(mat);
    double versorX[3], versorY[3], versorZ[3];
    matrix.GetVersor(0, versorX);
    matrix.GetVersor(1, versorY);
    matrix.GetVersor(2, versorZ);
    // these versors determine the semiplanes, versor Z determines XY plane.
    
    double extendStart[3];
    double extendEnd[3];
    double factor = 100000;

    double direction[3];
    direction[0] = local_end[0] - local_start[0];
    direction[1] = local_end[1] - local_start[1];
    direction[2] = local_end[2] - local_start[2];

    extendStart[0] = local_start[0] - factor * direction[0];
    extendStart[1] = local_start[1] - factor * direction[1];
    extendStart[2] = local_start[2] - factor * direction[2];

    extendEnd[0] = local_end[0] + factor * direction[0];
    extendEnd[1] = local_end[1] + factor * direction[1];
    extendEnd[2] = local_end[2] + factor * direction[2];

    // for now only contol y, because data test is oriented in such way
    bool controlParallel = false;
    bool controlParallelExtend = false;
    bool semiplaneControlExtend = false;
    bool semiplaneControl = false;

    //WRAPPED METER REFACTOR // 2008 09 15

    double start_end_vector[3];
    start_end_vector[0] = local_end[0] - local_start[0];
    start_end_vector[1] = local_end[1] - local_start[1];
    start_end_vector[2] = local_end[2] - local_start[2];

    double start_center[3];
    start_center[0] = local_wrapped_center[0] - local_start[0];
    start_center[1] = local_wrapped_center[1] - local_start[1];
    start_center[2] = local_wrapped_center[2] - local_start[2];

    double vtemp[3], finalDirection[3];
    vtkMath::Cross(start_end_vector, start_center, vtemp);
    vtkMath::Cross(vtemp, start_end_vector, finalDirection);

    double dott = vtkMath::Dot(finalDirection, versorY);
    if(dott > 0. || nControl!=0)
    {
      WrappingCore(local_start, local_wrapped_center, local_end,\
        true, controlParallel,\
        locator, temporaryIntersection, pointsIntersection1,\
        versorY, versorZ,nControl);

      WrappingCore(local_end, local_wrapped_center, local_start,\
        false, controlParallel,\
        locator, temporaryIntersection, pointsIntersection2,\
        versorY, versorZ,nControl);
    }
    else
    {
      AvoidWrapping(local_start, local_end);
      return;
    }



    if(pointsIntersection1->GetNumberOfPoints() == 0 || pointsIntersection2->GetNumberOfPoints() == 0) return;

    pointTangent1[0] = pointsIntersection1->GetPoint(0)[0];
    pointTangent1[1] = pointsIntersection1->GetPoint(0)[1];
    pointTangent1[2] = pointsIntersection1->GetPoint(0)[2];

    pointTangent2[0] = pointsIntersection2->GetPoint(0)[0];
    pointTangent2[1] = pointsIntersection2->GetPoint(0)[1];
    pointTangent2[2] = pointsIntersection2->GetPoint(0)[2];

    //here put the code for cut and clip, so there is a wrap
    m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start,  pointTangent1)) + 
      sqrt(vtkMath::Distance2BetweenPoints(pointTangent2, local_end));

    //search normal to plane

    m_PlaneSource->SetOrigin(local_wrapped_center);
    m_PlaneSource->SetPoint1(pointTangent1);
    m_PlaneSource->SetPoint2(pointTangent2);

    m_PlaneCutter->SetOrigin(local_wrapped_center);
    m_PlaneCutter->SetNormal(m_PlaneSource->GetNormal());


    m_Cutter->SetInput(transformFirstData->GetOutput());
    m_Cutter->SetCutFunction(m_PlaneCutter);

    double midPoint[3];
    midPoint[0] = (pointTangent2[0] + pointTangent1[0])/2;
    midPoint[1] = (pointTangent2[1] + pointTangent1[1])/2;
    midPoint[2] = (pointTangent2[2] + pointTangent1[2])/2;

    double normal[3];
    normal[0] = midPoint[0] - local_wrapped_center[0];
    normal[1] = midPoint[1] - local_wrapped_center[1];
    normal[2] = midPoint[2] - local_wrapped_center[2];

    /*if(controlParallelExtend==true && controlParallel==false)
    {
      double distance = vtkMath::Distance2BetweenPoints(midPoint,local_wrapped_center);
      double shift[3];
      shift[0] = midPoint[0] + normal[0];
      shift[1] = midPoint[1] + normal[1];
      shift[2] = midPoint[2] + normal[2];
      double distance2 = vtkMath::Distance2BetweenPoints(shift,local_wrapped_center);
      if(distance2<distance)
      {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
      }
    }*/

    if( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0) return; // midpoint and center are the same point

    vtkMath::Normalize(normal);

    double start_tangent[3];
    start_tangent[0] = pointTangent1[0] - local_start[0];
    start_tangent[1] = pointTangent1[1] - local_start[1];
    start_tangent[2] = pointTangent1[2] - local_start[2];

    double end_tangent[3];
    end_tangent[0] = pointTangent2[0] - local_end[0];
    end_tangent[1] = pointTangent2[1] - local_end[1];
    end_tangent[2] = pointTangent2[2] - local_end[2];

    double sumtangents[3];
    sumtangents[0] = start_tangent[0] + end_tangent[0];
    sumtangents[1] = start_tangent[1] + end_tangent[1];
    sumtangents[2] = start_tangent[2] + end_tangent[2];

    vtkMath::Normalize(sumtangents);
    double dotNormal = vtkMath::Dot(normal, sumtangents);

    vtkMath::Normalize(start_tangent);
    vtkMath::Normalize(end_tangent);
    double cross[3];
    vtkMath::Cross(start_tangent,end_tangent,cross);
    const double tolerance = 0.05;
    bool zerovector = false;
    if(cross[0] < tolerance && cross[1] < tolerance && cross[2] < tolerance)
    {
      zerovector = true;
    }
    if(dotNormal < 0. && zerovector == false)
    {
      normal[0] = -normal[0];
      normal[1] = -normal[1];
      normal[2] = -normal[2];
    }

    m_PlaneClip->SetOrigin(midPoint);
    m_PlaneClip->SetNormal(normal);


    m_Clip->SetInput(m_Cutter->GetOutput());
    m_Clip->SetClipFunction(m_PlaneClip);

    double clipLength = 0;
    double numberOfCells = m_Clip->GetOutput()->GetNumberOfCells();
    for(int i=0; i<numberOfCells; i++)
    {
      clipLength += sqrt(m_Clip->GetOutput()->GetCell(i)->GetLength2());
    }

    m_Distance += clipLength;


    ////////////////////////////////////////////////////////

    m_WrappedTangent1[0] = pointTangent1[0];
    m_WrappedTangent1[1] = pointTangent1[1];
    m_WrappedTangent1[2] = pointTangent1[2];

    m_WrappedTangent2[0] = pointTangent2[0];
    m_WrappedTangent2[1] = pointTangent2[1];
    m_WrappedTangent2[2] = pointTangent2[2];

    m_LineSource2->SetPoint1(pointTangent2[0],pointTangent2[1],pointTangent2[2]);
    m_LineSource2->SetPoint2(local_end[0],local_end[1],local_end[2]);

    m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
    m_LineSource->SetPoint2(pointTangent1[0],pointTangent1[1],pointTangent1[2]);

    m_Goniometer->AddInput(m_LineSource->GetOutput());
    m_Goniometer->AddInput(m_LineSource2->GetOutput());
    //m_Goniometer->AddInput(m_LineSourceMiddle->GetOutput());
    m_Goniometer->AddInput(m_Clip->GetOutput());


    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    GetWrappedMeterOutput()->Update(); 
  }
  else
    m_Distance = -1;
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::WrappingCore(double *init, double *center, double *end,\
                                      bool IsStart, bool controlParallel,\
                                      vtkOBBTree *locator, vtkPoints *temporaryIntersection, vtkPoints *pointsIntersection,\
                                      double *versorY, double *versorZ, int nControl)
//-----------------------------------------------------------------------
{
  double p1[3], p2[3], p3[3];
  p1[0] = init[0];
  p1[1] = init[1];
  p1[2] = init[2];

  const int factorLenght = 3;
  
  p2[0] = center[0]  + factorLenght * (center[0] - init[0]);
  p2[1] = center[1]  + factorLenght * (center[1] - init[1]);
  p2[2] = center[2]  + factorLenght * (center[2] - init[2]);

  p3[0] = end[0];
  p3[1] = end[1];
  p3[2] = end[2];

  double v2[3];
  int count =0;
  int n1 = -1; // number of intersections

  int precision = 50;
  short wrapside = -1; // m_WrapSide == 0 ? (-1) : (1);
  int invertDirection = 1;
  
  while(n1 != 0)
  {
    locator->IntersectWithLine(p1, p2, temporaryIntersection, NULL);
    n1 = temporaryIntersection->GetNumberOfPoints();

    if(n1 != 0)
    {
      pointsIntersection->DeepCopy(temporaryIntersection);
    }

    if (n1 == 0 && invertDirection == 1)
    {
      invertDirection = -1;
      precision = 5;
      n1 = 1;
    }
    else if (n1 == 0 && invertDirection == -1)
    {
      n1 = 1;
    }
    else if (n1 != 0 && invertDirection == -1)
    {
      break;
    }

    if(count == 0)
    {

      /*for(int i = 0; i<3; i++)
      {
        v1[i] = (p2[i] - p1[i]);
        v2[i] = (p3[i] - p2[i]);

      }*/
      
      double init_center[3];
      init_center[0] = center[0] - init[0];
      init_center[1] = center[1] - init[1];
      init_center[2] = center[2] - init[2];

      double end_center[3];
      end_center[0] = end[0] - center[0];
      end_center[1] = end[1] - center[1];
      end_center[2] = end[2] - center[2];

      /*double middle[3];
      middle[0] = (end[0] + init[0])/2.;
      middle[1] = (end[1] + init[1])/2.;
      middle[2] = (end[2] + init[2])/2.;
      double centerMiddle[3];
      centerMiddle[0] = center[0] - middle[0];
      centerMiddle[1] = center[1] - middle[1];
      centerMiddle[2] = center[2] - middle[2];*/


      /*double dotXM = vtkMath::Dot(centerMiddle, x);
      double dotYM = vtkMath::Dot(centerMiddle, versorY);
      double dotZM = vtkMath::Dot(centerMiddle, versorZ);*/

      //function

      vtkMAFSmartPointer<vtkPlaneSource> planeSource;
      planeSource->SetOrigin(init);
      planeSource->SetPoint1(center);
      planeSource->SetPoint2(end);
      planeSource->Update();

      double normal[3];
      planeSource->GetNormal(normal);

      
      //mafLogMessage("DotMX %.2f DotMY %.2f DotMZ %.2f", dotXM,dotYM,dotZM);

      
      double v2new[3];
      vtkMath::Cross(normal,init_center, v2new);
      vtkMath::Normalize(v2new);
      //double dot = vtkMath::Dot(v2new, v2);
      v2[0] = v2new[0];
      v2[1] = v2new[1];
      v2[2] = v2new[2];

      double dot2 = vtkMath::Dot(v2, versorY);
      double dot3 = vtkMath::Dot(v2, versorZ);

      
      double dot4 = -1;

      mafLogMessage("DotVersorY %.2f ", dot2);
      mafLogMessage("DotVersorZ %.2f ", dot3);


      if(IsStart == false)
      {
        double insertionVector[3];
        insertionVector[0] = -init_center[0];
        insertionVector[1] = -init_center[1];
        insertionVector[2] = -init_center[2];
        dot4 = vtkMath::Dot(insertionVector, versorZ);
      }

      mafLogMessage("Z: %.2f" , dot4);
      if(dot4 < 0.)
      {
        if(dot2 > 0.)
        {
          v2[0] = - v2[0];
          v2[1] = - v2[1];
          v2[2] = - v2[2];
        }
      }
      else
      {
        if(dot2 < 0.)
        {
          v2[0] = - v2[0];
          v2[1] = - v2[1];
          v2[2] = - v2[2];
        }
      }
      
      
      
      
      
    }

    for(int i = 0; i<3; i++)
      p2[i] += (invertDirection*(wrapside*precision) * v2[i]);

    count++;
  }
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::AvoidWrapping(double *local_start, double *local_end)
//-----------------------------------------------------------------------
{
  //if there is no intersection with geometry
  m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
  m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
  m_Goniometer->AddInput(m_LineSource->GetOutput());
  
  m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start, local_end));

  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
  GetWrappedMeterOutput()->Update();
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::InternalUpdateAutomated()
//-----------------------------------------------------------------------
{
  mafVME *start_vme = GetStartVME();
  mafVME *end_vme   = GetEnd1VME();
  mafVME *wrapped_vme   = GetWrappedVME();

  bool start_ok = true, end_ok = true;

  double orientation[3];
  m_Goniometer->RemoveAllInputs();


  if (start_vme && end_vme && wrapped_vme)
  { 
    if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("StartVME") != -1)
    {
      ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId("StartVME"),m_StartPoint,-1);
      m_TmpTransform->SetMatrix(*start_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
    }
    else
    {
      start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
    }

    if(wrapped_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("WrappedVME") != -1)
    {
      ((mafVMELandmarkCloud *)wrapped_vme)->GetLandmark(GetLinkSubId("WrappedVME"),m_WrappedVMECenter,-1);
      m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);
    }
    else
    {
      wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter, orientation);

      double translation[3];
      ((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);
      
      mafMatrix mat;
      mat.Identity();
      for(int i=0; i<3; i++)
        mat.SetElement(i,3,translation[i]);
      m_TmpTransform->SetMatrix(mat);
      m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);
    }

    if(end_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("EndVME1") != -1)
    {
      ((mafVMELandmarkCloud *)end_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_EndPoint,-1);
      m_TmpTransform->SetMatrix(*end_vme->GetOutput()->GetAbsMatrix());
      m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
    }
    else
    {
      end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
    }
  }
  else
  {
    start_ok = false;
    end_ok   = false;
  }

  m_WrappedTangent1[0] =  m_WrappedTangent1[1] = m_WrappedTangent1[2] = 0.0;
  m_WrappedTangent2[0] =  m_WrappedTangent2[1] = m_WrappedTangent2[2] = 0.0;
 

  if (start_ok && end_ok)
  {
    // compute distance between points
    m_Distance = 0.0;

    // compute start point in local coordinate system
    double local_start[3];
    m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
    m_TmpTransform->Invert();
    m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
    //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

    // compute end point in local coordinate system
    double local_end[3];
    m_TmpTransform->TransformPoint(m_EndPoint,local_end);

    // compute end point in local coordinate system
    double local_wrapped_center[3];
    m_TmpTransform->TransformPoint(m_WrappedVMECenter,local_wrapped_center);

    // use list of tangent point (raw)
    double pointTangent1[3];
    double pointTangent2[3];

    //control if the point are allineated
    double vec1[3], vec2[3], vec3[3];
    vec1[0] = local_start[0] - local_end[0];
    vec1[1] = local_start[1] - local_end[1];
    vec1[2] = local_start[2] - local_end[2];

    vec2[0] = local_start[0] - local_wrapped_center[0];
    vec2[1] = local_start[1] - local_wrapped_center[1];
    vec2[2] = local_start[2] - local_wrapped_center[2];

		vec3[0] = local_end[0] - local_wrapped_center[0];
		vec3[1] = local_end[1] - local_wrapped_center[1];
		vec3[2] = local_end[2] - local_wrapped_center[2];

    bool aligned = false;
    double vectorProduct[3];
    vtkMath::Cross(vec1,vec2, vectorProduct); 
    if(vectorProduct[0] == 0.0 && vectorProduct[1] == 0.0 && vectorProduct[2] == 0.0)
      aligned = true;
    
    // create ordered list of tangent point (2) real algorithm
    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
    transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform());
    transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)wrapped_vme)->GetOutput()->GetVTKData());
    transformFirstDataInput->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
    transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
    transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
    transformFirstData->Update(); 

    // here REAL ALGORITHM //////////////////////////////
    vtkMAFSmartPointer<vtkOBBTree> locator;
    locator->SetDataSet(transformFirstData->GetOutput());
    locator->SetGlobalWarningDisplay(0);
    locator->BuildLocator();

   //Control if Start or End point is inside vtk data (surface)
   if(locator->InsideOrOutside(local_start) <= 0 || locator->InsideOrOutside(local_end) <= 0) 
   {
     //if one point is inside connect start and end
     m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
     m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
     m_Goniometer->AddInput(m_LineSource->GetOutput());

     m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start, local_end));

     m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
     GetWrappedMeterOutput()->Update();

     return;
  }

    vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
    vtkMAFSmartPointer<vtkPoints> pointsIntersection1;
    vtkMAFSmartPointer<vtkPoints> pointsIntersection2;
   
    double p1[3], p2[3], p3[3];
    p1[0] = local_start[0];
    p1[1] = local_start[1];
    p1[2] = local_start[2];

    const int factorLenght = 3;
    p2[0] = local_wrapped_center[0] + factorLenght * (local_wrapped_center[0] - local_start[0]);
    p2[1] = local_wrapped_center[1] + factorLenght * (local_wrapped_center[1] - local_start[1]);
    p2[2] = local_wrapped_center[2] + factorLenght * (local_wrapped_center[2] - local_start[2]);

    p3[0] = local_end[0];
    p3[1] = local_end[1];
    p3[2] = local_end[2];

    double v1[3],v2[3],vtemp[3];
    int count =0;
    int n1 = -1; // number of intersections

    //control if there is an intersection
    locator->IntersectWithLine(local_start, local_end, temporaryIntersection, NULL);

//  code to control if exist an intersection between the line draw from start point to end point and 
//  the vtk data (surface)
     int nControl = temporaryIntersection->GetNumberOfPoints();
     if(aligned == true)
     {
       //if there is no intersection with geometry
       m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
       m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
       m_Goniometer->AddInput(m_LineSource->GetOutput());
       
       m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start, local_end));
 
       m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
       GetWrappedMeterOutput()->Update();
 
       return;
     }

     //WRAPPED METER REFACTOR // 2008 09 15

     //local_wrapped_center -> center of wrapped surface
     //need a vector that is the normal of the plane

     vtkMatrix4x4 *mat = ((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix();
     mafMatrix matrix;
     matrix.SetVTKMatrix(mat);
     double versorX[3], versorY[3], versorZ[3];
     matrix.GetVersor(0, versorX);
     matrix.GetVersor(1, versorY);
     matrix.GetVersor(2, versorZ);
     // these versors determine the semiplanes, versor Z determines XY plane.
     double t;
     double pseudoIntersection[3];

     // for now only contol y, because data test is oriented in such way
     bool controlParallel = (vtkPlane::IntersectWithLine(local_start,local_end,versorZ,local_wrapped_center, t, pseudoIntersection)!=0.)?true:false;
     bool semiplaneControl = (pseudoIntersection[1] - local_wrapped_center[1])<0? true: false ;
     

     if(nControl==0)
     {
       if(controlParallel == false || semiplaneControl == false /* INTERSECTION WRAPPED PLANE is positive or zero*/)
       {
         //if there is no intersection with geometry
         m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
         m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
         m_Goniometer->AddInput(m_LineSource->GetOutput());

         m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start, local_end));

         m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
         GetWrappedMeterOutput()->Update();

         return;
       }
       else
       {
         // just continue to calculate
       }
       
     }
     //WRAPPED METER REFACTOR // 2008 09 15

    int precision = 50;
    short wrapside = m_WrapSide == 0 ? (-1) : (1);
    int invertDirection = 1;
    while(n1 != 0)
    {
     locator->IntersectWithLine(p1, p2, temporaryIntersection, NULL);
     n1 = temporaryIntersection->GetNumberOfPoints();

     if(n1 != 0)
     {
       pointsIntersection1->DeepCopy(temporaryIntersection);
     }

     if (n1 == 0 && invertDirection == 1)
     {
       invertDirection = -1;
       precision = 5;
       n1 = 1;
     }
     else if (n1 == 0 && invertDirection == -1)
     {
       n1 = 1;
     }
     else if (n1 != 0 && invertDirection == -1)
     {
       break;
     }

     if(count == 0)
     {
       for(int i = 0; i<3; i++)
       {
         v1[i] = (p2[i] - p1[i]);
         v2[i] = (p3[i] - p2[i]);
       }

       // the order is important to understand the direction of the control
       vtkMath::Cross(v1,v2, vtemp);
       vtkMath::Cross(vtemp,v1, v2);
       vtkMath::Normalize(v2);
     }
     
     for(int i = 0; i<3; i++)
      p2[i] += (invertDirection*(wrapside*precision) * v2[i]);
      
     count++;
    }
    

    p1[0] = local_end[0];
    p1[1] = local_end[1];
    p1[2] = local_end[2];

    p2[0] = local_wrapped_center[0] + factorLenght * (local_wrapped_center[0] - local_end[0]);
    p2[1] = local_wrapped_center[1] + factorLenght * (local_wrapped_center[1] - local_end[1]);
    p2[2] = local_wrapped_center[2] + factorLenght * (local_wrapped_center[2] - local_end[2]);

    p3[0] = local_start[0];
    p3[1] = local_start[1];
    p3[2] = local_start[2];

    count = 0;
    int n2 = -1; // number of intersections
    precision = 50;
    invertDirection = 1;
    while(n2 != 0)
    {
      locator->IntersectWithLine(p1, p2, temporaryIntersection, NULL);
      n2 = temporaryIntersection->GetNumberOfPoints();

      if(n2 != 0)
      {
        pointsIntersection2->DeepCopy(temporaryIntersection);
      }

      if (n2 == 0 && invertDirection == 1)
      {
        invertDirection = -1;
        precision = 5;
        n2 = 1;
      }
      else if (n2 == 0 && invertDirection == -1)
      {
        n2 = 1;
      }
      else if (n2 != 0 && invertDirection == -1)
      {
        break;
      }

      if(count == 0)
      {
        for(int i = 0; i<3; i++)
        {
          v1[i] = (p2[i] - p1[i]);
          v2[i] = (p3[i] - p2[i]);
        }

        // the order is important to understand the direction of the control
        vtkMath::Cross(v1,v2, vtemp);
        vtkMath::Cross(vtemp,v1, v2);
        vtkMath::Normalize(v2);
      }

      for(int i = 0; i<3; i++)
        p2[i] += (invertDirection*(wrapside*precision) * v2[i]);

      count++;
    }
    ////////////////////////////////////////////////////
    
    if(pointsIntersection1->GetNumberOfPoints() == 0 || pointsIntersection2->GetNumberOfPoints() == 0) return;

    pointTangent1[0] = pointsIntersection1->GetPoint(0)[0];
    pointTangent1[1] = pointsIntersection1->GetPoint(0)[1];
    pointTangent1[2] = pointsIntersection1->GetPoint(0)[2];

    pointTangent2[0] = pointsIntersection2->GetPoint(0)[0];
    pointTangent2[1] = pointsIntersection2->GetPoint(0)[1];
    pointTangent2[2] = pointsIntersection2->GetPoint(0)[2];

    //here put the code for cut and clip, so there is a wrap
    m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start,  pointTangent1)) + 
                 sqrt(vtkMath::Distance2BetweenPoints(pointTangent2, local_end));

    //search normal to plane
    
    m_PlaneSource->SetOrigin(local_wrapped_center);
    m_PlaneSource->SetPoint1(pointTangent1);
    m_PlaneSource->SetPoint2(pointTangent2);
    
    m_PlaneCutter->SetOrigin(local_wrapped_center);
    m_PlaneCutter->SetNormal(m_PlaneSource->GetNormal());

    
    m_Cutter->SetInput(transformFirstData->GetOutput());
    m_Cutter->SetCutFunction(m_PlaneCutter);

    double midPoint[3];
    midPoint[0] = (pointTangent2[0] + pointTangent1[0])/2;
    midPoint[1] = (pointTangent2[1] + pointTangent1[1])/2;
    midPoint[2] = (pointTangent2[2] + pointTangent1[2])/2;

    double normal[3];
    normal[0] = midPoint[0] - local_wrapped_center[0];
    normal[1] = midPoint[1] - local_wrapped_center[1];
    normal[2] = midPoint[2] - local_wrapped_center[2];

		if( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0) return; // midpoint and center are the same point

    m_PlaneClip->SetOrigin(midPoint);
    m_PlaneClip->SetNormal(normal);

    
    m_Clip->SetInput(m_Cutter->GetOutput());
    m_Clip->SetClipFunction(m_PlaneClip);

    double clipLength = 0;
		double numberOfCells = m_Clip->GetOutput()->GetNumberOfCells();
    for(int i=0; i<numberOfCells; i++)
    {
      clipLength += sqrt(m_Clip->GetOutput()->GetCell(i)->GetLength2());
    }

    m_Distance += clipLength;


    ////////////////////////////////////////////////////////

    m_WrappedTangent1[0] = pointTangent1[0];
    m_WrappedTangent1[1] = pointTangent1[1];
    m_WrappedTangent1[2] = pointTangent1[2];
    
    m_WrappedTangent2[0] = pointTangent2[0];
    m_WrappedTangent2[1] = pointTangent2[1];
    m_WrappedTangent2[2] = pointTangent2[2];
    
    m_LineSource2->SetPoint1(pointTangent2[0],pointTangent2[1],pointTangent2[2]);
    m_LineSource2->SetPoint2(local_end[0],local_end[1],local_end[2]);

    m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
    m_LineSource->SetPoint2(pointTangent1[0],pointTangent1[1],pointTangent1[2]);

    m_Goniometer->AddInput(m_LineSource->GetOutput());
    m_Goniometer->AddInput(m_LineSource2->GetOutput());
    //m_Goniometer->AddInput(m_LineSourceMiddle->GetOutput());
    m_Goniometer->AddInput(m_Clip->GetOutput());


    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    GetWrappedMeterOutput()->Update(); 
  }
  else
    m_Distance = -1;
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::InternalUpdateManual()
//-----------------------------------------------------------------------
{
	//if(m_Gui == NULL) CreateGui();
	if(m_OrderMiddlePointsNameVMEList.size() == 0) SyncronizeList();

  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  if (GetMeterMode() == mafVMEWrappedMeter::POINT_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end_vme   = GetEnd1VME();

    bool start_ok = true, end_ok = true;

    bool result = MiddlePointsControl();

    double orientation[3];
		m_Goniometer->RemoveAllInputs();

		for(int i=0; i< m_MiddlePointList.size(); i++)
		{
			if(m_MiddlePointList[i]) delete m_MiddlePointList[i];
		}
		m_MiddlePointList.clear();

    std::vector<double *> localMiddlePointList;

    if (start_vme && end_vme && result)
    { 
      for(int j=0; j<m_OrderMiddlePointsNameVMEList.size(); j++)
      {
        mafNode *middleVME = IndexToMiddlePointVME(j);
        m_MiddlePointList.push_back(new double[3]);
        if(middleVME->IsMAFType(mafVMELandmarkCloud))
        {
          ((mafVMELandmarkCloud *)middleVME)->GetLandmark(m_OrderMiddlePointsNameVMEList[j],m_MiddlePointList[m_MiddlePointList.size()-1],-1);
          m_TmpTransform->SetMatrix(*((mafVME *)middleVME)->GetOutput()->GetAbsMatrix());
          m_TmpTransform->TransformPoint(m_MiddlePointList[m_MiddlePointList.size()-1],m_MiddlePointList[m_MiddlePointList.size()-1]);
        }
        else
        {
          ((mafVME *)middleVME)->GetOutput()->GetAbsPose(m_MiddlePointList[m_MiddlePointList.size()-1], orientation);
        }
        
      }

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
      m_Distance = 0;
      if(m_MiddlePointList.size() == 0)
        m_Distance = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint, m_EndPoint));
      else
			{
        for(int j=0; j<m_MiddlePointList.size(); j++)
        {
          double pointA[3], pointB[3];

          if(j==0)
          {
            pointA[0] = m_StartPoint[0];
            pointA[1] = m_StartPoint[1];
            pointA[2] = m_StartPoint[2];

            pointB[0] = m_MiddlePointList[j][0];
            pointB[1] = m_MiddlePointList[j][1];
            pointB[2] = m_MiddlePointList[j][2]; 
          }
          else
          {
            pointA[0] = m_MiddlePointList[j-1][0];
            pointA[1] = m_MiddlePointList[j-1][1];
            pointA[2] = m_MiddlePointList[j-1][2];

            pointB[0] = m_MiddlePointList[j][0];
            pointB[1] = m_MiddlePointList[j][1];
            pointB[2] = m_MiddlePointList[j][2];

          }
          m_Distance += sqrt(vtkMath::Distance2BetweenPoints(pointA, pointB));

          if(j == m_MiddlePointList.size()-1)
          {
            pointA[0] = m_MiddlePointList[j][0];
            pointA[1] = m_MiddlePointList[j][1];
            pointA[2] = m_MiddlePointList[j][2];

            pointB[0] = m_EndPoint[0];
            pointB[1] = m_EndPoint[1];
            pointB[2] = m_EndPoint[2];
            m_Distance += sqrt(vtkMath::Distance2BetweenPoints(pointA, pointB));
          }
        }
			}

      if(GetMeterMeasureType() == mafVMEWrappedMeter::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
      //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

      
      //middle points
      for(int j=0; j<m_MiddlePointList.size(); j++)
      {
        localMiddlePointList.push_back(new double[3]);
        m_TmpTransform->TransformPoint(m_MiddlePointList[j],localMiddlePointList[localMiddlePointList.size()-1]);
      }

      // compute end point in local coordinate system
      double local_end[3];
      m_TmpTransform->TransformPoint(m_EndPoint,local_end);

			
      if(m_MiddlePointList.size() == 0)
      {
        m_LineSource2->SetPoint1(local_start[0],local_start[1],local_start[2]);
        m_LineSource2->SetPoint2(local_start[0],local_start[1],local_start[2]);
        m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
        m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
				
        m_Goniometer->AddInput(m_LineSource->GetOutput());
        m_Goniometer->AddInput(m_LineSource2->GetOutput());

      }

      for(int j=0; j<m_MiddlePointList.size(); j++)
      {    
        m_LineSourceList.push_back(vtkLineSource::New());
        m_Goniometer->AddInput(m_LineSourceList[m_LineSourceList.size()-1]->GetOutput());
        if(j==0)
        {        
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint1(local_start[0],local_start[1],local_start[2]);
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint2(localMiddlePointList[0][0],localMiddlePointList[0][1],localMiddlePointList[0][2]);
        }
        else
        {         
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint1(localMiddlePointList[j-1][0],localMiddlePointList[j-1][1],localMiddlePointList[j-1][2]);
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint2(localMiddlePointList[j][0],localMiddlePointList[j][1],localMiddlePointList[j][2]);
        }
        

        if(j == m_MiddlePointList.size()-1)
        {
          m_LineSourceList.push_back(vtkLineSource::New());
          m_Goniometer->AddInput(m_LineSourceList[m_LineSourceList.size()-1]->GetOutput());
       
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint1(localMiddlePointList[j][0],localMiddlePointList[j][1],localMiddlePointList[j][2]);
          m_LineSourceList[m_LineSourceList.size()-1]->SetPoint2(local_end[0],local_end[1],local_end[2]);
        }
      }

      m_Goniometer->Modified();
    }
    else
      m_Distance = -1;

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    

    if(GetMeterMeasureType() == mafVMEWrappedMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);

    

    for(int i=0; i< m_MiddlePointList.size(); i++)
    {
      if(localMiddlePointList[i]) delete localMiddlePointList[i];
    }
    localMiddlePointList.clear();
  }
/*  else if (GetMeterMode() == mafVMEWrappedMeter::LINE_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *end2_vme  = GetEnd2VME();

    m_Goniometer->RemoveAllInputs();
		m_Goniometer->AddInput(m_LineSource->GetOutput());
		m_Goniometer->AddInput(m_LineSource2->GetOutput());
		for(int i=0; i< m_LineSourceList.size(); i++)
		{
			vtkDEL(m_LineSourceList[i]);
		}
		m_LineSourceList.clear();

    bool start_ok = true, end1_ok = true, end2_ok = true;
    double orientation[3];

    if (start_vme && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      //if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
      //  start_ok = start_landmark->GetLandmarkVisibility();

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
      //if (mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
      //  end1_ok = end1_landmark->GetLandmarkVisibility();

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
      //if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
      //  end2_ok = end2_landmark->GetLandmarkVisibility();

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

      if(GetMeterMeasureType() == mafVMEWrappedMeter::RELATIVE_MEASURE)
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

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEWrappedMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      m_EventSource->InvokeEvent(this, LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == mafVMEWrappedMeter::LINE_ANGLE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *end2_vme  = GetEnd2VME();


		m_Goniometer->RemoveAllInputs();
		m_Goniometer->AddInput(m_LineSource->GetOutput());
		m_Goniometer->AddInput(m_LineSource2->GetOutput());
		for(int i=0; i< m_LineSourceList.size(); i++)
		{
			vtkDEL(m_LineSourceList[i]);
		}
		m_LineSourceList.clear();


    double orientation[3];

    bool start_ok = true, end1_ok = true, end2_ok = true;
    if (start_vme && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      //if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
      //start_ok = start_landmark->GetLandmarkVisibility();

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

      //if(mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
      // end1_ok = end1_landmark->GetLandmarkVisibility();

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

      //if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
      //  end2_ok = end2_landmark->GetLandmarkVisibility();

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
        if(GetMeterMeasureType() == mafVMEWrappedMeter::RELATIVE_MEASURE)
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

    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == mafVMEWrappedMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Angle > 0 && m_Angle >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }*/
}
//-----------------------------------------------------------------------
int mafVMEWrappedMeter::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if(m_Gui == NULL) //this for update wrapped vme lists
  {
    InternalUpdateManual();
    CreateGui();
  }

  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
		m_OrderMiddlePointsVMEList.clear();

		for(int i=0; i<m_MiddlePointList.size(); i++)
		{
			mafNode *node;
			node = IndexToMiddlePointVME(i);
			if(node == NULL) continue;
			int vmeId = node->GetId();

			PushIdVector(vmeId);

			if(mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(node))
			{
				int index = -1;
				for(int j=0; j< lc->GetNumberOfLandmarks(); j++)
				{

          mafString name = "";
          name  = lc->GetLandmarkName(j);
          
          if(name.Equals(m_OrderMiddlePointsNameVMEList[i])) index = j;
          
				}
				
        PushIdVector(index);
			}
		}
		parent->StoreInteger("OrderMiddlePointVmeNumberOfElements", m_OrderMiddlePointsVMEList.size());
		parent->StoreVectorN("OrderMiddlePointVme",m_OrderMiddlePointsVMEList,m_OrderMiddlePointsVMEList.size());

    parent->StoreInteger("WrapMode", m_WrappedMode);
    parent->StoreInteger("WrapSide", m_WrapSide);
    parent->StoreInteger("WrapReverse", m_WrapReverse);

    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEWrappedMeter::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
			node->RestoreInteger("OrderMiddlePointVmeNumberOfElements", m_OrderMiddlePointsVMEListNumberOfElements);
			m_OrderMiddlePointsVMEList.resize(m_OrderMiddlePointsVMEListNumberOfElements);
			node->RestoreVectorN("OrderMiddlePointVme",m_OrderMiddlePointsVMEList,m_OrderMiddlePointsVMEListNumberOfElements);
			
      node->RestoreInteger("WrapMode", m_WrappedMode);
      node->RestoreInteger("WrapSide", m_WrapSide);
      node->RestoreInteger("WrapReverse", m_WrapReverse);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEWrappedMeter::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** mafVMEWrappedMeter::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "mafVMEWrappedMeter.xpm"
//  return mafVMEWrappedMeter_xpm;
//}
//-------------------------------------------------------------------------
mmaMeter *mafVMEWrappedMeter::GetMeterAttributes()
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
void mafVMEWrappedMeter::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetMeterMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *mafVMEWrappedMeter::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double mafVMEWrappedMeter::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double mafVMEWrappedMeter::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int mafVMEWrappedMeter::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double mafVMEWrappedMeter::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double mafVMEWrappedMeter::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEWrappedMeter::CreateGui()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;

  int num_mode = 3;
	int num_wrap = 3;
//	const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};;
  const wxString wrap_choices_string[] = {_("manual"), _("automated"), _("IOR_automated")};

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Combo(ID_WRAPPED_METER_MODE,_("wrap"),&m_WrappedMode,num_wrap,wrap_choices_string,_("Choose the meter mode"));
	//m_Gui->Combo(ID_METER_MODE,_("mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_("Choose the meter mode"));
  m_Gui->Divider();
  mafVME *start_vme = GetStartVME();
  if (start_vme && start_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("StartVME");
    m_StartVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)start_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_StartVmeName = start_vme ? start_vme->GetName() : _("none");
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));

  mafVME *end_vme1 = GetEnd1VME();
  if (end_vme1 && end_vme1->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("EndVME1");
    m_EndVme1Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme1)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End 1"), _("Select the end vme for point distance"));

  mafVME *end_vme2 = GetEnd2VME();
  if (end_vme2 && end_vme2->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("EndVME2");
    m_EndVme2Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme2)->GetLandmarkName(sub_id) : _("none");
  }
//  else
//    m_EndVme2Name = end_vme2 ? end_vme2->GetName() : _("none");
 //   m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,_("End 2"), _("Select the vme representing \nthe point for line distance"));

  
  mafVME *wrapped_vme = GetWrappedVME();
  m_WrappedVmeName = wrapped_vme ? wrapped_vme->GetName() : _("none");
  m_Gui->Button(ID_WRAPPED_METER_LINK,&m_WrappedVmeName,_("Wrapped"), _("Select the vme representing Vme to be wrapped"));
  m_Gui->Bool(ID_WRAPPED_SIDE,"reverse direction", &m_WrapSide ,1);
  //m_Gui->Bool(ID_WRAPPED_REVERSE,"reverse wrap", &m_WrapReverse,1);

 // if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
 //   m_Gui->Enable(ID_END2_METER_LINK,false);

  m_Gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode == AUTOMATED_WRAP || m_WrappedMode == IOR_AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == AUTOMATED_WRAP);

  m_Gui->Label(_("MidPoints"), true);
  m_ListBox=m_Gui->ListBox(ID_LISTBOX);

	//insert eventual middlepoints
	m_OrderMiddlePointsNameVMEList.clear();
	for(int j=0; j<m_OrderMiddlePointsVMEList.size();j++)
	{
		for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
		{	
			if(i->first.Equals("StartVME")) continue;
			else if(i->first.Equals("EndVME1")) continue;
			else if(i->first.Equals("EndVME2")) continue;
      else if(i->first.Equals("WrappedVME")) continue;
			else if(i->second.m_Node->GetId() == m_OrderMiddlePointsVMEList[j])
			{
				if(mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(i->second.m_Node))
				{
					int idx = m_OrderMiddlePointsVMEList[++j];
					mafVMELandmark *landmark  = lc->GetLandmark(idx);
					if(landmark)
					{
				    m_ListBox->Append(landmark->GetName());
						m_OrderMiddlePointsNameVMEList.push_back(landmark->GetName());
					}
				}
				else
				{
				  m_ListBox->Append(i->second.m_Node->GetName());
					m_OrderMiddlePointsNameVMEList.push_back(i->second.m_Node->GetName());
				}
			}
		}
	}
  
  m_Gui->Button(ID_ADD_POINT, _("Add"), "" ,"");
  m_Gui->Button(ID_REMOVE_POINT, _("Remove"), "" ,"");
  m_Gui->Button(ID_UP, _("Up"), "" ,"");
  m_Gui->Button(ID_DOWN, _("Down"), "" ,"");

  EnableManualModeWidget(m_WrappedMode == MANUAL_WRAP);

	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider(2);
  //m_Gui->Button(ID_SAVE_FILE_BUTTON, _("Save in file"),"" ,"");
	m_Gui->Divider();

  InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_START_METER_LINK:
      case ID_END1_METER_LINK:
    //  case ID_END2_METER_LINK:
      case ID_WRAPPED_METER_LINK:
      {
        mafID button_id = e->GetId();
        mafString title = _("Choose meter vme link");
        e->SetId(VME_CHOOSE);
        if (button_id == ID_WRAPPED_METER_LINK)
		  e->SetArg((long)&mafVMEWrappedMeter::VMESurfaceParametricAccept);
		else
          e->SetArg((long)&mafVMEWrappedMeter::VMEAccept);
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
    /*      else if (button_id == ID_END2_METER_LINK)
          {
            SetMeterLink("EndVME2", n);
            m_EndVme2Name = n->GetName();
          }*/
          else if (button_id == ID_WRAPPED_METER_LINK)
          {
            SetMeterLink("WrappedVME", n);
            m_WrappedVmeName = n->GetName();
          }
          m_Gui->Update();
          InternalUpdate();
        }
      }
      break;
	  case ID_METER_MODE:
	  {
		/*  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
		  {  
        m_Gui->Enable(ID_END2_METER_LINK,false);
		  }*/
		/*  else if(GetMeterAttributes()->m_MeterMode ==  LINE_DISTANCE)
		  { 
			  m_Gui->Enable(ID_END2_METER_LINK,true);
		  }
		  else if(GetMeterAttributes()->m_MeterMode ==  LINE_ANGLE)
		  {       
			  m_Gui->Enable(ID_END2_METER_LINK,true);
		  }*/
      
      Modified();
			InternalUpdate();
      mafID button_id = e->GetId();
      e->SetId(CAMERA_UPDATE);
      ForwardUpEvent(e);
     
	  }
    break;
		case ID_WRAPPED_METER_MODE:
			{
				EnableManualModeWidget(m_WrappedMode ==MANUAL_WRAP);

        m_Gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode == AUTOMATED_WRAP || m_WrappedMode == IOR_AUTOMATED_WRAP);
        m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == AUTOMATED_WRAP );
        m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == AUTOMATED_WRAP);
        

				Modified();
				InternalUpdate();
				mafID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);
			}
			break;
    case ID_WRAPPED_SIDE:
    case ID_WRAPPED_REVERSE:
      InternalUpdate();
      ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      break;
    case ID_ADD_POINT:
      {
        /*if(m_ListBox->GetCount()!=0)
        {
        wxMessageBox("Current max point number is one!");
        return;
        }*/
        mafString title = _("Choose a vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEWrappedMeter::VMEAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();

        if (n == NULL) return;

        wxString nameProfile ="";
        //mafString idNumber = wxString::Format(_("%d"),id);
        nameProfile = n->GetName();

        if(wxNOT_FOUND != m_ListBox->FindString(n->GetName()))
        {
          wxMessageBox(_("Can't introduce vme with the same name"));
          return;
        }

				if(mafString(n->GetName()) == mafString("StartVME") ||
					mafString(n->GetName()) == mafString("EndVME1")  ||
					mafString(n->GetName()) == mafString("EndVME2")  ||
          mafString(n->GetName()) == mafString("WrappedVME"))
				{
					wxMessageBox(_("Can't introduce vme with the name of StartVME or EndVME1 or EndVME2 or WrappedVME"));
					return;
				}

        /*for(int i=0; i<m_ListBox->GetCount();i++)
        {
        if(m_ProfilesNameList[i] == nameProfile)
        {
        wxMessageBox(L"Warning: you're introducing a profile\nwith the same name of a selected one.\n\
        Modify it and readd it"); 
        return;
        }
        }*/

        /*if(m_ProfilesNameList.size() >= 2)
        {
        wxMessageBox(L"Warning: Supported 2 profiles"); 
        return;
        }*/

        /*double temp = 0;
        for(int i=0; i<m_IdListBox.size(); i++)
        {
        if(m_IdListBox[i] > temp) temp = m_IdListBox[i];
        }

        int id = temp + 1;
        m_IdListBox.push_back(id);*/

        
        SetMeterLink(n->GetName(),n);

        //nameProfile += idNumber;
        //m_ProfilesNameList.push_back(nameProfile);
        //        m_PolylineList.push_back(n);

        //int a=m_ListBox->Append(wxString::Format("%s",n->GetName()));
        mafString t;
        t = n->GetName();
        //m_ListBox->Append(_(t));
        m_ListBox->Append(_(t));
        m_ListBox->SetStringSelection(_(t));

				
        //m_OrderedMidPoints.push_back(_(t));

        
        //m_IdList.push_back(new int(n->GetId()));
        
//        m_ListBox->SetClientData(m_ListBox->GetSelection(), m_IdList[m_IdList.size()-1] );

        

        //m_ListBox->AddItem(/*m_IdListBox[m_IdListBox.size()-1]*/0, nameProfile.GetCStr());
        m_ListBox->Update();
        m_Gui->Update();
        Modified();
        //InternalUpdate();

        InternalUpdate();
        ForwardUpEvent(&mafEvent(this,CAMERA_RESET));
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));

        m_Gui->Update();
      }
      break;
    case ID_REMOVE_POINT:
      { 
        if(m_ListBox->GetCount()!=0)
        {
          wxString name = m_ListBox->GetStringSelection();
          int number = m_ListBox->GetSelection();

          RemoveLink(name);
          m_ListBox->Delete(m_ListBox->FindString(m_ListBox->GetStringSelection()));          
          //m_OrderedMidPoints.erase(m_OrderedMidPoints.begin()+number);
					m_OrderMiddlePointsNameVMEList.clear();
					for(int i=0;i<m_ListBox->GetCount();i++)
					{
            m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

          Modified();
          InternalUpdate();
          ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;
    case ID_UP:
      {
        if(m_ListBox->GetCount()!=0)
        {
          wxString name = m_ListBox->GetStringSelection();
          int number = m_ListBox->GetSelection();
          if(number == 0) return;
          //RemoveLink(name);

          wxString temp;
          temp = m_ListBox->GetStringSelection(); 
          m_ListBox->Delete(m_ListBox->FindString(temp));
          m_ListBox->Insert(name, number-1);

          /*mafString temp2 = m_OrderedMidPoints[number-1];
          m_OrderedMidPoints[number] = temp2;
          m_OrderedMidPoints[number-1] = temp;*/

          m_ListBox->SetStringSelection(_(name));
//          m_ListBox->SetClientData(m_ListBox->GetSelection(), (void *) m_IdList[number]);

          /*wxString message;
          for(int i= 0 ; i< m_OrderedMidPoints.size(); i++)
            message.Append(m_OrderedMidPoints[i].GetCStr());

          wxMessageBox(message);*/
					m_OrderMiddlePointsNameVMEList.clear();
					for(int i=0;i<m_ListBox->GetCount();i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

          Modified();
          m_Gui->Update();
          InternalUpdate();
          ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;
    case ID_DOWN:
      {
        if(m_ListBox->GetCount()!=0)
        {
          wxString name = m_ListBox->GetStringSelection();
          int number = m_ListBox->GetSelection();
          if(number == m_ListBox->GetCount() - 1) return;
          //RemoveLink(name);

          wxString temp;
          temp = m_ListBox->GetStringSelection(); 
          m_ListBox->Delete(m_ListBox->FindString(temp));
          m_ListBox->Insert(name, number+1);

          /*mafString temp2 = m_OrderedMidPoints[number+1];
          m_OrderedMidPoints[number] = temp2;
          m_OrderedMidPoints[number+1] = temp;*/

          m_ListBox->SetStringSelection(_(name));

          /*wxString message;
          for(int i= 0 ; i< m_OrderedMidPoints.size(); i++)
            message.Append(m_OrderedMidPoints[i].GetCStr());

          wxMessageBox(message);*/
					m_OrderMiddlePointsNameVMEList.clear();
					for(int i=0;i<m_ListBox->GetCount();i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}
          
          Modified();
          InternalUpdate();
          ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;
		/*case ID_SAVE_FILE_BUTTON:
			{
        SaveInFile();
			}
			break;*/
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
void mafVMEWrappedMeter::SetMeterLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{
  if (n->IsMAFType(mafVMELandmark))
  {
    SetLink(link_name,n->GetParent(),((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
  }
  else
	{
    SetLink(link_name, n);
	}

	if( mafString(link_name) != mafString("StartVME") &&
		mafString(link_name) != mafString("EndVME1")  &&
		mafString(link_name) != mafString("EndVME2")  &&
    mafString(link_name) != mafString("WrappedVME"))
	{
		m_OrderMiddlePointsNameVMEList.push_back(n->GetName());
	}
}

//-------------------------------------------------------------------------
void mafVMEWrappedMeter::AddMidPoint(mafNode *node)
//-------------------------------------------------------------------------
{
  m_OrderMiddlePointsVMEList.push_back(node->GetId());
  if (node->IsA("mafVMELandmarkCloud"))
  {
    //Set first landmark as mid point
    m_OrderMiddlePointsVMEList.push_back(0);
  }
}

//-------------------------------------------------------------------------
void mafVMEWrappedMeter::RemoveLink(const char *link_name)
//-------------------------------------------------------------------------
{
	Superclass::RemoveLink(link_name);

	if( mafString(link_name) != mafString("StartVME") &&
		mafString(link_name) != mafString("EndVME1")  &&
		mafString(link_name) != mafString("EndVME2")  &&
    mafString(link_name) != mafString("WrappedVME"))
	{
		for(std::vector<mafString>::iterator it = m_OrderMiddlePointsNameVMEList.begin(); it != m_OrderMiddlePointsNameVMEList.end(); it++ )
		{
			if(mafString(*it) == mafString(link_name))
			{
		    m_OrderMiddlePointsNameVMEList.erase(it);
				break;
			}
		}
	}
}
//-------------------------------------------------------------------------
mafVME *mafVMEWrappedMeter::GetStartVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("StartVME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEWrappedMeter::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME1"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEWrappedMeter::GetEnd2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME2"));
}
//-------------------------------------------------------------------------
mafVME *mafVMEWrappedMeter::GetWrappedVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("WrappedVME"));
}
//-----------------------------------------------------------------------
mafNode::mafLinksMap *mafVMEWrappedMeter::GetMidPointsLinks()
//-----------------------------------------------------------------------
{
  //da ritornare la link maps
  return GetLinks();
}

//-------------------------------------------------------------------------
void mafVMEWrappedMeter::EnableManualModeWidget(bool flag)
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LISTBOX, flag);
  m_Gui->Enable(ID_ADD_POINT, flag);
  m_Gui->Enable(ID_REMOVE_POINT, flag);
  m_Gui->Enable(ID_UP, flag);
  m_Gui->Enable(ID_DOWN, flag);
}
//-------------------------------------------------------------------------
bool mafVMEWrappedMeter::MiddlePointsControl()
//-------------------------------------------------------------------------
{
  //mafLogMessage("LINKS");
  int numberOfMiddlePoints = 0;
  for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    mafString name = i->first;
    if(i->first.Equals("StartVME")) continue;
    else if(i->first.Equals("EndVME1")) continue;
		else if(i->first.Equals("EndVME2")) continue;
    else if(i->first.Equals("WrappedVME")) continue;
    else if(i->second.m_Node == NULL)
    {
      mafString message;
      message = mafString(i->first);
      message += _("doesn't exist");
      //mafLogMessage(message);
      return false;
    }
    else
    {
      numberOfMiddlePoints++;
      mafString message;
      message = mafString(i->first);
      //mafLogMessage(message);
    }
  }

  if(m_OrderMiddlePointsNameVMEList.size() == numberOfMiddlePoints)
  {
    for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
    {
      mafString name = i->first;
      if(i->first.Equals("StartVME")) continue;
      else if(i->first.Equals("EndVME1")) continue;
			else if(i->first.Equals("EndVME2")) continue;
      else if(i->first.Equals("WrappedVME")) continue;
      else
      {
				bool result = false;
				for(int i=0;i<m_OrderMiddlePointsNameVMEList.size();i++)
				{
          if(m_OrderMiddlePointsNameVMEList[i] == name) result = true;
				}
        return result;
      }
    }
  }
  else
    return false;

    

  return true;
}
//-------------------------------------------------------------------------
mafNode* mafVMEWrappedMeter::IndexToMiddlePointVME(int index)
//-------------------------------------------------------------------------
{
  mafString name = m_OrderMiddlePointsNameVMEList[index];

	mafNode *returnNode = NULL;
  for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    if(i->first.Equals(name)) returnNode =  i->second.m_Node;
  }

  return returnNode;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SyncronizeList()
//-------------------------------------------------------------------------
{
	m_OrderMiddlePointsNameVMEList.clear();
	for(int j=0; j<m_OrderMiddlePointsVMEList.size();j++)
	{
		for (mafLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
		{	
			if(i->first.Equals("StartVME")) continue;
			else if(i->first.Equals("EndVME1")) continue;
			else if(i->first.Equals("EndVME2")) continue;
      else if(i->first.Equals("WrappedVME")) continue;
			else if(i->second.m_Node->GetId() == m_OrderMiddlePointsVMEList[j])
			{
				if(mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(i->second.m_Node))
				{
					int idx = m_OrderMiddlePointsVMEList[++j];
					mafVMELandmark *landmark  = lc->GetLandmark(idx);
					if(landmark)
					{
						m_OrderMiddlePointsNameVMEList.push_back(landmark->GetName());
					}
				}
				else
				{
					m_OrderMiddlePointsNameVMEList.push_back(i->second.m_Node->GetName());
				}
			}
		}
	}
}
//-------------------------------------------------------------------------
double *mafVMEWrappedMeter::GetMiddlePointCoordinate(int index)
//-------------------------------------------------------------------------
{
	if(m_MiddlePointList.size() > 0)
	  return m_MiddlePointList[index];

	return NULL;
}
//-------------------------------------------------------------------------
double *mafVMEWrappedMeter::GetStartPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_StartPoint;
}
//-------------------------------------------------------------------------
double *mafVMEWrappedMeter::GetEndPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_EndPoint;
}
//-------------------------------------------------------------------------
void mafVMEWrappedMeter::SaveInFile()
//-------------------------------------------------------------------------
{
	/*mafString initialFileName;
	initialFileName = mafGetApplicationDirectory().c_str();
	initialFileName.Append("\\WrapperMeter.txt");

	mafString wildc = "configuration file (*.txt)|*.txt";
	mafString newFileName = mafGetSaveFile(initialFileName.GetCStr(), wildc).c_str();

  if (newFileName == "") return;

	std::ofstream outputFile(newFileName, std::ios::out);

	if (outputFile == NULL) {
		wxMessageBox("Error opening configuration file");
		return ;
	}

	for(int i=0; i<m_MiddlePointList.size();i++)
	{
    outputFile << m_MiddlePointList[i][0] << '\t'
			         << m_MiddlePointList[i][1] << '\t'
							 << m_MiddlePointList[i][2] << std::endl;
	}
	
  outputFile.close();*/

}
