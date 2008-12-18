/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medvmecomputewrapping.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-18 13:37:43 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Anupam Agrawal and Hui Wei
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

#include "medVMEComputeWrapping.h"
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
#include "mafVMESurfaceParametric.h"
#include "mafMatrix3x3.h"
#include "vtkCellArray.h"
#include <math.h>
#include <assert.h>
//#include <vld.h>

MAF_ID_IMP(medVMEComputeWrapping::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEComputeWrapping)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEComputeWrapping::medVMEComputeWrapping()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  m_Angle         = 0.0;
  m_WrappedMode   = IOR_AUTOMATED_WRAP;
  m_WrapSide      = 0;
  m_WrapReverse   = 0;
  
  m_StartVmeName  = "";
  m_EndVme1Name   = "";
  m_EndVme2Name   = "";
  m_WrappedVmeName   = "";
  m_WrappedVmeName2 = "";
  m_ViaPointName = "";
  m_AbCurve = 0;

  m_Gui = NULL;
  m_ListBox = NULL;
  m_Idx = 0;
  
  mafNEW(m_Transform);
  mafNEW(m_TmpTransform2);
  medVMEOutputComputeWrapping *output = medVMEOutputComputeWrapping::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
  

  m_Mat = new mafMatrix3x3();
  m_Imat = new mafMatrix3x3();

  vtkNEW(m_LineSource);
  vtkNEW(m_LineSource2);
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
medVMEComputeWrapping::~medVMEComputeWrapping()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  vtkDEL(m_LineSource);
  vtkDEL(m_LineSource2);
  vtkDEL(m_Goniometer);
  mafDEL(m_TmpTransform);
  mafDEL(m_TmpTransform2);

  mafDEL(m_Mat);
  mafDEL(m_Imat);

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
int medVMEComputeWrapping::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEComputeWrapping *meter = medVMEComputeWrapping::SafeDownCast(a);
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
bool medVMEComputeWrapping::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((medVMEComputeWrapping *)vme)->m_Transform->GetMatrix() && \
          GetLink("StartVME") == ((medVMEComputeWrapping *)vme)->GetLink("StartVME") && \
          GetLink("EndVME1") == ((medVMEComputeWrapping *)vme)->GetLink("EndVME1") && \
          GetLink("EndVME2") == ((medVMEComputeWrapping *)vme)->GetLink("EndVME2");
  }
  return ret;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::InternalInitialize()
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
mmaMaterial *medVMEComputeWrapping::GetMaterial()
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
medVMEOutputComputeWrapping *medVMEComputeWrapping::GetWrappedMeterOutput()
//-------------------------------------------------------------------------
{
  return (medVMEOutputComputeWrapping *) GetOutput();
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool medVMEComputeWrapping::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void medVMEComputeWrapping::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void medVMEComputeWrapping::InternalUpdate()
//-----------------------------------------------------------------------
{
	mafVME *wrapped_vme1 = GetWrappedVME1();
	mafVME *wrapped_vme2 = GetWrappedVME2();
	if( wrapped_vme1 && wrapped_vme2){
		dispatch(); 
	}

	
	
}
void medVMEComputeWrapping::dispatch(){
	int obbtreeFlag = 0;
	int obbtreeFlag1 = 0;
	int obbtreeFlag2 = 0;
	double local_start[3];
	double local_via[3];
	double local_end[3];
	double local_wrapped_center1[3];
	double local_wrapped_center2[3];
	double cosA,sinA,cosB,sinB;//used for get transform matrix
	vtkMAFSmartPointer<vtkOBBTree> locator1;
	vtkMAFSmartPointer<vtkOBBTree> locator2;

	mafVME *wrapped_vme1 = GetWrappedVME1();
	mafVME *wrapped_vme2 = GetWrappedVME2();
	vtkMAFSmartPointer<vtkPoints> temporaryIntersection;

	//obbtreeFlag1 = prepareData(1,local_start,local_via,local_wrapped_center1,locator1);
	//obbtreeFlag2 = prepareData(2,local_start,local_via,local_wrapped_center2,locator2);
	prepareData2();
	obbtreeFlag1 =1;
	obbtreeFlag2 = 1;
	if (obbtreeFlag1 == 1 && obbtreeFlag2 == 1 )
	{
		//invoke new method
		//get cylinder tangent as point i first
		
		vtkMAFSmartPointer<vtkPoints> pointsIntersection1;
		vtkMAFSmartPointer<vtkPoints> pointsIntersection2;
		bool aligned = false;
		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	//-------------prepare i point

		//doubleWrap(local_start,local_via,local_wrapped_center1,local_wrapped_center2,locator1,locator2,transformFirstData);
		//formatData();
		
		bool vFlag = GetViaPoint(m_ViaPoint);
		if (vFlag)
		{

			//------------prepare matrix--------------
			getWrapMatrix(m_InMx,m_InMy);
			mafMatrix::Invert(m_InMy,m_OutMy);
			mafMatrix::Invert(m_InMx,m_OutMx);

			cosA=0;sinA=0;cosB=0;sinB=0;
			//step1
			getAngleAandB(cosA,sinA,cosB,sinB);
			//step2
			getTransFormMatrix(cosA,sinA,cosB,sinB,m_Mat);//after this function mat has element value
			mafMatrix3x3::Invert(m_Mat->GetElements(),m_Imat->GetElements());//imat->Invert();
			

			//-----------------------------------------

			getEveryABCpoint(36);
		}else{

			directConnectSE();
		}
		
	}else{ //only intersect one Object
		//InternalUpdateAutomatedIOR();	
		if (obbtreeFlag1 == 1 && obbtreeFlag2 != 1)
		{	
			singleWrapAutomatedIOR(wrapped_vme1,local_start,local_end,local_wrapped_center1,locator1);
		}else if (obbtreeFlag2 == 1 && obbtreeFlag1 != 1)
		{
			singleWrapAutomatedIOR(wrapped_vme2,local_start,local_end,local_wrapped_center2,locator2);
		}else{//draw a straight line
			
			m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
			m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);

			m_Goniometer->AddInput(m_LineSource->GetOutput());
			m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
			GetWrappedMeterOutput()->Update();
		}
	}


}
void medVMEComputeWrapping::directConnectSE(){
	double d0;
	vtkLineSource *SE;
	vtkNEW(SE);
	m_Goniometer->RemoveAllInputs();

	SE->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);
	SE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
	m_Goniometer->AddInput(SE->GetOutput());
	d0 = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint,m_EndPoint));
	m_Distance = d0;

	m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update(); 
}

double medVMEComputeWrapping::testPlane(double *a,double *b,double *c,double *d){
	double ab[3],ac[3],cd[3],normalTmp[3];
	ab[0] = a[0]-b[0];
	ab[1] = a[1]-b[1];
	ab[0] = a[2]-b[2];

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
double medVMEComputeWrapping::getPi(){
	
	return 3.1415926;
}
double medVMEComputeWrapping::getSphereRadius(){
	double rtn = 0;
	mafVMESurfaceParametric *sphere = mafVMESurfaceParametric::SafeDownCast(GetWrappedVME1());
	rtn = sphere->GetSphereRadius();
	return rtn;

}
double medVMEComputeWrapping::getCylinderRadius(){
	double rtn = 0;
	mafVMESurfaceParametric *cylinder = mafVMESurfaceParametric::SafeDownCast(GetWrappedVME2());
	//rtn = cylindar->//(*cylindar).m_CylinderRadius;
	if (cylinder)
	{
		rtn = cylinder->GetCylinderRadius();
	}
	
	//rtn = 1;
	return rtn;
}
bool medVMEComputeWrapping::getCcoordinate2(double *bCoord,double *cCoord1,double *cCoord2){
	double a,b,c,r;//r is radius of cylinder
	double Yc,Xc,Zc,Yc2,Xc2,Zc2;
	double Xb,Yb,Zb;
	double Xr,Yr,Zr,R;
	double A,B,C,D;
	bool rtn = true;
	double testValue1,testValue2;

	r = getCylinderRadius();
	R = getSphereRadius();

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
double medVMEComputeWrapping::getFunctionValue2(double x,double filterFlag,double *filterPlaneNormal,double *outAPoint,double *outBPoint,double *outMPoint,double & Rm){
	double rtn = 0,rtn1 = 0,rtn2 = 0,rtn3 = 0,rtn4 = 0;
	
	//double oCoordTransform[3];
	double sphereR;
	double alCoord1[3],alCoord2[3],agCoord[3],ag2Coord[3],aCoordR[3],aCoordR1[3],aCoordR2[3];
	double mCoordR[3],mCoordR1[3],mCoordR2[3],mgCoord[3],mgCoord1[3],mgCoord2[3],mgCoord3[3],mgCoord4[3];
	double igCoord[3],iCoordR[3],ogCoord[3],olCoord[3],oCoordR[3];
	double bCoordR1[3],bCoordR2[3],bCoordR3[3],bCoordR4[3],bgCoord1[3],bgCoord2[3],bgCoord3[3],bgCoord4[3];
	double arc;
	double testValue;
	double Rm1,Rm2;
	sphereR = getSphereRadius();
	//-----get o global coord---------
	ogCoord[0]=m_StartWrapLocal[0];
	ogCoord[1]=m_StartWrapLocal[1];
	ogCoord[2]=m_StartWrapLocal[2];

	igCoord[0]=m_ViaWrapLocal[0];
	igCoord[1]=m_ViaWrapLocal[1];
	igCoord[2]=m_ViaWrapLocal[2];
	//--------------------------------
	
	Rm =0;
	Rm1 = 0;
	Rm2 = 0;

	
	//step3 o global --> o local so we can use equation(10)
	getLocalTransformedCoord(olCoord,ogCoord,m_Imat);
	//step4 get A point local coordinate;
	bool aFlag = getAcoordinateInLocal(alCoord1,alCoord2,olCoord,sphereR,x);
	if (aFlag)
	{

		//step5 a local --> a global
		double agCoord1[3],agCoord2[3],aGcoord1[3],aGcoord2[3],sphereCenter[3];

		getGlobalTransformedCoord(alCoord1,agCoord1,m_Mat);
		getGlobalTransformedCoord(alCoord2,agCoord2,m_Mat);
		getGlobalSphereCenter(sphereCenter);

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

		
		getWrapGlobalTransform(agCoord1,aGcoord1);
		getWrapGlobalTransform(agCoord2,aGcoord2);

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
					copyPointValue(agCoord1,agCoord);
					aFlag1 = true;
				}
				
			}else if (m_WrapSide==WRAP_SHOREST)
			{
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
					copyPointValue(agCoord2,agCoord);
					aFlag2 = true;
				}
			}else if (m_WrapSide==WRAP_SHOREST)
			{
				aFlag2 = true;
			}
		}
			
		//step6 o,i,a global --> sphere center based local
		getLocalCenterCoord(oCoordR,m_StartWrapLocal);
		getLocalCenterCoord(iCoordR,m_ViaWrapLocal);

		int ibflag = 1;
		if ( !aFlag2 && !aFlag1)
		{
			return -1;
		}else if (aFlag2 && aFlag1)
		{	//----------first a-----------
			getLocalCenterCoord(aCoordR1,agCoord1);
			//step7 get circle which intersect with sphere ,appendix D
			getIntersectCircle(aCoordR1,oCoordR,iCoordR,mCoordR1,Rm1);//Rm is computed by global coord
			bool bFlag1 =getBcoordinate(aCoordR1,oCoordR,iCoordR,mCoordR1,Rm1,sphereR,bCoordR1,bCoordR2);
			
			//-----------second a---------
			getLocalCenterCoord(aCoordR2,agCoord2);
			getIntersectCircle(aCoordR2,oCoordR,iCoordR,mCoordR2,Rm2);//Rm is computed by global coord
			bool bFlag2 =getBcoordinate(aCoordR2,oCoordR,iCoordR,mCoordR2,Rm2,sphereR,bCoordR3,bCoordR4);			
			
			if (bFlag1)
			{
				getGlobalCenterCoord(bgCoord1,bCoordR1);
				getGlobalCenterCoord(bgCoord2,bCoordR2);
				getGlobalCenterCoord(mgCoord1,mCoordR1);
				
				rtn1 = computeDistanceOfLm(ogCoord,igCoord,agCoord1,bgCoord1,mgCoord1,Rm1);
				rtn2 = computeDistanceOfLm(ogCoord,igCoord,agCoord1,bgCoord2,mgCoord1,Rm1);
			}else{
				rtn1 = 0;
				rtn2 = 0;
			}

			if (bFlag2)
			{
				getGlobalCenterCoord(bgCoord3,bCoordR3);
				getGlobalCenterCoord(bgCoord4,bCoordR4);
				getGlobalCenterCoord(mgCoord2,mCoordR2);
				
				rtn3 = computeDistanceOfLm(ogCoord,igCoord,agCoord2,bgCoord3,mgCoord2,Rm2);
				rtn4 = computeDistanceOfLm(ogCoord,igCoord,agCoord2,bgCoord4,mgCoord2,Rm2);
			}else{
				rtn3 = 0;
				rtn4 = 0;
			}

			if (rtn1!=0 && rtn1<rtn2 && rtn1<rtn3 && rtn1<rtn4)
			{
				Rm = Rm1;
				copyPointValue(agCoord1,m_APoint);
				copyPointValue(agCoord1,outAPoint);
				copyPointValue(bgCoord1,m_BPoint);
				copyPointValue(bgCoord1,outBPoint);
				getGlobalCenterCoord(outMPoint,mCoordR1);
				rtn = rtn1;
			}else if (rtn2!=0 && rtn2<rtn1 && rtn2<rtn3 && rtn2<rtn4)
			{
				Rm = Rm1;
				copyPointValue(agCoord1,m_APoint);
				copyPointValue(agCoord1,outAPoint);
				copyPointValue(bgCoord2,m_BPoint);
				copyPointValue(bgCoord2,outBPoint);
				getGlobalCenterCoord(outMPoint,mCoordR1);
				rtn = rtn2;
			}else if (rtn3!=0 && rtn3<rtn1 && rtn3<rtn2 && rtn3<rtn4)
			{
				Rm = Rm2;
				copyPointValue(agCoord2,m_APoint);
				copyPointValue(agCoord2,outAPoint);
				copyPointValue(bgCoord3,m_BPoint);
				copyPointValue(bgCoord3,outBPoint);
				getGlobalCenterCoord(outMPoint,mCoordR2);
				rtn = rtn3;
			}else if (rtn4!=0 && rtn4<rtn1 && rtn4<rtn2 && rtn4<rtn3)
			{
				Rm = Rm2;
				copyPointValue(agCoord2,m_APoint);
				copyPointValue(agCoord2,outAPoint);
				copyPointValue(bgCoord4,m_BPoint);
				copyPointValue(bgCoord4,outBPoint);
				getGlobalCenterCoord(outMPoint,mCoordR2);
				rtn = rtn4;
			}else{

				return -1;
			}
			
		}else if( aFlag1 || aFlag2){
			getLocalCenterCoord(aCoordR,agCoord);
			//step7 get circle which intersect with sphere ,appendix D
			getIntersectCircle(aCoordR,oCoordR,iCoordR,mCoordR,Rm);//Rm is computed by global coord
			//step8 get B coord,there are 2 point ,test which one is the shortest.use appendix E,use the same translation as in step7
			getGlobalCenterCoord(outMPoint,mCoordR);
			bool bFlag =getBcoordinate(aCoordR,oCoordR,iCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);
			if (bFlag)
			{
				copyPointValue(agCoord,m_APoint);
				copyPointValue(agCoord,outAPoint);
				
				getGlobalCenterCoord(bgCoord1,bCoordR1);
				getGlobalCenterCoord(bgCoord2,bCoordR2);
				getGlobalCenterCoord(mgCoord,mCoordR);

				rtn1 = computeDistanceOfLm(ogCoord,igCoord,agCoord,bgCoord1,mgCoord,Rm);
				rtn2 = computeDistanceOfLm(ogCoord,igCoord,agCoord,bgCoord2,mgCoord,Rm);
				if (rtn1<rtn2)
				{
					copyPointValue(bgCoord1,m_BPoint);
					copyPointValue(bgCoord1,outBPoint);
				}else{
					copyPointValue(bgCoord2,m_BPoint);
					copyPointValue(bgCoord2,outBPoint);
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

void medVMEComputeWrapping::computeAngleForB(double *aCoord,double *bCoord1,double *bCoord2,double *bCoord){
	double oa[3],ba1[3],ba2[3];
	double d1,d2,cosA1,cosA2;
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
bool medVMEComputeWrapping::getBcoordinateUpdate2(double *aCoord,double *bCoord,double *cCoord ,double *bCoord1,double *bCoord2){
	double blCoord[3];
	getLocalCenterCoord(blCoord,bCoord);
	double Rm = getCylinderRadius();
	double sphereR = getSphereRadius();
	double bCoordR1[3],bCoordR2[3];
	double aCoordR[3],oCoordR[3],cCoordR[3],mCoordR[3],iCoordR[3];

	getLocalCenterCoord(aCoordR,aCoord);
	getLocalCenterCoord(oCoordR,m_StartWrapLocal);
	getLocalCenterCoord(cCoordR,cCoord);
	getIntersectCircle(aCoordR,oCoordR,cCoordR,mCoordR,Rm);//Rm is computed by global coord

	bool rtn =getBcoordinate(aCoordR,oCoordR,cCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);

	getGlobalCenterCoord(bCoord1,bCoordR1);
	getGlobalCenterCoord(bCoord2,bCoordR2);
	return rtn;
}




void medVMEComputeWrapping::processViaPoint(){
	double r = getCylinderRadius();
	double x,y,z,distX,distY;
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

void medVMEComputeWrapping::copyPointValue(double *srcPoint,double *aimPoint){
	aimPoint[0] =	srcPoint[0] ;
	aimPoint[1] =	srcPoint[1] ;
	aimPoint[2] =	srcPoint[2] ;
}
void medVMEComputeWrapping::getEveryABCpoint(const int step){
	//const int step = 36;
	Apoint *pA,*pB,*pC;
	double dstep,d1,d2,y1,rm = 0;
	double y0 = 0;
	double x0 = 0;
	double aCoord[3],bCoord[3],aCoord2[3],bCoord2[3],bCoord1[3],cCoord1[3],cCoord2[3],cCoord[3],mCoord[3];
	bool cflag;
	double length = 0;
	double low,high,testValue;
	double testValue1,testValue2;
	double tanValue1,tanValue2,tanCost1,tanCost2;
	double cylinderR;
	int ibflag;
	vtkLineSource     *OA,*AB,*BC,*CI,*IE,*AR,*RB,*BI,*CA;
	vtkPolyData *hcurve;
	
	mafString logFname = "wholePathForAngle.txt";
	mafString logFname2 = "chartFile.txt";
	std::ofstream outputFile(logFname, std::ios::out);
	std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);
	int idx = 0;
	double aCoordFinal[3],bCoordFinal[3],cCoordFinal[3],mCoordFinal[3],rmFinal = 0,tmpLm = 0,LmFinal = 0,angleFinal = 0;
	double OAlength,ABcurve,BClength,CIcurve;

	double sphereCenter[3],cylinderCenter[3];
	double filterFlag =0,filterPlaneNormal[3];
	
	//outputFile << "write log for every angle and whole path length for this angle" << '\t'<<'\n';
	outputFile2<<"write log for every angle and path length for OA-ABcurve-BI"<<std::endl;
	outputFile.clear();
	low = 0;
	high = 2 * getPi();
	m_Goniometer->RemoveAllInputs();

	cylinderR = getCylinderRadius();
	vtkClipPolyData *clipData; 

	vtkClipPolyData *clipDataCI;
	
	dstep = (high-low)/step;


	getGlobalSphereCenter(sphereCenter);
	getGlobalCylinderCenter(cylinderCenter);

	getWrapLocalTransform(sphereCenter,m_SphereWrapLocal);
	//double convertCoord[3];
	//getWrapGlobalTransform(m_sphereWrapLocal,convertCoord);
	getWrapLocalTransform(m_StartPoint,m_StartWrapLocal);
	getWrapLocalTransform(m_ViaPoint,m_ViaWrapLocal);

//------------transform start ,sphere center,insertion 

	if (m_WrapSide==WRAP_FRONT || m_WrapSide == WRAP_BACK)
	{
		filterFlag = getFilterFlag(filterPlaneNormal);
	}
	
/*
	double viaProjectWrapLocal[3],viaProject90WrapLocal[3];//viaPoint with sphere z coord
	double cylinderCenterWrapLocal[3],filterPlaneNormal[3],filterVector1[3];

	cylinderCenterWrapLocal[0] = 0;cylinderCenterWrapLocal[1] = 0;cylinderCenterWrapLocal[2] = 0;
	copyPoint(m_viaWrapLocal,viaProjectWrapLocal);
	viaProjectWrapLocal[2] = m_sphereWrapLocal[2];
	get90DegreePointOnCylinder(viaProjectWrapLocal,m_sphereWrapLocal,viaProject90WrapLocal);

	vtkPlaneSource *planeSource;
	vtkNEW(planeSource);
	planeSource->SetOrigin(viaProject90WrapLocal);
	planeSource->SetPoint1(m_sphereWrapLocal);
	planeSource->SetPoint2(cylinderCenterWrapLocal);
	planeSource->GetNormal(filterPlaneNormal);
	filterVector1[0] = m_viaWrapLocal[0] - m_sphereWrapLocal[0];
	filterVector1[1] = m_viaWrapLocal[1] - m_sphereWrapLocal[1];
	filterVector1[2] = m_viaWrapLocal[2] - m_sphereWrapLocal[2];
	double filterFlag = vtkMath::Dot(filterPlaneNormal,filterVector1);
*/	


	
	for (double j=low;j<high;j = j+dstep)
	{

		length = 0;
		
		vtkNEW(clipData);
		vtkNEW(clipDataCI);

		y1 = getFunctionValue2(j,filterFlag,filterPlaneNormal,aCoord,bCoord,mCoord,rm);//m is global

		if (y1 != -1)
		{
		testValue = testPlane(aCoord,bCoord,m_StartWrapLocal,m_ViaWrapLocal);

		for (int i=0;i<10;i++)
		{
			cflag = getCcoordinate2(bCoord,cCoord1,cCoord2);
			d1 = computeDistanceOfLm2(m_StartWrapLocal,m_ViaWrapLocal,aCoord,bCoord,cCoord1,rm);
			d2 = computeDistanceOfLm2(m_StartWrapLocal,m_ViaWrapLocal,aCoord,bCoord,cCoord2,rm);
			cCoord[0] = cCoord1[0];cCoord[1] = cCoord1[1];cCoord[2] = cCoord1[2];
			if (d1>d2)
			{
				cCoord[0] = cCoord2[0];cCoord[1] = cCoord2[1];cCoord[2] = cCoord2[2];
			}		
			if (cflag)
			{
				testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
				testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;
				
				getBcoordinateUpdate2(aCoord,bCoord,cCoord,bCoord1,bCoord2);

				testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
				testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;
				
				tanValue1 = bCoord1[0]*cCoord[0]+bCoord1[1]*cCoord[1];
				tanValue2 = bCoord2[0]*cCoord[0]+bCoord2[1]*cCoord[1];

				tanCost1 = fabs(tanValue1-(cylinderR*cylinderR));
				tanCost2 = fabs(tanValue2 - (cylinderR*cylinderR));
				
				d1 = computeDistanceOfLm2(m_StartWrapLocal,m_ViaWrapLocal,aCoord,bCoord1,cCoord,rm);
				d2 = computeDistanceOfLm2(m_StartWrapLocal,m_ViaWrapLocal,aCoord,bCoord2,cCoord,rm);
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
			testValue = testPlane(aCoord,bCoord,m_StartWrapLocal,cCoord1);
			testValue = testPlane(aCoord,bCoord,m_StartWrapLocal,cCoord2);
		}//end of for
		
		testValue1 = cCoord[0]*cCoord[0]+cCoord[1]*cCoord[1];//Xc*Xc +Yc*Yc = r*r
		testValue2 = bCoord[0]*cCoord[0]+bCoord[1]*cCoord[1];//Xc*Xb + Yc*Yb = r*r;

		
		double aGcoord[3],bGcoord[3],cGcoord[3],mGcoord[3];
		getWrapGlobalTransform(aCoord,aGcoord);
		getWrapGlobalTransform(bCoord,bGcoord);
		getWrapGlobalTransform(cCoord,cGcoord);
		getWrapGlobalTransform(mCoord,mGcoord);

		//----------compute whole length--------------
		//------------store data---------------------
		OAlength = sqrt(vtkMath::Distance2BetweenPoints(aGcoord,m_StartPoint));
		ABcurve = getCutPlane2(aGcoord,bGcoord,mGcoord,clipData);//AB curve tmp
		BClength = sqrt(vtkMath::Distance2BetweenPoints(bGcoord,cGcoord));
		CIcurve = getCutPlaneForCI(bGcoord,cGcoord,clipDataCI); //tmp
		tmpLm = OAlength + ABcurve + BClength + CIcurve;

		if (idx==0 || (tmpLm<LmFinal) )//first time
		{
			if (!(cCoord[2]<m_ViaWrapLocal[2])){
				idx++;
				angleFinal = j;
				LmFinal = tmpLm;
				aCoordFinal[0] = aGcoord[0];aCoordFinal[1]=aGcoord[1];aCoordFinal[2]=aGcoord[2];
				bCoordFinal[0] = bGcoord[0];bCoordFinal[1]=bGcoord[1];bCoordFinal[2]=bGcoord[2];
				cCoordFinal[0] = cGcoord[0];cCoordFinal[1]=cGcoord[1];cCoordFinal[2]=cGcoord[2];
				mCoordFinal[0] = mGcoord[0];mCoordFinal[1]=mGcoord[1];mCoordFinal[2]=mGcoord[2];
				rmFinal = rm;
				outputFile << "for angle :"<<j<< ", length for whole path is :   " <<tmpLm<<"  ."<< '\t'<<'\n';
				outputFile2<<"  "<<j<<"  "<<tmpLm<<std::endl;
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
		vtkNEW(IE);

		vtkNEW(clipData);
		vtkNEW(clipDataCI);


		OA->SetPoint1(m_StartPoint[0],m_StartPoint[1],m_StartPoint[2]);//OA
		OA->SetPoint2(aCoordFinal[0],aCoordFinal[1],aCoordFinal[2]);

		ABcurve = getCutPlane2(aCoordFinal,bCoordFinal,mCoordFinal,clipData);//AB curve

		BC->SetPoint1(bCoordFinal[0],bCoordFinal[1],bCoordFinal[2]);
		BC->SetPoint2(cCoordFinal[0],cCoordFinal[1],cCoordFinal[2]);

		CIcurve = getCutPlaneForCI(bCoordFinal,cCoordFinal,clipDataCI);

		IE->SetPoint1(m_ViaPoint[0],m_ViaPoint[1],m_ViaPoint[2]);
		IE->SetPoint2(m_EndPoint[0],m_EndPoint[1],m_EndPoint[2]);
		//----------test--------------


		m_Goniometer->AddInput(OA->GetOutput());
		m_Goniometer->AddInput(clipData->GetOutput());
		m_Goniometer->AddInput(BC->GetOutput());
		m_Goniometer->AddInput(clipDataCI->GetOutput());
		m_Goniometer->AddInput(IE->GetOutput());

		//----------test--------------
		vtkDEL(OA);
		vtkDEL(BC);
		vtkDEL(IE);

		vtkDEL(clipData);
		vtkDEL(clipDataCI);

		outputFile.close();
		outputFile2.close();
		m_Distance = LmFinal;

		m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
		GetWrappedMeterOutput()->Update(); 

	}else{
		directConnectSE();
	}


}

double medVMEComputeWrapping::getFilterFlag(double *filterPlaneNormal){
	double viaProjectWrapLocal[3],viaProject90WrapLocal[3];//viaPoint with sphere z coord
	double cylinderCenterWrapLocal[3],filterVector1[3];
	double rtn = 0;
	cylinderCenterWrapLocal[0] = 0;cylinderCenterWrapLocal[1] = 0;cylinderCenterWrapLocal[2] = 0;
	copyPointValue(m_ViaWrapLocal,viaProjectWrapLocal);
	viaProjectWrapLocal[2] = m_SphereWrapLocal[2];
	get90DegreePointOnCylinder(viaProjectWrapLocal,m_SphereWrapLocal,viaProject90WrapLocal);

	vtkPlaneSource *planeSource;
	vtkNEW(planeSource);
	planeSource->SetOrigin(viaProject90WrapLocal);
	planeSource->SetPoint1(m_SphereWrapLocal);
	planeSource->SetPoint2(cylinderCenterWrapLocal);
	planeSource->GetNormal(filterPlaneNormal);
	filterVector1[0] = m_ViaWrapLocal[0] - m_SphereWrapLocal[0];
	filterVector1[1] = m_ViaWrapLocal[1] - m_SphereWrapLocal[1];
	filterVector1[2] = m_ViaWrapLocal[2] - m_SphereWrapLocal[2];
	rtn = vtkMath::Dot(filterPlaneNormal,filterVector1);
	//----------------test------------------
	/*double viaProject90[3];
	double sphereCenter[3],cylinderCenter[3];
	getGlobalSphereCenter(sphereCenter);
	getGlobalCylinderCenter(cylinderCenter);

	getWrapGlobalTransform(viaProject90WrapLocal,viaProject90);
	vtkLineSource *planeLine1,*planeLine2;
	vtkNEW(planeLine1);
	vtkNEW(planeLine2);
	planeLine1->SetPoint1(cylinderCenter[0],cylinderCenter[1],cylinderCenter[2]);
	planeLine1->SetPoint2(sphereCenter[0],sphereCenter[1],sphereCenter[2]);

	planeLine2->SetPoint1(cylinderCenter[0],cylinderCenter[1],cylinderCenter[2]);
	planeLine2->SetPoint2(viaProject90[0],viaProject90[1],viaProject90[2]);

	m_Goniometer->AddInput(planeLine1->GetOutput());
	m_Goniometer->AddInput(planeLine2->GetOutput());

	vtkDEL(planeLine1);
	vtkDEL(planeLine2);*/
	//----------------test over-------------

	return rtn;

}
	bool medVMEComputeWrapping::GetViaPoint(double *viaPoint){
		double startPoint[3],endPoint[3];
		double tmpIntersect1[3],tmpIntersect2[3];
		double d1,d2;
		bool rtn = false;

		vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
		vtkMAFSmartPointer<vtkPoints> pointsIntersection1;
		vtkMAFSmartPointer<vtkPoints> pointsIntersection2;

		// create ordered list of tangent point (2) real algorithm
		m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
		m_TmpTransform->Invert();
		m_TmpTransform->TransformPoint(m_StartPoint, startPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
		m_TmpTransform->TransformPoint(m_EndPoint, endPoint);  // m_TmpTransform needed to fix a memory leaks of GetInverse()


		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
		vtkMAFSmartPointer<vtkOBBTree> locator;
		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;

		transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)GetWrappedVME1())->GetAbsMatrixPipe()->GetVTKTransform());
		transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)GetWrappedVME1())->GetOutput()->GetVTKData());
		transformFirstDataInput->Update();

		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
		transformFirstData->Update(); 

		//-------test intersect---------------------
		//vtkMAFSmartPointer<vtkOBBTree> locator;
		locator->SetDataSet(transformFirstData->GetOutput());
		locator->SetGlobalWarningDisplay(0);
		locator->BuildLocator();
		
		if(locator->InsideOrOutside(startPoint) > 0 &&  locator->InsideOrOutside(endPoint) > 0) 
		{
			locator->IntersectWithLine(startPoint, endPoint, temporaryIntersection, NULL);	
			int num = temporaryIntersection->GetNumberOfPoints();
			if (num>0)
			{
				//vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
				transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)GetWrappedVME2())->GetAbsMatrixPipe()->GetVTKTransform());
				transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)GetWrappedVME2())->GetOutput()->GetVTKData());
				transformFirstDataInput->Update();

				//vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
				transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
				transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
				transformFirstData->Update(); 

				//-------test intersect---------------------
				//vtkMAFSmartPointer<vtkOBBTree> locator;
				locator->SetDataSet(transformFirstData->GetOutput());
				locator->SetGlobalWarningDisplay(0);
				locator->BuildLocator();


				//Control if Start or End point is inside vtk data (surface)
				if(locator->InsideOrOutside(startPoint) > 0 &&  locator->InsideOrOutside(endPoint) > 0) 
				{
					locator->IntersectWithLine(startPoint, endPoint, temporaryIntersection, NULL);	
					int num = temporaryIntersection->GetNumberOfPoints();
					if(num == 2)
					{
						temporaryIntersection->GetPoint(0,tmpIntersect1);
						temporaryIntersection->GetPoint(1,tmpIntersect2);
						d1 = vtkMath::Distance2BetweenPoints(endPoint,tmpIntersect1);
						d2 = vtkMath::Distance2BetweenPoints(endPoint,tmpIntersect2);

						if(d1<d2){
							viaPoint[0] = tmpIntersect1[0];viaPoint[1] = tmpIntersect1[1];viaPoint[2] = tmpIntersect1[2];
						}else{
							viaPoint[0] = tmpIntersect2[0];viaPoint[1] = tmpIntersect2[1];viaPoint[2] = tmpIntersect2[2];
						}
						rtn = true;


					}else if (num ==1)
					{
						temporaryIntersection->GetPoint(0,tmpIntersect1);
						viaPoint[0] = tmpIntersect1[0];viaPoint[1] = tmpIntersect1[1];viaPoint[2] = tmpIntersect1[2];
						rtn = true;
					}
				}
			}
		}


		return rtn;

	}

	void medVMEComputeWrapping::get90DegreePointOnCylinder(double *firstPoint,double *sphereCenter,double *secondPoint){
		double Px,Py,Cx,Cy,r;
		double A,B,C,a,b,c;
		r = getCylinderRadius();
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

//get ellipse
double medVMEComputeWrapping::getCutPlaneForCI(double *bCoord,double *cCoord,vtkClipPolyData *clipData){

		double rtn = 0;
		double midPoint[3];
		double vCi[3];
		double normal[3];
		m_TmpTransform->SetMatrix(*GetWrappedVME2()->GetOutput()->GetAbsMatrix());

		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)((mafVME *)GetWrappedVME2())->GetOutput()->GetVTKData());
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


double medVMEComputeWrapping::getCutPlane2(double *aPoint,double *bPoint,double *mPoint,vtkClipPolyData *clipData){

	double rtn = 0;
	double midPoint[3];

	m_TmpTransform2->SetMatrix(*GetWrappedVME1()->GetOutput()->GetAbsMatrix());

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform2->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)((mafVME *)GetWrappedVME1())->GetOutput()->GetVTKData());
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
double medVMEComputeWrapping::computeDistanceOfLm(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *rCoord,double rm){
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
double medVMEComputeWrapping::computeDistanceOfLm2(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double rm){
	
	double OA,AB2,BC,CI,ABcurve,rtn=0 ;

	OA = sqrt(vtkMath::Distance2BetweenPoints(oCoord,aCoord));
	AB2 = vtkMath::Distance2BetweenPoints(aCoord,bCoord);
	
	BC = sqrt(vtkMath::Distance2BetweenPoints(bCoord,cCoord));
	CI = sqrt(vtkMath::Distance2BetweenPoints(cCoord,iCoord));

	rtn = OA+ m_AbCurve+ BC +CI;
	
	return rtn;
}

void medVMEComputeWrapping::getLocalCenterCoord(double *localCoord,double *globalCoord){
	double sphCoord[3], tmpCoord[3];
	localCoord[0] = globalCoord[0] - m_SphereWrapLocal[0];
	localCoord[1] = globalCoord[1] - m_SphereWrapLocal[1];
	localCoord[2] = globalCoord[2] - m_SphereWrapLocal[2];
	
}
void medVMEComputeWrapping::getGlobalCenterCoord(double *globalCoord,double *localCoord){
	double sphCoord[3],tmpCoord[3];
	globalCoord[0] = localCoord[0] + m_SphereWrapLocal[0];
	globalCoord[1] = localCoord[1] + m_SphereWrapLocal[1];
	globalCoord[2] = localCoord[2] + m_SphereWrapLocal[2];
	
}
void medVMEComputeWrapping::getGlobalSphereCenter(double *sphCoord){
	sphCoord[0] = m_WrappedVMECenter1[0];
	sphCoord[1] = m_WrappedVMECenter1[1];
	sphCoord[2] = m_WrappedVMECenter1[2];
}
void medVMEComputeWrapping::getGlobalCylinderCenter(double *cylCoord){

	cylCoord[0] = m_WrappedVMECenter2[0];
	cylCoord[1] = m_WrappedVMECenter2[1];
	cylCoord[2] = m_WrappedVMECenter2[2];
}
// |x''|    -1 |x-Xr |
// |y''| = T   |y-Yr |            equation(8)
// |z''|       |z-Zr |
//from global get local
//imat is invert matrix
void medVMEComputeWrapping::getLocalTransformedCoord(double *localCoord,double *globalCoord,mafMatrix3x3 *imat){
	double sphCoord[3];
	double ele0,ele1,ele2;
	double tmpCoord[3];

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
void medVMEComputeWrapping::getGlobalTransformedCoord(double *localCoord,double *globalCoord,mafMatrix3x3 *mat){
	double sphCoord[3],tmpCoord[3];
	double ele0,ele1,ele2;
	for (int i=0;i<3;i++)
	{
		ele0 = mat->GetElement(i,0);
		ele1 = mat->GetElement(i,1);
		ele2 = mat->GetElement(i,2);
		globalCoord[i] = ele0*localCoord[0] + ele1*localCoord[1] + ele2*localCoord[2] + m_SphereWrapLocal[i];
	}
}
void medVMEComputeWrapping::getTransFormMatrix(double cosA,double sinA,double cosB,double sinB,mafMatrix3x3 *mat){

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

void medVMEComputeWrapping::getWrapMatrix(mafMatrix &inMx,mafMatrix &inMy){
	double sphereCenter[3],sphereCenterLocal[3],cylinderCenter[3],cylinderCenterLocal[3],normal1[3];
	double a,b,c,d;
	double u[3],uPj[3];
	double cosA,sinA,cosB,sinB;
	double xSphereCenterLocal[3],xySpherecenterLocal[3];
	double yMatrix[16],yIn[4],yOut[4];

	getGlobalSphereCenter(sphereCenter);
	getGlobalCylinderCenter(cylinderCenter);

	//step1 move to origin
	sphereCenterLocal[0] = sphereCenter[0]-cylinderCenter[0];
	sphereCenterLocal[1] = sphereCenter[1]-cylinderCenter[1];
	sphereCenterLocal[2] = sphereCenter[2]-cylinderCenter[2];

	cylinderCenterLocal[0] = cylinderCenter[0]-cylinderCenter[0];
	cylinderCenterLocal[1] = cylinderCenter[1]-cylinderCenter[1];
	cylinderCenterLocal[2] = cylinderCenter[2]-cylinderCenter[2];

	normal1[0] = sphereCenterLocal[0];normal1[1] = sphereCenterLocal[1];normal1[2] = sphereCenterLocal[2];

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
//transform coord from abs coord to a system defined by cylinder center and line (cylinder center and sphere center) as z axis
void medVMEComputeWrapping::getWrapLocalTransform(double *inCoord,double *outCoord){

	double sphereCenter[3],inCoordLocal[3],cylinderCenter[3],cylinderCenterLocal[3],normal1[3];
	double a,b,c,d;
	double u[3],uPj[3];
	double cosA,sinA,cosB,sinB;
	double xInCoordLocal[3],xyInCoordLocal[3];
	double yMatrix[16],yIn[4],yOut[4];

	getGlobalSphereCenter(sphereCenter);
	getGlobalCylinderCenter(cylinderCenter);

	//step1 move to origin
	inCoordLocal[0] = inCoord[0]-cylinderCenter[0];
	inCoordLocal[1] = inCoord[1]-cylinderCenter[1];
	inCoordLocal[2] = inCoord[2]-cylinderCenter[2];


	
	//x` = Rx * T * X
	xInCoordLocal[0] = inCoordLocal[0];
	xInCoordLocal[1] = inCoordLocal[1] * m_InMx.GetElement(1,1) + inCoordLocal[2]*m_InMx.GetElement(1,2);
	xInCoordLocal[2] = inCoordLocal[1] * m_InMx.GetElement(2,1) + inCoordLocal[2]*m_InMx.GetElement(2,2);
	
	//x` = Ry * Rx * T * X
	outCoord[0] = m_InMy.GetElement(0,0)*xInCoordLocal[0]+m_InMy.GetElement(0,2)*xInCoordLocal[2];
	outCoord[1] = xInCoordLocal[1];
	outCoord[2] = m_InMy.GetElement(2,0)*xInCoordLocal[0]+m_InMy.GetElement(2,2)*xInCoordLocal[2];
	
	
}
void medVMEComputeWrapping::getWrapGlobalTransform(double *inCoord,double *outCoord){

	double sphereCenter[3],inCoordLocal[3],cylinderCenter[3],cylinderCenterLocal[3],normal1[3];
	double a,b,c,d;
	double u[3],uPj[3];
	double cosA,sinA,cosB,sinB;
	double yInCoordLocal[3],xyInCoordLocal[3];
	double yMatrix[16],yIn[4],yOut[4];

	getGlobalSphereCenter(sphereCenter);
	getGlobalCylinderCenter(cylinderCenter);

	
	//X = Ry(-1)*X
	//step1 move to origin
	yInCoordLocal[0] = inCoord[0]*m_OutMy.GetElement(0,0) + inCoord[2]*m_OutMy.GetElement(0,2);
	yInCoordLocal[1] = inCoord[1];
	yInCoordLocal[2] = inCoord[0]*m_OutMy.GetElement(2,0) + inCoord[2]*m_OutMy.GetElement(2,2);
	//X = Rx(-1)*Ry(-1)*X
	xyInCoordLocal[0] = yInCoordLocal[0];
	xyInCoordLocal[1] = yInCoordLocal[1]*m_OutMx.GetElement(1,1) + yInCoordLocal[2]*m_OutMx.GetElement(1,2);
	xyInCoordLocal[2] = yInCoordLocal[1]*m_OutMx.GetElement(2,1) + yInCoordLocal[2]*m_OutMx.GetElement(2,2);

	outCoord[0] = xyInCoordLocal[0] + cylinderCenter[0];
	outCoord[1] = xyInCoordLocal[1] + cylinderCenter[1];
	outCoord[2] = xyInCoordLocal[2] + cylinderCenter[2];

}


void medVMEComputeWrapping::getWrapAngleAandB(double &cosA,double &sinA,double &cosB,double &sinB){
	double paraA[3],paraB[3],center[3],normal1[3],normal2[3];
	double cCcoord[3],sCcoord[3],orientation[3],sCcoord1[3],cCcoord1[3],sCpcoord[3];
	double distance;
	//---------get angle a----------
	getGlobalCylinderCenter(cCcoord);
	getGlobalSphereCenter(sCcoord);
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
void medVMEComputeWrapping::getAngleAandB(double & cosA,double & sinA,double & cosB,double & sinB){
	
	double olCoord[3],olprojectCoord[3];
	double center[3];
	mafVME *start_vme = GetStartVME();
	double orientation[3];
	double paraA[3],paraB[3];
	double distance;
    
	olCoord[0] = m_StartWrapLocal[0] - m_SphereWrapLocal[0];
	olCoord[1] = m_StartWrapLocal[1] - m_SphereWrapLocal[1];
	olCoord[2] = m_StartWrapLocal[2] - m_SphereWrapLocal[2];
	
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
	sinB = sqrt(1 - cosB * cosB);
	//------------@todo there are some cases that may affect cos value and sin value;
}
//olCoord is o'' coord point o local coordinate.
//x is a global and unique variable
//R is radius of sphere
bool medVMEComputeWrapping::getAcoordinateInLocal(double *Ap1,double *Ap2,double *olCoord,double R,double x){
	double A,B;
	double sinA1 ,sinA2,cosA1,cosA2;
	double cosA,sinA;
	
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
		if (testValue1>0.01 || testValue2>0.01)
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
void medVMEComputeWrapping::getIntersectCircle(double *Ap,double *oCoord,double *iCoord,double *mCoord,double & Rm){
	//ux +vy +wz +t = 0;
	double u,v,w,t,tmp ;
	double mgCoord[3];
	double agCoord[3];
	double testValue;

	getUVWT(Ap,oCoord,iCoord,u,v,w,t);//plane

	tmp = (-t)/( u*u + v*v + w*w ) ;
	mCoord[0] = u * tmp;
	mCoord[1] = v * tmp;
	mCoord[2] = w * tmp;
	testValue = testPlane(Ap,oCoord,iCoord,mCoord);
	testValue = testPlane(Ap,oCoord,iCoord,Ap);
	//------------------------
	getGlobalCenterCoord(mgCoord,mCoord);
	m_MPoint[0] = mgCoord[0];m_MPoint[1] = mgCoord[1];m_MPoint[2] = mgCoord[2];
	getGlobalCenterCoord(agCoord,Ap);
	Rm = sqrt(vtkMath::Distance2BetweenPoints(agCoord,  mgCoord)) ;
	Rm = sqrt( (mgCoord[0]-agCoord[0])*(mgCoord[0]-agCoord[0]) + (mgCoord[1]-agCoord[1])*(mgCoord[1]-agCoord[1]) + (mgCoord[2]-agCoord[2])*(mgCoord[2]-agCoord[2]) );
	testValue = testPlane(Ap,oCoord,iCoord,mCoord);
}

void medVMEComputeWrapping::getUVWT(double *Ap,double *oCoord,double *iCoord,double & u,double & v,double & w,double & t){
	double col1[3],col2[3],col3[3];
	//-------------------u----
	col1[0] = oCoord[1];  col2[0] = oCoord[2];  col3[0] = 1;
	col1[1] = iCoord[1];  col2[1] = iCoord[2];  col3[1] = 1;
	col1[2] = Ap[1];      col2[2] = Ap[2];      col3[2] = 1;

	u = detValue(col1,col2,col3);
	//-------------------v----
	col1[0] = oCoord[0];  col2[0] = oCoord[2];  col3[0] = 1;
	col1[1] = iCoord[0];  col2[1] = iCoord[2];  col3[1] = 1;
	col1[2] = Ap[0];      col2[2] = Ap[2];      col3[2] = 1;
	v = -detValue(col1,col2,col3);
	//-------------------w----
	col1[0] = oCoord[0];  col2[0] = oCoord[1];  col3[0] = 1;
	col1[1] = iCoord[0];  col2[1] = iCoord[1];  col3[1] = 1;
	col1[2] = Ap[0];      col2[2] = Ap[1];      col3[2] = 1;
	w = detValue(col1,col2,col3);
	//-------------------t----
	col1[0] = oCoord[0];  col2[0] = oCoord[1];  col3[0] = oCoord[2];
	col1[1] = iCoord[0];  col2[1] = iCoord[1];  col3[1] = iCoord[2];
	col1[2] = Ap[0];      col2[2] = Ap[1];      col3[2] = Ap[2];
	t = -detValue(col1,col2,col3);
	//------------------------

}
double medVMEComputeWrapping::detValue(double *col1,double *col2,double *col3){
	double rtn = 0;
	double pRtn = col1[0]*col2[1]*col3[2] + col1[1]*col2[2]*col3[0] + col2[0]*col3[1]*col1[2];
	double nRtn = col3[0]*col2[1]*col1[2] + col3[1]*col2[2]*col1[0] + col2[0]*col1[1]*col3[2];
	rtn = pRtn - nRtn;
	return rtn;
}
//appendix E
bool medVMEComputeWrapping::getBcoordinate(double *Ap,double *oCoord,double *iCoord,double *mCoord,double Rm,double R,double *bCoord1,double *bCoord2){
// B on plane
//( Xb-Xr)2+(Yb-Yr)2+(Zb-Zr)2 = R2
//( Xb-Xi)2+(Yb-Yi)2+(Zb-Zi)2+rm2 = (Xm-Xi)2+(Ym-Yi)2+(Zm-Zi)2
	double u,v,w,t;
	double A,B,C,D,E,F,G,H,I,J;
	double a,b,c;
	double delta = 0;
	bool rtn = true;
	double testValue;
	
	testValue = testPlane(Ap,oCoord,iCoord,mCoord);
	getUVWT(Ap,oCoord,iCoord,u,v,w,t);//plane

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
	testValue = testPlane(Ap,oCoord,iCoord,bCoord1);
	testValue = testPlane(Ap,oCoord,iCoord,bCoord2);
	return rtn;
	
}
bool medVMEComputeWrapping::getBcoordinateUpdate(double *cCoord ,double *bCoord1,double *bCoord2){
    double blCoord[3];
	getLocalCenterCoord(blCoord,m_BPoint);
	double Rm = getCylinderRadius();
	double sphereR = getSphereRadius();
	double bCoordR1[3],bCoordR2[3];
	double aCoordR[3],oCoordR[3],cCoordR[3],mCoordR[3],iCoordR[3];
	
	getLocalCenterCoord(aCoordR,m_APoint);
	getLocalCenterCoord(oCoordR,m_StartPoint);
	getLocalCenterCoord(cCoordR,cCoord);
	
	getIntersectCircle(aCoordR,oCoordR,cCoordR,mCoordR,Rm);//Rm is computed by global coord
	
	bool rtn =getBcoordinate(aCoordR,oCoordR,cCoordR,mCoordR,Rm,sphereR,bCoordR1,bCoordR2);
	
	getGlobalCenterCoord(bCoord1,bCoordR1);
	getGlobalCenterCoord(bCoord2,bCoordR2);
	return rtn;
}
//appendix F
bool medVMEComputeWrapping::getCcoordinate(double *cCoord1,double *cCoord2){
	double a,b,c,r;//r is radius of cylinder
	double Yc,Xc,Zc,Yc2,Xc2,Zc2;
	double Xb,Yb,Zb;
	double RCoord[3],Xr,Yr,Zr,R;
	double A,B,C,D;
	bool rtn = true;
	
	r = getCylinderRadius();
	R = getSphereRadius();
	
	getGlobalSphereCenter(RCoord);//coordinate of sphere center;
	
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

void medVMEComputeWrapping::prepareData2(){
	mafVME *start_vme = GetStartVME();
	mafVME *end_vme   = GetEnd1VME();
	mafVME *wrapped_vme = GetWrappedVME1();
	mafVME *wrapped_vme2 = GetWrappedVME2();

	double orientation[3];
	if (start_vme && end_vme && wrapped_vme && wrapped_vme2)
	{

		//---------------startPoint-----------------------------------
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
		//---------------endPoint-----------------------------------
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
		//---------------sphere center-----------------------------------
		if(wrapped_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("WrappedVME") != -1)
		{
			((mafVMELandmarkCloud *)wrapped_vme)->GetLandmark(GetLinkSubId("WrappedVME"),m_WrappedVMECenter1,-1);
			m_TmpTransform->SetMatrix(*wrapped_vme->GetOutput()->GetAbsMatrix());
			m_TmpTransform->TransformPoint(m_WrappedVMECenter1,m_WrappedVMECenter1);
		}
		else
		{
			wrapped_vme->GetOutput()->GetAbsPose(m_WrappedVMECenter1, orientation);

			double translation[3];
			((vtkPolyData *)wrapped_vme->GetOutput()->GetVTKData())->GetCenter(translation);

			mafMatrix mat;
			mat.Identity();
			for(int i=0; i<3; i++)
				mat.SetElement(i,3,translation[i]);
			m_TmpTransform->SetMatrix(mat);
			m_TmpTransform->TransformPoint(m_WrappedVMECenter1,m_WrappedVMECenter1);
		}
		//---------------cylinder center-----------------------------------
		if(wrapped_vme2->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("WrappedVME") != -1)
		{
			((mafVMELandmarkCloud *)wrapped_vme2)->GetLandmark(GetLinkSubId("WrappedVME"),m_WrappedVMECenter2,-1);
			m_TmpTransform->SetMatrix(*wrapped_vme2->GetOutput()->GetAbsMatrix());
			m_TmpTransform->TransformPoint(m_WrappedVMECenter2,m_WrappedVMECenter2);
		}
		else
		{
			wrapped_vme2->GetOutput()->GetAbsPose(m_WrappedVMECenter2, orientation);

			double translation[3];
			((vtkPolyData *)wrapped_vme2->GetOutput()->GetVTKData())->GetCenter(translation);

			mafMatrix mat;
			mat.Identity();
			for(int i=0; i<3; i++)
				mat.SetElement(i,3,translation[i]);
			m_TmpTransform->SetMatrix(mat);
			m_TmpTransform->TransformPoint(m_WrappedVMECenter2,m_WrappedVMECenter2);
		}
	}
	
}
int medVMEComputeWrapping::prepareData(int wrappedFlag,double *local_start,double *local_via,double *local_wrapped_center, vtkOBBTree *locator)
{
	int obbtreeFlag = 0;
	mafVME *start_vme = GetStartVME();
	mafVME *end_vme   = GetEnd1VME();
	mafVME *via_vme = GetViaPointVME();
	mafVME *wrapped_vme = GetWrappedVME1();
	if (wrappedFlag ==2)
	{
		wrapped_vme = GetWrappedVME2();
	}
	bool start_ok = true, end_ok = true;
	double orientation[3];
	int nControl = 0;
	vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
	m_Goniometer->RemoveAllInputs();

	if (start_vme && end_vme && wrapped_vme && via_vme)
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
		if(via_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("viaPoint") != -1)
		{
			((mafVMELandmarkCloud *)via_vme)->GetLandmark(GetLinkSubId("EndVME1"),m_ViaPoint,-1);
			m_TmpTransform->SetMatrix(*via_vme->GetOutput()->GetAbsMatrix());
			m_TmpTransform->TransformPoint(m_ViaPoint,m_ViaPoint);
		}
		else
		{
			via_vme->GetOutput()->GetAbsPose(m_ViaPoint, orientation);
		}

	}else{
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

		double pointTangent1[3];
		double pointTangent2[3];

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
		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
		transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform());
		transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)wrapped_vme)->GetOutput()->GetVTKData());
		transformFirstDataInput->Update();

		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
		transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
		transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
		transformFirstData->Update(); 

		//-------test intersect---------------------
		//vtkMAFSmartPointer<vtkOBBTree> locator;
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

void medVMEComputeWrapping::singleWrapAutomatedIOR(mafVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator){

	vtkMAFSmartPointer<vtkPoints> pointsIntersection1;
	vtkMAFSmartPointer<vtkPoints> pointsIntersection2;
	//vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
	double pointTangent1[3];
	double pointTangent2[3];
	bool aligned = false;
	double vectorProduct[3];
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	
	getTwoTangentPoint(wrapped_vme,local_start,local_end,local_wrapped_center,locator,transformFirstData,pointsIntersection1,pointsIntersection2);

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

	/**/m_PlaneSource->SetOrigin(local_wrapped_center);
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
	//m_Clip->SetInput(transformFirstData->GetOutput());
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
	m_Goniometer->AddInput(m_LineSource2->GetOutput());//m_Goniometer->AddInput(m_LineSourceMiddle->GetOutput());

	m_Goniometer->AddInput(m_Clip->GetOutput());

	m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	GetWrappedMeterOutput()->Update();


}

void medVMEComputeWrapping::getTwoTangentPoint(mafVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator,vtkTransformPolyDataFilter *transformFirstData ,vtkPoints *pointsIntersection1,vtkPoints *pointsIntersection2){
	
	vtkMAFSmartPointer<vtkPoints> temporaryIntersection;
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
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
	transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)wrapped_vme)->GetAbsMatrixPipe()->GetVTKTransform());
	transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)wrapped_vme)->GetOutput()->GetVTKData());
	transformFirstDataInput->Update();

	//vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstData;
	transformFirstData->SetTransform((vtkAbstractTransform *)m_TmpTransform->GetVTKTransform());
	transformFirstData->SetInput((vtkPolyData *)transformFirstDataInput->GetOutput());
	transformFirstData->Update(); 


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
}

//-----------------------------------------------------------------------
void medVMEComputeWrapping::InternalUpdateAutomatedIOR()
//-----------------------------------------------------------------------
{
  mafVME *start_vme = GetStartVME();
  mafVME *end_vme   = GetEnd1VME();
  mafVME *wrapped_vme = GetWrappedVME1();
  if (!wrapped_vme)
  {
	  wrapped_vme = GetWrappedVME2();
  }

	
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

    /**/m_PlaneSource->SetOrigin(local_wrapped_center);
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
	//m_Clip->SetInput(transformFirstData->GetOutput());
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
   m_Goniometer->AddInput(m_LineSource2->GetOutput());//m_Goniometer->AddInput(m_LineSourceMiddle->GetOutput());
    
    m_Goniometer->AddInput(m_Clip->GetOutput());



    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
    GetWrappedMeterOutput()->Update(); 
  }
  else
    m_Distance = -1;
}
//-----------------------------------------------------------------------
void medVMEComputeWrapping::WrappingCore(double *init, double *center, double *end,\
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

  int precision = 5;
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
      precision = 0.05;
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
void medVMEComputeWrapping::AvoidWrapping(double *local_start, double *local_end)
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
void medVMEComputeWrapping::InternalUpdateAutomated()
//-----------------------------------------------------------------------
{
  mafVME *start_vme = GetStartVME();
  mafVME *end_vme   = GetEnd1VME();
  mafVME *wrapped_vme   = GetWrappedVME1();

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
void medVMEComputeWrapping::InternalUpdateManual()
//-----------------------------------------------------------------------
{
	//if(m_Gui == NULL) CreateGui();
	if(m_OrderMiddlePointsNameVMEList.size() == 0) SyncronizeList();

  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  if (GetMeterMode() == medVMEComputeWrapping::POINT_DISTANCE)
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

      if(GetMeterMeasureType() == medVMEComputeWrapping::RELATIVE_MEASURE)
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
    

    if(GetMeterMeasureType() == medVMEComputeWrapping::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      m_EventSource->InvokeEvent(this,LENGTH_THRESHOLD_EVENT);

    

    for(int i=0; i< m_MiddlePointList.size(); i++)
    {
      if(localMiddlePointList[i]) delete localMiddlePointList[i];
    }
    localMiddlePointList.clear();
  }

}
//-----------------------------------------------------------------------
int medVMEComputeWrapping::InternalStore(mafStorageElement *parent)
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
				//mafVMELandmark *landmark= lc->GetLandmark(m_ListBox->GetString(i));
				int index = -1;
				for(int j=0; j< lc->GetNumberOfLandmarks(); j++)
				{
					mafVME *child = lc->GetLandmark(j);
					if(mafString(child->GetName()).Equals(m_OrderMiddlePointsNameVMEList[i])) index = j;
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
int medVMEComputeWrapping::InternalRestore(mafStorageElement *node)
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
void medVMEComputeWrapping::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** medVMEComputeWrapping::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "medVMEComputeWrapping.xpm"
//  return medVMEComputeWrapping_xpm;
//}
//-------------------------------------------------------------------------
mmaMeter *medVMEComputeWrapping::GetMeterAttributes()
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
void medVMEComputeWrapping::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetMeterMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *medVMEComputeWrapping::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double medVMEComputeWrapping::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double medVMEComputeWrapping::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int medVMEComputeWrapping::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double medVMEComputeWrapping::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double medVMEComputeWrapping::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
mafGUI* medVMEComputeWrapping::CreateGui()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;

  int num_mode = 1;
	int num_wrap =1;
	m_WrapSide = 0;
//	const wxString mode_choices_string[] = {_("point distance"), _("line distance"), _("line angle")};;
  const wxString wrap_choices_string[] = { _("Sphere_Cylinder")};
  const wxString wrap_side_string[] = { _("front"),_("back"),_("shortest")};

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_WrappedMode = IOR_AUTOMATED_WRAP;
  m_Gui->Combo(ID_WRAPPED_METER_MODE,_("wrap"),&m_WrappedMode,1,wrap_choices_string,_("Choose the meter mode"));
  m_Gui->Combo(ID_WRAPPED_METER_TYPE,_("type"),&m_WrapSide,3,wrap_side_string,_("Choose the wrap type"));
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
    m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_("End"), _("Select the end vme for point distance"));

  /*mafVME *end_vme2 = GetEnd2VME();
  if (end_vme2 && end_vme2->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("EndVME2");
    m_EndVme2Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme2)->GetLandmarkName(sub_id) : _("none");
  }*/
//  else
//    m_EndVme2Name = end_vme2 ? end_vme2->GetName() : _("none");
 //   m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,_("End 2"), _("Select the vme representing \nthe point for line distance"));

  
  mafVME *wrapped_vme = GetWrappedVME1();
  m_WrappedVmeName = wrapped_vme ? wrapped_vme->GetName() : _("none");

  mafVME *wrapped_vme2 = GetWrappedVME2();
  m_WrappedVmeName2 = wrapped_vme2 ? wrapped_vme2->GetName() : _("none");

  mafVME *viaPointvme2 = GetViaPointVME();
  m_ViaPointName = viaPointvme2 ? viaPointvme2->GetName() : _("none");

  m_Gui->Button(ID_WRAPPED_METER_LINK,&m_WrappedVmeName,_("Wrapped sphere"), _("Select the vme representing Vme to be wrapped1"));
  m_Gui->Button(ID_WRAPPED_METER_LINK2,&m_WrappedVmeName2,_("Wrapped cylinder"), _("Select the vme representing Vme to be wrapped2"));
 // m_Gui->Button(ID_VIA_POINT,&m_viaPointName,_("viaPoint"), _("Select the point on the cylinder"));
 // m_Gui->Bool(ID_WRAPPED_SIDE,"show all paths", &m_WrapSide ,0);
  //m_Gui->Bool(ID_WRAPPED_REVERSE,"reverse wrap", &m_WrapReverse,1);

 // if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
 //   m_Gui->Enable(ID_END2_METER_LINK,false);

  m_Gui->Enable(ID_WRAPPED_METER_LINK, m_WrappedMode == IOR_AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_METER_LINK2,m_WrappedMode == IOR_AUTOMATED_WRAP);
  m_Gui->Enable(ID_VIA_POINT, m_WrappedMode == IOR_AUTOMATED_WRAP);

  m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == IOR_AUTOMATED_WRAP);
  m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == IOR_AUTOMATED_WRAP);



	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Divider(2);
  //m_Gui->Button(ID_SAVE_FILE_BUTTON, _("Save in file"),"" ,"");
	m_Gui->Divider();

  InternalUpdate();

  return m_Gui;
}
void medVMEComputeWrapping::eventWraped(mafEvent *e){

	mafID button_id = e->GetId();
	mafString title = _("Choose meter vme link");
	e->SetId(VME_CHOOSE);
	if (button_id == ID_WRAPPED_METER_LINK)
		e->SetArg((long)&medVMEComputeWrapping::VMESurfaceParametricAccept);
	else if (button_id == ID_WRAPPED_METER_LINK2)
		e->SetArg((long)&medVMEComputeWrapping::VMESurfaceParametricAccept);
	else if (button_id == ID_VIA_POINT)
		e->SetArg((long)&medVMEComputeWrapping::VMESurfaceParametricAccept);
	else
		e->SetArg((long)&medVMEComputeWrapping::VMEAccept);
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
		else if (button_id == ID_WRAPPED_METER_LINK)
		{
			SetMeterLink("WrappedVME1", n);
			//n->SetName("wrapped1");
			m_WrappedVmeName = n->GetName();
		}
		else if (button_id == ID_WRAPPED_METER_LINK2)
		{
			SetMeterLink("WrappedVME2", n);
			//n->SetName("wrapped2");
			m_WrappedVmeName2 = n->GetName();
		}
		else if (button_id == ID_VIA_POINT)
		{
			SetMeterLink("viaPoint", n);
			//n->SetName("wrapped2");
			m_ViaPointName = n->GetName();
		}
		
		m_Gui->Update();
		InternalUpdate();
		mafID button_id = e->GetId();
		e->SetId(CAMERA_UPDATE);
		ForwardUpEvent(e);
	}

}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::OnEvent(mafEventBase *maf_event)
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
		eventWraped(e);
      }
      break;
	  case ID_WRAPPED_METER_LINK2:
	  {
		eventWraped(e);
	  }
	  break;
	  case ID_VIA_POINT:
	  {
		  eventWraped(e);
	  }
	  break;
	  case ID_METER_MODE:
	  {
		
		  Modified();
		  InternalUpdate();
		  mafID button_id = e->GetId();
		  e->SetId(CAMERA_UPDATE);
		  ForwardUpEvent(e);
     
	  }
    break;
		case ID_WRAPPED_METER_MODE:
			{
				//EnableManualModeWidget(m_WrappedMode ==MANUAL_WRAP);

        m_Gui->Enable(ID_WRAPPED_METER_LINK,  m_WrappedMode == IOR_AUTOMATED_WRAP);
		m_Gui->Enable(ID_WRAPPED_METER_LINK2, m_WrappedMode == IOR_AUTOMATED_WRAP);
		m_Gui->Enable(ID_VIA_POINT, m_WrappedMode == IOR_AUTOMATED_WRAP);
        m_Gui->Enable(ID_WRAPPED_SIDE, m_WrappedMode == IOR_AUTOMATED_WRAP );
        m_Gui->Enable(ID_WRAPPED_REVERSE, m_WrappedMode == IOR_AUTOMATED_WRAP);
        

				Modified();
				InternalUpdate();
				mafID button_id = e->GetId();
				e->SetId(CAMERA_UPDATE);
				ForwardUpEvent(e);
			}
			break;
	case ID_WRAPPED_METER_TYPE:
		InternalUpdate();
		ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
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
        e->SetArg((long)&medVMEComputeWrapping::VMEAccept);
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

        
        
        SetMeterLink(n->GetName(),n);
        mafString t;
        t = n->GetName();
        //m_ListBox->Append(_(t));
        m_ListBox->Append(_(t));
        m_ListBox->SetStringSelection(_(t));

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


          m_ListBox->SetStringSelection(_(name));

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
          m_ListBox->SetStringSelection(_(name));

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
void medVMEComputeWrapping::SetMeterLink(const char *link_name, mafNode *n)
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
void medVMEComputeWrapping::AddMidPoint(mafNode *node)
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
void medVMEComputeWrapping::RemoveLink(const char *link_name)
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
mafVME *medVMEComputeWrapping::GetStartVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("StartVME"));
}
//-------------------------------------------------------------------------
mafVME *medVMEComputeWrapping::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME1"));
}
//-------------------------------------------------------------------------
mafVME *medVMEComputeWrapping::GetEnd2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("EndVME2"));
}
//-------------------------------------------------------------------------
mafVME  *medVMEComputeWrapping::GetWrappedVME1()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("WrappedVME1"));
}
mafVME  *medVMEComputeWrapping::GetWrappedVME2()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("WrappedVME2"));
}
mafVME  *medVMEComputeWrapping::GetViaPointVME()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("viaPoint"));
}

std::vector<mafVME *> medVMEComputeWrapping::GetWrappedVMEList(){
//-------------------------------------------------------------------------
	std::vector<mafVME *> wrappedVmeList;
	mafVME * tmp = mafVME::SafeDownCast(GetLink("WrappedVME1"));
	wrappedVmeList.push_back(tmp);
	tmp = mafVME::SafeDownCast(GetLink("WrappedVME2"));
	wrappedVmeList.push_back(tmp);
	return wrappedVmeList;

}

//-----------------------------------------------------------------------
mafNode::mafLinksMap *medVMEComputeWrapping::GetMidPointsLinks()
//-----------------------------------------------------------------------
{
  //da ritornare la link maps
  return GetLinks();
}

//-------------------------------------------------------------------------
void medVMEComputeWrapping::EnableManualModeWidget(bool flag)
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LISTBOX, flag);
  m_Gui->Enable(ID_ADD_POINT, flag);
  m_Gui->Enable(ID_REMOVE_POINT, flag);
  m_Gui->Enable(ID_UP, flag);
  m_Gui->Enable(ID_DOWN, flag);
}
//-------------------------------------------------------------------------
bool medVMEComputeWrapping::MiddlePointsControl()
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
mafNode* medVMEComputeWrapping::IndexToMiddlePointVME(int index)
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
void medVMEComputeWrapping::SyncronizeList()
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
double *medVMEComputeWrapping::GetMiddlePointCoordinate(int index)
//-------------------------------------------------------------------------
{
	if(m_MiddlePointList.size() > 0)
	  return m_MiddlePointList[index];

	return NULL;
}
//-------------------------------------------------------------------------
double *medVMEComputeWrapping::GetStartPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_StartPoint;
}
//-------------------------------------------------------------------------
double *medVMEComputeWrapping::GetEndPointCoordinate()
//-------------------------------------------------------------------------
{
  return m_EndPoint;
}
//-------------------------------------------------------------------------
void medVMEComputeWrapping::SaveInFile()
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
