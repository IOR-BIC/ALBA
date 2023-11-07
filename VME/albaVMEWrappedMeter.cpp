/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEWrappedMeter
 Authors: Daniele Giunchi
 
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

#include "albaVMEWrappedMeter.h"
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

#include "vtkALBADataPipe.h"
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

ALBA_ID_IMP(albaVMEWrappedMeter::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEWrappedMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEWrappedMeter::albaVMEWrappedMeter()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  m_Angle         = 0.0;
	m_WrappedMode   = MANUAL_WRAP;
  m_WrapSide      = 0;
  m_WrapReverse   = 0;
  
  m_StartVmeName  = "";
  m_EndVme1Name   = "";
  m_WrappedVmeName   = "";

  m_Gui = NULL;
  m_ListBox = NULL;
  
  albaNEW(m_Transform);
  albaVMEOutputWrappedMeter *output = albaVMEOutputWrappedMeter::New(); // an output with no data
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

  albaNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_Goniometer->GetOutput());

}
//-------------------------------------------------------------------------
albaVMEWrappedMeter::~albaVMEWrappedMeter()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);

  /*vtkDEL(m_PlaneSource);
  vtkDEL(m_PlaneCutter);
  vtkDEL(m_Cutter);
  vtkDEL(m_PlaneClip);
  vtkDEL(m_Clip);*/

  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
//  vtkDEL(m_LineSourceMiddle);
  vtkDEL(m_Goniometer);
  albaDEL(m_TmpTransform);

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
int albaVMEWrappedMeter::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a)==ALBA_OK)
	{
		albaVMEWrappedMeter *meter = albaVMEWrappedMeter::SafeDownCast(a);
		m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

		albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
		if (dpipe)
		{
			dpipe->SetInput(m_Goniometer->GetOutput());
		}
		return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEWrappedMeter::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((albaVMEWrappedMeter *)vme)->m_Transform->GetMatrix() && \
          GetLink("StartVME") == ((albaVMEWrappedMeter *)vme)->GetLink("StartVME") && \
          GetLink("EndVME1") == ((albaVMEWrappedMeter *)vme)->GetLink("EndVME1") && \
          GetLink("EndVME2") == ((albaVMEWrappedMeter *)vme)->GetLink("EndVME2");
  }
  return ret;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::InternalInitialize()
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
mmaMaterial *albaVMEWrappedMeter::GetMaterial()
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
albaVMEOutputWrappedMeter *albaVMEWrappedMeter::GetWrappedMeterOutput()
//-------------------------------------------------------------------------
{
  return (albaVMEOutputWrappedMeter *) GetOutput();
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool albaVMEWrappedMeter::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::InternalUpdate()
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
void albaVMEWrappedMeter::InternalUpdateAutomatedIOR()
//-----------------------------------------------------------------------
{
  albaVME *start_vme = GetStartVME();
  albaVME *end_vme   = GetEnd1VME();
  albaVME *wrapped_vme   = GetWrappedVME();

  bool start_ok = true, end_ok = true;

  double orientation[3];
  m_Goniometer->RemoveAllInputs();


  if (start_vme && end_vme && wrapped_vme)
  { 
    start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
		end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);

    wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter, orientation);
	  double translation[3];
    ((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);
    albaMatrix mat;
    mat.Identity();
    for(int i=0; i<3; i++)
      mat.SetElement(i,3,translation[i]);
    m_TmpTransform->SetMatrix(mat);
    m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);
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
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
    transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform());
    transformFirstDataInput->SetInput((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData());
    transformFirstDataInput->Update();

    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
    transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
    transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
    transformFirstData->Update(); 

    // here REAL ALGORITHM //////////////////////////////
    vtkALBASmartPointer<vtkOBBTree> locator;
    locator->SetDataSet(transformFirstData->GetOutput());
    locator->SetGlobalWarningDisplay(0);
    locator->BuildLocator();

    //Control if Start or End point is inside vtk data (surface)
    if(locator->InsideOrOutside(local_start) <= 0 || locator->InsideOrOutside(local_end) <= 0) 
    {
      AvoidWrapping(local_start,local_end);

      return;
    }

    vtkALBASmartPointer<vtkPoints> temporaryIntersection;
    vtkALBASmartPointer<vtkPoints> pointsIntersection1;
    vtkALBASmartPointer<vtkPoints> pointsIntersection2;

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

    vtkMatrix4x4 *mat = wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix();
    albaMatrix matrix;
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


    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    GetWrappedMeterOutput()->Update(); 
  }
  else
    m_Distance = -1;
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::WrappingCore(double *init, double *center, double *end,\
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

      vtkALBASmartPointer<vtkPlaneSource> planeSource;
      planeSource->SetOrigin(init);
      planeSource->SetPoint1(center);
      planeSource->SetPoint2(end);
      planeSource->Update();

      double normal[3];
      planeSource->GetNormal(normal);

      
      //albaLogMessage("DotMX %.2f DotMY %.2f DotMZ %.2f", dotXM,dotYM,dotZM);

      
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

      albaLogMessage("DotVersorY %.2f ", dot2);
      albaLogMessage("DotVersorZ %.2f ", dot3);


      if(IsStart == false)
      {
        double insertionVector[3];
        insertionVector[0] = -init_center[0];
        insertionVector[1] = -init_center[1];
        insertionVector[2] = -init_center[2];
        dot4 = vtkMath::Dot(insertionVector, versorZ);
      }

      albaLogMessage("Z: %.2f" , dot4);
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
void albaVMEWrappedMeter::AvoidWrapping(double *local_start, double *local_end)
//-----------------------------------------------------------------------
{
  //if there is no intersection with geometry
  m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
  m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
  m_Goniometer->AddInput(m_LineSource->GetOutput());
  
  m_Distance = sqrt(vtkMath::Distance2BetweenPoints(local_start, local_end));

  InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
  GetWrappedMeterOutput()->Update();
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::InternalUpdateAutomated()
//-----------------------------------------------------------------------
{
  albaVME *start_vme = GetStartVME();
  albaVME *end_vme   = GetEnd1VME();
  albaVME *wrapped_vme   = GetWrappedVME();

  bool start_ok = true, end_ok = true;

  double orientation[3];
  m_Goniometer->RemoveAllInputs();


  if (start_vme && end_vme && wrapped_vme)
  { 
    start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
		end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);

    wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter, orientation);
    double translation[3];
    ((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);
    albaMatrix mat;
    mat.Identity();
    for(int i=0; i<3; i++)
      mat.SetElement(i,3,translation[i]);
    m_TmpTransform->SetMatrix(mat);
    m_TmpTransform->TransformPoint(m_WrappedVMECenter,m_WrappedVMECenter);

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
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
    transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform());
    transformFirstDataInput->SetInput((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData());
    transformFirstDataInput->Update();

    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
    transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
    transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
    transformFirstData->Update(); 

    // here REAL ALGORITHM //////////////////////////////
    vtkALBASmartPointer<vtkOBBTree> locator;
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

     InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
     GetWrappedMeterOutput()->Update();

     return;
  }

    vtkALBASmartPointer<vtkPoints> temporaryIntersection;
    vtkALBASmartPointer<vtkPoints> pointsIntersection1;
    vtkALBASmartPointer<vtkPoints> pointsIntersection2;
   
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
 
       InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
       GetWrappedMeterOutput()->Update();
 
       return;
     }

     //WRAPPED METER REFACTOR // 2008 09 15

     //local_wrapped_center -> center of wrapped surface
     //need a vector that is the normal of the plane

     vtkMatrix4x4 *mat = wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix();
     albaMatrix matrix;
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

         InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
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


    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    GetWrappedMeterOutput()->Update(); 
  }
  else
    m_Distance = -1;
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::InternalUpdateManual()
//-----------------------------------------------------------------------
{
	//if(m_Gui == NULL) CreateGui();
	if(m_OrderMiddlePointsNameVMEList.size() == 0) SyncronizeList();

  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  if (GetMeterMode() == albaVMEWrappedMeter::POINT_DISTANCE)
  {
    albaVME *start_vme = GetStartVME();
    albaVME *end_vme   = GetEnd1VME();

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
        albaVME *middleVME = IndexToMiddlePointVME(j);
        m_MiddlePointList.push_back(new double[3]);
        if(middleVME->IsALBAType(albaVMELandmarkCloud))
        {
          ((albaVMELandmarkCloud *)middleVME)->GetLandmark(m_OrderMiddlePointsNameVMEList[j],m_MiddlePointList[m_MiddlePointList.size()-1],-1);
          m_TmpTransform->SetMatrix(*middleVME->GetOutput()->GetAbsMatrix());
          m_TmpTransform->TransformPoint(m_MiddlePointList[m_MiddlePointList.size()-1],m_MiddlePointList[m_MiddlePointList.size()-1]);
        }
        else
        {
          middleVME->GetOutput()->GetAbsPose(m_MiddlePointList[m_MiddlePointList.size()-1], orientation);
        }
        
      }

      start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
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

      if(GetMeterMeasureType() == albaVMEWrappedMeter::RELATIVE_MEASURE)
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

    InvokeEvent(this,VME_OUTPUT_DATA_UPDATE);
    

    if(GetMeterMeasureType() == albaVMEWrappedMeter::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);

    

    for(int i=0; i< m_MiddlePointList.size(); i++)
    {
      if(localMiddlePointList[i]) delete localMiddlePointList[i];
    }
    localMiddlePointList.clear();
  }
}
//-----------------------------------------------------------------------
int albaVMEWrappedMeter::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if(m_Gui == NULL) //this for update wrapped vme lists
  {
    InternalUpdateManual();
    CreateGui();
  }

  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
		m_OrderMiddlePointsVMEList.clear();

		for(int i=0; i<m_MiddlePointList.size(); i++)
		{
			albaVME *node;
			node = IndexToMiddlePointVME(i);
			if(node == NULL) continue;
			int vmeId = node->GetId();

			PushIdVector(vmeId);

			if(albaVMELandmarkCloud *lc = albaVMELandmarkCloud::SafeDownCast(node))
			{
				int index = -1;
				for(int j=0; j< lc->GetNumberOfLandmarks(); j++)
				{

          albaString name = "";
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

    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEWrappedMeter::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
			node->RestoreInteger("OrderMiddlePointVmeNumberOfElements", m_OrderMiddlePointsVMEListNumberOfElements);
			m_OrderMiddlePointsVMEList.resize(m_OrderMiddlePointsVMEListNumberOfElements);
			node->RestoreVectorN("OrderMiddlePointVme",m_OrderMiddlePointsVMEList,m_OrderMiddlePointsVMEListNumberOfElements);
			
      node->RestoreInteger("WrapMode", m_WrappedMode);
      node->RestoreInteger("WrapSide", m_WrapSide);
      node->RestoreInteger("WrapReverse", m_WrapReverse);
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMEWrappedMeter::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** albaVMEWrappedMeter::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "albaVMEWrappedMeter.xpm"
//  return albaVMEWrappedMeter_xpm;
//}
//-------------------------------------------------------------------------
mmaMeter *albaVMEWrappedMeter::GetMeterAttributes()
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
void albaVMEWrappedMeter::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetMeterMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *albaVMEWrappedMeter::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double albaVMEWrappedMeter::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double albaVMEWrappedMeter::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int albaVMEWrappedMeter::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double albaVMEWrappedMeter::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double albaVMEWrappedMeter::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
albaGUI* albaVMEWrappedMeter::CreateGui()
//-------------------------------------------------------------------------
{
  int num_mode = 3;
	int num_wrap = 3;

	const wxString wrap_choices_string[] = {_("Manual"), _("Automated"), _("IOR_automated")};

  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Combo(ID_WRAPPED_METER_MODE,_("Wrap"),&m_WrappedMode,num_wrap,wrap_choices_string,_("Choose the meter mode"));
	m_Gui->Divider();

	albaVME *start_vme = GetStartVME();
  m_StartVmeName = start_vme ? start_vme->GetName() : _("none");
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));

  albaVME *end_vme1 = GetEnd1VME();
  m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End 1"), _("Select the end vme for point distance"));

  
  albaVME *wrapped_vme = GetWrappedVME();
  m_WrappedVmeName = wrapped_vme ? wrapped_vme->GetName() : _("none");
  m_Gui->Button(ID_WRAPPED_METER_LINK,&m_WrappedVmeName,_("Wrapped"), _("Select the vme representing Vme to be wrapped"));
  m_Gui->Bool(ID_WRAPPED_SIDE,"Reverse direction", &m_WrapSide ,1);

  m_Gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode == AUTOMATED_WRAP || m_WrappedMode == IOR_AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == AUTOMATED_WRAP);

  m_Gui->Label(_("MidPoints"), true);
  m_ListBox=m_Gui->ListBox(ID_LISTBOX);

	//insert eventual middlepoints
	m_OrderMiddlePointsNameVMEList.clear();
	for(int j=0; j<m_OrderMiddlePointsVMEList.size();j++)
	{
		for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
		{	
			if(i->first.Equals("StartVME")) continue;
			else if(i->first.Equals("EndVME1")) continue;
			else if(i->first.Equals("EndVME2")) continue;
      else if(i->first.Equals("WrappedVME")) continue;
			else if(i->second.m_Node->GetId() == m_OrderMiddlePointsVMEList[j])
			{
				if(albaVMELandmarkCloud *lc = albaVMELandmarkCloud::SafeDownCast(i->second.m_Node))
				{
					int idx = m_OrderMiddlePointsVMEList[++j];
					albaVMELandmark *landmark  = lc->GetLandmark(idx);
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
  
	m_Gui->TwoButtons(ID_ADD_POINT, ID_REMOVE_POINT, "Add", "Remove");
	m_Gui->TwoButtons(ID_UP, ID_DOWN, "Up", "Down");

  EnableManualModeWidget(m_WrappedMode == MANUAL_WRAP);

	m_Gui->Divider();
	m_Gui->Divider();
  //m_Gui->Button(ID_SAVE_FILE_BUTTON, _("Save in file"),"" ,"");
	m_Gui->Divider();

  InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case ID_START_METER_LINK:
			case ID_END1_METER_LINK:
			case ID_WRAPPED_METER_LINK:
			{
				albaID button_id = e->GetId();
				albaString title = _("Choose meter vme link");
				e->SetId(VME_CHOOSE);
				if (button_id == ID_WRAPPED_METER_LINK)
					e->SetPointer(&albaVMEWrappedMeter::VMESurfaceParametricAccept);
				else
					e->SetPointer(&albaVMEWrappedMeter::VMEAccept);
				e->SetString(&title);
				ForwardUpEvent(e);
				albaVME *n = e->GetVme();
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
				Modified();
				InternalUpdate();
				albaID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);
			}
			break;
			case ID_WRAPPED_METER_MODE:
			{
				EnableManualModeWidget(m_WrappedMode == MANUAL_WRAP);

				m_Gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode == AUTOMATED_WRAP || m_WrappedMode == IOR_AUTOMATED_WRAP);
				m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == AUTOMATED_WRAP);
				m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == AUTOMATED_WRAP);


				Modified();
				InternalUpdate();
				albaID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);
			}
			break;
			case ID_WRAPPED_SIDE:
			case ID_WRAPPED_REVERSE:
				InternalUpdate();
				ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));
				break;
			case ID_ADD_POINT:
			{
				/*if(m_ListBox->GetCount()!=0)
				{
				wxMessageBox("Current max point number is one!");
				return;
				}*/
				albaString title = _("Choose a vme");
				e->SetId(VME_CHOOSE);
				e->SetPointer(&albaVMEWrappedMeter::VMEAccept);
				e->SetString(&title);
				ForwardUpEvent(e);
				albaVME *n = e->GetVme();

				if (n == NULL) return;

				wxString nameProfile = "";
				//albaString idNumber = albaString::Format(_("%d"),id);
				nameProfile = n->GetName();

				if (wxNOT_FOUND != m_ListBox->FindString(n->GetName()))
				{
					wxMessageBox(_("Can't introduce vme with the same name"));
					return;
				}

				if (albaString(n->GetName()) == albaString("StartVME") ||
					albaString(n->GetName()) == albaString("EndVME1") ||
					albaString(n->GetName()) == albaString("EndVME2") ||
					albaString(n->GetName()) == albaString("WrappedVME"))
				{
					wxMessageBox(_("Can't introduce vme with the name of StartVME or EndVME1 or EndVME2 or WrappedVME"));
					return;
				}


				SetMeterLink(n->GetName(), n);

				wxString t;
				t = n->GetName();
				m_ListBox->Append(_(t));
				m_ListBox->SetStringSelection(_(t));


				m_ListBox->Update();
				m_Gui->Update();
				Modified();

				InternalUpdate();
				GetLogicManager()->CameraReset();
				ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));

				m_Gui->Update();
			}
			break;
			case ID_REMOVE_POINT:
			{
				if (m_ListBox->GetCount() != 0)
				{
					wxString name = m_ListBox->GetStringSelection();
					int number = m_ListBox->GetSelection();

					RemoveLink(name);
					m_ListBox->Delete(m_ListBox->FindString(m_ListBox->GetStringSelection()));
					m_OrderMiddlePointsNameVMEList.clear();
					for (int i = 0; i < m_ListBox->GetCount(); i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

					Modified();
					InternalUpdate();
					ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));
				}
			}
			break;
			case ID_UP:
			{
				if (m_ListBox->GetCount() != 0)
				{
					wxString name = m_ListBox->GetStringSelection();
					int number = m_ListBox->GetSelection();
					if (number == 0) return;

					wxString temp;
					temp = m_ListBox->GetStringSelection();
					m_ListBox->Delete(m_ListBox->FindString(temp));
					m_ListBox->Insert(name, number - 1);


					m_ListBox->SetStringSelection(_(name));

					m_OrderMiddlePointsNameVMEList.clear();
					for (int i = 0; i < m_ListBox->GetCount(); i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

					Modified();
					m_Gui->Update();
					InternalUpdate();
					ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));
				}
			}
			break;
			case ID_DOWN:
			{
				if (m_ListBox->GetCount() != 0)
				{
					wxString name = m_ListBox->GetStringSelection();
					int number = m_ListBox->GetSelection();
					if (number == m_ListBox->GetCount() - 1) return;
					//RemoveLink(name);

					wxString temp;
					temp = m_ListBox->GetStringSelection();
					m_ListBox->Delete(m_ListBox->FindString(temp));
					m_ListBox->Insert(name, number + 1);

	
					m_ListBox->SetStringSelection(_(name));

					m_OrderMiddlePointsNameVMEList.clear();
					for (int i = 0; i < m_ListBox->GetCount(); i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

					Modified();
					InternalUpdate();
					ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));
				}
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
void albaVMEWrappedMeter::SetMeterLink(const char *link_name, albaVME *n)
//-------------------------------------------------------------------------
{
  SetLink(link_name, n);

	if( albaString(link_name) != albaString("StartVME") &&
		albaString(link_name) != albaString("EndVME1")  &&
		albaString(link_name) != albaString("EndVME2")  &&
    albaString(link_name) != albaString("WrappedVME"))
	{
		m_OrderMiddlePointsNameVMEList.push_back(n->GetName());
	}
}

//-------------------------------------------------------------------------
void albaVMEWrappedMeter::AddMidPoint(albaVME *node)
//-------------------------------------------------------------------------
{
  m_OrderMiddlePointsVMEList.push_back(node->GetId());
  if (node->IsA("albaVMELandmarkCloud"))
  {
    //Set first landmark as mid point
    m_OrderMiddlePointsVMEList.push_back(0);
  }
}

//-------------------------------------------------------------------------
void albaVMEWrappedMeter::RemoveLink(const char *link_name)
//-------------------------------------------------------------------------
{
	Superclass::RemoveLink(link_name);

	if( albaString(link_name) != albaString("StartVME") &&
		albaString(link_name) != albaString("EndVME1")  &&
		albaString(link_name) != albaString("EndVME2")  &&
    albaString(link_name) != albaString("WrappedVME"))
	{
		for(std::vector<albaString>::iterator it = m_OrderMiddlePointsNameVMEList.begin(); it != m_OrderMiddlePointsNameVMEList.end(); it++ )
		{
			if(albaString(*it) == albaString(link_name))
			{
		    m_OrderMiddlePointsNameVMEList.erase(it);
				break;
			}
		}
	}
}
//-------------------------------------------------------------------------
albaVME *albaVMEWrappedMeter::GetStartVME()
//-------------------------------------------------------------------------
{
  return GetLink("StartVME");
}
//-------------------------------------------------------------------------
albaVME *albaVMEWrappedMeter::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return GetLink("EndVME1");
}
//-------------------------------------------------------------------------
albaVME *albaVMEWrappedMeter::GetWrappedVME()
//-------------------------------------------------------------------------
{
  return GetLink("WrappedVME");
}
//-----------------------------------------------------------------------
albaVME::albaLinksMap *albaVMEWrappedMeter::GetMidPointsLinks()
//-----------------------------------------------------------------------
{
  //da ritornare la link maps
  return GetLinks();
}

//-------------------------------------------------------------------------
void albaVMEWrappedMeter::EnableManualModeWidget(bool flag)
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LISTBOX, flag);
  m_Gui->Enable(ID_ADD_POINT, flag);
  m_Gui->Enable(ID_REMOVE_POINT, flag);
  m_Gui->Enable(ID_UP, flag);
  m_Gui->Enable(ID_DOWN, flag);
}
//-------------------------------------------------------------------------
bool albaVMEWrappedMeter::MiddlePointsControl()
//-------------------------------------------------------------------------
{
  //albaLogMessage("LINKS");
  int numberOfMiddlePoints = 0;
  for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    albaString name = i->first;
    if(i->first.Equals("StartVME")) continue;
    else if(i->first.Equals("EndVME1")) continue;
		else if(i->first.Equals("EndVME2")) continue;
    else if(i->first.Equals("WrappedVME")) continue;
    else if(i->second.m_Node == NULL)
    {
      albaString message;
      message = albaString(i->first);
      message += _("doesn't exist");
      //albaLogMessage(message);
      return false;
    }
    else
    {
      numberOfMiddlePoints++;
      albaString message;
      message = albaString(i->first);
      //albaLogMessage(message);
    }
  }

  if(m_OrderMiddlePointsNameVMEList.size() == numberOfMiddlePoints)
  {
    for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
    {
      albaString name = i->first;
      if(i->first.Equals("StartVME")) continue;
      else if(i->first.Equals("EndVME1")) continue;
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
albaVME* albaVMEWrappedMeter::IndexToMiddlePointVME(int index)
//-------------------------------------------------------------------------
{
  albaString name = m_OrderMiddlePointsNameVMEList[index];

	albaVME *returnNode = NULL;
  for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); i++)
  {
    if(i->first.Equals(name)) returnNode =  i->second.m_Node;
  }

  return returnNode;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SyncronizeList()
//-------------------------------------------------------------------------
{
	m_OrderMiddlePointsNameVMEList.clear();
	for(int j=0; j<m_OrderMiddlePointsVMEList.size();j++)
	{
		for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
		{	
			if(i->first.Equals("StartVME")) continue;
			else if(i->first.Equals("EndVME1")) continue;
			else if(i->first.Equals("EndVME2")) continue;
      else if(i->first.Equals("WrappedVME")) continue;
			else if(i->second.m_Node->GetId() == m_OrderMiddlePointsVMEList[j])
			{
				if(albaVMELandmarkCloud *lc = albaVMELandmarkCloud::SafeDownCast(i->second.m_Node))
				{
					int idx = m_OrderMiddlePointsVMEList[++j];
					albaVMELandmark *landmark  = lc->GetLandmark(idx);
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
double *albaVMEWrappedMeter::GetMiddlePointCoordinate(int index)
//-------------------------------------------------------------------------
{
	if(m_MiddlePointList.size() > 0)
	  return m_MiddlePointList[index];

	return NULL;
}
//-------------------------------------------------------------------------
double *albaVMEWrappedMeter::GetStartPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_StartPoint;
}
//-------------------------------------------------------------------------
double *albaVMEWrappedMeter::GetEndPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_EndPoint;
}
//-------------------------------------------------------------------------
void albaVMEWrappedMeter::SaveInFile()
//-------------------------------------------------------------------------
{
	/*albaString initialFileName;
	initialFileName = albaGetDocumentsDirectory().char_str();
	initialFileName.Append("\\WrapperMeter.txt");

	albaString wildc = "configuration file (*.txt)|*.txt";
	albaString newFileName = albaGetSaveFile(initialFileName.GetCStr(), wildc).char_str();

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
