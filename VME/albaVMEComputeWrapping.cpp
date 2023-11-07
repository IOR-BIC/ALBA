/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: medvmecomputewrapping
 Authors: Anupam Agrawal and Hui Wei
 
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

#include "albaVMEComputeWrapping.h"
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
#include "vtkPatchLine.h"
#include "albaVMESurfaceParametric.h"
#include "albaMatrix3x3.h"
#include "vtkCellArray.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "albaGUIRollOut.h"


#include <time.h>
//#include <vld.h>

ALBA_ID_IMP(albaVMEComputeWrapping::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEComputeWrapping)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEComputeWrapping::albaVMEComputeWrapping()
//-------------------------------------------------------------------------
{
	m_Distance      = -1.0;
	m_Angle         = 0.0;
	m_WrappedMode1  = SPHERE_CYLINDER;
	m_WrappedMode2  = IOR_AUTOMATED_WRAP;
	m_WrapSide      = 0;
	m_WrapReverse   = 0;
	m_WrapReverseNew = 0;
	m_WrappedClass = NEW_METER;

	m_StartVmeName  = "";
	m_EndVme1Name   = "";
	m_WrappedVmeName1   = "";
	m_WrappedVmeName2 = "";

	m_WrappedVmeName = "";
	m_ViaPointName = "";
	m_AbCurve = 0;

	m_Gui = NULL;
	m_ListBox = NULL;
	m_Idx = 0;
	m_PathNum = 36;

	//m_Tolerance = GetCylinderRadius()/4.0;

	albaNEW(m_Transform);
	albaNEW(m_TmpTransform2);
	albaVMEOutputComputeWrapping *output = albaVMEOutputComputeWrapping::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	m_WrappedMode1 = -1;
	m_WrappedMode2 = -1;


	m_Mat = new albaMatrix3x3();
	m_Imat = new albaMatrix3x3();

	vtkNEW(m_LineSource);
	vtkNEW(m_LineSource2);
	vtkNEW(m_Goniometer);
	vtkNEW(m_LinePatcher);

	m_Goniometer->AddInput(m_LineSource->GetOutput());
	m_Goniometer->AddInput(m_LineSource2->GetOutput());

	albaNEW(m_TmpTransform);

	DependsOnLinkedNodeOn();

	// attach a data pipe which creates a bridge between VTK and ALBA
	albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
	dpipe->SetDependOnAbsPose(true);
	SetDataPipe(dpipe);

	//-------------------------------------------------
	m_LinePatcher->SetInput(m_Goniometer->GetOutput());
	dpipe->SetInput(m_LinePatcher->GetOutput());	 

}
//-------------------------------------------------------------------------
albaVMEComputeWrapping::~albaVMEComputeWrapping()
//-------------------------------------------------------------------------
{
	albaDEL(m_Transform);
	vtkDEL(m_LineSource);
	vtkDEL(m_LineSource2);
	vtkDEL(m_Goniometer);
	vtkDEL(m_LinePatcher);
	albaDEL(m_TmpTransform);
	albaDEL(m_TmpTransform2);

	albaDEL(m_Mat);
	albaDEL(m_Imat);

	for (int i = 0; i < (int)m_ExportPointList.size(); i++) {
		delete[] m_ExportPointList[i];
	}
	m_ExportPointList.clear();

	for(int i=0; i< (int)m_MiddlePointList.size(); i++)
	{
		delete[] m_MiddlePointList[i];
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
int albaVMEComputeWrapping::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a)==ALBA_OK)
	{
		albaVMEComputeWrapping *meter = albaVMEComputeWrapping::SafeDownCast(a);
		m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

		//BES: 11.5.2012 - deep copy settings	
		this->m_Angle = meter->m_Angle;
		this->m_WrappedMode1 = meter->m_WrappedMode1;
		this->m_WrappedMode2 = meter->m_WrappedMode2;
		this->m_WrapSide = meter->m_WrapSide;
		this->m_WrapReverse = meter->m_WrapReverse;
		this->m_WrapReverseNew = meter->m_WrapReverseNew;
		this->m_WrappedClass = meter->m_WrappedClass;

		this->m_StartVmeName = meter->m_StartVmeName;
		this->m_EndVme1Name = meter->m_EndVme1Name;
		this->m_WrappedVmeName1 = meter->m_WrappedVmeName1;
		this->m_WrappedVmeName2 = meter->m_WrappedVmeName2;

		this->m_WrappedVmeName = meter->m_WrappedVmeName;
		this->m_ViaPointName = meter->m_ViaPointName;
		this->m_AbCurve = meter->m_AbCurve;

		this->m_Idx = meter->m_Idx;
		this->m_PathNum = meter->m_PathNum;

		memcpy(this->m_Alist, meter->m_Alist, sizeof(this->m_Alist));		
		memcpy(this->m_APoint, meter->m_APoint, sizeof(this->m_APoint));
		
		//TODO: Whoever implemented this class,  please DeepCopy other members !!!
		//I have no idea which are important and which are not, too many of them

		albaDataPipeCustom *dpipe =
			albaDataPipeCustom::SafeDownCast(GetDataPipe());

		if (dpipe)
		{
			dpipe->SetInput(m_LinePatcher->GetOutput());
		}
		return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEComputeWrapping::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		ret = m_Transform->GetMatrix() == ((albaVMEComputeWrapping *)vme)->m_Transform->GetMatrix() && \
			GetLink("StartVME") == ((albaVMEComputeWrapping *)vme)->GetLink("StartVME") && \
			GetLink("EndVME1") == ((albaVMEComputeWrapping *)vme)->GetLink("EndVME1") && \
			GetLink("EndVME2") == ((albaVMEComputeWrapping *)vme)->GetLink("EndVME2");
	}
	return ret;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::InternalInitialize()
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
mmaMaterial *albaVMEComputeWrapping::GetMaterial()
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
albaVMEOutputComputeWrapping *albaVMEComputeWrapping::GetWrappedMeterOutput()
//-------------------------------------------------------------------------
{
	return (albaVMEOutputComputeWrapping *) GetOutput();
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool albaVMEComputeWrapping::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void albaVMEComputeWrapping::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	GetMeterAttributes();
}
//-----------------------------------------------------------------------
void albaVMEComputeWrapping::InternalUpdate()
//-----------------------------------------------------------------------
{

	bool invokeFlag = false;

	if (m_WrappedClass == NEW_METER)//new method 
	{
		albaVME *wrapped_vme1 = GetWrappedVME1();
		albaVME *wrapped_vme2 = GetWrappedVME2();

		if( m_WrappedMode1== SPHERE_CYLINDER && wrapped_vme1 && wrapped_vme2){
			invokeFlag = true;
		}else if(m_WrappedMode1 == SINGLE_SPHERE && wrapped_vme1){
			invokeFlag =true;
		}else if(m_WrappedMode1 == SINGLE_CYLINDER && wrapped_vme2){
			invokeFlag = true;
		}else if (m_WrappedMode1 == DOUBLE_CYLINDER && wrapped_vme1 && wrapped_vme2)
		{
			invokeFlag = true;
		}
		if(invokeFlag){
			Dispatch(); 

		}
	}else if (m_WrappedClass == OLD_METER)//old method
	{
		if(m_WrappedMode2 == MANUAL_WRAP)
		{
			InternalUpdateManual();
		}
		else if(m_WrappedMode2 == AUTOMATED_WRAP)
		{
			InternalUpdateAutomated();
		}
		else if(m_WrappedMode2 == IOR_AUTOMATED_WRAP)
		{
			InternalUpdateAutomatedIOR();
		}

	}

}
//main method
//dispatch different cases to different methods
void albaVMEComputeWrapping::Dispatch(){
	int obbtreeFlag = 0;
	int obbtreeFlag1 = 0;
	int obbtreeFlag2 = 0;

	double cosA,sinA,cosB,sinB;//used for get transform matrix
	vtkALBASmartPointer<vtkOBBTree> locator1;
	vtkALBASmartPointer<vtkOBBTree> locator2;

	albaString logFname = "dispatch.txt";
	std::ofstream outputFile(logFname, std::ios::out|std::ios::app);
	//outputFile.clear();

	albaVME *wrapped_vme1 = GetWrappedVME1();
	albaVME *wrapped_vme2 = GetWrappedVME2();
	vtkALBASmartPointer<vtkPoints> temporaryIntersection;

	m_Goniometer->RemoveAllInputs();

	bool prepareflag = PrepareData2();

	for (int i = 0; i < (int)m_ExportPointList.size(); i++) {
		delete[] m_ExportPointList[i];
	}
	m_ExportPointList.clear();
	//---------------------test code----------------------------
	/*
	vtkLineSource *SE;
	vtkNEW(SE);
	m_Goniometer->RemoveAllInputs();

	albaTransform* transform;
	albaNEW(transform);
	transform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	transform->Invert();

	transform->TransformPoint(m_StartPoint, m_StartPoint);
	transform->TransformPoint(m_EndPoint, m_EndPoint);
	albaDEL(transform);
	SE->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	SE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
	m_Goniometer->AddInput(SE->GetOutput());

	*/
	//------------------------------------------------
	if (prepareflag)
	{

		vtkALBASmartPointer<vtkPoints> pointsIntersection1;
		vtkALBASmartPointer<vtkPoints> pointsIntersection2;
		bool aligned = false;
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
		//-------------prepare i point


		GetWrapMatrix(m_InMx2,m_InMy2,2);//second cylinder vme
		albaMatrix::Invert(m_InMy2,m_OutMy2);
		albaMatrix::Invert(m_InMx2,m_OutMx2);


		int vFlag ;
		bool alignFlag = CheckAlign();
		bool radiusFlag = CheckTwoRadius();

		//if (alignFlag && radiusFlag)//center of two object align
		if (radiusFlag)
		{
			vFlag = GetViaPoint(m_ViaPoint,true);
			if (vFlag ==NON_WRAP || vFlag == INSIDE_FLAG)
			{
				DirectConnectSE();
				if (vFlag ==  INSIDE_FLAG)
				{
					albaMessage(_("start point or end point is inside surface vme"));
				}
				outputFile<<"---------after directConnectSE:length="<<m_Distance<<std::endl;
				outputFile.close();
			}else 
			{
				if (vFlag == WRAP_SPHERE_CYLINDER || vFlag == SINGLE_SPHERE || vFlag == WRAP_SPHERE_CYLINDER_2 || vFlag == SINGLE_SPHERE2)
				{
					//------------prepare matrix--------------
					cosA=0;sinA=0;cosB=0;sinB=0;
					//step1
					GetAngleAandB(cosA,sinA,cosB,sinB);
					//step2
					GetTransFormMatrix(cosA,sinA,cosB,sinB,m_Mat);//after this function mat has element value
					albaMatrix3x3::Invert(m_Mat->GetElements(),m_Imat->GetElements());//imat->Invert();
				}


				if (vFlag == WRAP_SPHERE_CYLINDER)
				{
					GetSphereCylinderWrap(m_PathNum,m_EndPoint);

					outputFile<<"---------after GetSphereCylinderWrap:length="<<m_Distance<<std::endl;
					outputFile.close();


				}else if (vFlag == WRAP_SPHERE_CYLINDER_2)
				{
					GetSphereCylinderWrapAdvance(m_PathNum);
				}else if (vFlag == CYLINDER_CYLINDER )//double cylinder
				{
					GetWrapMatrix(m_InMx1,m_InMy1,1);// need to prepare first cylinder vme
					albaMatrix::Invert(m_InMy1,m_OutMy1);
					albaMatrix::Invert(m_InMx1,m_OutMx1);

					//getCylinderCylinderWrap(m_PathNum);
					WrapCylinderCylinderObstacleSet();
				}else if (vFlag == CYLINDER_CYLINDER_1)
				{
					WrapCylinderOnlyObstacleSet(1);
				}else if (vFlag == CYLINDER_CYLINDER_2)
				{
					WrapCylinderOnlyObstacleSet(2);
				}
				else if (vFlag == SINGLE_CYLINDER){
					//WrapCylinderOnly(0);
					WrapCylinderOnly(m_PathNum);
					//WrapCylinderOnlyObstacleSet(2);//second cylinder
					outputFile<<"---------after WrapCylinderOnly:length="<<m_Distance<<std::endl;
					outputFile.close();
				}else if (vFlag == SINGLE_SPHERE)
				{
					WrapSphereOnly(m_PathNum,false);
					outputFile<<"---------after wrapSphereOnly:length="<<m_Distance<<std::endl;
					outputFile.close();
				}else if (vFlag == SINGLE_SPHERE2)
				{
					WrapSphereOnly(m_PathNum,true);
				}
			}
		}/*else if(radiusFlag){//two object do not align
		 if (vFlag ==WRAP_SPHERE_CYLINDER || vFlag == WRAP_SPHERE_CYLINDER_2)
			{
			wrapSphereCylinderSeparately(m_PathNum,true);


			}
			}*/else{
				if (!alignFlag)
				{
					albaMessage(_("axis of cylinder should pass through center of sphere"));
				}
				if (!radiusFlag)
				{
					albaMessage(_("radius of sphere should be bigger than  radius of cylinder"));
				}
				// directConnectSE();
		}
	}

}
//if two objects do not align
/*void medVMEComputeWrapping::wrapSphereCylinderSeparately(const int step,bool allowIntersectFlag){
double dist1,dist2;
double sphereCenter[3],cylinderCenter[3];
//wrapSphereOnly(m_PathNum,false);
//put point B as start point 
//WrapCylinderOnly(m_PathNum,false);
getGlobalSphereCenter(sphereCenter);
GetGlobalCylinderCenter(cylinderCenter);

dist1 = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,sphereCenter));
dist2 = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,cylinderCenter));
if (dist1<dist2)//if start point is near sphere center,wrap sphere first
{
wrapSphereOnly(m_PathNum,true);

}else{//if start point is near cylinder center, wrap cylinder first
WrapCylinderOnly(m_PathNum);
}
//exchange start point to new point
}
*/
//transform clip data which has many points
void albaVMEComputeWrapping::TransformOutputClipData(vtkPolyData *clipData){
	albaTransform* transform;
	albaNEW(transform);
	transform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	transform->Invert();
	transform->Update();

	vtkPoints *pRet = clipData->GetPoints();

	double x[3];
	int N = pRet->GetNumberOfPoints();
	for (int j = 0; j < N; j++)
	{    
		transform->TransformPoint(pRet->GetPoint(j), x);
		pRet->SetPoint(j, x);
	}

	albaDEL(transform);

}
//transform one point
void albaVMEComputeWrapping::TransformOutputPoint( double *point){
	albaTransform* transform;
	albaNEW(transform);
	transform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	transform->Invert();
	transform->TransformPoint(point, point);
	albaDEL(transform);
}
//transform polydata
void albaVMEComputeWrapping::TransformOutput(vtkPolyData *pData ){

	albaTransform* transform;
	albaNEW(transform);
	transform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	transform->Invert();

	vtkPoints* pRet = pData->GetPoints();
	double x[3];
	int N = pRet->GetNumberOfPoints();
	for (int i = 0; i < N; i++)
	{    
		transform->TransformPoint(pRet->GetPoint(i), x);
		pRet->SetPoint(i, x);
	}

	albaDEL(transform);

}
//draw a straight line
void albaVMEComputeWrapping::DirectConnectSE(){
	double d0;
	vtkLineSource *SE;
	vtkNEW(SE);
	m_Goniometer->RemoveAllInputs();

	TransformOutputPoint(m_StartPoint);
	TransformOutputPoint(m_EndPoint);

	SE->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	SE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
	m_Goniometer->AddInput(SE->GetOutput());
	d0 = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,m_EndPoint));
	m_Distance = d0;

	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 
}
//note : c coord is using local value
//in this function ,convert c from local to global
//c is output,cCoord is input
//this method is to get a minimum value
//type 1 angleValue ,2 in one plane,3 distance,4 distanceSum 
double albaVMEComputeWrapping::GoldenSectionSearch(double low,double high,double accuracy,double *A,double *b,double *c,double *cCoord,int type){
	double y1,y2;
	double r = 0.618;
	double x1,x2;
	bool flag = true;
	double rtn;

	x1=0;x2=0;
	double tmpY1,tmpY2;
	tmpY1=0;tmpY2=0;
	int idx = 0;

	x1 = low + (1-r)*(high-low);
	x2 = low + r*(high-low);
	while ( flag )
	{		
		m_Idx++;
		//		x1 = low + (1-r)*(high-low);
		//		x2 = low + r*(high-low);

		c[2]=x1;

		if (type==1 && cCoord==NULL)//first time invoke
		{
			y1 = GetAngleValue(A,b,c);
		}else if (type==2)
		{
			y1 = GetPlaneValue(A,b,cCoord,c);
		}else if (type==3)
		{
			y1 = GetDistanceValue(A,c,b);
		}else if (type==4 && cCoord==NULL)
		{
			y1 = GetDistanceSumValue(A,b,c);
		}

		c[2]=x2;

		if (type==1 && cCoord==NULL)
		{
			y2 = GetAngleValue(A,b,c);
		}else if(type==2){

			y2 = GetPlaneValue(A,b,cCoord,c);
		}else if (type==3)
		{
			y2 = GetDistanceValue(A,c,b);
		}
		else if (type==4 && cCoord==NULL)
		{
			y2 = GetDistanceSumValue(A,b,c);
		}

		if (fabs(y1)> fabs(y2))
		{
			low=x1;
			x1=x2;
			x2 = low+r*(high-low);

		}else{
			high = x2;
			x2 = x1;
			x1 = low + (1-r)*(high-low);			
		}
		if (abs(y1-y2)<accuracy)
		{
			flag = false;
		}
		idx++;
		if (m_Idx>10000)
		{
			y1 = -1;
			break;
		}
	}//end of while
	//x = x1

	rtn = y1;
	return rtn;
}
//distance sum of three point
double albaVMEComputeWrapping::GetDistanceSumValue(double *A,double *b ,double *c){

	double dac,dcb;
	double rtn;

	dac = sqrt(vtkMath::Distance2BetweenPoints(A,c));
	dcb = sqrt(vtkMath::Distance2BetweenPoints(c,b));

	rtn = dac + dcb;
	return rtn;
}
//get angle between two vector
double albaVMEComputeWrapping::GetAngleValue(double *A ,double *b,double *c){
	double oa[3],ob[3];
	double doa,dob;
	double rtn;

	oa[0]= b[0] - A[0];
	oa[1]= b[1] - A[1];
	oa[2]= b[2] - A[2];

	ob[0]= c[0] - A[0];
	ob[1]= c[1] - A[1];
	ob[2]= c[2] - A[2];

	doa = sqrt(vtkMath::Distance2BetweenPoints(A,b));
	dob = sqrt(vtkMath::Distance2BetweenPoints(A,c));

	rtn = vtkMath::Dot(oa,ob);
	rtn = rtn / (doa * dob);
	rtn = acos(rtn);

	return rtn;
}
//to test if three point in one plane
double albaVMEComputeWrapping::GetPlaneValue(double *a,double *b,double *c,double *d){
	double ab[3],ac[3],cd[3],normalTmp[3];
	ab[0] = a[0]-b[0];
	ab[1] = a[1]-b[1];
	ab[2] = a[2]-b[2];

	ac[0] = a[0]-c[0];
	ac[1] = a[1]-c[1];
	ac[2] = a[2]-c[2];

	cd[0] =c[0]-d[0];
	cd[1] =c[1]-d[1];
	cd[2] =c[2]-d[2];

	vtkMath::Cross(ab,ac,normalTmp);
	vtkMath::Normalize(normalTmp);
	vtkMath::Normalize(cd);
	double rtn = vtkMath::Dot(normalTmp,cd);
	return rtn;

}
//all parameters use local value
double albaVMEComputeWrapping::GetDistanceValue(double *start,double *cCoord,double *end){

	double curveLength,rtn;

	curveLength = CaculateHelix2(NULL,cCoord,end,false);

	rtn = sqrt(vtkMath::Distance2BetweenPoints(start,cCoord));
	rtn += curveLength;

	return rtn;
}
//S-T~H-G~Q-P
//The obstacle-set Method for Representing Muscle Paths in Musculoskeletal Models
//Author: BrIAN A.Garner and MARCUS G.Pandy
void albaVMEComputeWrapping::WrapCylinderCylinderObstacleSet(){
	double segLength1,segLength2;
	int idx1,idx2;
	double d1,d2;
	double S[3],P[3];
	double T[3],H[3],Tg[3],Hg[3];
	double G[3],Q[3],Gg[3],Qg[3];
	double center1[3],center2[3];
	bool exchangeFlag = false;
	int step = 10;
	vtkPolyData *hCurve1,*hCurve2;
	double curveLength1=0,curveLength2=0;
	vtkLineSource *Line1,*Line2,*Line3;
	boolean reverseFlag = false;
	if (m_WrapReverseNew)
	{
		reverseFlag = true;
	}
	//vtkLineSource *Line4;

	m_Goniometer->RemoveAllInputs();

	CopyPointValue(m_StartPoint,S);
	CopyPointValue(m_EndPoint,P);

	idx1 = 1;//by default first cylinder should near start point
	idx2 = 2;
	GetGlobalCylinderCenter(center1,1);
	GetGlobalCylinderCenter(center2,2);
	d1 = GetPointLineDistance(S,center1,m_CylinderAxis1);//distance from start point to cylinder axis1
	d2 = GetPointLineDistance(S,center2,m_CylinderAxis2);//distance from start point to cylinder axis2
	if (d1>d2)//start point is near cylinder1
	{
		idx1=2;
		idx2=1;
		exchangeFlag = true;
	}


	//---test code---------
	/*	albaString logFname = "HCoord.txt";	//for debug
	std::ofstream outputFile(logFname, std::ios::out);	//for debug
	outputFile.clear();	//for debug
	*/

	//use S ,P get H first
	WrapCylinderOnlyObstacleSetBasic2(S,P,idx1,reverseFlag,segLength1,T,H);
	//------transform it to global------------
	GetWrapGlobalTransform(T,Tg,idx1);
	GetWrapGlobalTransform(H,Hg,idx1);
	//outputFile<<" H coord: 0"<<"   "<< H[0]<<"  "<<H[1]<<"  "<<H[2]<<std::endl;	//for debug

	//loop this paragraph
	for (int i=0;i<step;i++)
	{
		//use H ,P get G,Q
		WrapCylinderOnlyObstacleSetBasic2(Hg,P,idx2,reverseFlag,segLength1,G,Q);
		GetWrapGlobalTransform(G,Gg,idx2);

		//use S, G get T,H
		WrapCylinderOnlyObstacleSetBasic2(S,Gg,idx1,reverseFlag,segLength1,T,H);
		GetWrapGlobalTransform(H,Hg,idx1);
		if (i==step-1)
		{
			GetWrapGlobalTransform(T,Tg,idx1);
			GetWrapGlobalTransform(Q,Qg,idx2);
		}
		//outputFile<<" H coord: "<<i<<"   "<< H[0]<<"  "<<H[1]<<"  "<<H[2]<<std::endl;	//for debug
	} 
	//outputFile.close();	//for debug

	//------------out put result----------------
	bool wrapObj1 = WrapCondition(T,H,reverseFlag);
	bool wrapObj2 = WrapCondition(G,Q,reverseFlag);

	vtkNEW(Line1);
	vtkNEW(Line2);
	vtkNEW(Line3);
	vtkNEW(hCurve1);
	vtkNEW(hCurve2);

	//vtkNEW(Line4);

	if (wrapObj1 && wrapObj2)
	{
		curveLength1 = CaculateHelix2(hCurve1,T,H,true,idx1);
		curveLength2 = CaculateHelix2(hCurve2,G,Q,true,idx2);
		m_Distance = sqrt(vtkMath::Distance2BetweenPoints(S,Tg));
		m_Distance += curveLength1;
		m_Distance += sqrt(vtkMath::Distance2BetweenPoints(Hg,Gg));
		m_Distance += curveLength2;
		m_Distance += sqrt(vtkMath::Distance2BetweenPoints(Qg,P));

		Line1->SetPoint1(m_StartPoint);
		Line1->SetPoint2(Tg);

		Line2->SetPoint1(Hg);
		Line2->SetPoint2(Gg);

		Line3->SetPoint1(Qg);
		Line3->SetPoint2(P);

		//Line4->SetPoint1(Tg);
		//Line4->SetPoint2(Hg);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Tg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Hg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Gg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Qg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_Goniometer->AddInput(Line1->GetOutput());
		m_Goniometer->AddInput(Line2->GetOutput());
		m_Goniometer->AddInput(Line3->GetOutput());
		m_Goniometer->AddInput(hCurve1);
		m_Goniometer->AddInput(hCurve2);

		//m_Goniometer->AddInput(Line4->GetOutput());
	}
	else if (wrapObj1 && !wrapObj2)
	{
		curveLength1 = CaculateHelix2(hCurve1,T,H,true,idx1);
		m_Distance = sqrt(vtkMath::Distance2BetweenPoints(S,Tg));
		m_Distance += curveLength1;
		m_Distance += sqrt(vtkMath::Distance2BetweenPoints(Hg,P));

		Line1->SetPoint1(S);
		Line1->SetPoint2(Tg);

		Line2->SetPoint1(Hg);
		Line2->SetPoint2(P);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Tg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Hg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_Goniometer->AddInput(Line1->GetOutput());
		m_Goniometer->AddInput(hCurve1);
		m_Goniometer->AddInput(Line2->GetOutput());

	}else if ( !wrapObj1 && wrapObj2)
	{	
		curveLength2 = CaculateHelix2(hCurve2,G,Q,true,idx2);
		m_Distance = sqrt(vtkMath::Distance2BetweenPoints(S,Gg));
		m_Distance += curveLength2;
		m_Distance += sqrt(vtkMath::Distance2BetweenPoints(Qg,P));

		Line1->SetPoint1(S);
		Line1->SetPoint2(Gg);

		Line2->SetPoint1(Qg);
		Line2->SetPoint2(P);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Gg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(Qg ,m_ExportPointList[m_ExportPointList.size()-1]);

		m_Goniometer->AddInput(Line1->GetOutput());
		m_Goniometer->AddInput(hCurve2);
		m_Goniometer->AddInput(Line2->GetOutput());
	}else{ //!wrapObj1 && !wrapObj2

		m_Distance = sqrt(vtkMath::Distance2BetweenPoints(S,P));
		Line1->SetPoint1(S);
		Line1->SetPoint2(P);
		m_Goniometer->AddInput(Line1->GetOutput());

	}
	vtkDEL(Line1);
	vtkDEL(Line2);
	vtkDEL(Line3);
	vtkDEL(hCurve1);
	vtkDEL(hCurve2);
	//m_Goniometer->Update();
	//m_Goniometer->GetOutput()->Update();

	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 
}
void albaVMEComputeWrapping::WrapCylinderOnlyObstacleSetBasic2(double *Sg,double *Pg,int idx,bool reverseFlag,double &segLength,double *Tout,double *Qout){
	double P[3],S[3],Q[3],T[3];
	//express P and S in cylinder frame
	GetWrapLocalTransform(Pg,P,idx);
	GetWrapLocalTransform(Sg,S,idx);
	
	double R = GetCylinderRadius(idx);
	double r = R;
	if (reverseFlag)
	{
		r = -R;
	}

	//1.compute xy Coordinate of Q
	ComputeTangentXYQ(P,r,Q);
	// compute xy coordinate of T
	ComputeTangentXYT(S,r,T);
	//2.compute xy coordinate of segment lengths in xy plane
	segLength = GetQTsegment(R,Q,T);
	//3.compute z coordinates of Q
	ComputeQz(P,S,Q,T,segLength);
	//compute zcoordinates of T
	ComputeTz(P,S,Q,T,segLength);
	//---------we already get Q and T------------
	CopyPointValue(T,Tout);
	CopyPointValue(Q,Qout);

}



// S-T~Q-P 
/*void medVMEComputeWrapping::WrapCylinderOnlyObstacleSetBasic(double *Sg,double *Pg,int idx,double &segLength,double *Tout,double *Qout){
	double P[3],S[3],Q[3],T[3];
	//express P and S in cylinder frame
	GetWrapLocalTransform(Pg,P,idx);
	GetWrapLocalTransform(Sg,S,idx);
	double R = GetCylinderRadius(idx);


	//1.compute xy Coordinate of Q
	ComputeTangentXYQ(P,R,Q);
	// compute xy coordinate of T
	ComputeTangentXYT(S,R,T);
	//2.compute xy coordinate of segment lengths in xy plane
	segLength = GetQTsegment(R,Q,T);
	//3.compute z coordinates of Q
	ComputeQz(P,S,Q,T,segLength);
	//compute zcoordinates of T
	ComputeTz(P,S,Q,T,segLength);
	//---------we already get Q and T------------
	CopyPointValue(T,Tout);
	CopyPointValue(Q,Qout);

}*/

void albaVMEComputeWrapping::WrapCylinderOnlyObstacleSet(int idx){
	//P-Q~T-S
	double P[3],S[3],Q[3],T[3];
	double Qg[3],Tg[3];
	double segLength,curveLength;
	vtkPolyData *hCurve;
	vtkLineSource *Line1,*Line2;
	vtkNEW(Line1);
	vtkNEW(Line2);
	CopyPointValue(m_StartPoint,S);
	CopyPointValue(m_EndPoint,P);
	boolean reverseFlag = false;
	if (m_WrapReverseNew)
	{
		reverseFlag = true;
	}
//WrapCylinderOnlyObstacleSetBasic(m_StartPoint,m_EndPoint,idx,segLength,T,Q);
	WrapCylinderOnlyObstacleSetBasic2(m_StartPoint,m_EndPoint,idx,reverseFlag,segLength,T,Q);
	//------transform it to global------------
	GetWrapGlobalTransform(Q,Qg,idx);
	GetWrapGlobalTransform(T,Tg,idx);
	/*
	//express P and S in cylinder frame
	GetWrapLocalTransform(m_EndPoint,P,idx);
	GetWrapLocalTransform(m_StartPoint,S,idx);
	double R = GetCylinderRadius(idx);

	//1.compute xy Coordinate of Q
	ComputeTangentXYQ(P,R,Q);
	// compute xy coordinate of T
	ComputeTangentXYT(S,R,T);
	//2.compute xy coordinate of segment lengths in xy plane
	segLength = GetQTsegment(R,Q,T);
	//3.compute z coordinates of Q
	ComputeQz(P,S,Q,T,segLength);
	//compute zcoordinates of T
	ComputeTz(P,S,Q,T,segLength);
	//---------we already get Q and T------------
	//------transform it to global------------
	GetWrapGlobalTransform(Q,Qg,idx);
	GetWrapGlobalTransform(T,Tg,idx);
	*/
	//---------------------------------------
	vtkNEW(hCurve);
	curveLength = CaculateHelix2(hCurve,Q,T,true,idx);

	m_Distance = sqrt(vtkMath::Distance2BetweenPoints(S,Tg));
	m_Distance += curveLength;
	m_Distance += sqrt(vtkMath::Distance2BetweenPoints(Qg,P));

	Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
	Line1->SetPoint2(Qg[0],Qg[1],Qg[2]);

	//TransformOutputPoint( m_StartPoint);
	//TransformOutputPoint(cCoordGlobal4);
	Line2->SetPoint1(Tg[0],Tg[1],Tg[2]);
	Line2->SetPoint2(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	//--------------------------------------
	m_Goniometer->AddInput(Line1->GetOutput());
	m_Goniometer->AddInput(Line2->GetOutput());
	m_Goniometer->AddInput(hCurve);

	vtkDEL(Line1);
	vtkDEL(Line2);
	vtkDEL(hCurve);

	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 
}
double albaVMEComputeWrapping::GetQTsegment(double R,double *Q,double *T){
	double rtn = 0;
	double value = 0;
	value = (1.0 - ( (Q[0]-T[0])*(Q[0]-T[0]) + (Q[1]-T[1])*(Q[1]-T[1]) )/(2*R*R));
	rtn = R *  acos(value);
	//rtn = fabs(rtn);
	return rtn;
}
/************************************************************************/
/*       |X1 Y1 1|   if Det >0 orientation is counter clockwise 
/*	Det =|X2 Y2 1|   if det =0 points are colinear
/*       |X3 Y3 1|   if Det<0 orientation is clockwise
/* for our case (0,0)is the origin of the coordinate system (S--T--Q--P)
/* so det = Qx*Ty-Qy*Tx
/************************************************************************/

bool albaVMEComputeWrapping::WrapCondition(double *Q,double *T,bool reverseFlag){
	bool rtn = true;
	double det = Q[0]*T[1]- Q[1]*T[0];
	int flag = 1;
	if (reverseFlag)
	{
		flag = -1;
	}
	if ( flag * det<0)
	{
		rtn = false;
	}
	return rtn;

}
/************************************************************************/
/* P fix point
/* R cylinder radius
/* rtn return point with x,y value
/************************************************************************/
void albaVMEComputeWrapping::ComputeTangentXYQ(double *P,double R,double *rtn){

	rtn[0] = ( P[0] * R*R + R * P[1] * sqrt( P[0]*P[0] + P[1]*P[1] - R*R ))/( P[0]*P[0] + P[1]*P[1]);
	rtn[1] = ( P[1] * R*R - R * P[0] * sqrt( P[0]*P[0] + P[1]*P[1] - R*R ))/( P[0]*P[0] + P[1]*P[1]);

}
void albaVMEComputeWrapping::ComputeTangentXYT(double *S,double R, double *rtn){
	rtn[0] = ( S[0] * R*R - R * S[1] * sqrt( S[0]*S[0] + S[1]*S[1] - R*R ))/( S[0]*S[0] + S[1]*S[1]);
	rtn[1] = ( S[1] * R*R + R * S[0] * sqrt( S[0]*S[0] + S[1]*S[1] - R*R ))/( S[0]*S[0] + S[1]*S[1]);

}
double albaVMEComputeWrapping::ComputeQTLength(double *Q,double *T,double R){
	double rtn = 0;
	rtn = R * acos( 1.0 - ( (Q[0]-T[0])*(Q[0]-T[0]) + (Q[1]-T[1])*(Q[1]-T[1]) )/(2*R*R) );
	rtn = fabs(rtn);
	return rtn;
}
/************************************************************************/
/* point Q and T only has x,y value                                                                     */
/************************************************************************/
void albaVMEComputeWrapping::ComputeQz(double *P,double *S,double *Q,double *T,double QTlength){

	double PQxy = 0;
	double TSxy = 0;
	PQxy = sqrt( (Q[0]-P[0])*(Q[0]-P[0]) + (Q[1] -P[1])*(Q[1] -P[1]) );
	TSxy = sqrt( (S[0]-T[0])*(S[0]-T[0]) + (S[1] -T[1])*(S[1] -T[1]) );

	Q[2] = P[2]+ ( (S[2] - P[2])*PQxy / (PQxy + QTlength+ TSxy));

}
void albaVMEComputeWrapping::ComputeTz(double *P,double *S,double *Q,double *T,double QTlength){

	double PQxy = 0;
	double TSxy = 0;
	PQxy = sqrt( (Q[0]-P[0])*(Q[0]-P[0]) + (Q[1] -P[1])*(Q[1] -P[1]) );
	TSxy = sqrt( (S[0]-T[0])*(S[0]-T[0]) + (S[1] -T[1])*(S[1] -T[1]) );

	T[2] = S[2]- ( (S[2] - P[2])*TSxy / (PQxy + QTlength+ TSxy));
}



//suppose local end z value is smaller than local start value
//if it doesnt ,exchange these two points.
void albaVMEComputeWrapping::WrapCylinderOnly(int step){
	double viaLocal[3],startLocal[3],endLocal[3],cCoord1[3],cCoord2[3],cCoord3[3],cCoord4[3],tmpCoord[3];
	double Zi,Zo,Zl,Zh;

	double zValue1,zValue2;
	double cCoordGlobal1[3],cCoordGlobal2[3],cCoordGlobal3[3],cCoordGlobal4[3];
	double CIcurve,CIcurve1,CIcurve2 ;
	double d1,d2;

	vtkPolyData *hCurve;
	vtkLineSource *Line1,*Line2;

	zValue1 = 0.00;
	zValue2 = 0.00;

	albaString logFname = "angleLengthCylinder.txt";
	std::ofstream outputFile(logFname, std::ios::out);
	outputFile.clear();
	m_Goniometer->RemoveAllInputs();

	GetWrapLocalTransform(m_EndPoint,endLocal);
	GetWrapLocalTransform(m_StartPoint,startLocal);

	//m_Tolerance = GetCylinderRadius()/4.0;

	if (IsEndPonintOnCylinder(m_Tolerance,endLocal) && IsEndPonintOnCylinder(m_Tolerance,startLocal))//both on surface
	{
		//draw helix directly
		vtkNEW(hCurve);
		CIcurve = CaculateHelix2(hCurve,endLocal,startLocal,true);
		m_Distance = CIcurve;
		TransformOutput(hCurve);
		m_Goniometer->AddInput(hCurve);
		vtkDEL(hCurve);

	}else if (IsEndPonintOnCylinder(m_Tolerance,startLocal) || IsEndPonintOnCylinder(m_Tolerance,endLocal) )//start on surface or end on surface
	{

		//get c from end
		Zl = endLocal[2];
		Zh = startLocal[2];
		if (Zl>Zh)
		{
			Zl = startLocal[2];
			Zh = endLocal[2];
		}

		vtkNEW(Line1);
		vtkNEW(Line2);
		vtkNEW(hCurve);
		//vtkNEW(hCurve2);

		if (IsEndPonintOnCylinder(m_Tolerance,endLocal) )//end on surface
		{
			GetCcoordinateForCylinder(startLocal,cCoord1,cCoord2,2);

			GoldenSectionSearch(Zl,Zh,0.001,startLocal,endLocal,cCoord1,NULL,3);
			GoldenSectionSearch(Zl,Zh,0.001,startLocal,endLocal,cCoord2,NULL,3);

			CIcurve1 = CaculateHelix2(NULL,cCoord1,endLocal,false);
			CIcurve2 = CaculateHelix2(NULL,cCoord2,endLocal,false);

			GetWrapGlobalTransform(cCoord1,cCoordGlobal1);
			GetWrapGlobalTransform(cCoord2,cCoordGlobal2);

			d1 = CIcurve1 + sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,cCoordGlobal1));
			d2 = CIcurve2 + sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,cCoordGlobal2));

			if (d1<d2)
			{
				TransformOutputPoint( m_StartPoint);
				TransformOutputPoint(cCoordGlobal1);

				m_ExportPointList.push_back(new double[3]);
				CopyPointValue(cCoordGlobal1,m_ExportPointList[m_ExportPointList.size()-1]);

				Line1->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
				Line1->SetPoint2(cCoordGlobal1[0],cCoordGlobal1[1],cCoordGlobal1[2]);
				CIcurve1 = CaculateHelix2(hCurve,cCoord1,endLocal,true);
				m_Distance = d1;
			}else{
				TransformOutputPoint( m_StartPoint);
				TransformOutputPoint(cCoordGlobal2);

				m_ExportPointList.push_back(new double[3]);
				CopyPointValue(cCoordGlobal2,m_ExportPointList[m_ExportPointList.size()-1]);

				Line1->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
				Line1->SetPoint2(cCoordGlobal2[0],cCoordGlobal2[1],cCoordGlobal2[2]);
				CIcurve1 = CaculateHelix2(hCurve,cCoord2,endLocal,true);
				m_Distance = d2;
			}

		}
		else if (IsEndPonintOnCylinder(m_Tolerance,startLocal) )//start on surface
		{
			GetCcoordinateForCylinder(endLocal,cCoord3,cCoord4,2);

			GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord3,NULL,3);
			GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord4,NULL,3);

			CIcurve1 = CaculateHelix2(NULL,cCoord3,startLocal,false);
			CIcurve2 = CaculateHelix2(NULL,cCoord4,startLocal,false);

			GetWrapGlobalTransform(cCoord3,cCoordGlobal3);
			GetWrapGlobalTransform(cCoord4,cCoordGlobal4);

			d1 = CIcurve1 + sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,cCoordGlobal3));
			d2 = CIcurve2 + sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,cCoordGlobal4));

			if (d1<d2)
			{
				TransformOutputPoint( m_EndPoint);
				TransformOutputPoint(cCoordGlobal3);
				m_ExportPointList.push_back(new double[3]);
				CopyPointValue(cCoordGlobal3,m_ExportPointList[m_ExportPointList.size()-1]);
				Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
				Line1->SetPoint2(cCoordGlobal3[0],cCoordGlobal3[1],cCoordGlobal3[2]);
				CIcurve1 = CaculateHelix2(hCurve,cCoord3,startLocal,true);
				m_Distance = d1;
			}else{
				TransformOutputPoint( m_EndPoint);
				TransformOutputPoint(cCoordGlobal4);
				m_ExportPointList.push_back(new double[3]);
				CopyPointValue(cCoordGlobal4,m_ExportPointList[m_ExportPointList.size()-1]);

				Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
				Line1->SetPoint2(cCoordGlobal4[0],cCoordGlobal4[1],cCoordGlobal4[2]);
				CIcurve1 = CaculateHelix2(hCurve,cCoord4,startLocal,true);
				m_Distance = d2;
			}


		}


		TransformOutput(hCurve);
		m_Goniometer->AddInput(Line1->GetOutput());
		//m_Goniometer->AddInput(L2);
		m_Goniometer->AddInput(hCurve);

		vtkDEL(Line1);
		//vtkDEL(Line2);
		vtkDEL(hCurve);
		//vtkDEL(hCurve2);

		outputFile << " d1="<<d1<< "   d2=" <<d2<<std::endl;

	}else{//both start and end are outside cylinder
		//----------suppose start point is high than end point,or else exchange it -----------------

		if (endLocal[2]>startLocal[2])//if end point high than start point needs exchanging,
		{
			CopyPointValue(m_StartPoint,tmpCoord);
			CopyPointValue(m_EndPoint,m_StartPoint);
			CopyPointValue(tmpCoord,m_EndPoint);
			GetWrapLocalTransform(m_EndPoint,endLocal);
			GetWrapLocalTransform(m_StartPoint,startLocal);
			GetViaPoint(m_ViaPoint,true);
		}

		GetWrapLocalTransform(m_ViaPoint,viaLocal);//by default via point computed near end point

		//get C coord x,y value
		//----------------from end point-----------------------
		GetCcoordinateForCylinder(endLocal,cCoord1,cCoord2,2);

		Zh = startLocal[2];
		Zl = endLocal[2];
		GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord1,NULL,4);//use distanceSum 4 or angleValue 1
		GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord2,NULL,4);

		GetWrapGlobalTransform(cCoord1,cCoordGlobal1);
		GetWrapGlobalTransform(cCoord2,cCoordGlobal2);


		d1 =  sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,cCoordGlobal1));
		d2 =  sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,cCoordGlobal2));

		//outputFile<<"-------------from start point----------------"<<std::endl;
		//-----------get first points through start
		GetViaPoint(m_ViaPoint,false);//get second via point near start point
		GetWrapLocalTransform(m_ViaPoint,viaLocal);
		GetCcoordinateForCylinder(startLocal,cCoord3,cCoord4,2);
		//--------- which two points would pair ,only judge x,y value-------------
		double testD1 = sqrt( (cCoord1[0]-cCoord3[0])*(cCoord1[0]-cCoord3[0])+(cCoord1[1]-cCoord3[1])*(cCoord1[1]-cCoord3[1]) );
		double testD2 = sqrt((cCoord2[0]-cCoord3[0])*(cCoord2[0]-cCoord3[0])+(cCoord2[1]-cCoord3[1])*(cCoord2[1]-cCoord3[1]) );

		if (testD1>testD2)//it means c1--c4,needs changing 
		{
			CopyPointValue(cCoord1,tmpCoord);//change global value
			CopyPointValue(cCoord2,cCoord1);
			CopyPointValue(tmpCoord,cCoord2);
		}
		//then c1--c3,c2--c4

		//Zi = viaLocal[2];
		Zi = endLocal[2];
		Zo = startLocal[2];
		//note c coord is local value;
		GoldenSectionSearch(Zi,Zo,0.001,startLocal,endLocal,cCoord3,cCoord1,2);
		GoldenSectionSearch(Zi,Zo,0.001,startLocal,endLocal,cCoord4,cCoord2,2);


		GetWrapGlobalTransform(cCoord1,cCoordGlobal1);
		GetWrapGlobalTransform(cCoord3,cCoordGlobal3);
		GetWrapGlobalTransform(cCoord2,cCoordGlobal2);
		GetWrapGlobalTransform(cCoord4,cCoordGlobal4);

		vtkNEW(hCurve);
		CIcurve1 = CaculateHelix2(NULL,cCoord1,cCoord3,false);
		CIcurve2 = CaculateHelix2(NULL,cCoord2,cCoord4,false);

		d1 += CIcurve1;
		d2 += CIcurve2;

		d1 += sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,cCoordGlobal3));
		d2 += sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,cCoordGlobal4));

		vtkNEW(Line1);
		vtkNEW(Line2);

		if (d1<d2)
		{

			TransformOutputPoint( m_EndPoint);
			TransformOutputPoint(cCoordGlobal1);

			Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
			Line1->SetPoint2(cCoordGlobal1[0],cCoordGlobal1[1],cCoordGlobal1[2]);

			TransformOutputPoint( m_StartPoint);
			TransformOutputPoint(cCoordGlobal3);

			Line2->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
			Line2->SetPoint2(cCoordGlobal3[0],cCoordGlobal3[1],cCoordGlobal3[2]);

			m_ExportPointList.push_back(new double[3]);
			CopyPointValue(cCoordGlobal1,m_ExportPointList[m_ExportPointList.size()-1]);

			m_ExportPointList.push_back(new double[3]);
			CopyPointValue(cCoordGlobal3,m_ExportPointList[m_ExportPointList.size()-1]);


			CIcurve1 = CaculateHelix2(hCurve,cCoord1,cCoord3,true);
			m_Distance = d1;
		}else{

			TransformOutputPoint( m_EndPoint);
			TransformOutputPoint(cCoordGlobal2);
			Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
			Line1->SetPoint2(cCoordGlobal2[0],cCoordGlobal2[1],cCoordGlobal2[2]);

			TransformOutputPoint( m_StartPoint);
			TransformOutputPoint(cCoordGlobal4);
			Line2->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
			Line2->SetPoint2(cCoordGlobal4[0],cCoordGlobal4[1],cCoordGlobal4[2]);

			m_ExportPointList.push_back(new double[3]);
			CopyPointValue(cCoordGlobal2,m_ExportPointList[m_ExportPointList.size()-1]);
			m_ExportPointList.push_back(new double[3]);
			CopyPointValue(cCoordGlobal4,m_ExportPointList[m_ExportPointList.size()-1]);

			CIcurve1 = CaculateHelix2(hCurve,cCoord2,cCoord4,true);
			m_Distance = d2;
		}


		TransformOutput(hCurve);

		m_Goniometer->AddInput(Line1->GetOutput());
		m_Goniometer->AddInput(Line2->GetOutput());
		m_Goniometer->AddInput(hCurve);

		vtkDEL(Line1);
		vtkDEL(Line2);
		vtkDEL(hCurve);

	}

	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 

}

//compute X,Y value of a coordinate,do not compute Z value
void albaVMEComputeWrapping::GetCcoordinateForCylinder(double *oCoord,double *outC1,double *outC2,int objIdx){
	double r = GetCylinderRadius(objIdx);
	double D,Xo,Yo,Zo;
	Xo = oCoord[0];
	Yo = oCoord[1];
	Zo = oCoord[2];
	D = (-2*r*r*Yo)*(-2*r*r*Yo) - 4*(Yo*Yo + Xo*Xo)*(r*r*r*r - Xo*Xo*r*r);
	outC1[1] = ( 2*r*r*Yo + sqrt(D) ) / ( 2*(Yo*Yo+Xo*Xo) );//y1 value
	outC2[1] = ( 2*r*r*Yo - sqrt(D) ) / ( 2*(Yo*Yo+Xo*Xo) );//y2 value
	outC1[0] = ( r*r - outC1[1]*Yo ) / Xo; //x1 value
	outC2[0] = ( r*r - outC2[1]*Yo ) / Xo;  //x2 value


}
//use old method to get tangent
void albaVMEComputeWrapping::WrapSingleCylinder(double vId){

	vtkLineSource *SC,*CE,*SCL,*CEL;
	vtkNEW(SC);
	vtkNEW(CE);
	vtkNEW(SCL);
	vtkNEW(CEL);
	m_Goniometer->RemoveAllInputs();

	albaVME *wrapped_vme1 = GetWrappedVME1();
	double local_start[3],local_end[3],local_wrapped_center[3];

	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();

	m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
	m_TmpTransform->TransformPoint(m_EndPoint,local_end);
	m_TmpTransform->TransformPoint(m_WrappedVMECenter1,local_wrapped_center);
	//--------------test code----------------------
/*	SC->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	SC->SetPoint2(m_WrappedVMECenter1[0],m_WrappedVMECenter1[1],m_WrappedVMECenter1[2]);

	CE->SetPoint1(m_WrappedVMECenter1[0],m_WrappedVMECenter1[1],m_WrappedVMECenter1[2]);
	CE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);

	SCL->SetPoint1(local_start[0],local_start[1],local_start[2]);
	SCL->SetPoint2(local_wrapped_center[0],local_wrapped_center[1],local_wrapped_center[2]);

	CEL->SetPoint1(local_wrapped_center[0],local_wrapped_center[1],local_wrapped_center[2]);
	CEL->SetPoint2(local_end[0],local_end[1],local_end[2]);

	m_Goniometer->AddInput(SCL->GetOutput());
	m_Goniometer->AddInput(CEL->GetOutput());
*/
	//---------------over--------------------------

	// create ordered list of tangent point (2) real algorithm
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
	transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapped_vme1->GetAbsMatrixPipe()->GetVTKTransform());
	transformFirstDataInput->SetInput((vtkPolyData *)wrapped_vme1->GetOutput()->GetVTKData());
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

	SingleWrapAutomatedIOR(wrapped_vme1,local_start,local_end,local_wrapped_center,locator);
	vtkDEL(SC);
	vtkDEL(CE);
	vtkDEL(SCL);
	vtkDEL(CEL);

}
//test if these three point in one plane
double albaVMEComputeWrapping::TestPlane(double *a,double *b,double *c,double *d){
	double ab[3],ac[3],cd[3],normalTmp[3];
	ab[0] = a[0]-b[0];
	ab[1] = a[1]-b[1];
	ab[2] = a[2]-b[2];

	ac[0] = a[0]-c[0];
	ac[1] = a[1]-c[1];
	ac[2] = a[2]-c[2];

	cd[0] =c[0]-d[0];
	cd[1] =c[1]-d[1];
	cd[2] =c[2]-d[2];

	vtkMath::Cross(ab,ac,normalTmp);
	vtkMath::Normalize(normalTmp);
	vtkMath::Normalize(cd);
	double rtn = vtkMath::Dot(normalTmp,cd);
	return rtn;

}
//fixed value
double albaVMEComputeWrapping::GetPi(){

	return 3.1415926;
}
//sphere radius
double albaVMEComputeWrapping::GetSphereRadius(){
	double rtn = 0;
	albaVMESurfaceParametric *sphere = albaVMESurfaceParametric::SafeDownCast(GetWrappedVME1());
	rtn = sphere->GetSphereRadius();
	return rtn;

}
//cylinder radius
double albaVMEComputeWrapping::GetCylinderRadius(){
	double rtn = 0;
	rtn = GetCylinderRadius(2);
	return rtn;

}
double albaVMEComputeWrapping::GetCylinderRadius(int objIdx){
	double rtn = 0;
	albaVMESurfaceParametric *cylinder ;
	if (objIdx==1)
	{
		cylinder = albaVMESurfaceParametric::SafeDownCast(GetWrappedVME1());
	}else{
		cylinder = albaVMESurfaceParametric::SafeDownCast(GetWrappedVME2());
	}
	if (cylinder)
	{
		rtn = cylinder->GetCylinderRadius();
	}
	//rtn = 1;
	return rtn;
}

//compute C point
bool albaVMEComputeWrapping::GetCcoordinate2(double *bCoord,double *cCoord1,double *cCoord2){
	double a,b,c,r;//r is radius of cylinder
	double Yc,Xc,Zc,Yc2,Xc2,Zc2;
	double Xb,Yb,Zb;
	double Xr,Yr,Zr,R;
	double A,B,C,D;
	bool rtn = true;
	double testValue1,testValue2;

	r = GetCylinderRadius();
	R = GetSphereRadius();

	//getGlobalSphereCenter(RCoord);//coordinate of sphere center;

	Xb = bCoord[0];Yb = bCoord[1];Zb = bCoord[2];
	Xr = m_SphereWrapLocal[0];Yr = m_SphereWrapLocal[1];Zr = m_SphereWrapLocal[2];


	a = (Yb*Yb) + (Xb*Xb);//Yb*Yb + Xb*Xb;
	b = -2*r*r*Yb;//-(2r*rYb)
	c = r*r*( r*r - Xb*Xb );//r*r*(r*r-Xb*Xb)

	Yc = ( -b + sqrt(b*b - 4*a*c )) / ( 2*a );
	Yc2 = ( -b - sqrt(b*b - 4*a*c )) / ( 2*a );

	Xc = ( r*r - Yc*Yb ) / Xb;
	Xc2 = ( r*r - Yc2*Yb ) / Xb;

	testValue1 = Xc*Xc+Yc*Yc; //==r*r
	testValue2 = Xc2*Xc2+Yc2*Yc2; //==r*r
	testValue1 = Xc*Xb +Yc*Yb ;//==r*r
	testValue2 = Xc2*Xb +Yc2*Yb;//==r*r

	A = 2*(Xr-Xb);
	B = 2*(Yr-Yb);
	C = 2*(Zr-Zb);
	D = Xb*Xb + Yb*Yb +Zb*Zb + R*R -( Xr*Xr + Yr*Yr +Zr*Zr);

	Zc = - (A *Xc + B*Yc + D) / C;
	Zc2 = - (A *Xc2 + B*Yc2 + D) / C;

	cCoord1[0]=Xc; cCoord1[1]=Yc; cCoord1[2]=Zc;
	cCoord2[0]=Xc2; cCoord2[1]=Yc2; cCoord2[2]=Zc2;


	return rtn;

}
//get point A and point B ,then Use A and start ,end point can get a plane ,so get a curve on sphere surface
double albaVMEComputeWrapping::GetFunctionValue2(double x,double filterFlag,double *filterPlaneNormal,double *endPoint,double *outAPoint,double *outBPoint,double *outMPoint,double & Rm){
	double rtn = 0,rtn1 = 0,rtn2 = 0,rtn3 = 0,rtn4 = 0;

	//double oCoordTransform[3];
	double sphereR;
	double alCoord1[3],alCoord2[3],agCoord[3],aCoordR[3],aCoordR1[3],aCoordR2[3];
	double mCoordR[3],mCoordR1[3],mCoordR2[3],mgCoord[3],mgCoord1[3],mgCoord2[3];
	double igCoord[3],iCoordR[3],ogCoord[3],olCoord[3],oCoordR[3];
	double bCoordR1[3],bCoordR2[3],bCoordR3[3],bCoordR4[3],bgCoord1[3],bgCoord2[3],bgCoord3[3],bgCoord4[3];

	double Rm1,Rm2;
	sphereR = GetSphereRadius();
	//-----get o global coord---------
	ogCoord[0]=m_StartWrapLocal[0];
	ogCoord[1]=m_StartWrapLocal[1];
	ogCoord[2]=m_StartWrapLocal[2];

	igCoord[0]=endPoint[0];
	igCoord[1]=endPoint[1];
	igCoord[2]=endPoint[2];
	//--------------------------------

	Rm =0;
	Rm1 = 0;
	Rm2 = 0;


	//step3 o global --> o local so we can use equation(10)
	GetLocalTransformedCoord(olCoord,ogCoord,m_Imat);
	//step4 get A point local coordinate;
	bool aFlag = GetAcoordinateInLocal(alCoord1,alCoord2,olCoord,sphereR,x);
	if (aFlag)
	{

		//step5 a local --> a global
		double agCoord1[3],agCoord2[3],aGcoord1[3],aGcoord2[3],sphereCenter[3];

		GetGlobalTransformedCoord(alCoord1,agCoord1,m_Mat);
		GetGlobalTransformedCoord(alCoord2,agCoord2,m_Mat);
		GetGlobalSphereCenter(sphereCenter);

		//----------------check if OA is tangent to sphere----------------
		double aD,aD12,aD13,aD22,aD23;
		double center[3],testValue1,testValue2;
		double filterVector2[3],filterFlag2;
		bool aFlag1 = false,aFlag2 =false;
		center[0] = 0;center[1]= 0;center[2]=0;

		aD = vtkMath::Distance2BetweenPoints(center,olCoord);
		aD12 = vtkMath::Distance2BetweenPoints(olCoord,alCoord1);
		aD13 = vtkMath::Distance2BetweenPoints(center,alCoord1);
		aD22 = vtkMath::Distance2BetweenPoints(olCoord,alCoord2);
		aD23 = vtkMath::Distance2BetweenPoints(center,alCoord2);
		testValue1 = fabs(aD12 + aD13 - aD);
		testValue2 = fabs(aD22 + aD23 - aD);


		GetWrapGlobalTransform(agCoord1,aGcoord1);
		GetWrapGlobalTransform(agCoord2,aGcoord2);


		aD12 = vtkMath::Distance2BetweenPoints(aGcoord1,m_StartPoint);
		aD13 = vtkMath::Distance2BetweenPoints(aGcoord1,sphereCenter);
		aD22 = vtkMath::Distance2BetweenPoints(aGcoord2,m_StartPoint);
		aD23 = vtkMath::Distance2BetweenPoints(aGcoord2,sphereCenter);

		aD = vtkMath::Distance2BetweenPoints(m_StartPoint,sphereCenter);

		if (fabs(aD12+aD13-aD)<0.01)
		{

			if (m_WrapSide==WRAP_FRONT || m_WrapSide == WRAP_BACK){
				filterVector2[0] = agCoord1[0] - m_SphereWrapLocal[0];
				filterVector2[1] = agCoord1[1] - m_SphereWrapLocal[1];
				filterVector2[2] = agCoord1[2] - m_SphereWrapLocal[2];
				filterFlag2 = vtkMath::Dot(filterPlaneNormal,filterVector2);
				if ( (m_WrapSide==WRAP_FRONT && filterFlag2*filterFlag>0) || (m_WrapSide==WRAP_BACK && filterFlag2*filterFlag<0) )
				{
					CopyPointValue(agCoord1,agCoord);
					aFlag1 = true;
				}

			}else if (m_WrapSide==WRAP_SHOREST)
			{
				CopyPointValue(agCoord1,agCoord);
				aFlag1 = true;
			}
		} 
		if (fabs(aD22+aD23 -aD)<0.01)
		{

			if (m_WrapSide==WRAP_FRONT || m_WrapSide == WRAP_BACK){

				filterVector2[0] = agCoord2[0] - m_SphereWrapLocal[0];
				filterVector2[1] = agCoord2[1] - m_SphereWrapLocal[1];
				filterVector2[2] = agCoord2[2] - m_SphereWrapLocal[2];
				filterFlag2 = vtkMath::Dot(filterPlaneNormal,filterVector2);
				if ( (m_WrapSide==WRAP_FRONT && filterFlag2*filterFlag>0) || (m_WrapSide==WRAP_BACK && filterFlag2*filterFlag<0) )
				{
					CopyPointValue(agCoord2,agCoord);
					aFlag2 = true;
				}
			}else if (m_WrapSide==WRAP_SHOREST)
			{
				CopyPointValue(agCoord2,agCoord);
				aFlag2 = true;
			}
		}

		//step6 o,i,a global --> sphere center based local
		GetLocalCenterCoord(oCoordR,m_StartWrapLocal);
		GetLocalCenterCoord(iCoordR,endPoint);

		int ibflag = 1;
		if ( !aFlag2 && !aFlag1)
		{
			return -1;
		}else if (aFlag2 && aFlag1)
		{	//----------first a-----------
			GetLocalCenterCoord(aCoordR1,agCoord1);
			//step7 get circle which intersect with sphere ,appendix D
			GetIntersectCircle(aCoordR1,oCoordR,iCoordR,mCoordR1,Rm1);//Rm is computed by global coord
			bool bFlag1 =GetBcoordinate(aCoordR1,oCoordR,iCoordR,mCoordR1,Rm1,sphereR,bCoordR1,bCoordR2);

			//-----------second a---------
			GetLocalCenterCoord(aCoordR2,agCoord2);
			GetIntersectCircle(aCoordR2,oCoordR,iCoordR,mCoordR2,Rm2);//Rm is computed by global coord
			bool bFlag2 =GetBcoordinate(aCoordR2,oCoordR,iCoordR,mCoordR2,Rm2,sphereR,bCoordR3,bCoordR4);			

			if (bFlag1)
			{
				GetGlobalCenterCoord(bgCoord1,bCoordR1);
				GetGlobalCenterCoord(bgCoord2,bCoordR2);
				GetGlobalCenterCoord(mgCoord1,mCoordR1);

				rtn1 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord1,bgCoord1,mgCoord1,Rm1);
				rtn2 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord1,bgCoord2,mgCoord1,Rm1);
			}else{
				rtn1 = 0;
				rtn2 = 0;
			}

			if (bFlag2)
			{
				GetGlobalCenterCoord(bgCoord3,bCoordR3);
				GetGlobalCenterCoord(bgCoord4,bCoordR4);
				GetGlobalCenterCoord(mgCoord2,mCoordR2);

				rtn3 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord2,bgCoord3,mgCoord2,Rm2);
				rtn4 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord2,bgCoord4,mgCoord2,Rm2);
			}else{
				rtn3 = 0;
				rtn4 = 0;
			}

			if (rtn1!=0 && rtn1<rtn2 && rtn1<rtn3 && rtn1<rtn4)
			{
				Rm = Rm1;
				CopyPointValue(agCoord1,m_APoint);
				CopyPointValue(agCoord1,outAPoint);
				CopyPointValue(bgCoord1,m_BPoint);
				CopyPointValue(bgCoord1,outBPoint);
				GetGlobalCenterCoord(outMPoint,mCoordR1);
				rtn = rtn1;
			}else if (rtn2!=0 && rtn2<rtn1 && rtn2<rtn3 && rtn2<rtn4)
			{
				Rm = Rm1;
				CopyPointValue(agCoord1,m_APoint);
				CopyPointValue(agCoord1,outAPoint);
				CopyPointValue(bgCoord2,m_BPoint);
				CopyPointValue(bgCoord2,outBPoint);
				GetGlobalCenterCoord(outMPoint,mCoordR1);
				rtn = rtn2;
			}else if (rtn3!=0 && rtn3<rtn1 && rtn3<rtn2 && rtn3<rtn4)
			{
				Rm = Rm2;
				CopyPointValue(agCoord2,m_APoint);
				CopyPointValue(agCoord2,outAPoint);
				CopyPointValue(bgCoord3,m_BPoint);
				CopyPointValue(bgCoord3,outBPoint);
				GetGlobalCenterCoord(outMPoint,mCoordR2);
				rtn = rtn3;
			}else if (rtn4!=0 && rtn4<rtn1 && rtn4<rtn2 && rtn4<rtn3)
			{
				Rm = Rm2;
				CopyPointValue(agCoord2,m_APoint);
				CopyPointValue(agCoord2,outAPoint);
				CopyPointValue(bgCoord4,m_BPoint);
				CopyPointValue(bgCoord4,outBPoint);
				GetGlobalCenterCoord(outMPoint,mCoordR2);
				rtn = rtn4;
			}else{

				return -1;
			}

		}else if( aFlag1 || aFlag2){
			GetLocalCenterCoord(aCoordR,agCoord);
			//step7 get circle which intersect with sphere ,appendix D
			GetIntersectCircle(aCoordR,oCoordR,iCoordR,mCoordR,Rm);//Rm is computed by global coord
			//step8 get B coord,there are 2 point ,test which one is the shortest.use appendix E,use the same translation as in step7
			GetGlobalCenterCoord(outMPoint,mCoordR);
			bool bFlag =GetBcoordinate(aCoordR,oCoordR,iCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);
			if (bFlag)
			{
				CopyPointValue(agCoord,m_APoint);
				CopyPointValue(agCoord,outAPoint);

				GetGlobalCenterCoord(bgCoord1,bCoordR1);
				GetGlobalCenterCoord(bgCoord2,bCoordR2);
				GetGlobalCenterCoord(mgCoord,mCoordR);

				rtn1 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord,bgCoord1,mgCoord,Rm);
				rtn2 = ComputeDistanceOfLm(ogCoord,igCoord,agCoord,bgCoord2,mgCoord,Rm);
				if (rtn1<rtn2)
				{
					CopyPointValue(bgCoord1,m_BPoint);
					CopyPointValue(bgCoord1,outBPoint);
				}else{
					CopyPointValue(bgCoord2,m_BPoint);
					CopyPointValue(bgCoord2,outBPoint);
				}

			}else{
				return -1;
			}

		}
	}else{

		return -1;
	}

	return rtn;

}
//compute angle for point B 
void albaVMEComputeWrapping::ComputeAngleForB(double *aCoord,double *bCoord1,double *bCoord2,double *bCoord){
	double oa[3],ba1[3],ba2[3];
	double cosA1,cosA2;
	oa[0] = m_StartWrapLocal[0] - aCoord[0];
	oa[1] = m_StartWrapLocal[1] - aCoord[1];
	oa[2] = m_StartWrapLocal[2] - aCoord[2];

	ba1[0] = bCoord1[0] - aCoord[0];
	ba1[1] = bCoord1[1] - aCoord[1];
	ba1[2] = bCoord1[2] - aCoord[2];

	ba2[0] = bCoord2[0] - aCoord[0];
	ba2[1] = bCoord2[1] - aCoord[1];
	ba2[2] = bCoord2[2] - aCoord[2];

	//a dot b = |a||b|cosA
	vtkMath::Normalize(oa);
	vtkMath::Normalize(ba1);
	vtkMath::Normalize(ba2);

	cosA1 = vtkMath::Dot(oa,ba1); 
	cosA2 = vtkMath::Dot(oa,ba2);
	if (cosA1<cosA2)
	{
		bCoord[0] = bCoord1[0];
		bCoord[1] = bCoord1[1];
		bCoord[2] = bCoord1[2];
	}else{
		bCoord[0] = bCoord2[0];
		bCoord[1] = bCoord2[1];
		bCoord[2] = bCoord2[2];
	}
}
//get B point on the surface
bool albaVMEComputeWrapping::GetBcoordinateUpdate2(double *aCoord,double *bCoord,double *cCoord ,double *bCoord1,double *bCoord2){
	double blCoord[3];
	GetLocalCenterCoord(blCoord,bCoord);
	double Rm = GetCylinderRadius();
	double sphereR = GetSphereRadius();
	double bCoordR1[3],bCoordR2[3];
	double aCoordR[3],oCoordR[3],cCoordR[3],mCoordR[3];

	GetLocalCenterCoord(aCoordR,aCoord);
	GetLocalCenterCoord(oCoordR,m_StartWrapLocal);
	GetLocalCenterCoord(cCoordR,cCoord);
	GetIntersectCircle(aCoordR,oCoordR,cCoordR,mCoordR,Rm);//Rm is computed by global coord

	bool rtn =GetBcoordinate(aCoordR,oCoordR,cCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);

	GetGlobalCenterCoord(bCoord1,bCoordR1);
	GetGlobalCenterCoord(bCoord2,bCoordR2);
	return rtn;
}



//let point on the surface
void albaVMEComputeWrapping::ProcessViaPoint(){
	double r = GetCylinderRadius();
	double x,y,distX,distY;
	double newX[3],newY[3];

	//------try x---------
	x = sqrt(r*r - m_ViaPoint[1]*m_ViaPoint[1]);
	if ( m_ViaPoint[0]<0)
	{
		x=-x;
	}
	newX[0]=x;
	newX[0]=m_ViaPoint[1];
	newX[0]=m_ViaPoint[2];
	//-------try y--------
	y = sqrt(r*r - m_ViaPoint[0]*m_ViaPoint[0]);
	if (m_ViaPoint[1]<0)
	{
		y=-y;
	}
	newY[0]=m_ViaPoint[0];
	newY[1]=y;
	newY[2]=m_ViaPoint[2];
	//----------
	distX = sqrt(vtkMath::Distance2BetweenPoints(m_ViaPoint,newX));
	distY = sqrt(vtkMath::Distance2BetweenPoints(m_ViaPoint,newY));
	if(distX<distY){
		m_ViaPoint[0]=x;
	}else{
		m_ViaPoint[1]=y;
	}

}
//only copy value of a point,do not create it
void albaVMEComputeWrapping::CopyPointValue(double *srcPoint,double *aimPoint){
	aimPoint[0] =	srcPoint[0] ;
	aimPoint[1] =	srcPoint[1] ;
	aimPoint[2] =	srcPoint[2] ;
}
//process direction of helix
void albaVMEComputeWrapping::FormatParameterForHelix(double  tc,double  to,double  Zc,double Zo,double & c ,double & k,double & tcRtn,double & toRtn){

	double dif1,dif2,tmp;

	tcRtn = tc;
	toRtn = to;
	if (tc <to)
	{
		dif1 = (to-tc);
		dif2 = (tc+2*GetPi())-to;
		if (dif1>dif2)
		{
			tmp = tc;
			tcRtn = to;
			toRtn = tmp + 2*GetPi();
		}
	}else if (to <tc)
	{
		dif1 = (tc-to);
		dif2 = (to+2*GetPi())-tc;
		if (dif1>dif2)
		{
			tmp = to;
			toRtn = tc;
			tcRtn = tmp + 2*GetPi();
		}
	}

	c = ( Zc - Zo) / ( tcRtn - toRtn );
	k = Zc - ( c * tcRtn );
	k = Zo - ( c * toRtn );
}
double albaVMEComputeWrapping::CaculateHelix2(vtkPolyData * hCurve,double *cCoord,double *vCoord,double drawFlag,int objIdx){
	vtkALBASmartPointer<vtkCellArray> cells;
	vtkPoints *pts = vtkPoints::New();
	double Xc,Yc,Zc,Xo,Yo,Zo;
	double r;
	double cosTc,sinTc,cosTo,sinTo;
	double tc,to,c,k;
	double diff1,diff2;
	double rtn = 0.0;

	int pNum = 0;

	Xc = cCoord[0];Yc = cCoord[1];Zc = cCoord[2];
	Xo = vCoord[0];Yo = vCoord[1];Zo = vCoord[2];

	r = GetCylinderRadius(objIdx);//sqrt( Xc*Xc + Yc*Yc );

	cosTc = Xc/r;
	sinTc = Yc/r;
	tc = acos(cosTc);
	//if ( (cosTc>0 && sinTc>0) || (cosTc<0 && sinTc>0) ) use the same value that means :angle<90 or  180>angle>90
	if ( (cosTc<=0 && sinTc<0) || (cosTc>0 && sinTc<0))// 270>angle>180 or 360>angle>270
	{
		tc = 2*GetPi() -tc;
	}
	cosTo = Xo/r;
	sinTo = Yo/r;
	to = acos(cosTo);
	if ( (cosTo<=0 && sinTo<0) || (cosTo>0 && sinTo<0))// 270>angle>180 or 360>angle>270
	{
		to = 2*GetPi() -to;
	}

	//we compute from small angle to big angle
	if(tc<to){
		diff1 = to-tc;
		diff2 = (tc+2*GetPi()) -to;
		if (diff2<diff1)
		{
			tc = tc +2*GetPi();

		}
	}else if (to<tc)
	{
		diff1 = tc -to;
		diff2 = (to + 2*GetPi()) -tc;
		if (diff2<diff1)
		{
			to = to + 2*GetPi();
		}
	}

	c = (Zc-Zo) /(tc-to);
	k = Zc - c*tc;

	rtn = DrawHelix(tc,to,c,k, cells,pts,drawFlag,objIdx);

	if (drawFlag)
	{
		hCurve->SetPoints( pts );
		hCurve->SetLines(cells);
	}
	return rtn;
}
//compute helix formula and then draw curve
double albaVMEComputeWrapping::CaculateHelix2(vtkPolyData * hCurve,double *cCoord,double *vCoord,double drawFlag){
	double rtn;
	rtn = CaculateHelix2(hCurve,cCoord,vCoord,drawFlag,2);
	return rtn;

}

double albaVMEComputeWrapping::ComputeHelixCost2(double tc,double to,double c,double k,double *cCoord,double *vCoord){
	double firstPoint[3],lastPoint[3],firstPoint2[3],lastPoint2[3];
	double rtnArr[4],rtn;
	double r;
	r = GetCylinderRadius();
	firstPoint[0] = r * cos(tc);
	firstPoint[1] = r * sin(tc);
	firstPoint[2] = c * tc + k;

	firstPoint2[0] = r * cos(tc);
	firstPoint2[1] = -r * sin(tc);
	firstPoint2[2] = c * tc + k;

	lastPoint[0] = r * cos(to);
	lastPoint[1] = r * sin(to);
	lastPoint[2] = c * to + k;

	lastPoint2[0] = r * cos(to);
	lastPoint2[1] = -r * sin(to);
	lastPoint2[2] = c * to + k;

	rtnArr[0] = sqrt(vtkMath::Distance2BetweenPoints(cCoord,firstPoint));//c  & first
	rtnArr[0]+= sqrt(vtkMath::Distance2BetweenPoints(vCoord,lastPoint));//i & last

	rtnArr[1] = sqrt(vtkMath::Distance2BetweenPoints(cCoord,firstPoint2));//c  & first
	rtnArr[1]+= sqrt(vtkMath::Distance2BetweenPoints(vCoord,lastPoint));//i & last

	rtnArr[2] = sqrt(vtkMath::Distance2BetweenPoints(cCoord,firstPoint));//c  & first
	rtnArr[2]+= sqrt(vtkMath::Distance2BetweenPoints(vCoord,lastPoint2));//i & last

	rtnArr[3] = sqrt(vtkMath::Distance2BetweenPoints(cCoord,lastPoint2));//c  & first
	rtnArr[3]+= sqrt(vtkMath::Distance2BetweenPoints(vCoord,firstPoint2));//i & last
	rtn = rtnArr[0];
	for (int i=0;i<4;i++)
	{
		if (rtnArr[i]<rtn)
		{
			rtn = rtnArr[i];
		}
	}
	return rtn;
}
double albaVMEComputeWrapping::DrawHelix(double tc,double to,double c,double k,vtkCellArray * cells,vtkPoints *pts,bool drawFlag, int objIdx){

	int j=0;
	double i = 0;
	double x,y,z,r;
	double dMin,dMax;
	double beforePoint[3],nextPoint[3],gPoint[3];
	double rtn = 0;
	double dstep;
	int step = 10;

	r = GetCylinderRadius(objIdx);//sqrt( Xc*Xc + Yc*Yc );

	dMin = tc;
	dMax = to;
	if (to <tc)
	{
		dMin = to;
		dMax = tc;
	}
	dstep = (dMax - dMin)/step;
	for ( i=dMin, j=0 ;j<=step;i=i+dstep,j++)
	{


		x = r*cos(i);
		y = r*sin(i);
		z = c*i+k;

		nextPoint[0] = x;
		nextPoint[1] = y;
		nextPoint[2] = z;

		if (drawFlag)
		{
			GetWrapGlobalTransform(nextPoint,gPoint,objIdx);
			pts->InsertPoint( j, gPoint[0], gPoint[1], gPoint[2] );
		}

		if (j!=0 )
		{

			if (drawFlag)
			{
				vtkIdType lineids[2];
				lineids[0] = j-1;
				lineids[1] = j;
				cells->InsertNextCell(2,lineids);
			}
			rtn += sqrt(vtkMath::Distance2BetweenPoints(beforePoint,nextPoint));
		}
		beforePoint[0] = x;
		beforePoint[1] = y;
		beforePoint[2] = z;


	}

	return rtn;

}
//compute points for helix
double albaVMEComputeWrapping::DrawHelix(double tc,double to,double c,double k,vtkCellArray * cells,vtkPoints *pts,bool drawFlag){
	double rtn;
	rtn = DrawHelix(tc,to,c,k, cells,pts,drawFlag ,2);
	return rtn;

}
//compute curve length YUxuan DingLi
double albaVMEComputeWrapping::ComputeCurveLength(double *aCoord,double *bCoord,double rm){
	double rtn = 0;
	double disAB2 ;
	disAB2 = vtkMath::Distance2BetweenPoints(aCoord,bCoord);
	rtn = rm * acos( (2*rm*rm - disAB2 )/(2*rm*rm) );
	return rtn;

}

double albaVMEComputeWrapping::ComputeDistanceOfLm3(double *start,double *cCoord,double *iCoord){
	double rtn = 0;
	double oc,ci;
	oc = sqrt(vtkMath::Distance2BetweenPoints(start,cCoord));
	ci = CaculateHelix2(NULL,cCoord,m_ViaWrapLocal,false);//only compute length
	rtn = oc + ci;
	return rtn;
}

//wrap sphere
void albaVMEComputeWrapping::WrapSphereOnly(const int step,bool allowIntersectFlag){
	double low,high;
	double cylinderR;
	double dstep,y1,rm = 0;
	double sphereCenter[3],cylinderCenter[3];
	double filterFlag =0,filterPlaneNormal[3];
	double aCoord[3],bCoord[3],mCoord[3];
	double OAlength,ABcurve,BElength;
	double aCoordFinal[3],bCoordFinal[3],mCoordFinal[3],rmFinal = 0,tmpLm = 0,LmFinal = 0,angleFinal = 0.000;
	double aGcoord[3],bGcoord[3],mGcoord[3];
	vtkLineSource     *OA,*BE;
	vtkClipPolyData *clipData; 
	int idx = 0;

	m_Goniometer->RemoveAllInputs();
	m_WrapSide=WRAP_SHOREST;//only compute shorest

	low = 0;
	high = 2 * GetPi();
	cylinderR = GetCylinderRadius();

	dstep = (high-low)/step;
	GetGlobalSphereCenter(sphereCenter);
	GetGlobalCylinderCenter(cylinderCenter);

	GetWrapLocalTransform(sphereCenter,m_SphereWrapLocal);
	GetWrapLocalTransform(m_StartPoint,m_StartWrapLocal);
	GetWrapLocalTransform(m_EndPoint,m_EndWrapLocal);

	if (m_WrapSide==WRAP_FRONT || m_WrapSide == WRAP_BACK)
	{
		filterFlag = GetFilterFlag(filterPlaneNormal);
	}
	filterFlag = 0;
	for (double j=low;j<high;j = j+dstep)
	{

		y1 = GetFunctionValue2(j,filterFlag,filterPlaneNormal,m_EndWrapLocal,aCoord,bCoord,mCoord,rm);//m is global

		if (y1 != -1)
		{

			OAlength = sqrt(vtkMath::Distance2BetweenPoints(aCoord,m_SphereWrapLocal));
			ABcurve = ComputeCurveLength(aCoord,bCoord,rm);
			BElength = sqrt(vtkMath::Distance2BetweenPoints(bCoord,m_EndWrapLocal));
			tmpLm = OAlength + ABcurve + BElength;

			GetWrapGlobalTransform(bCoord,bGcoord);


			bool intersectFlag = false;
			if (!allowIntersectFlag)
			{
				intersectFlag = IsLineInterSectObject(GetWrappedVME2(),bGcoord,m_EndPoint);
			}

			if (!intersectFlag && (idx==0 || (tmpLm<LmFinal) ) )//first time
			{

				idx++;
				angleFinal = j;
				LmFinal = tmpLm;

				aCoordFinal[0] = aCoord[0];aCoordFinal[1]=aCoord[1];aCoordFinal[2]=aCoord[2];
				bCoordFinal[0] = bCoord[0];bCoordFinal[1]=bCoord[1];bCoordFinal[2]=bCoord[2];
				mCoordFinal[0] = mCoord[0];mCoordFinal[1]=mCoord[1];mCoordFinal[2]=mCoord[2];
				rmFinal = rm;

				//outputFile << "for angle :"<<j<< ", length for whole path is :   " <<LmFinal<<"  ."<< '\t'<<'\n';
			}
		}
	}

	//--------------single path-------------
	if(idx>0){

		vtkNEW(OA);
		vtkNEW(BE);
		vtkNEW(clipData);



		GetWrapGlobalTransform(aCoordFinal,aGcoord);
		GetWrapGlobalTransform(bCoordFinal,bGcoord);
		GetWrapGlobalTransform(mCoordFinal,mGcoord);



		//----------------------test code ----------------	
		/*	vtkLineSource     *L1,*L2;
		vtkNEW(L1);
		vtkNEW(L2);
		L1->SetPoint1(aGcoord[0],aGcoord[1],aGcoord[2]);
		L1->SetPoint2(mGcoord[0],mGcoord[1],mGcoord[2]);

		L2->SetPoint1(bGcoord[0],bGcoord[1],bGcoord[2]);
		L2->SetPoint2(mGcoord[0],mGcoord[1],mGcoord[2]);
		m_Goniometer->AddInput(L1->GetOutput());
		m_Goniometer->AddInput(L2->GetOutput());*/
		//-------------------------------------------------
		ABcurve = GetCutPlane2(aGcoord,bGcoord,mGcoord,clipData);//AB curve
		clipData->Update();


		TransformOutputPoint(m_EndPoint);
		TransformOutputPoint(m_StartPoint);
		TransformOutputPoint(aGcoord);
		TransformOutputPoint(bGcoord);
		TransformOutputPoint(mGcoord);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(aGcoord,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(bGcoord,m_ExportPointList[m_ExportPointList.size()-1]);

		OA->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);//OA
		OA->SetPoint2(aGcoord[0],aGcoord[1],aGcoord[2]);


		BE->SetPoint1(bGcoord[0],bGcoord[1],bGcoord[2]);
		BE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);

		m_Goniometer->AddInput(OA->GetOutput());

		TransformOutputClipData(clipData->GetOutput());
		m_Goniometer->AddInput(clipData->GetOutput());
		clipData->SetOutput(NULL);

		m_Goniometer->AddInput(BE->GetOutput());

		vtkDEL(OA);
		vtkDEL(BE);
		vtkDEL(clipData);

		m_Distance = LmFinal;

		InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
		GetWrappedMeterOutput()->Update(); 
	}else{
		DirectConnectSE();

	}


}

void albaVMEComputeWrapping::GetSphereCylinderWrapAdvance(const int step){


	double filterFlag;
	double cCoord1[3],cCoord2[3];
	double endLocal[3],startLocal[3],sphereCenter[3],sphereCenterLocal[3];
	double Zh,Zl,dstep;
	double tangVia1[3],tangVia2[3];



	GetGlobalSphereCenter(sphereCenter);
	GetWrapLocalTransform(sphereCenter,sphereCenterLocal);
	GetWrapLocalTransform(m_EndPoint,endLocal);
	GetWrapLocalTransform(m_StartPoint,startLocal);

	filterFlag = 1;//getFilterFlag(filterPlaneNormal);

	Zh = sphereCenterLocal[2]-GetSphereRadius();
	Zl = endLocal[2];
	dstep = (Zh-Zl)/10;
	//------------------compute insertion point---------------------
	if (filterFlag == WRAP_FRONT) 
	{
		//----------wrap sphere only
	}else if (filterFlag == WRAP_BACK)
	{
		GetCcoordinateForCylinder(endLocal,cCoord1,cCoord2,2);

		GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord1,NULL,1);
		GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord2,NULL,1);

		GetWrapGlobalTransform(cCoord1,tangVia1);
		GetWrapGlobalTransform(cCoord2,tangVia2);

		GetSphereCylinderWrap(m_PathNum,tangVia1);
		TransformOutputPoint(tangVia1);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(tangVia1,m_ExportPointList[m_ExportPointList.size()-1]);

	}
}
//get a tangent Point from end  
//we get two point,if mode is 1,use distance,if mode is 2, use same side to choose a point from these two
//if mode is 2,need p2Global.
//pointGlobal is return parameter.
void albaVMEComputeWrapping::GetOneSideTangentPointOnCylinder(double *startLocal,double *endLocal,int vmeIdx,int mode,double *p1Global,double *pointGlobal){


	double Zl,Zh;//Zi,Zo,
	double cCoord1_1[3],cCoord1_2[3],cCoordGlobal1[3],cCoordGlobal2[3],pointLocal[3];
	double d1,d2;


	GetCcoordinateForCylinder(endLocal,cCoord1_1,cCoord1_2,vmeIdx);//last parameter shows which vme object

	Zh = startLocal[2];
	Zl = endLocal[2];
	if (Zh<Zl)
	{
		Zh = endLocal[2];
		Zl = startLocal[2];
	}

	GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord1_1,NULL,4);//use distanceSum 4 or angleValue 1
	GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord1_2,NULL,4);

	//GetWrapGlobalTransform(cCoord1_1,cCoordGlobal1,vmeIdx);
	//GetWrapGlobalTransform(cCoord1_2,cCoordGlobal2,vmeIdx);
	if (mode ==1 && p1Global==NULL)//use distance to choose
	{
		d1 =  sqrt(vtkMath::Distance2BetweenPoints(endLocal,cCoord1_1));
		d2 =  sqrt(vtkMath::Distance2BetweenPoints(endLocal,cCoord1_2));
		d1 += sqrt(vtkMath::Distance2BetweenPoints(cCoord1_1,startLocal));
		d2 += sqrt(vtkMath::Distance2BetweenPoints(cCoord1_2,startLocal));//obj 2
		if (d1<d2)
		{
			CopyPointValue(cCoord1_1,pointLocal);
		}else{
			CopyPointValue(cCoord1_2,pointLocal);
		}
		GetWrapGlobalTransform(pointLocal,pointGlobal,vmeIdx);
	}else if (mode ==2 && p1Global != NULL)//use plane to choose
	{

		GetWrapGlobalTransform(cCoord1_1,cCoordGlobal1,vmeIdx);
		GetWrapGlobalTransform(cCoord1_2,cCoordGlobal2,vmeIdx);
		ChooseSameSidePoint(m_EndPoint,p1Global,cCoordGlobal1,cCoordGlobal2,vmeIdx,pointGlobal);
	}


	//GetWrapGlobalTransform(point,pointGlobal,vmeIdx);
}
//--we want to get p2,because p1 and p2 is on same vme so use local value
//-get tangent point from start ,and with the same plane of p1
void albaVMEComputeWrapping::GetOneSideTangentPointOnCylinderWithSamePlane(double *startLocal,double *endLocal,double *p1,int vmeIdx,double *p2Global)
{
	double Zl,Zh;//Zi,Zo,
	double cCoord2_1[3],cCoord2_2[3],cCoordGlobal1[3],cCoordGlobal2[3],p1Global[3];

	Zh = startLocal[2];
	Zl = endLocal[2];
	if (Zh<Zl)
	{
		Zh = endLocal[2];
		Zl = startLocal[2];
	}

	GetCcoordinateForCylinder(startLocal,cCoord2_1,cCoord2_2,vmeIdx);

	GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord2_1,p1,2);//last parameter use distanceSum 4 or angleValue 1
	GoldenSectionSearch(Zl,Zh,0.001,endLocal,startLocal,cCoord2_2,p1,2);//use plane 2

	GetWrapGlobalTransform(p1,p1Global,vmeIdx);
	GetWrapGlobalTransform(cCoord2_1,cCoordGlobal1,vmeIdx);
	GetWrapGlobalTransform(cCoord2_2,cCoordGlobal2,vmeIdx);

	ChooseSameSidePoint(m_EndPoint,p1Global,cCoordGlobal1,cCoordGlobal2,vmeIdx,p2Global);
}
/************************************************************************/
/* to test if need wrap double cylinder.
/* in general path is end---p1-vme2-p2--p3-vme1-p4-start
/* if p2-start do not intersect with vme1 for example, path will not touch vme1 at all
/* these points (start ,end ,p1,p2 are global value)
/* rtn is length of path, or else  rtn is -1,if path do not intersect with second vme
/************************************************************************/
/*double medVMEComputeWrapping::isWrapDoubleCylinder(double *start,double *end,int vmeIdx,double *p1,double *p2){



}
*/

//find a tangent line of one cylinder from end point we can get point 1 and 2.
//then from point 2 as start point to get another tangent line of second cylinder we get point 3 .
//from point 3 get point 2 and from point 2 get point 3 iteratively.
//util these two point has nothing different from last one.
void albaVMEComputeWrapping::GetCylinderCylinderWrap(const int step){


	double viaLocal[3],startLocal[3],endLocal[3];//,tmpCoord[3],cCoord4[3],cCoord3[3]
	double cCoord1_1[3],cCoord1_2[3],cCoord4_1[3],cCoord4_2[3];
	double Zl,Zh;//Zi,Zo,

	double zValue1,zValue2,testValue;
	double cCoordGlobal1[3],cCoordGlobal2[3];//cCoordGlobal3[3],,cCoordGlobal4[3]
	double p1[3],p2_1[3],p13[3],p4[3],p22[3],p3_2[3],p2Global[3],p1Global[3],p3Global[3],p4Global[3];
	double flagP2[3];//testP2[3],testP3[3],
	//double CIcurve ;//,CIcurve1,CIcurve2
	double d1,d2,dCurve12,dCurve34;
	double center1[3],center2[3];
	bool stopFlag = false;
	bool exchangeFlag = false;
	//double planeAB[3];//,planeAC[3]
	vtkPolyData *hCurve12,*hCurve34;
	vtkLineSource *Line1,*Line2,*Line3;//,*Line5;,*Line4
	int idx1,idx2;
	double lineLength=0.00;
	//--------------check distance between end point and center of object--------------
	GetGlobalCylinderCenter(center1,1);
	GetGlobalCylinderCenter(center2,2);

	idx1=1;//by default end is near obj2
	idx2=2;

	d1 = GetPointLineDistance(m_EndPoint,center1,m_CylinderAxis1);//distance from end point to cylinder axis
	d2 = GetPointLineDistance(m_EndPoint,center2,m_CylinderAxis2);
	if (d1<d2)//end point is near cylinder1
	{
		idx1=2;
		idx2=1;
		exchangeFlag = true;
	}
	//-------------------the shot distance decide obj1 index that is idx1-------------------------------
	zValue1 = 0.00;
	zValue2 = 0.00;

	m_Goniometer->RemoveAllInputs();

	GetWrapLocalTransform(m_EndPoint,endLocal,idx2);//relative to vme2
	GetWrapLocalTransform(m_StartPoint,startLocal,idx2);
	GetWrapLocalTransform(m_ViaPoint,viaLocal,idx2);//by default via point computed near end point

	//get C coord x,y value
	//-------------*get p1 from end point vme 2*--------------------------

	GetOneSideTangentPointOnCylinder(startLocal,endLocal,idx2,1,NULL,p1Global);//choose by distance,mode 1 .parameter :(double *startLocal,double *endLocal,int vmeIdx,int mode,double *p1Global,double *pointGlobal)
	GetWrapLocalTransform(p1Global,p1,idx2);

	//-------------*get p2 from start point vme2*------------------------
	GetOneSideTangentPointOnCylinderWithSamePlane(startLocal,endLocal,p1,idx2,p2Global);//
	//----------check if line p2-start intersect with vme1

	vtkNEW(Line1);
	vtkNEW(Line2);

	bool intersectFlag = false;
	if (exchangeFlag)
	{
		intersectFlag = IsLineInterSectObject(GetWrappedVME2(),p2Global,m_StartPoint);
	}else{
		intersectFlag = IsLineInterSectObject(GetWrappedVME1(),p2Global,m_StartPoint);
	}

	if (intersectFlag)//if line p2-start intersect vme1 go on
	{
		//-------------*get p4 from start point to vme 1*--------------------------

		GetWrapLocalTransform(m_EndPoint,endLocal,idx1);//last parameter means object index
		GetWrapLocalTransform(m_StartPoint,startLocal,idx1);//last parameter means object index
		GetOneSideTangentPointOnCylinder(endLocal,startLocal,idx1,2,p1Global,p4Global);//choose by same side ,parameter :(double *startLocal,double *endLocal,int vmeIdx,int mode,double *p1Global,double *pointGlobal)

		vtkNEW(Line3);

		//relative to vme1 
		while ( !stopFlag )//if two points is very near
		{	
			vtkLineSource *tmpLine;
			vtkNEW(tmpLine);


			CopyPointValue(p2Global,flagP2);

			//-------*get p3 from p2*------------------- 

			GetWrapLocalTransform(p2Global,p2_1,idx1);// relative to vme 1
			GetWrapLocalTransform(p1Global,p1,idx1);
			GetWrapLocalTransform(m_StartPoint,startLocal,idx1);
			GetWrapLocalTransform(m_EndPoint,endLocal,idx1);
			GetWrapLocalTransform(p4Global,p4,idx1);
			GetOneSideTangentPointOnCylinderWithSamePlane(p2_1,startLocal,p1,idx1,p3Global);

			//-----------*get p2 from p3  relative vme 2*-------------------
			GetWrapLocalTransform(p3Global,p3_2,idx2);//
			GetWrapLocalTransform(p1Global,p1,idx2);//p1 relative vme2
			GetWrapLocalTransform(m_EndPoint,endLocal,idx2);
			GetWrapLocalTransform(m_StartPoint,startLocal,idx2);
			GetWrapLocalTransform(p4Global,p4,idx2);
			GetOneSideTangentPointOnCylinderWithSamePlane(p3_2,endLocal,p4,idx2,p2Global);

			stopFlag = CheckNearEnough(flagP2,p2Global);//@to do check two points

		}

		//-----------------get P1 from end point idx2-------------
		GetWrapLocalTransform(m_EndPoint,endLocal,idx2);
		GetWrapLocalTransform(p2Global,p22,idx2);
		GetWrapLocalTransform(m_StartPoint,startLocal,idx2);
		GetWrapLocalTransform(p3Global,p3_2,idx2);

		Zh = startLocal[2];
		Zl = endLocal[2];
		if (Zh<Zl)
		{
			Zh = endLocal[2];
			Zl = startLocal[2];
		}
		testValue = GetPointOnPlane(Zl,Zh,endLocal,p3_2,p22,cCoord1_1);
		testValue = GetPointOnPlane(Zl,Zh,endLocal,p3_2,p22,cCoord1_2);

		GetWrapGlobalTransform(cCoord1_1,cCoordGlobal1,idx2);
		GetWrapGlobalTransform(cCoord1_2,cCoordGlobal2,idx2);

		//---------------choose one point witch at same side as p1--------------------------
		ChooseSameSidePoint(m_EndPoint,p2Global,cCoordGlobal1,cCoordGlobal2,idx2,p1Global);
		//-----------------get P4 from start point idx------------

		GetWrapLocalTransform(m_EndPoint,endLocal,idx1);
		GetWrapLocalTransform(p3Global,p13,idx1);
		GetWrapLocalTransform(p2Global,p2_1,idx1);
		GetWrapLocalTransform(m_StartPoint,startLocal,idx1);


		testValue = GetPointOnPlane(Zl,Zh,startLocal,p13,p2_1,cCoord4_1);
		testValue = GetPointOnPlane(Zl,Zh,startLocal,p13,p2_1,cCoord4_2);

		GetWrapGlobalTransform(cCoord4_1,cCoordGlobal1,idx1);
		GetWrapGlobalTransform(cCoord4_2,cCoordGlobal2,idx1);

		ChooseSameSidePoint(m_StartPoint,p3Global,cCoordGlobal1,cCoordGlobal2,idx1,p4Global);	
		//--------------------------------------------------------
		GetWrapLocalTransform(p1Global,p1,idx2);
		GetWrapLocalTransform(p2Global,p22,idx2);
		GetWrapLocalTransform(p3Global,p13,idx1);
		GetWrapLocalTransform(p4Global,p4,idx1);
		vtkNEW(hCurve12);
		dCurve12 = CaculateHelix2(hCurve12,p1,p22,true,idx2);
		//finally, connect with start point
		vtkNEW(hCurve34);
		dCurve34 = CaculateHelix2(hCurve34,p13,p4,true,idx1);//do transform inside
		//----------------------draw line---------------
		TransformOutputPoint(m_EndPoint);
		TransformOutputPoint(p1Global);
		TransformOutputPoint(p2Global);
		TransformOutputPoint(p3Global);
		TransformOutputPoint(p4Global);
		TransformOutputPoint(m_StartPoint);
		TransformOutput(hCurve12);
		TransformOutput(hCurve34);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p1Global,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p2Global,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p3Global,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p4Global,m_ExportPointList[m_ExportPointList.size()-1]);

		lineLength += sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,p1Global));  //end--p1
		lineLength += sqrt(vtkMath::Distance2BetweenPoints(p2Global,p3Global));    //p2--p3
		lineLength += sqrt(vtkMath::Distance2BetweenPoints(p4Global,m_StartPoint));//p4--start
		lineLength += dCurve12;
		lineLength += dCurve34;
		m_Distance = lineLength;

		Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
		Line1->SetPoint2(p1Global[0],p1Global[1],p1Global[2]);
		Line2->SetPoint1(p2Global[0],p2Global[1],p2Global[2]);
		Line2->SetPoint2(p3Global[0],p3Global[1],p3Global[2]);
		Line3->SetPoint1(p4Global[0],p4Global[1],p4Global[2]);
		Line3->SetPoint2(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
		m_Goniometer->AddInput(Line1->GetOutput());//end--p1
		m_Goniometer->AddInput(Line2->GetOutput());//p2--p3
		m_Goniometer->AddInput(Line3->GetOutput());//p4--start
		m_Goniometer->AddInput(hCurve12);
		m_Goniometer->AddInput(hCurve34);
	}else{//only connect end-p1-p2-start
		TransformOutputPoint(m_EndPoint);
		TransformOutputPoint(p1Global);
		TransformOutputPoint(p2Global);
		TransformOutputPoint(m_StartPoint);
		GetWrapLocalTransform(p1Global,p1,idx2);
		GetWrapLocalTransform(p2Global,p22,idx2);

		vtkNEW(hCurve12);
		dCurve12 = CaculateHelix2(hCurve12,p1,p22,true,idx2);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p1Global,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(p2Global,m_ExportPointList[m_ExportPointList.size()-1]);

		lineLength += sqrt(vtkMath::Distance2BetweenPoints(m_EndPoint,p1Global));  //end--p1
		lineLength += dCurve12;
		lineLength += sqrt(vtkMath::Distance2BetweenPoints(p2Global,m_StartPoint));    //p2--p3
		m_Distance = lineLength;

		Line1->SetPoint1(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
		Line1->SetPoint2(p1Global[0],p1Global[1],p1Global[2]);
		Line2->SetPoint1(p2Global[0],p2Global[1],p2Global[2]);
		Line2->SetPoint2(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);

		m_Goniometer->AddInput(Line1->GetOutput());//end--p1
		m_Goniometer->AddInput(Line2->GetOutput());//p2--p3
		m_Goniometer->AddInput(hCurve12);

	}
		
	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 
}
/************************************************************************/
// output point is input parameter as well,it with x and y value.
// this method will get z value,which is the nearest point with plane
/************************************************************************/
double albaVMEComputeWrapping::GetPointOnPlane(double zL ,double zH,double *point1,double *point2,double *point3,double *output){
	int step = 100;
	double factor = (zH -zL)/100;
	double rtn,tmpValue,zValue;
	int idx = 0;

	while (zL<zH)
	{
		output[2] = zL;
		tmpValue = GetPlaneValue(point1,point2,point3,output);
		if (idx ==0)
		{
			rtn = tmpValue;
			zValue = zL;
		}
		else if (fabs(tmpValue) < fabs(rtn))
		{
			rtn = tmpValue;
			zValue = zL;
		}
		zL = zL + factor;
		idx ++;
	}
	output[2] = zValue;
	return rtn;
}
//distance between point and line
//use area formula
double albaVMEComputeWrapping::GetPointLineDistance(double *point,double *Lpoint1,double *Lpoint2){
	/*a=sqrt£¨(x1-x2)^2+(y1-y2)^2+(z1-z2)^2£©
	b=sqrt£¨(x2-x3)^2+(y2-y3)^2+(z2-z3)^2£©
	c=sqrt£¨(x3-x1)^2+(y3-y1)^2+(z3-z1)^2)
	Áîs=£¨a+b+c)/2,  area formula S=sqrt£¨s*(s-a)(s-b)(s-c));
	distance between point (x1,y1,z1) to line is: d=2*S/b¡£	 
	*/
	double rtn ;
	double a,b,c,s,S,test;
	a = sqrt(  (point[0]-Lpoint1[0])*(point[0]-Lpoint1[0]) + (point[1]-Lpoint1[1]) * (point[1]-Lpoint1[1]) + (point[2]-Lpoint1[2])*(point[2]-Lpoint1[2]));
	b = sqrt(  (Lpoint1[0]-Lpoint2[0])*(Lpoint1[0]-Lpoint2[0]) + (Lpoint1[1]-Lpoint2[1])*(Lpoint1[1]-Lpoint2[1]) + (Lpoint1[2]-Lpoint2[2])*(Lpoint1[2]-Lpoint2[2]));
	test = vtkMath::Distance2BetweenPoints(Lpoint1,Lpoint2);
	c = sqrt(  (Lpoint2[0]-point[0])*(Lpoint2[0]-point[0]) + (Lpoint2[1]-point[1])*(Lpoint2[1]-point[1]) + (Lpoint2[2]-point[2])*(Lpoint2[2]-point[2]));
	s = (a+b+c)/2;
	S = sqrt(s*(s-a)*(s-b)*(s-c));

	rtn = 2*S/b;
	return rtn;

}
void albaVMEComputeWrapping::ChooseSameSidePoint(double *planePoint1,double *filterPoint,double *inputPoint1,double *inputPoint2,int cylinderIdx,double *rtnPoint){
	double cylinderCenter[3],filterPlaneNormal[3];
	GetGlobalCylinderCenter(cylinderCenter,cylinderIdx);

	vtkPlaneSource *planeSource;
	vtkNEW(planeSource);
	planeSource->SetOrigin(m_EndPoint);
	planeSource->SetPoint1(cylinderCenter);
	if (cylinderIdx==1)
	{
		planeSource->SetPoint2(m_CylinderAxis1);
	}else{
		planeSource->SetPoint2(m_CylinderAxis2);
	}
	planeSource->GetNormal(filterPlaneNormal);

	double endP1[3],endP4_1[3],endP4_2[3];
	double director,filterFlag1,filterFlag2;
	endP1[0] = filterPoint[0] - planePoint1[0];
	endP1[1] = filterPoint[1] - planePoint1[1];
	endP1[2] = filterPoint[2] - planePoint1[2];
	vtkMath::Normalize(endP1);
	director = vtkMath::Dot(endP1,filterPlaneNormal);

	endP4_1[0] = inputPoint1[0] - m_EndPoint[0];
	endP4_1[1] = inputPoint1[1] - m_EndPoint[1];
	endP4_1[2] = inputPoint1[2] - m_EndPoint[2];

	endP4_2[0] = inputPoint2[0] - m_EndPoint[0];
	endP4_2[1] = inputPoint2[1] - m_EndPoint[1];
	endP4_2[2] = inputPoint2[2] - m_EndPoint[2];

	vtkMath::Normalize(endP4_1);
	vtkMath::Normalize(endP4_2);
	filterFlag1 = vtkMath::Dot(endP4_1,filterPlaneNormal);
	filterFlag2 = vtkMath::Dot(endP4_2,filterPlaneNormal);
	if ( director*filterFlag1>0 && director*filterFlag2<0)
	{
		CopyPointValue(inputPoint1,rtnPoint);
	}else if ( director*filterFlag2>0 && director*filterFlag1<0)
	{
		CopyPointValue(inputPoint2,rtnPoint);
	}else if (director*filterFlag2>0 && director*filterFlag1>0) //same director
	{

	}	

}

bool albaVMEComputeWrapping::CheckNearEnough(double *firstPoint,double *secondPoint){
	bool rtn = false;
	double distance;
	distance = (firstPoint[0]-secondPoint[0])*(firstPoint[0]-secondPoint[0]) +(firstPoint[1]-secondPoint[1])*(firstPoint[1]-secondPoint[1])+(firstPoint[2]-secondPoint[2])*(firstPoint[2]-secondPoint[2]);
	if (distance<2)
	{
		rtn = true;
	}
	return rtn;
}
//-------------
//viaPoint is a point on the cylinder surface
void albaVMEComputeWrapping::GetSphereCylinderWrap(const int step,double *viaPoint){
	//const int step = 36;
	clock_t startTime,finishTime;
	double totalTime;
	double dstep,d1,d2,y1,rm = 0;
	double y0 = 0;
	double x0 = 0;
	double aCoord[3],bCoord[3],bCoord2[3],bCoord1[3],cCoord1[3],cCoord2[3],cCoord[3],mCoord[3];
	bool cflag;
	double length = 0;
	double low,high,testValue;
	double testValue1,testValue2;
	double tanValue1,tanValue2,tanCost1,tanCost2;
	double cylinderR;
	double viaWrapLocal[3];
	int ibflag;
	vtkLineSource     *OA,*BC;
	vtkPolyData *hcurve;

	albaString logFname = "wholePathForAngle.txt";
	albaString logFname2 = "chartFile.txt";
	std::ofstream outputFile(logFname, std::ios::out);
	std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);
	int idx = 0;
	double aCoordFinal[3],bCoordFinal[3],cCoordFinal[3],mCoordFinal[3],rmFinal = 0,tmpLm = 0,LmFinal = 0,angleFinal = 0.000;
	double OAlength,ABcurve,BClength,CIcurve;

	double sphereCenter[3],cylinderCenter[3];
	double filterFlag =0,filterPlaneNormal[3];
	double aGcoord[3],bGcoord[3],cGcoord[3],mGcoord[3];
	startTime = clock();

	//outputFile << "write log for every angle and whole path length for this angle" << '\t'<<'\n';
	//outputFile2<<"write log for every angle and path length for OA-ABcurve-BI"<<std::endl;
	outputFile.clear();
	low = 0;
	high = 2 * GetPi();
	m_Goniometer->RemoveAllInputs();

	cylinderR = GetCylinderRadius();
	vtkClipPolyData *clipData; 

	vtkClipPolyData *clipDataCI;

	dstep = (high-low)/step;


	GetGlobalSphereCenter(sphereCenter);
	GetGlobalCylinderCenter(cylinderCenter);

	GetWrapLocalTransform(sphereCenter,m_SphereWrapLocal);

	GetWrapLocalTransform(m_StartPoint,m_StartWrapLocal);

	GetWrapLocalTransform(viaPoint,viaWrapLocal);

	//------------transform start ,sphere center,insertion 

	if (m_WrapSide==WRAP_FRONT || m_WrapSide == WRAP_BACK)
	{
		filterFlag = GetFilterFlag(filterPlaneNormal);
	}


	for (double j=low;j<high;j = j+dstep)
	{

		length = 0;

		vtkNEW(clipData);
		vtkNEW(clipDataCI);

		y1 = GetFunctionValue2(j,filterFlag,filterPlaneNormal,viaWrapLocal,aCoord,bCoord,mCoord,rm);//m is global

		if (y1 != -1)
		{
			testValue = TestPlane(aCoord,bCoord,m_StartWrapLocal,viaWrapLocal);

			for (int i=0;i<10;i++)
			{
				cflag = GetCcoordinate2(bCoord,cCoord1,cCoord2);
				d1 = ComputeDistanceOfLm2(m_StartWrapLocal,viaWrapLocal,aCoord,bCoord,cCoord1,rm);
				d2 = ComputeDistanceOfLm2(m_StartWrapLocal,viaWrapLocal,aCoord,bCoord,cCoord2,rm);
				cCoord[0] = cCoord1[0];cCoord[1] = cCoord1[1];cCoord[2] = cCoord1[2];
				if (d1>d2)
				{
					cCoord[0] = cCoord2[0];cCoord[1] = cCoord2[1];cCoord[2] = cCoord2[2];
				}		
				if (cflag)
				{
					testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
					testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;

					GetBcoordinateUpdate2(aCoord,bCoord,cCoord,bCoord1,bCoord2);

					testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
					testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;

					tanValue1 = bCoord1[0]*cCoord[0]+bCoord1[1]*cCoord[1];
					tanValue2 = bCoord2[0]*cCoord[0]+bCoord2[1]*cCoord[1];

					tanCost1 = fabs(tanValue1-(cylinderR*cylinderR));
					tanCost2 = fabs(tanValue2 - (cylinderR*cylinderR));

					d1 = ComputeDistanceOfLm2(m_StartWrapLocal,viaWrapLocal,aCoord,bCoord1,cCoord,rm);
					d2 = ComputeDistanceOfLm2(m_StartWrapLocal,viaWrapLocal,aCoord,bCoord2,cCoord,rm);
					if (fabs((d2-d1)/d2)<0.01 )
					{
						if (tanCost1<tanCost2)
						{
							ibflag = 1;
						}else{
							ibflag = 2;
						}
					}else{
						if (d1<d2)
						{
							ibflag = 1;
						}else{
							ibflag = 2;
						}

					}

					if (ibflag ==1)
					{
						bCoord[0]=bCoord1[0];bCoord[1]=bCoord1[1];bCoord[2]=bCoord1[2];
					}else{

						bCoord[0]=bCoord2[0];bCoord[1]=bCoord2[1];bCoord[2]=bCoord2[2];
					}

					testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
					testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;
				}
				testValue = TestPlane(aCoord,bCoord,m_StartWrapLocal,cCoord1);
				testValue = TestPlane(aCoord,bCoord,m_StartWrapLocal,cCoord2);
			}//end of for

			testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
			testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;

			OAlength = sqrt(vtkMath::Distance2BetweenPoints(aCoord,m_StartWrapLocal));
			//ABcurve = getCutPlane2(aGcoord,bGcoord,mGcoord,clipData);//AB curve tmp

			ABcurve = ComputeCurveLength(aCoord,bCoord,rm);
			BClength = sqrt(vtkMath::Distance2BetweenPoints(bCoord,cCoord));
			CIcurve = CaculateHelix2(NULL,cCoord,viaWrapLocal,false);//@todo weih
			tmpLm = OAlength + ABcurve + BClength + CIcurve;

			if (idx==0 || (tmpLm<LmFinal) )//first time
			{
				if (!(cCoord[2]<m_EndWrapLocal[2])){
					idx++;
					angleFinal = j;
					LmFinal = tmpLm;

					aCoordFinal[0] = aCoord[0];aCoordFinal[1]=aCoord[1];aCoordFinal[2]=aCoord[2];
					bCoordFinal[0] = bCoord[0];bCoordFinal[1]=bCoord[1];bCoordFinal[2]=bCoord[2];
					cCoordFinal[0] = cCoord[0];cCoordFinal[1]=cCoord[1];cCoordFinal[2]=cCoord[2];
					mCoordFinal[0] = mCoord[0];mCoordFinal[1]=mCoord[1];mCoordFinal[2]=mCoord[2];
					rmFinal = rm;

					outputFile << "for angle :"<<j<< ", length for whole path is :   " <<LmFinal<<"  ."<< '\t'<<'\n';
					//outputFile2<<"  "<<j<<"  "<<tmpLm<<std::endl;
				}
			}

			vtkDEL(clipData);
			vtkDEL(clipDataCI);

		}
	}
	//--------------single path-------------
	if(idx>0){

		vtkNEW(OA);
		vtkNEW(BC);
		vtkNEW(clipData);
		vtkNEW(clipDataCI);
		vtkNEW(hcurve);

		GetWrapGlobalTransform(aCoordFinal,aGcoord);
		GetWrapGlobalTransform(bCoordFinal,bGcoord);
		GetWrapGlobalTransform(cCoordFinal,cGcoord);
		GetWrapGlobalTransform(mCoordFinal,mGcoord);


		ABcurve = GetCutPlane2(aGcoord,bGcoord,mGcoord,clipData);//AB curve
		clipData->Update();

		TransformOutputClipData(clipData->GetOutput());
		m_Goniometer->AddInput(clipData->GetOutput());
		clipData->SetOutput(NULL);

		TransformOutputPoint(m_StartPoint);
		TransformOutputPoint(aGcoord);
		TransformOutputPoint(bGcoord);
		TransformOutputPoint(cGcoord);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(aGcoord,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(bGcoord,m_ExportPointList[m_ExportPointList.size()-1]);

		m_ExportPointList.push_back(new double[3]);
		CopyPointValue(cGcoord,m_ExportPointList[m_ExportPointList.size()-1]);

		OA->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);//OA
		OA->SetPoint2(aGcoord[0],aGcoord[1],aGcoord[2]);

		BC->SetPoint1(bGcoord[0],bGcoord[1],bGcoord[2]);
		BC->SetPoint2(cGcoord[0],cGcoord[1],cGcoord[2]);

		CIcurve = CaculateHelix2(hcurve,cCoordFinal,viaWrapLocal,true);//do transform inside
		TransformOutput(hcurve);


		m_Goniometer->AddInput(OA->GetOutput());
		m_Goniometer->AddInput(clipData->GetOutput());
		m_Goniometer->AddInput(BC->GetOutput());

		m_Goniometer->AddInput(hcurve);//m_Goniometer->AddInput(clipDataCI->GetOutput());


		vtkDEL(OA);
		vtkDEL(BC);
		vtkDEL(hcurve);
		vtkDEL(clipData);
		vtkDEL(clipDataCI);

		outputFile.close();

		m_Distance = LmFinal;
		angleFinal = angleFinal *180.0 /GetPi();

		finishTime = clock();
		totalTime = (double) (finishTime - startTime) / CLOCKS_PER_SEC;

		char lmBuffer[50];   
		char timeBuffer[50];
		_gcvt( LmFinal, 20, lmBuffer ); 
		_gcvt(totalTime,20,timeBuffer);


		outputFile2<< "   step:"<<step<<"                angle:"<<angleFinal<<"               length:"<<lmBuffer<<"                            needs:"<<timeBuffer<<"seconds "<<std::endl;
		outputFile2.close();


		InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
		GetWrappedMeterOutput()->Update(); 

	}else{
		DirectConnectSE();
	}



}

double albaVMEComputeWrapping::GetFilterFlag(double *filterPlaneNormal){
	double viaProjectWrapLocal[3],viaProject90WrapLocal[3];//viaPoint with sphere z coord
	double cylinderCenterWrapLocal[3],filterVector1[3],startWrapLocal[3];
	double rtn = 0;
	cylinderCenterWrapLocal[0] = 0;cylinderCenterWrapLocal[1] = 0;cylinderCenterWrapLocal[2] = 0;
	CopyPointValue(m_ViaWrapLocal,viaProjectWrapLocal);
	viaProjectWrapLocal[2] = m_SphereWrapLocal[2];
	Get90DegreePointOnCylinder(viaProjectWrapLocal,m_SphereWrapLocal,viaProject90WrapLocal);
	GetWrapLocalTransform(m_StartPoint,startWrapLocal);
	vtkPlaneSource *planeSource;
	vtkNEW(planeSource);
	planeSource->SetOrigin(viaProject90WrapLocal);
	planeSource->SetPoint1(m_SphereWrapLocal);
	planeSource->SetPoint2(startWrapLocal);
	planeSource->GetNormal(filterPlaneNormal);

	filterVector1[0] = m_ViaWrapLocal[0] - m_SphereWrapLocal[0];
	filterVector1[1] = m_ViaWrapLocal[1] - m_SphereWrapLocal[1];
	filterVector1[2] = 0;//m_SphereWrapLocal[2]-m_SphereWrapLocal[2] use viapoint but z value with same value to sphere center

	rtn = vtkMath::Dot(filterPlaneNormal,filterVector1);

	return rtn;

}

bool albaVMEComputeWrapping::IsLineInterSectObject(albaVME *wrapVME,double *point1,double *point2){
	double startPoint[3],endPoint[3];

	bool rtn = false;

	vtkALBASmartPointer<vtkPoints> temporaryIntersection;
	vtkALBASmartPointer<vtkPoints> pointsIntersection1;
	vtkALBASmartPointer<vtkPoints> pointsIntersection2;

	// create ordered list of tangent point (2) real algorithm
	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();
	m_TmpTransform->TransformPoint(point1, startPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
	m_TmpTransform->TransformPoint(point2, endPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()


	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
	vtkALBASmartPointer<vtkOBBTree> locator;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;

	transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapVME->GetAbsMatrixPipe()->GetVTKTransform());
	transformFirstDataInput->SetInput((vtkPolyData *)wrapVME->GetOutput()->GetVTKData());
	transformFirstDataInput->Update();

	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
	transformFirstData->Update(); 

	//-------test intersect sphere---------------------
	//vtkALBASmartPointer<vtkOBBTree> locator;
	locator->SetDataSet(transformFirstData->GetOutput());//SPHERE
	locator->SetGlobalWarningDisplay(0);
	locator->BuildLocator();

	if(locator->InsideOrOutside(startPoint) > 0 &&  locator->InsideOrOutside(endPoint) > 0) //both outside
	{
		locator->IntersectWithLine(startPoint, endPoint, temporaryIntersection, NULL);	
		int numSphere = temporaryIntersection->GetNumberOfPoints();
		if (numSphere>0)
		{
			rtn = true;
			//vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
		}
	}else if (locator->InsideOrOutside(startPoint) < 0 || locator->InsideOrOutside(endPoint) < 0)//one inside or both
	{
		rtn = true;
	}
	return rtn;


}
//if correct return true
bool albaVMEComputeWrapping::CheckTwoRadius(){
	bool rtn = true;
	double cylinderRadius ;
	double sphereRadius;
	int type1,type2;
	if (m_WrappedMode1 == SPHERE_CYLINDER)
	{
		albaVME *wrapped_vme1 = GetWrappedVME1();
		albaVME *wrapped_vme2 = GetWrappedVME2();

		albaVMESurfaceParametric *surface1 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme1);
		albaVMESurfaceParametric *surface2 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme2);

		type1 = surface1->GetGeometryType();
		type2 = surface2->GetGeometryType();

		if (type1 == albaVMESurfaceParametric::PARAMETRIC_SPHERE && type2 == albaVMESurfaceParametric::PARAMETRIC_CYLINDER){
			cylinderRadius = GetCylinderRadius();
			sphereRadius = GetSphereRadius();
			if (sphereRadius <= cylinderRadius)
			{
				rtn = false;
			}

		}


	}
	return rtn;
}
//if correct return true
bool albaVMEComputeWrapping::CheckAlign(){
	bool rtn =false;
	vtkLineSource *cAxis,*centerline;
	vtkNEW(cAxis);
	vtkNEW(centerline);
	double cylinderCenter[3];
	double sphereCenter[3];
	double sphereCylinderVector[3],axisVector[3];
	double axis[3];
	CopyPointValue(m_CylinderAxis2,axis);

	GetGlobalCylinderCenter(cylinderCenter);
	GetGlobalSphereCenter(sphereCenter);

	sphereCylinderVector[0] = sphereCenter[0]-cylinderCenter[0];
	sphereCylinderVector[1] = sphereCenter[1]-cylinderCenter[1];
	sphereCylinderVector[2] = sphereCenter[2]-cylinderCenter[2];

	axisVector[0] = axis[0] - cylinderCenter[0];
	axisVector[1] = axis[1] - cylinderCenter[1];
	axisVector[2] = axis[2] - cylinderCenter[2];

	vtkMath::Normalize(sphereCylinderVector);
	vtkMath::Normalize(axisVector);
	double value = vtkMath::Dot(sphereCylinderVector,axisVector);
	double d = vtkMath::Distance2BetweenPoints(sphereCenter,cylinderCenter);

	if (m_WrappedMode1 == SPHERE_CYLINDER)
	{
		if (  fabs(d-0.0)<0.000001)
		{
			if (value<0.000001)
			{
				rtn =true;
			}
		}
		else if ( fabs(fabs(value) - 1.0) < GetCylinderRadius()/1000)
		{
			rtn = true;

		}
	}else{

		rtn = true;
	}

	//-------------------test code---------------------------
	/*cAxis->SetPoint1(cylinderCenter[0],cylinderCenter[1],cylinderCenter[2]);
	cAxis->SetPoint2(m_CylinderAxis2[0],m_CylinderAxis2[1],m_CylinderAxis2[2]);
	centerline->SetPoint1(cylinderCenter[0],cylinderCenter[1],cylinderCenter[2]);
	centerline->SetPoint2(sphereCenter[0],sphereCenter[1],sphereCenter[2]);

	m_Goniometer->AddInput(cAxis->GetOutput());
	m_Goniometer->AddInput(centerline->GetOutput());*/
	//-------------------test over---------------------------
	vtkDEL(cAxis);
	vtkDEL(centerline);

	return rtn;

}
int albaVMEComputeWrapping::GetViaPoint(double *viaPoint,bool isNearEndflag){
	double startPoint[3],endPoint[3];
	double tmpIntersect1[3],tmpIntersect2[3];
	double d1,d2;
	int rtn = NON_WRAP;
	bool objFlag1 = false;
	bool objFlag2 = false;
	bool objInsideFlag1 = false;
	bool objInsideFlag2 = false;
	bool troughFlag = true;

	int insideFlag1,insideFlag2;
	int numCylinder =0,numSphere=0;

	albaVME *wrapped_vme1 = GetWrappedVME1();
	albaVME *wrapped_vme2 = GetWrappedVME2();

	albaVMESurfaceParametric *surface1 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme1);
	albaVMESurfaceParametric *surface2 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme2);
	int type1,type2;

	double endWrapLocal[3],startWrapLocal[3],startWrapGlobal[3];

	bool nearFlagEnd,nearFlagStart;
	if (m_WrappedMode1 == SPHERE_CYLINDER || m_WrappedMode1 == SINGLE_CYLINDER || m_WrappedMode1 == CYLINDER_CYLINDER)
	{
		GetWrapLocalTransform(m_EndPoint,endWrapLocal,2);//relative to second vme
		GetWrapLocalTransform(m_StartPoint,startWrapLocal,2);

		//m_Tolerance = GetCylinderRadius()/4.0; // choose a suitable value
		nearFlagEnd = IsEndPonintOnCylinder(m_Tolerance,endWrapLocal);
		nearFlagStart = IsEndPonintOnCylinder(m_Tolerance,startWrapLocal);

		if (nearFlagEnd)
		{
			GetWrapGlobalTransform(endWrapLocal,m_EndPoint,2);//change end Point,adjusted value
		}
		if (nearFlagStart)
		{
			GetWrapGlobalTransform(startWrapLocal,m_StartPoint,2);//change end Point
		}

	}

	vtkALBASmartPointer<vtkPoints> temporaryIntersection;
	vtkALBASmartPointer<vtkPoints> pointsIntersection1;
	vtkALBASmartPointer<vtkPoints> pointsIntersection2;

	// create ordered list of tangent point (2) real algorithm
	//----------------------important-------------------------------------
	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();
	m_TmpTransform->TransformPoint(m_StartPoint, startPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
	m_TmpTransform->TransformPoint(m_EndPoint, endPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()


	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
	vtkALBASmartPointer<vtkOBBTree> locator;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;

	if (wrapped_vme1)
	{
		transformFirstDataInput->SetTransform((vtkAbstractTransform *)GetWrappedVME1()->GetAbsMatrixPipe()->GetVTKTransform());
		transformFirstDataInput->SetInput((vtkPolyData *)GetWrappedVME1()->GetOutput()->GetVTKData());
		transformFirstDataInput->Update();

		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
		transformFirstData->Update(); 

		//-------test intersect sphere---------------------
		//vtkALBASmartPointer<vtkOBBTree> locator;
		locator->SetDataSet(transformFirstData->GetOutput());//SPHERE
		locator->SetGlobalWarningDisplay(0);
		locator->BuildLocator();

		insideFlag1 = locator->InsideOrOutside(startPoint);
		insideFlag2 =  locator->InsideOrOutside(endPoint);
		if(insideFlag1>0 &&  insideFlag2 > 0) //SPHERE
		{
			locator->IntersectWithLine(startPoint, endPoint, temporaryIntersection, NULL);	
			numSphere = temporaryIntersection->GetNumberOfPoints();
			if (numSphere>0)
			{
				objFlag1 = true;
				//vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
			}
		}else if (insideFlag1<0 || insideFlag2<0)
		{
			objInsideFlag1 = true;
		}
	}
	//---------test intersect cylinder-------------------

	if (wrapped_vme2)
	{
		transformFirstDataInput->SetTransform((vtkAbstractTransform *)GetWrappedVME2()->GetAbsMatrixPipe()->GetVTKTransform());
		transformFirstDataInput->SetInput((vtkPolyData *)GetWrappedVME2()->GetOutput()->GetVTKData());
		transformFirstDataInput->Update();

		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
		transformFirstData->Update(); 

		locator->SetDataSet(transformFirstData->GetOutput());
		locator->SetGlobalWarningDisplay(0);
		locator->BuildLocator();

		insideFlag1 = locator->InsideOrOutside(startPoint);
		insideFlag2 =  locator->InsideOrOutside(endPoint);
		//Control if Start or End point is inside vtk data (surface)
		if(insideFlag1 > 0 &&  insideFlag2 > 0) 
		{
			locator->IntersectWithLine(startPoint, endPoint, temporaryIntersection, NULL);	
			numCylinder = temporaryIntersection->GetNumberOfPoints();
			if (numCylinder>0)
			{			
				temporaryIntersection->GetPoint(0,tmpIntersect1);
				temporaryIntersection->GetPoint(1,tmpIntersect2);
				d1 = vtkMath::Distance2BetweenPoints(endPoint,tmpIntersect1);
				d2 = vtkMath::Distance2BetweenPoints(endPoint,tmpIntersect2);
				objFlag2 = true;
			}
			double x,y,r;
			double tmpIntersectLocal1[3],tmpIntersectLocal2[3];
			//x = endWrapLocal[0];//note:here
			//y = endWrapLocal[1];

			GetWrapLocalTransform(tmpIntersect1,tmpIntersectLocal1);
			GetWrapLocalTransform(tmpIntersect2,tmpIntersectLocal2);

			x = tmpIntersectLocal1[0];
			y = tmpIntersectLocal1[1];

			r = GetCylinderRadius();
			if ( fabs(x*x +y*y - r*r)<m_Tolerance)
			{
				//x = startWrapLocal[0];
				//y = startWrapLocal[1];
				x = tmpIntersectLocal2[0];
				y = tmpIntersectLocal2[1];

				if ( fabs(x*x +y*y - r*r)<m_Tolerance)
				{
					troughFlag = false;
				}
			}



		}else if (insideFlag2<0 || insideFlag1<0)
		{
			objInsideFlag2 = true;

		}
	}

	//choose SPHERE_CYLINDER and first object is sphere,second object is cylinder 
	if (m_WrappedMode1 == SPHERE_CYLINDER )
	{

		type1 = surface1->GetGeometryType();
		type2 = surface2->GetGeometryType();

		if (type1 == albaVMESurfaceParametric::PARAMETRIC_SPHERE && type2 == albaVMESurfaceParametric::PARAMETRIC_CYLINDER)
		{

			if (objInsideFlag1 || objInsideFlag2)//start or end inside object
			{
				rtn = INSIDE_FLAG;
			}else{
				if (objFlag1 )//intersect sphere
				{
					if (nearFlagEnd || nearFlagStart)//very near cylinder
					{
						if (nearFlagStart)//exchange start and end
						{
							GetWrapGlobalTransform(startWrapLocal,startWrapGlobal);//change start end Point
							CopyPointValue(m_EndPoint,m_StartPoint);
							CopyPointValue(startWrapGlobal,m_EndPoint);
						}else if (nearFlagEnd)
						{
							GetWrapGlobalTransform(endWrapLocal,m_EndPoint);//change end Point
						}
						rtn = WRAP_SPHERE_CYLINDER;
					}else{
						rtn = SINGLE_SPHERE;

					}
				}else if (objFlag2)
				{
					rtn = SINGLE_CYLINDER;
				}else{
					rtn = NON_WRAP;
				}

			}
		}
	}else if (m_WrappedMode1 == SINGLE_SPHERE )
	{

		type1 = surface1->GetGeometryType();
		if (type1 ==  albaVMESurfaceParametric::PARAMETRIC_SPHERE)
		{
			if (objInsideFlag1 )//start or end inside sphere
			{
				rtn = INSIDE_FLAG;
			}else{
				if (objFlag1)
				{
					rtn = SINGLE_SPHERE2;
				}

			}
		}

	}else if ( m_WrappedMode1 == SINGLE_CYLINDER )
	{
		type2 = surface2->GetGeometryType();
		if (type2 ==  albaVMESurfaceParametric::PARAMETRIC_CYLINDER)
		{
			if (objInsideFlag2)//start or end inside cylinder
			{
				rtn = INSIDE_FLAG;
			}else{
				if (objFlag2 && !troughFlag)//intersect point on the surface
				{
					rtn = SINGLE_CYLINDER;
				}
			}
		}
	}else if (m_WrappedMode1 == DOUBLE_CYLINDER)
	{
		type1 = surface1->GetGeometryType();
		type2 = surface2->GetGeometryType();
		if (type1 ==albaVMESurfaceParametric::PARAMETRIC_CYLINDER && type2 ==albaVMESurfaceParametric::PARAMETRIC_CYLINDER)
		{
			if (objFlag1 && objFlag2)
			{
				rtn = CYLINDER_CYLINDER;
			}else if (objFlag1 )
			{
				rtn = CYLINDER_CYLINDER_1;
			}else if (objFlag2)
			{
				rtn = CYLINDER_CYLINDER_2;
			}
			else{
				rtn = NON_WRAP;
			}

		}
	}


	if (isNearEndflag)//near end point
	{
		if(d1<d2 ){
			CopyPointValue(tmpIntersect1,viaPoint);
		}else{
			CopyPointValue(tmpIntersect2,viaPoint);
		}
	}else{//near start point

		if (d1>d2)
		{
			CopyPointValue(tmpIntersect1,viaPoint);
		}else{
			CopyPointValue(tmpIntersect2,viaPoint);
		}
	}

	//single cylinder,double cylinder,sphere-cylinder


	return rtn;

}


void albaVMEComputeWrapping::Get90DegreePointOnCylinder(double *firstPoint,double *sphereCenter,double *secondPoint){
	double Px,Py,Cx,Cy,r;
	double A,B,C,a,b,c;
	r = GetCylinderRadius();
	//X*X + Y*Y = r*r;
	//(X-Cx)2 +(Y-Cy)2 +(Px-Cx)2+(Py-Cy)2=(X-Px)2 +(Y-Py)2;
	Px = firstPoint[0];
	Py = firstPoint[1];
	Cx = sphereCenter[0];
	Cy = sphereCenter[1];

	A = (Px-Cx);
	B = (Py-Cy);
	C = (Cx*Cx + Cy*Cy - Px*Cx -Py*Cy);
	a = (B*B + A*A);
	b = (2*C*A);
	c = (C*C-B*B*r*r);
	//although we can get two point, we only use one
	secondPoint[0] = ( -b+sqrt(b*b - 4*a*c) ) / (2*a);
	secondPoint[1] = -( C + A *secondPoint[0]) / B;
	secondPoint[2] = firstPoint[2];
}

double albaVMEComputeWrapping::GetCutPlaneForCylinder(double *center,double *t1,double *t2,albaVME *wrapped_vme,vtkClipPolyData *clipData){

	double rtn = 0;

	double vCi[3];
	double normal[3];
	m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData());

	transformFirstData->Update(); 

	vtkPlaneSource *planeSource;
	vtkPlane *planeCutter;
	vtkCutter *cutter;
	vtkPlane *planeClip;

	vtkNEW(planeSource);
	vtkNEW(planeCutter);
	vtkNEW(cutter);
	vtkNEW(planeClip);

	planeSource->SetOrigin(center);
	planeSource->SetPoint1(t1);
	planeSource->SetPoint2(t2);

	planeCutter->SetOrigin(center);
	planeCutter->SetNormal(planeSource->GetNormal());

	cutter->SetInput(transformFirstData->GetOutput());
	cutter->SetCutFunction(planeCutter);


	vCi[0] = t1[0]-t2[0];
	vCi[1] = t1[1]-t2[1];
	vCi[2] = t1[2]-t2[2];

	vtkMath::Cross(vCi,planeSource->GetNormal(),normal);
	if( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0) return 0; // midpoint and center are the same point

	vtkMath::Normalize(normal);
	normal[0]=-normal[0];
	normal[1]=-normal[1];
	normal[2]=-normal[2];

	planeClip->SetOrigin(t1);//midPoint
	planeClip->SetNormal(normal);//normal

	clipData->SetInput(cutter->GetOutput());
	clipData->SetClipFunction(planeClip);
	clipData->Update();

	double numberOfCells = clipData->GetOutput()->GetNumberOfCells();
	for(int i=0; i<numberOfCells; i++)
	{
		rtn += sqrt(clipData->GetOutput()->GetCell(i)->GetLength2());
	}

	vtkDEL(planeSource);
	vtkDEL(planeCutter);
	vtkDEL(cutter);
	vtkDEL(planeClip);
	return rtn;
}


//get ellipse
double albaVMEComputeWrapping::GetCutPlaneForCI(double *bCoord,double *cCoord,vtkClipPolyData *clipData){

	double rtn = 0;

	double vCi[3];
	double normal[3];
	m_TmpTransform->SetMatrix(*GetWrappedVME2()->GetOutput()->GetAbsMatrix());

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)GetWrappedVME2()->GetOutput()->GetVTKData());
	transformFirstData->Update(); 

	vtkPlaneSource *planeSource;
	vtkPlane *planeCutter;
	vtkCutter *cutter;
	vtkPlane *planeClip;

	vtkNEW(planeSource);
	vtkNEW(planeCutter);
	vtkNEW(cutter);
	vtkNEW(planeClip);

	planeSource->SetOrigin(bCoord);
	planeSource->SetPoint1(cCoord);
	planeSource->SetPoint2(m_ViaPoint);

	planeCutter->SetOrigin(m_ViaPoint);
	planeCutter->SetNormal(planeSource->GetNormal());

	cutter->SetInput(transformFirstData->GetOutput());
	cutter->SetCutFunction(planeCutter);


	vCi[0] = cCoord[0]-m_ViaPoint[0];
	vCi[1] = cCoord[1]-m_ViaPoint[1];
	vCi[2] = cCoord[2]-m_ViaPoint[2];

	vtkMath::Cross(vCi,planeSource->GetNormal(),normal);
	if( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0) return 0; // midpoint and center are the same point

	vtkMath::Normalize(normal);
	normal[0]=-normal[0];
	normal[1]=-normal[1];
	normal[2]=-normal[2];

	planeClip->SetOrigin(cCoord);//midPoint
	planeClip->SetNormal(normal);//normal

	clipData->SetInput(cutter->GetOutput());
	clipData->SetClipFunction(planeClip);
	clipData->Update();

	double numberOfCells = clipData->GetOutput()->GetNumberOfCells();
	for(int i=0; i<numberOfCells; i++)
	{
		rtn += sqrt(clipData->GetOutput()->GetCell(i)->GetLength2());
	}

	vtkDEL(planeSource);
	vtkDEL(planeCutter);
	vtkDEL(cutter);
	vtkDEL(planeClip);
	return rtn;
}


double albaVMEComputeWrapping::GetCutPlane2(double *aPoint,double *bPoint,double *mPoint,vtkClipPolyData *clipData){

	double rtn = 0;
	double midPoint[3];

	m_TmpTransform2->SetMatrix(*GetWrappedVME1()->GetOutput()->GetAbsMatrix());
	//m_TmpTransform2->Invert();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform2->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)GetWrappedVME1()->GetOutput()->GetVTKData());
	transformFirstData->Update(); 
	vtkPlaneSource *planeSource;
	vtkPlane *planeCutter;
	vtkCutter *cutter;
	vtkPlane *planeClip;

	vtkNEW(planeSource);
	vtkNEW(planeCutter);
	vtkNEW(cutter);
	vtkNEW(planeClip);

	planeSource->SetOrigin(mPoint);
	planeSource->SetPoint1(aPoint);
	planeSource->SetPoint2(bPoint);

	planeCutter->SetOrigin(mPoint);
	planeCutter->SetNormal(planeSource->GetNormal());

	cutter->SetInput(transformFirstData->GetOutput());
	cutter->SetCutFunction(planeCutter);


	midPoint[0] = (bPoint[0] + aPoint[0])/2;
	midPoint[1] = (bPoint[1] + aPoint[1])/2;
	midPoint[2] = (bPoint[2] + aPoint[2])/2;

	double normal[3];
	normal[0] = midPoint[0] - mPoint[0];
	normal[1] = midPoint[1] - mPoint[1];
	normal[2] = midPoint[2] - mPoint[2];

	if( normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0) return 0; // midpoint and center are the same point

	vtkMath::Normalize(normal);

	planeClip->SetOrigin(midPoint);//midPoint
	planeClip->SetNormal(normal);//normal

	clipData->SetInput(cutter->GetOutput());
	clipData->SetClipFunction(planeClip);
	clipData->Update();
	//outCurve = m_Clip->GetOutput();
	double clipLength = 0;
	double numberOfCells = clipData->GetOutput()->GetNumberOfCells();
	for(int i=0; i<numberOfCells; i++)
	{
		clipLength += sqrt(clipData->GetOutput()->GetCell(i)->GetLength2());
	}

	rtn = clipLength;

	vtkDEL(planeSource);
	vtkDEL(planeCutter);
	vtkDEL(cutter);
	vtkDEL(planeClip);

	return rtn;

}



//equation (14)
double albaVMEComputeWrapping::ComputeDistanceOfLm(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *rCoord,double rm){
	//Lm = OA + ABcurve +BI
	double OA,AB,BI,ABcurve,tmp,rtn=0,angle ;
	//a dot b = |a|*|b|cosA
	double ao[3],bo[3];
	ao[0] = aCoord[0] - rCoord[0];
	ao[1] = aCoord[1] - rCoord[1];
	ao[2] = aCoord[2] - rCoord[2];

	bo[0] = bCoord[0] - rCoord[0];
	bo[1] = bCoord[1] - rCoord[1];
	bo[2] = bCoord[2] - rCoord[2];

	vtkMath::Normalize(ao);
	vtkMath::Normalize(bo);
	tmp = vtkMath::Dot(ao,bo);

	angle = acos(tmp / (rm*rm));

	OA = sqrt(vtkMath::Distance2BetweenPoints(oCoord,aCoord));
	AB = sqrt(vtkMath::Distance2BetweenPoints(aCoord,bCoord));

	ABcurve = angle * rm;
	BI = sqrt(vtkMath::Distance2BetweenPoints(bCoord,iCoord));

	//rtn = OA+AB+BI;
	m_AbCurve = ABcurve;
	rtn = OA+ABcurve+BI;

	return rtn;
}
//Lm = OA + ABcurve +BC + CI
double albaVMEComputeWrapping::ComputeDistanceOfLm2(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double rm){

	double OA,AB2,BC,CI,rtn=0 ;

	OA = sqrt(vtkMath::Distance2BetweenPoints(oCoord,aCoord));
	AB2 = vtkMath::Distance2BetweenPoints(aCoord,bCoord);

	BC = sqrt(vtkMath::Distance2BetweenPoints(bCoord,cCoord));
	CI = sqrt(vtkMath::Distance2BetweenPoints(cCoord,iCoord));

	rtn = OA+ m_AbCurve+ BC +CI;

	return rtn;
}

void albaVMEComputeWrapping::GetLocalCenterCoord(double *localCoord,double *globalCoord){
	localCoord[0] = globalCoord[0] - m_SphereWrapLocal[0];
	localCoord[1] = globalCoord[1] - m_SphereWrapLocal[1];
	localCoord[2] = globalCoord[2] - m_SphereWrapLocal[2];

}
void albaVMEComputeWrapping::GetGlobalCenterCoord(double *globalCoord,double *localCoord){

	globalCoord[0] = localCoord[0] + m_SphereWrapLocal[0];
	globalCoord[1] = localCoord[1] + m_SphereWrapLocal[1];
	globalCoord[2] = localCoord[2] + m_SphereWrapLocal[2];

}
void albaVMEComputeWrapping::GetGlobalSphereCenter(double *sphCoord){
	sphCoord[0] = m_WrappedVMECenter1[0];
	sphCoord[1] = m_WrappedVMECenter1[1];
	sphCoord[2] = m_WrappedVMECenter1[2];
}
void albaVMEComputeWrapping::GetGlobalCylinderCenter(double *cylCoord){

	/*cylCoord[0] = m_WrappedVMECenter2[0];
	cylCoord[1] = m_WrappedVMECenter2[1];
	cylCoord[2] = m_WrappedVMECenter2[2];*/
	GetGlobalCylinderCenter(cylCoord,2);
}
void albaVMEComputeWrapping::GetGlobalCylinderCenter(double *cylCoord,int objIdx){
	if (objIdx == 2)
	{
		CopyPointValue(m_WrappedVMECenter2,cylCoord);
	}else{
		CopyPointValue(m_WrappedVMECenter1,cylCoord);
	}
}
// |x''|    -1 |x-Xr |
// |y''| = T   |y-Yr |            equation(8)
// |z''|       |z-Zr |
//from global get local
//imat is invert matrix
void albaVMEComputeWrapping::GetLocalTransformedCoord(double *localCoord,double *globalCoord,albaMatrix3x3 *imat){
	double ele0,ele1,ele2;
	for (int i=0;i<3;i++)
	{
		ele0 = imat->GetElement(i,0);
		ele1 = imat->GetElement(i,1);
		ele2 = imat->GetElement(i,2);
		localCoord[i] = ele0*(globalCoord[0]-m_SphereWrapLocal[0]) + ele1*(globalCoord[1]-m_SphereWrapLocal[1]) + ele2*(globalCoord[2]-m_SphereWrapLocal[2]);	
	}
}
//|x|     |x''|  |Xr|
//|y| = T |y''| +|Yr|
//|z|     |z''|  |Zr|
void albaVMEComputeWrapping::GetGlobalTransformedCoord(double *localCoord,double *globalCoord,albaMatrix3x3 *mat){
	double ele0,ele1,ele2;
	for (int i=0;i<3;i++)
	{
		ele0 = mat->GetElement(i,0);
		ele1 = mat->GetElement(i,1);
		ele2 = mat->GetElement(i,2);
		globalCoord[i] = ele0*localCoord[0] + ele1*localCoord[1] + ele2*localCoord[2] + m_SphereWrapLocal[i];
	}
}
void albaVMEComputeWrapping::GetTransFormMatrix(double cosA,double sinA,double cosB,double sinB,albaMatrix3x3 *mat){

	// i is row and j is column 
	mat->SetElement(0,0,cosB);
	mat->SetElement(1,0,-sinB);
	mat->SetElement(2,0,0);

	mat->SetElement(0,1,sinB*cosA);
	mat->SetElement(1,1,cosB*cosA);
	mat->SetElement(2,1,sinA);

	mat->SetElement(0,2,-sinB*sinA);
	mat->SetElement(1,2,-cosB*sinA);
	mat->SetElement(2,2,cosA);	
}
//by default objIdx means 2.it means second vme
void albaVMEComputeWrapping::GetWrapMatrix(albaMatrix &inMx,albaMatrix &inMy,int objIdx){
	double axisLocal[3],cylinderCenter[3],cylinderCenterLocal[3],normal1[3];
	double a,b,c,d;
	double cosA,sinA;

	if (m_WrappedMode1 == SPHERE_ONLY)
	{
		//-------rotation x axis-----
		inMx.SetElement(0,0,1);
		inMx.SetElement(0,1,0);
		inMx.SetElement(0,2,0);
		inMx.SetElement(0,3,0);

		inMx.SetElement(1,0,0);
		inMx.SetElement(1,1,1);
		inMx.SetElement(1,2,0);
		inMx.SetElement(1,3,0);

		inMx.SetElement(2,0,0);
		inMx.SetElement(2,1,0);
		inMx.SetElement(2,2,1);
		inMx.SetElement(2,3,0);

		inMx.SetElement(3,0,0);
		inMx.SetElement(3,1,0);
		inMx.SetElement(3,2,0);
		inMx.SetElement(3,3,1);
		//-------rotation y axis-----
		inMy.SetElement(0,0,1);
		inMy.SetElement(0,1,0);
		inMy.SetElement(0,2,0);
		inMy.SetElement(0,3,0);

		inMy.SetElement(1,0,0);
		inMy.SetElement(1,1,1);
		inMy.SetElement(1,2,0);
		inMy.SetElement(1,3,0);

		inMy.SetElement(2,0,0);
		inMy.SetElement(2,1,0);
		inMy.SetElement(2,2,1);
		inMy.SetElement(2,3,0);

		inMy.SetElement(3,0,0);
		inMy.SetElement(3,1,0);
		inMy.SetElement(3,2,0);
		inMy.SetElement(3,3,1);


	}else{
		//step1 move to origin
		GetGlobalCylinderCenter(cylinderCenter,objIdx);
		if (objIdx==1)
		{
			axisLocal[0] = m_CylinderAxis1[0]-cylinderCenter[0];
			axisLocal[1] = m_CylinderAxis1[1]-cylinderCenter[1];
			axisLocal[2] = m_CylinderAxis1[2]-cylinderCenter[2];
		}else if (objIdx ==2)
		{
			axisLocal[0] = m_CylinderAxis2[0]-cylinderCenter[0];
			axisLocal[1] = m_CylinderAxis2[1]-cylinderCenter[1];
			axisLocal[2] = m_CylinderAxis2[2]-cylinderCenter[2];
		}

		cylinderCenterLocal[0] = cylinderCenter[0]-cylinderCenter[0];
		cylinderCenterLocal[1] = cylinderCenter[1]-cylinderCenter[1];
		cylinderCenterLocal[2] = cylinderCenter[2]-cylinderCenter[2];

		CopyPointValue(axisLocal,normal1);

		vtkMath::Normalize(normal1);
		a = normal1[0];b = normal1[1];c = normal1[2];
		d = sqrt(b*b + c*c);
		cosA = c/d;
		sinA = b/d;
		//-------rotation x axis-----
		inMx.SetElement(0,0,1);
		inMx.SetElement(0,1,0);
		inMx.SetElement(0,2,0);
		inMx.SetElement(0,3,0);

		inMx.SetElement(1,0,0);
		inMx.SetElement(1,1,c/d);
		inMx.SetElement(1,2,-b/d);
		inMx.SetElement(1,3,0);

		inMx.SetElement(2,0,0);
		inMx.SetElement(2,1,b/d);
		inMx.SetElement(2,2,c/d);
		inMx.SetElement(2,3,0);

		inMx.SetElement(3,0,0);
		inMx.SetElement(3,1,0);
		inMx.SetElement(3,2,0);
		inMx.SetElement(3,3,1);
		//-------rotation y axis-----
		inMy.SetElement(0,0,d);
		inMy.SetElement(0,1,0);
		inMy.SetElement(0,2,-a);
		inMy.SetElement(0,3,0);

		inMy.SetElement(1,0,0);
		inMy.SetElement(1,1,1);
		inMy.SetElement(1,2,0);
		inMy.SetElement(1,3,0);

		inMy.SetElement(2,0,a);
		inMy.SetElement(2,1,0);
		inMy.SetElement(2,2,d);
		inMy.SetElement(2,3,0);

		inMy.SetElement(3,0,0);
		inMy.SetElement(3,1,0);
		inMy.SetElement(3,2,0);
		inMy.SetElement(3,3,1);

	}
}
void albaVMEComputeWrapping::GetWrapMatrix(albaMatrix &inMx,albaMatrix &inMy){
	GetWrapMatrix(inMx,inMy,2);

}
void albaVMEComputeWrapping::GetWrapLocalTransform(double *inCoord,double *outCoord){
	GetWrapLocalTransform(inCoord,outCoord,2);
}
//transform coord from abs coord to a system defined by cylinder center and line (cylinder center and sphere center) as z axis
void albaVMEComputeWrapping::GetWrapLocalTransform(double *inCoord,double *outCoord,int objIdx){

	double inCoordLocal[3],cylinderCenter[3];

	double xInCoordLocal[3];

	GetGlobalCylinderCenter(cylinderCenter,objIdx);

	if (!GetWrappedVME2())
	{
		cylinderCenter[0]=0;
		cylinderCenter[1]=0;
		cylinderCenter[2]=0;
	}

	//step1 move to origin
	inCoordLocal[0] = inCoord[0]-cylinderCenter[0];
	inCoordLocal[1] = inCoord[1]-cylinderCenter[1];
	inCoordLocal[2] = inCoord[2]-cylinderCenter[2];

	if (objIdx==1)
	{
		//x` = Rx * T * X
		xInCoordLocal[0] = inCoordLocal[0];
		xInCoordLocal[1] = inCoordLocal[1] * m_InMx1.GetElement(1,1) + inCoordLocal[2]*m_InMx1.GetElement(1,2);
		xInCoordLocal[2] = inCoordLocal[1] * m_InMx1.GetElement(2,1) + inCoordLocal[2]*m_InMx1.GetElement(2,2);

		//x` = Ry * Rx * T * X
		outCoord[0] = m_InMy1.GetElement(0,0)*xInCoordLocal[0]+m_InMy1.GetElement(0,2)*xInCoordLocal[2];
		outCoord[1] = xInCoordLocal[1];
		outCoord[2] = m_InMy1.GetElement(2,0)*xInCoordLocal[0]+m_InMy1.GetElement(2,2)*xInCoordLocal[2];	
	}else if (objIdx ==2)
	{
		//x` = Rx * T * X
		xInCoordLocal[0] = inCoordLocal[0];
		xInCoordLocal[1] = inCoordLocal[1] * m_InMx2.GetElement(1,1) + inCoordLocal[2]*m_InMx2.GetElement(1,2);
		xInCoordLocal[2] = inCoordLocal[1] * m_InMx2.GetElement(2,1) + inCoordLocal[2]*m_InMx2.GetElement(2,2);

		//x` = Ry * Rx * T * X
		outCoord[0] = m_InMy2.GetElement(0,0)*xInCoordLocal[0]+m_InMy2.GetElement(0,2)*xInCoordLocal[2];
		outCoord[1] = xInCoordLocal[1];
		outCoord[2] = m_InMy2.GetElement(2,0)*xInCoordLocal[0]+m_InMy2.GetElement(2,2)*xInCoordLocal[2];
	}




}
//if start or end point near cylinder surface,project it on the cylinder
bool albaVMEComputeWrapping::IsEndPonintOnCylinder(double tolerance,double *endWrapLocal){
	//double endWrapLocal[3];
	bool rtn = false;
	double x,y,distX,distY;
	double newX[3],newY[3];	
	double testValue ;
	double r = GetCylinderRadius();
	double d1,d2;
	double sphereCenter[3],cylinderCenter[3],cylinderCenterLocal[3],sphereCenterLocal[3];

	testValue = endWrapLocal[0] *endWrapLocal[0] + endWrapLocal[1]*endWrapLocal[1];
	GetGlobalCylinderCenter(cylinderCenter);
	GetGlobalSphereCenter(sphereCenter);
	GetWrapLocalTransform(sphereCenter,sphereCenterLocal);
	GetWrapLocalTransform(cylinderCenter,cylinderCenterLocal);

	//GetWrapGlobalTransform(endWrapLocal,endWrapLocal);

	d1 = vtkMath::Distance2BetweenPoints(sphereCenterLocal,endWrapLocal);
	d2 = vtkMath::Distance2BetweenPoints(cylinderCenterLocal,endWrapLocal);

	if (   (fabs( sqrt(testValue ) - r) < tolerance ) && d2<d1)
	{

		//------try x---------
		x = sqrt(r*r - endWrapLocal[1]*endWrapLocal[1]);
		if ( endWrapLocal[0]<0)
		{
			x=-x;
		}
		newX[0]=x;
		newX[1]=endWrapLocal[1];
		newX[2]=endWrapLocal[2];
		//-------try y--------
		y = sqrt(r*r - endWrapLocal[0]*endWrapLocal[0]);
		if (endWrapLocal[1]<0)
		{
			y=-y;
		}
		newY[0]=endWrapLocal[0];
		newY[1]=y;
		newY[2]=endWrapLocal[2];
		//----------
		distX = sqrt(vtkMath::Distance2BetweenPoints(endWrapLocal,newX));
		distY = sqrt(vtkMath::Distance2BetweenPoints(endWrapLocal,newY));
		if(distX<distY){
			endWrapLocal[0]=x;
		}else{
			endWrapLocal[1]=y;
		}
		rtn = true;
	}
	return rtn;
}
void albaVMEComputeWrapping::GetWrapGlobalTransform(double *inCoord,double *outCoord,int objIdx){

	double cylinderCenter[3];
	double yInCoordLocal[3],xyInCoordLocal[3];

	GetGlobalCylinderCenter(cylinderCenter,objIdx);

	if (!GetWrappedVME2())
	{
		cylinderCenter[0]=0;
		cylinderCenter[1]=0;
		cylinderCenter[2]=0;
	}
	if (objIdx ==1)
	{
		//X = Ry(-1)*X
		//step1 move to origin
		yInCoordLocal[0] = inCoord[0]*m_OutMy1.GetElement(0,0) + inCoord[2]*m_OutMy1.GetElement(0,2);
		yInCoordLocal[1] = inCoord[1];
		yInCoordLocal[2] = inCoord[0]*m_OutMy1.GetElement(2,0) + inCoord[2]*m_OutMy1.GetElement(2,2);
		//X = Rx(-1)*Ry(-1)*X
		xyInCoordLocal[0] = yInCoordLocal[0];
		xyInCoordLocal[1] = yInCoordLocal[1]*m_OutMx1.GetElement(1,1) + yInCoordLocal[2]*m_OutMx1.GetElement(1,2);
		xyInCoordLocal[2] = yInCoordLocal[1]*m_OutMx1.GetElement(2,1) + yInCoordLocal[2]*m_OutMx1.GetElement(2,2);

		outCoord[0] = xyInCoordLocal[0] + cylinderCenter[0];
		outCoord[1] = xyInCoordLocal[1] + cylinderCenter[1];
		outCoord[2] = xyInCoordLocal[2] + cylinderCenter[2];
	}else if (objIdx ==2)
	{
		//X = Ry(-1)*X
		//step1 move to origin
		yInCoordLocal[0] = inCoord[0]*m_OutMy2.GetElement(0,0) + inCoord[2]*m_OutMy2.GetElement(0,2);
		yInCoordLocal[1] = inCoord[1];
		yInCoordLocal[2] = inCoord[0]*m_OutMy2.GetElement(2,0) + inCoord[2]*m_OutMy2.GetElement(2,2);
		//X = Rx(-1)*Ry(-1)*X
		xyInCoordLocal[0] = yInCoordLocal[0];
		xyInCoordLocal[1] = yInCoordLocal[1]*m_OutMx2.GetElement(1,1) + yInCoordLocal[2]*m_OutMx2.GetElement(1,2);
		xyInCoordLocal[2] = yInCoordLocal[1]*m_OutMx2.GetElement(2,1) + yInCoordLocal[2]*m_OutMx2.GetElement(2,2);

		outCoord[0] = xyInCoordLocal[0] + cylinderCenter[0];
		outCoord[1] = xyInCoordLocal[1] + cylinderCenter[1];
		outCoord[2] = xyInCoordLocal[2] + cylinderCenter[2];
	}
}
void albaVMEComputeWrapping::GetWrapGlobalTransform(double *inCoord,double *outCoord){
	GetWrapGlobalTransform(inCoord,outCoord,2);
}


void albaVMEComputeWrapping::GetWrapAngleAandB(double &cosA,double &sinA,double &cosB,double &sinB){
	double paraA[3],paraB[3],center[3],normal1[3],normal2[3];
	double cCcoord[3],sCcoord[3],orientation[3],sCcoord1[3],sCpcoord[3];
	double distance;
	//---------get angle a----------
	GetGlobalCylinderCenter(cCcoord);
	GetGlobalSphereCenter(sCcoord);
	GetWrappedVME2()->GetOutput()->GetAbsPose(cCcoord,orientation);//cylinder center
	GetWrappedVME1()->GetOutput()->GetAbsPose(sCcoord,orientation);//sphere center

	sCcoord1[0] = sCcoord[0]-cCcoord[0];//local coord
	sCcoord1[1] = sCcoord[1]-cCcoord[1];
	sCcoord1[2] = sCcoord[2]-cCcoord[2];
	normal1[0]=sCcoord1[0];normal1[1]=sCcoord1[1];normal1[2]=sCcoord1[2];

	paraA[0] = 0; paraA[1] = 0; paraA[2] = -1;
	paraB[0] = 0; paraB[1] = 1; paraB[2] = 0;
	center[0] = 0; center[1] = 0; center[2] = 0;
	//---------get angle a----------
	//a dot b = |a||b|cosA

	vtkMath::Normalize(normal1);
	distance = sqrt(vtkMath::Distance2BetweenPoints(center,sCcoord1));
	cosA = vtkMath::Dot(paraA,normal1) / (distance*1);
	sinA = sqrt(1 - cosA * cosA);	//@todo
	//---------get angle b----------
	sCpcoord[0] = sCcoord1[0];//projection poing in x'y'plane.
	sCpcoord[1] = sCcoord1[1];
	sCpcoord[2] = 0;
	normal2[0]=sCpcoord[0],normal2[1]=sCpcoord[1];normal2[2]=sCpcoord[2];

	distance = sqrt(vtkMath::Distance2BetweenPoints(center,sCpcoord));
	cosB = vtkMath::Dot(paraB,normal2) / (distance*1);
	sinB = sqrt(1 - cosB * cosB);
}

//these parameters as return parameters;
//A,B means angle in fig5
void albaVMEComputeWrapping::GetAngleAandB(double & cosA,double & sinA,double & cosB,double & sinB){

	double olCoord[3],olprojectCoord[3];
	double center[3];
	albaVME *start_vme = GetStartVME();
	double paraA[3],paraB[3];
	double distance;
	double sphereCenter[3];
	GetGlobalSphereCenter(sphereCenter);

	olCoord[0] =  m_StartPoint[0] -sphereCenter[0];//m_StartWrapLocal[0] - m_SphereWrapLocal[0];
	olCoord[1] =  m_StartPoint[1] -sphereCenter[1];//m_StartWrapLocal[1] - m_SphereWrapLocal[1];
	olCoord[2] =  m_StartPoint[2] -sphereCenter[2];//m_StartWrapLocal[2] - m_SphereWrapLocal[2];

	paraA[0] = 0; paraA[1] = 0; paraA[2] = -1;
	paraB[0] = 0; paraB[1] = 1; paraB[2] = 0;
	center[0] = 0; center[1] = 0; center[2] = 0;

	//---------get angle a----------
	//a dot b = |a||b|cosA
	distance = sqrt(vtkMath::Distance2BetweenPoints(center,olCoord));
	cosA = vtkMath::Dot(paraA,olCoord) / (distance*1);
	sinA = sqrt(1 - cosA * cosA);	//@todo
	//---------get angle b----------
	olprojectCoord[0] = olCoord[0];//projection poing in x'y'plane.
	olprojectCoord[1] = olCoord[1];
	olprojectCoord[2] = 0;
	distance = sqrt(vtkMath::Distance2BetweenPoints(center,olprojectCoord));
	cosB = vtkMath::Dot(paraB,olprojectCoord) / (distance*1);
	//sinB = -sqrt(1 - cosB * cosB);
	sinB = olprojectCoord[0]/distance;

	//------------@todo there are some cases that may affect cos value and sin value;
}
//olCoord is o'' coord point o local coordinate.
//x is a global and unique variable
//R is radius of sphere
bool albaVMEComputeWrapping::GetAcoordinateInLocal(double *Ap1,double *Ap2,double *olCoord,double R,double x){
	double A,B;
	double sinA1 ,sinA2,cosA1,cosA2;

	//A1 from 0 to pai
	A = olCoord[0]*cos(x)+ olCoord[1]*sin(x);
	B = olCoord[2];
	double sqrtTest = R*R*B*B - (B*B+A*A)*(R*R-A*A);
	if ( sqrtTest>0 || fabs(sqrtTest)<0.000001)
	{
		if (fabs(sqrtTest)<0.000001)
		{
			sqrtTest = 0;
		}

		sinA1 =	( R*B + sqrt(sqrtTest ) ) / (B*B+A*A);
		sinA2 = ( R*B - sqrt( sqrtTest) ) / (B*B+A*A);
		cosA1 = sqrt(1-sinA1*sinA1);
		cosA2 = -sqrt(1-sinA2*sinA2);


		Ap1[0] =R* cosA1 * cos(x);
		Ap1[1] =R* cosA1 * sin(x);
		Ap1[2] =R* sinA1;

		Ap2[0] =R* cosA2 * cos(x);
		Ap2[1] =R* cosA2 * sin(x);
		Ap2[2] =R* sinA2;

		//---------------test codes-------------
		double aD,aD12,aD13,aD22,aD23;
		double center[3],testValue1,testValue2;
		center[0] = 0;center[1]= 0;center[2]=0;

		aD = vtkMath::Distance2BetweenPoints(center,olCoord);
		aD12 = vtkMath::Distance2BetweenPoints(olCoord,Ap1);
		aD13 = vtkMath::Distance2BetweenPoints(center,Ap1);
		aD22 = vtkMath::Distance2BetweenPoints(olCoord,Ap2);
		aD23 = vtkMath::Distance2BetweenPoints(center,Ap2);
		testValue1 = fabs(aD12 + aD13 - aD);
		testValue2 = fabs(aD22 + aD23 - aD);
		if (testValue1>0.01 && testValue2>0.01)
		{
			return false;
		}
		//---------------test codes-------------
		return true;
	}else{
		return false;
	}
}

//apppendix D
void albaVMEComputeWrapping::GetIntersectCircle(double *Ap,double *oCoord,double *iCoord,double *mCoord,double & Rm){
	//ux +vy +wz +t = 0;
	double u,v,w,t,tmp ;
	double mgCoord[3];
	double agCoord[3];
	double testValue;

	GetUVWT(Ap,oCoord,iCoord,u,v,w,t);//plane

	tmp = (-t)/( u*u + v*v + w*w ) ;
	mCoord[0] = u * tmp;
	mCoord[1] = v * tmp;
	mCoord[2] = w * tmp;
	testValue = TestPlane(Ap,oCoord,iCoord,mCoord);
	testValue = TestPlane(Ap,oCoord,iCoord,Ap);
	//------------------------
	GetGlobalCenterCoord(mgCoord,mCoord);
	m_Mpoint[0] = mgCoord[0];m_Mpoint[1] = mgCoord[1];m_Mpoint[2] = mgCoord[2];
	GetGlobalCenterCoord(agCoord,Ap);
	Rm = sqrt(vtkMath::Distance2BetweenPoints(agCoord,  mgCoord)) ;
	Rm = sqrt( (mgCoord[0]-agCoord[0])*(mgCoord[0]-agCoord[0]) + (mgCoord[1]-agCoord[1])*(mgCoord[1]-agCoord[1]) + (mgCoord[2]-agCoord[2])*(mgCoord[2]-agCoord[2]) );
	testValue = TestPlane(Ap,oCoord,iCoord,mCoord);
}

void albaVMEComputeWrapping::GetUVWT(double *Ap,double *oCoord,double *iCoord,double & u,double & v,double & w,double & t){
	double col1[3],col2[3],col3[3];
	//-------------------u----
	col1[0] = oCoord[1];  col2[0] = oCoord[2];  col3[0] = 1;
	col1[1] = iCoord[1];  col2[1] = iCoord[2];  col3[1] = 1;
	col1[2] = Ap[1];      col2[2] = Ap[2];      col3[2] = 1;

	u = DetValue(col1,col2,col3);
	//-------------------v----
	col1[0] = oCoord[0];  col2[0] = oCoord[2];  col3[0] = 1;
	col1[1] = iCoord[0];  col2[1] = iCoord[2];  col3[1] = 1;
	col1[2] = Ap[0];      col2[2] = Ap[2];      col3[2] = 1;
	v = -DetValue(col1,col2,col3);
	//-------------------w----
	col1[0] = oCoord[0];  col2[0] = oCoord[1];  col3[0] = 1;
	col1[1] = iCoord[0];  col2[1] = iCoord[1];  col3[1] = 1;
	col1[2] = Ap[0];      col2[2] = Ap[1];      col3[2] = 1;
	w = DetValue(col1,col2,col3);
	//-------------------t----
	col1[0] = oCoord[0];  col2[0] = oCoord[1];  col3[0] = oCoord[2];
	col1[1] = iCoord[0];  col2[1] = iCoord[1];  col3[1] = iCoord[2];
	col1[2] = Ap[0];      col2[2] = Ap[1];      col3[2] = Ap[2];
	t = -DetValue(col1,col2,col3);
	//------------------------

}
double albaVMEComputeWrapping::DetValue(double *col1,double *col2,double *col3){
	double rtn = 0;
	double pRtn = col1[0]*col2[1]*col3[2] + col1[1]*col2[2]*col3[0] + col2[0]*col3[1]*col1[2];
	double nRtn = col3[0]*col2[1]*col1[2] + col3[1]*col2[2]*col1[0] + col2[0]*col1[1]*col3[2];
	rtn = pRtn - nRtn;
	return rtn;
}
//appendix E
bool albaVMEComputeWrapping::GetBcoordinate(double *Ap,double *oCoord,double *iCoord,double *mCoord,double Rm,double R,double *bCoord1,double *bCoord2){
	// B on plane
	//( Xb-Xr)2+(Yb-Yr)2+(Zb-Zr)2 = R2
	//( Xb-Xi)2+(Yb-Yi)2+(Zb-Zi)2+rm2 = (Xm-Xi)2+(Ym-Yi)2+(Zm-Zi)2
	double u,v,w,t;
	double A,B,C,D,E,F,G,H,I,J;
	double a,b,c;
	double delta = 0;
	bool rtn = true;
	double testValue;

	testValue = TestPlane(Ap,oCoord,iCoord,mCoord);
	GetUVWT(Ap,oCoord,iCoord,u,v,w,t);//plane

	A = 2*iCoord[0];
	B = 2*iCoord[1];
	C = 2*iCoord[2];
	D = (mCoord[0] - iCoord[0])*(mCoord[0] - iCoord[0]) + (mCoord[1]-iCoord[1])*(mCoord[1]-iCoord[1]) + (mCoord[2]-iCoord[2])*(mCoord[2]-iCoord[2]);
	D = D - ( iCoord[0]*iCoord[0] +iCoord[1]*iCoord[1]+ iCoord[2]*iCoord[2] ) - R*R - Rm*Rm;

	E = u*B - v*A;
	F = -(v*D - t*B);
	G = v*C - w*B;

	H = -C*E - A*G;
	I = -C*F - D*G;
	J = B*G;

	//-------------ax2+bx+c=0----------------
	a = (1+ (H*H)/(J*J) + (E*E)/(G*G) );
	b = (2*H*I)/(J*J) + (2*E*F)/(G*G) ;
	c = (I*I)/(J*J) + (F*F)/(G*G) - (R*R);
	delta = b*b - 4*a*c;
	if (delta>=0)
	{
		bCoord1[0] = (-b + sqrt( b*b - 4*a*c) )/ (2*a); //Xb1
		bCoord2[0] = (-b - sqrt( b*b - 4*a*c) )/ (2*a); //Xb2

		bCoord1[1] = (H*bCoord1[0]+I) /J; //Yb1
		bCoord2[1] = (H*bCoord2[0]+I) /J; //Yb2

		bCoord1[2] = (E*bCoord1[0] + F) / G; //Zb1
		bCoord2[2] = (E*bCoord2[0] + F) / G; //Zb1
	}else{
		rtn = false ;
	}
	testValue = TestPlane(Ap,oCoord,iCoord,bCoord1);
	testValue = TestPlane(Ap,oCoord,iCoord,bCoord2);
	return rtn;

}
bool albaVMEComputeWrapping::GetBcoordinateUpdate(double *cCoord ,double *bCoord1,double *bCoord2){
	double blCoord[3];
	GetLocalCenterCoord(blCoord,m_BPoint);
	double Rm = GetCylinderRadius();
	double sphereR = GetSphereRadius();
	double bCoordR1[3],bCoordR2[3];
	double aCoordR[3],oCoordR[3],cCoordR[3],mCoordR[3];

	GetLocalCenterCoord(aCoordR,m_APoint);
	GetLocalCenterCoord(oCoordR,m_StartPoint);
	GetLocalCenterCoord(cCoordR,cCoord);

	GetIntersectCircle(aCoordR,oCoordR,cCoordR,mCoordR,Rm);//Rm is computed by global coord

	bool rtn =GetBcoordinate(aCoordR,oCoordR,cCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);

	GetGlobalCenterCoord(bCoord1,bCoordR1);
	GetGlobalCenterCoord(bCoord2,bCoordR2);
	return rtn;
}
//appendix F
bool albaVMEComputeWrapping::GetCcoordinate(double *cCoord1,double *cCoord2){
	double a,b,c,r;//r is radius of cylinder
	double Yc,Xc,Zc,Yc2,Xc2,Zc2;
	double Xb,Yb,Zb;
	double RCoord[3],Xr,Yr,Zr,R;
	double A,B,C,D;
	bool rtn = true;

	r = GetCylinderRadius();
	R = GetSphereRadius();

	GetGlobalSphereCenter(RCoord);//coordinate of sphere center;

	Xb = m_BPoint[0];Yb = m_BPoint[1];Zb = m_BPoint[2];
	Xr = RCoord[0];Yr = RCoord[1];Zr = RCoord[2];

	a = (Yb*Yb) + (Xb*Xb);//Yb*Yb + Xb*Xb;
	b = -2*r*r*Yb;//-(2r*rYb)
	c = r*r*( r*r - Xb*Xb );//r*r*(r*r-Xb*Xb)

	Yc = ( -b + sqrt(b*b - 4*a*c )) / ( 2*a );
	Yc2 = ( -b - sqrt(b*b - 4*a*c )) / ( 2*a );

	Xc = ( r*r - Yc*Yb ) / Xb;
	Xc2 = ( r*r - Yc2*Yb ) / Xb;

	//A*Xc + B*Yc + C*Zc + D = 0
	//A = 2*Xr;
	//B = 2*Yr;
	//C = 2*( Zr-Zb );
	//D = R*R - 2*r*r - ( Xr*Xr +Yr*Yr + Zr*Zr ) + ( Xb*Xb + Yb*Yb +Zb*Zb );
	//Zc = - (A *Xc +B*Yc + D) / C
	A = 2*(Xr-Xb);
	B = 2*(Yr-Yb);
	C = 2*(Zr-Zb);
	D = Xb*Xb + Yb*Yb +Zb*Zb + R*R -( Xr*Xr + Yr*Yr +Zr*Zr);

	Zc = - (A *Xc + B*Yc + D) / C;
	Zc2 = - (A *Xc2 + B*Yc2 + D) / C;

	cCoord1[0]=Xc; cCoord1[1]=Yc; cCoord1[2]=Zc;
	cCoord2[0]=Xc2; cCoord2[1]=Yc2; cCoord2[2]=Zc2;

	return rtn;
}

bool albaVMEComputeWrapping::PrepareData2(){
	albaVME *start_vme = GetStartVME();
	albaVME *end_vme   = GetEnd1VME();
	albaVME *wrapped_vme = GetWrappedVME1();
	albaVME *wrapped_vme2 = GetWrappedVME2();

	albaVMESurfaceParametric *surface1 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme);
	albaVMESurfaceParametric *surface2 = albaVMESurfaceParametric::SafeDownCast(wrapped_vme2);

	double orientation[3];
	int type1;//,type2;
	bool rtn = false;
	if (start_vme )
	{
			start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
	}
	if( end_vme )
	{
			end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
	}
	if ( wrapped_vme )
	{
		wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter1, orientation);

		double translation[3];
		((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);

		albaMatrix mat;
		mat.Identity();
		for(int i=0; i<3; i++)
			mat.SetElement(i,3,translation[i]);
		m_TmpTransform->SetMatrix(mat);
		m_TmpTransform->TransformPoint(m_WrappedVMECenter1,m_WrappedVMECenter1);


		//------------------------get sphere axis-----------------
		type1 = surface1->GetGeometryType();
		if (type1 == albaVMESurfaceParametric::PARAMETRIC_SPHERE )
		{
			double pointOnAxis[3] ;

			pointOnAxis[0]=10;pointOnAxis[1]=0;pointOnAxis[2]=0;
			m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
			m_TmpTransform->TransformPoint(pointOnAxis,pointOnAxis);
			CopyPointValue(pointOnAxis,m_SphereAxis);
		}else if ( type1 == albaVMESurfaceParametric::PARAMETRIC_CYLINDER)
		{
			GetCylinderAxis(wrapped_vme,1);
		}
	}
	if( wrapped_vme2)
	{
		wrapped_vme2->GetOutput()->GetAbsPose(m_WrappedVMECenter2, orientation);

		double translation[3];
		((vtkPolyData *)wrapped_vme2->GetOutput()->GetVTKData())->GetCenter(translation);

		albaMatrix mat;
		mat.Identity();
		for(int i=0; i<3; i++)
			mat.SetElement(i,3,translation[i]);
		m_TmpTransform->SetMatrix(mat);
		m_TmpTransform->TransformPoint(m_WrappedVMECenter2,m_WrappedVMECenter2);

		//------------------------get cylinder axis-----------------
		GetCylinderAxis(wrapped_vme2,2);
		//------------------------over------------------------------
	}

	if (m_WrappedMode1 == SPHERE_CYLINDER || m_WrappedMode1 == SINGLE_CYLINDER)
	{
		m_Tolerance = GetCylinderRadius()/4.0 ;
	}
	rtn = true;

	return rtn;

}
void albaVMEComputeWrapping::GetCylinderAxis(albaVME *wrapped_vme,int objIdx){

	double x[3] = {0,0,1};
	double pointOnAxis[3] ;
	m_TmpTransform->TransformPoint(x,x);

	int cylinderAxisD  = albaVMESurfaceParametric::SafeDownCast(wrapped_vme)->GetCylinderAxis();
	if (cylinderAxisD ==0)//x
	{
		pointOnAxis[0]=10;pointOnAxis[1]=0;pointOnAxis[2]=0;
		m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
		m_TmpTransform->TransformPoint(pointOnAxis,pointOnAxis);
		//CopyPointValue(pointOnAxis,m_CylinderAxis);
	}else if (cylinderAxisD ==1)//y
	{
		pointOnAxis[0]=0;pointOnAxis[1]=10;pointOnAxis[2]=0;
		m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
		m_TmpTransform->TransformPoint(pointOnAxis,pointOnAxis);
		//CopyPointValue(pointOnAxis,m_CylinderAxis);		

	}else if (cylinderAxisD == 2)//z
	{
		pointOnAxis[0]=0;pointOnAxis[1]=0;pointOnAxis[2]=10;
		m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
		m_TmpTransform->TransformPoint(pointOnAxis,pointOnAxis);
		//CopyPointValue(pointOnAxis,m_CylinderAxis);
	}
	if (objIdx ==1)
	{
		CopyPointValue(pointOnAxis,m_CylinderAxis1);
	}else if (objIdx ==2)
	{
		CopyPointValue(pointOnAxis,m_CylinderAxis2);
	}
}
int albaVMEComputeWrapping::PrepareData(int wrappedFlag,double *local_start,double *local_via,double *local_wrapped_center, vtkOBBTree *locator)
{
	int obbtreeFlag = 0;
	albaVME *start_vme = GetStartVME();
	albaVME *end_vme   = GetEnd1VME();
	albaVME *via_vme = GetViaPointVME();
	albaVME *wrapped_vme = GetWrappedVME1();
	if (wrappedFlag ==2)
	{
		wrapped_vme = GetWrappedVME2();
	}
	bool start_ok = true, end_ok = true;
	double orientation[3];
	int nControl = 0;
	vtkALBASmartPointer<vtkPoints> temporaryIntersection;
	m_Goniometer->RemoveAllInputs();

	if (start_vme && end_vme && wrapped_vme && via_vme)
	{ 
		start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation);
		end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation);
		via_vme->GetOutput()->GetAbsPose(m_ViaPoint, orientation);

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
	if (start_ok && end_ok)
	{
		// compute distance between points
		m_Distance = 0.0;

		// compute start point in local coordinate system
		m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
		m_TmpTransform->Invert();
		m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
		//GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

		// compute end point in local coordinate system

		m_TmpTransform->TransformPoint(m_ViaPoint,local_via);

		// compute end point in local coordinate system
		m_TmpTransform->TransformPoint(m_WrappedVMECenter,local_wrapped_center);

		// use list of tangent point (raw)		
		//control if the point are allineated
		double vec1[3], vec2[3], vec3[3];
		vec1[0] = local_start[0] - local_via[0];
		vec1[1] = local_start[1] - local_via[1];
		vec1[2] = local_start[2] - local_via[2];

		vec2[0] = local_start[0] - local_wrapped_center[0];
		vec2[1] = local_start[1] - local_wrapped_center[1];
		vec2[2] = local_start[2] - local_wrapped_center[2];

		vec3[0] = local_via[0] - local_wrapped_center[0];
		vec3[1] = local_via[1] - local_wrapped_center[1];
		vec3[2] = local_via[2] - local_wrapped_center[2];

		bool aligned = false;
		double vectorProduct[3];
		vtkMath::Cross(vec1,vec2, vectorProduct); 
		if(vectorProduct[0] == 0.0 && vectorProduct[1] == 0.0 && vectorProduct[2] == 0.0)
			aligned = true;

		if(aligned == true)
		{
			AvoidWrapping(local_start,local_via);
			return -1;
		}	
		// create ordered list of tangent point (2) real algorithm
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
		transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform());
		transformFirstDataInput->SetInput((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData());
		transformFirstDataInput->Update();

		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
		transformFirstData->Update(); 

		//-------test intersect---------------------
		//vtkALBASmartPointer<vtkOBBTree> locator;
		locator->SetDataSet(transformFirstData->GetOutput());
		locator->SetGlobalWarningDisplay(0);
		locator->BuildLocator();

		//Control if Start or End point is inside vtk data (surface)
		if(locator->InsideOrOutside(local_start) <= 0 || locator->InsideOrOutside(local_via) <= 0) 
		{
			obbtreeFlag = 0;
		}else{
			//control if there is an intersection
			locator->IntersectWithLine(local_start, local_via, temporaryIntersection, NULL);
			//  code to control if exist an intersection between the line draw from start point to end point and 
			nControl = temporaryIntersection->GetNumberOfPoints();	//  the vtk data (surface)	
			if(nControl==2){
				obbtreeFlag=1;
			}
		}
	}
	//--------------over---------------
	return obbtreeFlag;

}

void albaVMEComputeWrapping::SingleWrapAutomatedIOR(albaVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator){

	vtkALBASmartPointer<vtkPoints> pointsIntersection1;
	vtkALBASmartPointer<vtkPoints> pointsIntersection2;
	//vtkALBASmartPointer<vtkPoints> temporaryIntersection;
	double pointTangent1[3];
	double pointTangent2[3];
	bool aligned = false;
	double curveLength = 0.00;
	vtkLineSource *ST1,*ET2;
	vtkNEW(ST1);
	vtkNEW(ET2);
	vtkClipPolyData *clipData; 

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;

	GetTwoTangentPoint(wrapped_vme,local_start,local_end,local_wrapped_center,locator,transformFirstData,pointsIntersection1,pointsIntersection2);

	if(pointsIntersection1->GetNumberOfPoints() == 0 || pointsIntersection2->GetNumberOfPoints() == 0) return;

	pointTangent1[0] = pointsIntersection1->GetPoint(0)[0];
	pointTangent1[1] = pointsIntersection1->GetPoint(0)[1];
	pointTangent1[2] = pointsIntersection1->GetPoint(0)[2];

	pointTangent2[0] = pointsIntersection2->GetPoint(0)[0];
	pointTangent2[1] = pointsIntersection2->GetPoint(0)[1];
	pointTangent2[2] = pointsIntersection2->GetPoint(0)[2];

	vtkNEW(clipData);

	//--------------out put----------------------

	ST1->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	ST1->SetPoint2(pointTangent1[0],pointTangent1[1],pointTangent1[2]);

	ET2->SetPoint1(pointTangent2[0],pointTangent2[1],pointTangent2[2]);
	ET2->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
	m_Goniometer->AddInput(ST1->GetOutput());
	m_Goniometer->AddInput(ET2->GetOutput());

	curveLength = GetCutPlaneForCylinder(local_wrapped_center,pointTangent1,pointTangent2,GetWrappedVME1(),clipData);
	m_Goniometer->AddInput(clipData->GetOutput());
	//---------------over--------------------------

	InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update();


}

void albaVMEComputeWrapping::GetTwoTangentPoint(albaVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator,vtkTransformPolyDataFilter *transformFirstData ,vtkPoints *pointsIntersection1,vtkPoints *pointsIntersection2){

	vtkALBASmartPointer<vtkPoints> temporaryIntersection;
	bool aligned = false;

	//Control if Start or End point is inside vtk data (surface)
	if(locator->InsideOrOutside(local_start) <= 0 || locator->InsideOrOutside(local_end) <= 0) 
	{
		AvoidWrapping(local_start,local_end);
		return;
	}

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

	// create ordered list of tangent point (2) real algorithm
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
	transformFirstDataInput->SetTransform((vtkAbstractTransform *)wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform());
	transformFirstDataInput->SetInput((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData());
	transformFirstDataInput->Update();

	//vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
	transformFirstData->Update(); 


	vtkMatrix4x4 *mat = wrapped_vme->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix();
	albaMatrix matrix;
	matrix.SetVTKMatrix(mat);
	double versorX[3], versorY[3], versorZ[3];
	matrix.GetVersor(0, versorX);
	matrix.GetVersor(1, versorY);
	matrix.GetVersor(2, versorZ);

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

		//--------------test code----------------------
		vtkLineSource *SC,*CE;
		vtkNEW(SC);
		vtkNEW(CE);


		SC->SetPoint1(pointsIntersection1->GetPoint(0)[0],pointsIntersection1->GetPoint(0)[1],pointsIntersection1->GetPoint(0)[2]);
		SC->SetPoint2(m_WrappedVMECenter1[0],m_WrappedVMECenter1[1],m_WrappedVMECenter1[2]);

		CE->SetPoint1(m_WrappedVMECenter1[0],m_WrappedVMECenter1[1],m_WrappedVMECenter1[2]);
		CE->SetPoint2(pointsIntersection2->GetPoint(0)[0],pointsIntersection2->GetPoint(0)[1],pointsIntersection2->GetPoint(0)[2]);
		m_Goniometer->AddInput(SC->GetOutput());
		m_Goniometer->AddInput(CE->GetOutput());

		vtkDEL(SC);
		vtkDEL(CE);
		//---------------over--------------------------
	}
	else
	{
		AvoidWrapping(local_start, local_end);
		return;
	}
}


//-----------------------------------------------------------------------
int albaVMEComputeWrapping::InternalStore(albaStorageElement *parent)
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
				//albaVMELandmark *landmark= lc->GetLandmark(m_ListBox->GetString(i));
				int index = -1;
				for(int j=0; j< lc->GetNumberOfLandmarks(); j++)
				{
					albaVME *child = lc->GetLandmark(j);
					if(albaString(child->GetName()).Equals(m_OrderMiddlePointsNameVMEList[i])) index = j;
				}

				PushIdVector(index);
			}
		}
		parent->StoreInteger("OrderMiddlePointVmeNumberOfElements", m_OrderMiddlePointsVMEList.size());
		parent->StoreVectorN("OrderMiddlePointVme",m_OrderMiddlePointsVMEList,m_OrderMiddlePointsVMEList.size());

		parent->StoreInteger("WrapMode", m_WrappedMode1);
		parent->StoreInteger("WrapMode2", m_WrappedMode2);
		parent->StoreInteger("WrapSide", m_WrapSide);
		parent->StoreInteger("WrapReverse", m_WrapReverse);
		parent->StoreInteger("WrapClass",m_WrappedClass);

		return ALBA_OK;
	}
	return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEComputeWrapping::InternalRestore(albaStorageElement *node)
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

			node->RestoreInteger("WrapMode", m_WrappedMode1);
			node->RestoreInteger("WrapMode2", m_WrappedMode2);
			node->RestoreInteger("WrapSide", m_WrapSide);
			node->RestoreInteger("WrapReverse", m_WrapReverse);
			node->RestoreInteger("WrapClass",m_WrappedClass);
			return ALBA_OK;
		}
	}

	return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMEComputeWrapping::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
	Superclass::Print(os,tabs);
	albaIndent indent(tabs);

	albaMatrix m = m_Transform->GetMatrix();
	m.Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
mmaMeter *albaVMEComputeWrapping::GetMeterAttributes()
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
void albaVMEComputeWrapping::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetMeterMode()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_DistanceRange[0] = min;
	GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *albaVMEComputeWrapping::GetDistanceRange() 
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetMeterColorMode()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetMeterCapping()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_GenerateEvent = generate;
}

//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_InitMeasure = init_measure;
}

//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double albaVMEComputeWrapping::GetMeterRadius()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
	GetMeterAttributes()->m_DeltaPercent = delta_percent;
}

//-------------------------------------------------------------------------
double albaVMEComputeWrapping::GetDistance()
//-------------------------------------------------------------------------
{
	return m_Distance;
}
//-------------------------------------------------------------------------
double albaVMEComputeWrapping::GetAngle()
//-------------------------------------------------------------------------
{
	return m_Angle;
}
albaGUI* albaVMEComputeWrapping::CreateGuiForNewMeter( albaGUI *gui ){

	albaID sub_id = -1;

	int num_mode = 1;
	int num_wrap =1;
	m_WrapSide = 0;
	//	const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};;
	const wxString wrap_choices_string[] = { _("Sphere_Cylinder"),_("single Sphere"),_("single Cylinder"),_("double Cylinder")};
	const wxString wrap_side_string[] = { _("front"),_("back"),_("shortest")};


	gui->SetListener(this);
	gui->Divider();
	//m_WrappedMode1 = SPHERE_CYLINDER;
	gui->Combo(ID_WRAPPED_METER_MODE,_("wrap"),&m_WrappedMode1,4,wrap_choices_string,_("Choose the meter mode"));
	//m_Gui->Combo(ID_WRAPPED_METER_TYPE,_("type"),&m_WrapSide,3,wrap_side_string,_("Choose the wrap type"));
	//m_Gui->Combo(ID_METER_MODE,_("mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_("Choose the meter mode"));
	gui->Divider();

	albaVME *start_vme = GetStartVME();
	m_StartVmeName = start_vme ? start_vme->GetName() : _("none");
	gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));

	albaVME *end_vme1 = GetEnd1VME();
	m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");
	gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End"), _("Select the end vme for point distance"));

	gui->Integer(ID_WRAPPED_METER_NUM,_("pathNum"),&m_PathNum,0,360);

	albaVME *wrapped_vme1 = GetWrappedVME1();
	m_WrappedVmeName1 = wrapped_vme1 ? wrapped_vme1->GetName() : _("none");

	albaVME *wrapped_vme2 = GetWrappedVME2();
	m_WrappedVmeName2 = wrapped_vme2 ? wrapped_vme2->GetName() : _("none");

	albaVME *viaPointvme2 = GetViaPointVME();
	m_ViaPointName = viaPointvme2 ? viaPointvme2->GetName() : _("none");

	gui->Button(ID_WRAPPED_METER_LINK1,&m_WrappedVmeName1,_("Wrapped object1"), _("Select the vme representing Vme to be wrapped1"));
	gui->Button(ID_WRAPPED_METER_LINK2,&m_WrappedVmeName2,_("Wrapped object2"), _("Select the vme representing Vme to be wrapped2"));
	gui->Bool(ID_WRAPPED_SIDE_NEW,"reverse", &m_WrapReverseNew ,0);
	gui->Divider();
	gui->Divider();
	gui->Divider(2);
	gui->Divider();

	gui->Enable(ID_WRAPPED_METER_LINK1, m_WrappedMode1==SPHERE_CYLINDER || m_WrappedMode1 == SPHERE_ONLY || m_WrappedMode1 ==DOUBLE_CYLINDER  );//sphere
	gui->Enable(ID_WRAPPED_METER_LINK2, m_WrappedMode1 ==SPHERE_CYLINDER || m_WrappedMode1 ==DOUBLE_CYLINDER || m_WrappedMode1 == CYLINDER_ONLY );//cylinder
	gui->Enable(ID_WRAPPED_SIDE_NEW,m_WrappedMode1==DOUBLE_CYLINDER);
	gui->Update();

	InternalUpdate();

	return gui;
}
albaGUI* albaVMEComputeWrapping::CreateGuiForOldMeter( albaGUI *gui ){

	albaID sub_id = -1;

	int num_mode = 3;
	int num_wrap = 3;
	//	const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};;
	const wxString wrap_choices_string[] = {_("manual"), _("automated"), _("IOR_automated")};


	gui->Combo(ID_WRAPPED_METER_MODE,_("wrap"),&m_WrappedMode2,num_wrap,wrap_choices_string,_("Choose the meter mode"));
	//m_Gui->Combo(ID_METER_MODE,_("mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_("Choose the meter mode"));
	gui->Divider();

	albaVME *start_vme = GetStartVME();
	m_StartVmeName = start_vme ? start_vme->GetName() : _("none");
	gui->Button(ID_START_METER_LINK,&m_StartVmeName,_("Start"), _("Select the start vme for the meter"));

	albaVME *end_vme1 = GetEnd1VME();
	m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _("none");
	gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End"), _("Select the end vme for point distance"));

	albaVME *wrapped_vme = GetWrappedVME();
	m_WrappedVmeName = wrapped_vme ? wrapped_vme->GetName() : _("none");
	gui->Button(ID_WRAPPED_METER_LINK,&m_WrappedVmeName,_("Wrapped Object"), _("Select the vme representing Vme to be wrapped"));
	gui->Bool(ID_WRAPPED_SIDE,"reverse direction", &m_WrapSide ,1);
	
	gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode2 == AUTOMATED_WRAP || m_WrappedMode2 == IOR_AUTOMATED_WRAP);
	gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode2 == AUTOMATED_WRAP);
	gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode2 == AUTOMATED_WRAP);

	gui->Label(_("MidPoints"), true);
	m_ListBox=gui->ListBox(ID_LISTBOX);

	//insert eventual middlepoints
	m_OrderMiddlePointsNameVMEList.clear();
	for(int j=0; j<m_OrderMiddlePointsVMEList.size();j++)
	{
		for (albaLinksMap::iterator i = GetLinks()->begin(); i != GetLinks()->end(); ++i)
		{	
			if(i->first.Equals("StartVME")) continue;
			else if(i->first.Equals("EndVME1")) continue;
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

	gui->Button(ID_ADD_POINT, _("Add"), "" ,"");
	gui->Button(ID_REMOVE_POINT, _("Remove"), "" ,"");
	gui->Button(ID_UP, _("Up"), "" ,"");
	gui->Button(ID_DOWN, _("Down"), "" ,"");

	EnableManualModeWidget(gui, m_WrappedMode2 == MANUAL_WRAP);

	gui->Divider();
	gui->Divider();
	gui->Divider(2);
	//m_Gui->Button(ID_SAVE_FILE_BUTTON, _("Save in file"),"" ,"");
	gui->Divider();

	EnableManualModeWidget(gui,m_WrappedMode2 ==MANUAL_WRAP);
	gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode2 == AUTOMATED_WRAP || m_WrappedMode2 == IOR_AUTOMATED_WRAP);
	gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode2 == AUTOMATED_WRAP );
	gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode2 == AUTOMATED_WRAP);


	gui->Update();


	InternalUpdate();

	return gui;
}

albaGUI* albaVMEComputeWrapping::CreateGui(){
	m_Gui = NULL;
	m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
	m_Gui->SetListener(this);
	m_Gui->Divider();

	if(m_Gui)
	{
		m_GuiNewMeter = new albaGUI(this);
		m_GuiOldMeter = new albaGUI(this);


		CreateGuiForNewMeter(m_GuiNewMeter);
		CreateGuiForOldMeter(m_GuiOldMeter);
		if (m_WrappedClass==NEW_METER)
		{
			m_RollOutNewMeter = m_Gui->RollOut(ID_ROLLOUT_NEW,_("Geodesic method"), m_GuiNewMeter, true);
			m_RollOutOldMeter = m_Gui->RollOut(ID_ROLLOUT_OLD,_("Pivot-set method"), m_GuiOldMeter, false);
		}else if (m_WrappedClass==OLD_METER)
		{
			m_RollOutNewMeter = m_Gui->RollOut(ID_ROLLOUT_NEW,_("Geodesic method"), m_GuiNewMeter, false);
			m_RollOutOldMeter = m_Gui->RollOut(ID_ROLLOUT_OLD,_("Pivot-set method"), m_GuiOldMeter, true);
		}




		m_Gui->Update();
	}
	return m_Gui;
}

void albaVMEComputeWrapping::EventWraped(albaEvent *e){

	albaID button_id = e->GetId();
	albaString title = _("Choose meter vme link");
	e->SetId(VME_CHOOSE);
	if (button_id == ID_WRAPPED_METER_LINK1)
		e->SetPointer(&albaVMEComputeWrapping::VMESurfaceParametricAccept);
	else if (button_id == ID_WRAPPED_METER_LINK2)
		e->SetPointer(&albaVMEComputeWrapping::VMESurfaceParametricAccept);

	else if (button_id == ID_WRAPPED_METER_LINK)//old object
		e->SetPointer(&albaVMEComputeWrapping::VMESurfaceParametricAccept);

	else if (button_id == ID_VIA_POINT)
		e->SetPointer(&albaVMEComputeWrapping::VMESurfaceParametricAccept);
	else
		e->SetPointer(&albaVMEComputeWrapping::VMEAccept);
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
		else if (button_id == ID_WRAPPED_METER_LINK1)
		{
			SetMeterLink("WrappedVME1", n);
			//n->SetName("wrapped1");
			m_WrappedVmeName1 = n->GetName();
		}
		else if (button_id == ID_WRAPPED_METER_LINK2)
		{
			SetMeterLink("WrappedVME2", n);
			//n->SetName("wrapped2");
			m_WrappedVmeName2 = n->GetName();
		}
		else if (button_id == ID_WRAPPED_METER_LINK)
		{
			SetMeterLink("WrappedVME",n);
			m_WrappedVmeName = n->GetName();


		}
		else if (button_id == ID_VIA_POINT)
		{
			SetMeterLink("viaPoint", n);
			//n->SetName("wrapped2");
			m_ViaPointName = n->GetName();
		}

		m_GuiNewMeter->Update();
		m_GuiOldMeter->Update();
		//m_RollOutNewMeter->Update();
		//m_RollOutOldMeter->Update();

		m_Gui->Update();
		InternalUpdate();
		/*albaID button_id = e->GetId();*/
		e->SetId(CAMERA_UPDATE);
		ForwardUpEvent(e);
	}

}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{

	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
	    

		switch(e->GetId())
		{

		case ID_ROLLOUT_NEW:
		case ID_ROLLOUT_OLD:
			{
				// if I'm using the old meter
				if (m_WrappedClass == OLD_METER)
				{

					m_WrappedClass = NEW_METER;  // switch to the new one
					m_RollOutNewMeter->RollOut(true); // show the new one
					
					m_RollOutOldMeter->RollOut(false); // hide the old one
				}
				else // if I'm using the new meter
				{
					m_WrappedClass = OLD_METER;  // switch to the old one
					m_RollOutOldMeter->RollOut(true); // show the old one

					m_RollOutNewMeter->RollOut(false); // hide the new one
				}
			}
		break;

		case ID_START_METER_LINK:
		case ID_END1_METER_LINK:
		case ID_WRAPPED_METER_LINK1:
		case ID_WRAPPED_METER_LINK2:
		case ID_WRAPPED_METER_LINK:
		case ID_VIA_POINT:
			{
				EventWraped(e);
			}
			break;
		case ID_WRAPPED_METER_CLASS:
			{
				//CreateGui();

				if (m_WrappedClass == OLD_METER)
				{
					m_BSizer->Show( m_Gui,true);
					//m_Gui->Add(bSizer, 1, wxEXPAND);
				}else{
					//m_Gui->Remove(bSizer);
					m_BSizer->Show( m_Gui,false);
				}
				m_Gui->Update();

				Modified();
				InternalUpdate();
				albaID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);

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
				//EnableManualModeWidget(m_WrappedMode ==MANUAL_WRAP);

				//m_Gui->Enable(ID_WRAPPED_METER_LINK,  m_WrappedMode == IOR_AUTOMATED_WRAP);

				if (m_WrappedClass == NEW_METER)
				{
					m_GuiNewMeter->Enable(ID_WRAPPED_METER_LINK1, m_WrappedMode1==SPHERE_CYLINDER || m_WrappedMode1 == SPHERE_ONLY || m_WrappedMode1 ==DOUBLE_CYLINDER  );//sphere
					m_GuiNewMeter->Enable(ID_WRAPPED_METER_LINK2, m_WrappedMode1 ==SPHERE_CYLINDER || m_WrappedMode1 ==DOUBLE_CYLINDER || m_WrappedMode1 == CYLINDER_ONLY );//cylinder
					if (m_WrappedMode1 ==DOUBLE_CYLINDER )
					{
						if (m_WrappedVmeName1=="none" || m_WrappedVmeName1 =="")
						{
							m_WrappedVmeName1 = _("cylinder");
						}
						if (m_WrappedVmeName2=="none" || m_WrappedVmeName2 =="")
						{
							m_WrappedVmeName2 = _("cylinder");
						}
						m_GuiNewMeter->Enable(ID_WRAPPED_SIDE_NEW,true);

					}else if (m_WrappedMode1 ==SPHERE_CYLINDER )
					{
						if (m_WrappedVmeName1=="none" || m_WrappedVmeName1 ==""){
							m_WrappedVmeName1 = _("sphere");
						}
						if (m_WrappedVmeName2=="none" || m_WrappedVmeName2 =="")
						{
							m_WrappedVmeName2 = _("cylinder");
						}
						m_GuiNewMeter->Enable(ID_WRAPPED_SIDE_NEW,false);

					}else{
						m_GuiNewMeter->Enable(ID_WRAPPED_SIDE_NEW,false);
					}

				}else if (m_WrappedClass == OLD_METER)
				{
					EnableManualModeWidget(m_GuiOldMeter,m_WrappedMode2 ==MANUAL_WRAP);
					m_GuiOldMeter->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode2 == AUTOMATED_WRAP || m_WrappedMode2 == IOR_AUTOMATED_WRAP);
					m_GuiOldMeter->Enable(ID_WRAPPED_SIDE, m_WrappedMode2 == AUTOMATED_WRAP );
					m_GuiOldMeter->Enable(ID_WRAPPED_REVERSE, m_WrappedMode2 == AUTOMATED_WRAP);
				}
				m_GuiNewMeter->Update();
				m_Gui->Update();
				Modified();
				InternalUpdate();
				albaID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);
			}
			break;
		case ID_WRAPPED_METER_TYPE:
			InternalUpdate();
			ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
			break;
		case ID_WRAPPED_SIDE_NEW:
		case ID_WRAPPED_SIDE:
		case ID_WRAPPED_REVERSE:
			InternalUpdate();
			ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
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
				e->SetPointer(&albaVMEComputeWrapping::VMEAccept);
				e->SetString(&title);
				ForwardUpEvent(e);
				albaVME *n = e->GetVme();

				if (n == NULL) return;

				wxString nameProfile ="";
				//albaString idNumber = albaString::Format(_("%d"),id);
				nameProfile = n->GetName();

				if(wxNOT_FOUND != m_ListBox->FindString(n->GetName()))
				{
					wxMessageBox(_("Can't introduce vme with the same name"));
					return;
				}

				if(albaString(n->GetName()) == albaString("StartVME") ||
					albaString(n->GetName()) == albaString("EndVME1")  ||
					albaString(n->GetName()) == albaString("WrappedVME") ||
					albaString(n->GetName()) == albaString("WrappedVME1") ||
					albaString(n->GetName()) == albaString("WrappedVME2"))
				{
					wxMessageBox(_("Can't introduce vme with the name of StartVME or EndVME1 or WrappedVME or WrappedVME1 or WrappedVME2"));
					return;
				}



				SetMeterLink(n->GetName(),n);
				wxString t;
				t = n->GetName();
				//m_ListBox->Append(_(t));
				m_ListBox->Append(_(t));
				m_ListBox->SetStringSelection(_(t));

				m_ListBox->Update();
				m_Gui->Update();
				Modified();
				//InternalUpdate();

				InternalUpdate();
				GetLogicManager()->CameraReset();
				ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));

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
					ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
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


					m_ListBox->SetStringSelection(_(name));

					m_OrderMiddlePointsNameVMEList.clear();
					for(int i=0;i<m_ListBox->GetCount();i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

					Modified();
					m_Gui->Update();
					InternalUpdate();
					ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
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
					m_ListBox->SetStringSelection(_(name));

					m_OrderMiddlePointsNameVMEList.clear();
					for(int i=0;i<m_ListBox->GetCount();i++)
					{
						m_OrderMiddlePointsNameVMEList.push_back(m_ListBox->GetString(i));
					}

					Modified();
					InternalUpdate();
					ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
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
void albaVMEComputeWrapping::SetMeterLink(const char *link_name, albaVME *n)
//-------------------------------------------------------------------------
{
	SetLink(link_name, n);
	
	if( albaString(link_name) != albaString("StartVME") &&
		albaString(link_name) != albaString("EndVME1")  &&
		albaString(link_name) != albaString("WrappedVME") &&
		albaString(link_name) != albaString("WrappedVME1") &&
		albaString(link_name) != albaString("WrappedVME2"))
	{
		m_OrderMiddlePointsNameVMEList.push_back(n->GetName());
		m_TestList.push_back(n->GetName());
	}
}

//-------------------------------------------------------------------------
void albaVMEComputeWrapping::AddMidPoint(albaVME *node)
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
void albaVMEComputeWrapping::RemoveLink(const char *link_name)
//-------------------------------------------------------------------------
{
	Superclass::RemoveLink(link_name);

	if( albaString(link_name) != albaString("StartVME") &&
		albaString(link_name) != albaString("EndVME1")  &&
		albaString(link_name) != albaString("WrappedVME") &&
		albaString(link_name) != albaString("WrappedVME1") &&
		albaString(link_name) != albaString("WrappedVME2"))
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
albaVME *albaVMEComputeWrapping::GetStartVME()
//-------------------------------------------------------------------------
{
	return GetLink("StartVME");
}
//-------------------------------------------------------------------------
albaVME *albaVMEComputeWrapping::GetEnd1VME()
//-------------------------------------------------------------------------
{
	return GetLink("EndVME1");
}
//-------------------------------------------------------------------------
albaVME  *albaVMEComputeWrapping::GetWrappedVME1()
//-------------------------------------------------------------------------
{
	return GetLink("WrappedVME1");
}
albaVME  *albaVMEComputeWrapping::GetWrappedVME2()
//-------------------------------------------------------------------------
{
	return GetLink("WrappedVME2");
}
albaVME *albaVMEComputeWrapping::GetWrappedVME()
//-------------------------------------------------------------------------
{
	return GetLink("WrappedVME");
}
albaVME  *albaVMEComputeWrapping::GetViaPointVME()
//-------------------------------------------------------------------------
{
	return GetLink("viaPoint");
}

std::vector<albaVME *> albaVMEComputeWrapping::GetWrappedVMEList(){
	//-------------------------------------------------------------------------
	std::vector<albaVME *> wrappedVmeList;
	albaVME * tmp = GetLink("WrappedVME1");
	wrappedVmeList.push_back(tmp);
	tmp = GetLink("WrappedVME2");
	wrappedVmeList.push_back(tmp);
	return wrappedVmeList;

}

//-----------------------------------------------------------------------
albaVME::albaLinksMap *albaVMEComputeWrapping::GetMidPointsLinks()
//-----------------------------------------------------------------------
{
	//da ritornare la link maps
	return GetLinks();
}

//-------------------------------------------------------------------------
void albaVMEComputeWrapping::EnableManualModeWidget(albaGUI *gui,bool flag)
//-------------------------------------------------------------------------
{
	gui->Enable(ID_LISTBOX, flag);
	gui->Enable(ID_ADD_POINT, flag);
	gui->Enable(ID_REMOVE_POINT, flag);
	gui->Enable(ID_UP, flag);
	gui->Enable(ID_DOWN, flag);
}

//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SyncronizeList()
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
double *albaVMEComputeWrapping::GetMiddlePointCoordinate(int index)
//-------------------------------------------------------------------------
{
	if(m_MiddlePointList.size() > 0)
		return m_MiddlePointList[index];

	return NULL;
}
//-------------------------------------------------------------------------
double *albaVMEComputeWrapping::GetExportPointCoordinate(int index)
//-------------------------------------------------------------------------
{
	if(m_ExportPointList.size() > 0)
		return m_ExportPointList[index];

	return NULL;
}
//-------------------------------------------------------------------------
double *albaVMEComputeWrapping::GetStartPointCoordinate()
//-------------------------------------------------------------------------
{
	return m_StartPoint;
}
//-------------------------------------------------------------------------
double *albaVMEComputeWrapping::GetEndPointCoordinate()
//-------------------------------------------------------------------------
{
	return m_EndPoint;
}
//-------------------------------------------------------------------------
void albaVMEComputeWrapping::SaveInFile()
//-------------------------------------------------------------------------
{
	/*albaString initialFileName;
	initialFileName = albaGetApplicationDirectory().char_str();
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




//------------------------------------------------------***** old method begin from albaVMEWrappedMeter (weih copy it)*****--------------------------
//-------------------------------------------------------------------------
double albaVMEComputeWrapping::GetInitMeasure()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetDeltaPercent()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
int albaVMEComputeWrapping::GetGenerateEvent()
//-------------------------------------------------------------------------
{
	return GetMeterAttributes()->m_GenerateEvent;
}

//-----------------------------------------------------------------------
void albaVMEComputeWrapping::InternalUpdateManual()//first
//-----------------------------------------------------------------------
{
	//if(m_Gui == NULL) CreateGui();
	if(m_OrderMiddlePointsNameVMEList.size() == 0) SyncronizeList();

	GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
	GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
	GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

	double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

	if (GetMeterMode() == albaVMEComputeWrapping::POINT_DISTANCE)
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

			if(GetMeterMeasureType() == albaVMEComputeWrapping::RELATIVE_MEASURE)
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
		
		if(GetMeterMeasureType() == albaVMEComputeWrapping::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
			InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
		
		for(int i=0; i< m_MiddlePointList.size(); i++)
		{
			if(localMiddlePointList[i]) delete localMiddlePointList[i];
		}
		localMiddlePointList.clear();
	}
}

//-----------------------------------------------------------------------
void albaVMEComputeWrapping::InternalUpdateAutomated()//second
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
void albaVMEComputeWrapping::InternalUpdateAutomatedIOR()//third
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
		//--------------------test code---------------------
		/*
		vtkLineSource     *testLineSource1,*testLineSource2;
		vtkNEW(testLineSource1);
		vtkNEW(testLineSource2);
		testLineSource1->SetPoint1(pointTangent1[0],pointTangent1[1],pointTangent1[2]);
		testLineSource1->SetPoint2(local_wrapped_center[0],local_wrapped_center[1],local_wrapped_center[2]);
		testLineSource2->SetPoint1(pointTangent2[0],pointTangent2[1],pointTangent2[2]);
		testLineSource2->SetPoint2(local_wrapped_center[0],local_wrapped_center[1],local_wrapped_center[2]);

		m_Goniometer->AddInput(testLineSource1->GetOutput());
		m_Goniometer->AddInput(testLineSource2->GetOutput());
		*/
		//--------------------------------------------------
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

		double normal[3],tanLine[3];
		//-----------get normal ------------------
		/*normal[0] = midPoint[0] - local_wrapped_center[0];
		normal[1] = midPoint[1] - local_wrapped_center[1];
		normal[2] = midPoint[2] - local_wrapped_center[2];*/
		tanLine[0] = pointTangent2[0] - pointTangent1[0];
		tanLine[1] = pointTangent2[1] - pointTangent1[1];
		tanLine[2] = pointTangent2[2] - pointTangent1[2];

		vtkMath::Cross(tanLine,m_PlaneSource->GetNormal(),normal);
		//-------------------------get normal over---------------------------
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
void albaVMEComputeWrapping::WrappingCore(double *init, double *center, double *end,\
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

		//------------------test code-----------------
		/*vtkLineSource     *testLineSource;
		vtkNEW(testLineSource);
		testLineSource->SetPoint1(p1[0],p1[1],p1[2]);
		testLineSource->SetPoint2(p2[0],p2[1],p2[2]);
		m_Goniometer->AddInput(testLineSource->GetOutput());*/
		//--------------------------------------------


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

			double init_center[3];
			init_center[0] = center[0] - init[0];
			init_center[1] = center[1] - init[1];
			init_center[2] = center[2] - init[2];

			double end_center[3];
			end_center[0] = end[0] - center[0];
			end_center[1] = end[1] - center[1];
			end_center[2] = end[2] - center[2];

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
		//-----------------test code-----------------
		/*if (count>100)
		{
		break;
		}*/
	}
}
//-----------------------------------------------------------------------
void albaVMEComputeWrapping::AvoidWrapping(double *local_start, double *local_end)
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
//-------------------------------------------------------------------------
bool albaVMEComputeWrapping::MiddlePointsControl()
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
		else if(i->first.Equals("WrappedVME1")) continue;
		else if(i->first.Equals("WrappedVME2")) continue;
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
			else if(i->first.Equals("EndVME2")) continue;
			else if(i->first.Equals("WrappedVME")) continue;
			else if(i->first.Equals("WrappedVME1")) continue;
			else if(i->first.Equals("WrappedVME2")) continue;
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
albaVME* albaVMEComputeWrapping::IndexToMiddlePointVME(int index)
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
//------------------------------------------------------old method over---------------------------

// 'Wrapped Meter' (under Operations->Create->Drive) is for single object wrapping whereas 'Multi-obj Wrapped Meter' is for sphere-capped cylindrical wrapping.
// Data requirements (for sphere-capped cylindrical wrapping program to work properly)
//
// 1. sphere radius > cylinder radius
// 2. start/origin point of action line should be outside the sphere and end/insertion point should preferrably be on the cylinder surface or outside.
//    When end points is outside the cylinder surface, the program performs wrapping over sphere object only.
// 3. The line conecting the start point and  the end point must intersect at least one of the two objects for wrapping to occur. Otherwise (when it does not intersect any object), it will simply connect the two points by a straight line.
// 4. "Wrapped sphere" (input button name in the interface) must be a parametric surface object of sphere type.
// 5. "Wrapped cylinder" must be a parametric surface object of cylinder type.
// 6. Start and end points can be either landmark vme or parametric surface vme.
// 7. 'pathNum' in interface indicates total number of paths considered to find the shortest one. Too lower value may have effect on accuracy and too high will have effect on speed.Default value is set to 36.
//
// This program is based on follwoing paper with necessary extensions/modifications:
// A. Audenaert and E. Audenaert, "Global optimization method for combined spherical-cylindrical wrapping in musculoskeletal upper limb modelling", Computer Methods and Programs in Biomedicine, 2008.