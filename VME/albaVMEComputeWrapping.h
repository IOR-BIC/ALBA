/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEComputeWrapping.H,v $
Language:  C++
Date:      $Date: 2012-04-10 17:11:26 $
Version:   $Revision: 1.1.2.19 $
Authors:   Anupam Agrawal and Hui Wei
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
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


#ifndef __medVMEComputeWrapping_h
#define __medVMEComputeWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaVMEOutputComputeWrapping.h"
#include "albaEvent.h"
#include "albaVMESurfaceParametric.h"
#include "vtkALBASmartPointer.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkCutter.h"
#include "vtkClipPolyData.h"
#include "vtkPatchLine.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMeter;
class mmaMaterial;
class vtkLineSource;
class vtkLineSource;
class vtkAppendPolyData;
class vtkOBBTree;
class vtkPoints;
class albaMatrix3x3;
class vtkTransformPolyDataFilter;
class vtkCellArray;
class albaGUIRollOut;
/** medVMEComputeWrapping - 
*/
class ALBA_EXPORT albaVMEComputeWrapping : public albaVME
{


public:
	ALBA_ID_DEC(LENGTH_THRESHOLD_EVENT);
	enum INTERSECT_MODE{
		NON_WRAP = 0,
		SINGLE_SPHERE = 1,
		SINGLE_CYLINDER = 2,
		SINGLE_CYLINDER2 = 3,
		WRAP_CYLINDER_CYLINDER = 4,
		WRAP_SPHERE_CYLINDER = 5,
		WRAP_SPHERE_CYLINDER_2 = 6,
		SINGLE_SPHERE2=7,
		CYLINDER_CYLINDER=8,
		CYLINDER_CYLINDER_1=9,
		CYLINDER_CYLINDER_2=10,
		INSIDE_FLAG = -1
	};
	enum WRAP_MODE_ID{
		SPHERE_CYLINDER = 0,
		SPHERE_ONLY = 1,
		CYLINDER_ONLY = 2,
		DOUBLE_CYLINDER = 3
		
		
	};
	enum METER_MODALITY_TYPE_ID
	{
		MANUAL_WRAP=0,
		AUTOMATED_WRAP,
		IOR_AUTOMATED_WRAP,
	};
	enum WRAP_MODE_CLASS_ID
	{
		OLD_METER = 0,
		NEW_METER = 1,
	};
	enum METER_WRAP_TYPE_ID{
		WRAP_FRONT =0,
		WRAP_BACK = 1,
		WRAP_SHOREST = 2
	};
	enum METER_MEASURE_TYPE_ID
	{
		POINT_DISTANCE=0,
		//  LINE_DISTANCE,
		//  LINE_ANGLE
	};
	/*enum METER_MODALITY_TYPE_ID
	{

		IOR_AUTOMATED_WRAP=0,
		//MANUAL_WRAP,
		//AUTOMATED_WRAP,
	};*/
	enum METER_COLOR_TYPE_ID
	{
		ONE_COLOR=0,
		RANGE_COLOR
	};
	enum METER_REPRESENTATION_ID
	{
		LINE_REPRESENTATION=0,
		TUBE_REPRESENTATION
	};
	enum METER_MEASURE_ID
	{
		ABSOLUTE_MEASURE=0,
		RELATIVE_MEASURE
	};
	albaTypeMacro(albaVMEComputeWrapping,albaVME);

	enum METER_WIDGET_ID
	{
		ID_START_METER_LINK = Superclass::ID_LAST,
		ID_END1_METER_LINK,
		ID_WRAPPED_METER_LINK1,
		ID_WRAPPED_METER_LINK2,
		ID_WRAPPED_METER_LINK,
		ID_VIA_POINT,
		ID_METER_MODE,
		ID_WRAPPED_METER_MODE,
		ID_WRAPPED_METER_TYPE,
		ID_WRAPPED_METER_CLASS,
		ID_WRAPPED_METER_NUM,
		ID_WRAPPED_SIDE,
		ID_WRAPPED_SIDE_NEW,
		ID_WRAPPED_REVERSE,
		ID_LISTBOX,
		ID_ADD_POINT,
		ID_REMOVE_POINT,
		ID_UP,
		ID_DOWN,
		ID_SAVE_FILE_BUTTON,
		ID_LAST,
		ID_ROLLOUT_NEW,
		ID_ROLLOUT_OLD,
	};
	class Apoint
	{
	public:
		double  m_X;
		double  m_Y;
		double  m_Z;
	};

	static bool VMEAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVME));};
	static bool VMESurfaceParametricAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMESurfaceParametric));};

	/** Precess events coming from other objects */ 
	virtual void OnEvent(albaEventBase *alba_event);

	/** print a dump of this object */
	virtual void Print(std::ostream& os, const int tabs=0);

	/** Copy the contents of another VME-Meter into this one. */
	virtual int DeepCopy(albaVME *a);

	/** Compare with another VME-Meter. */
	virtual bool Equals(albaVME *vme);

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual albaString GetVisualPipe() {return albaString("albaPipeComputeWrapping");};

	/** return the right type of output */  
	albaVMEOutputComputeWrapping *GetWrappedMeterOutput();

	/**
	Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
	set or get the Pose for a specified time. When setting, if the time does not exist
	the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
	interpolates on the fly according to the matrix interpolator.*/
	virtual void SetMatrix(const albaMatrix &mat);

	/**
	Return the list of timestamps for this VME. Timestamps list is 
	obtained merging timestamps for matrixes and VME items*/
	virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

	/** return always false since (currently) the slicer is not an animated VME (position 
	is the same for all timestamps). */
	virtual bool IsAnimated();

	/** return an xpm-icon that can be used to represent this node */
	//static char ** GetIcon();

	void EnableManualModeWidget(albaGUI *gui,bool flag);

	/**
	Get/Set the kind of measure  performed: distance between points (POINT) or
	distance between a point and a line. */
	int GetMeterMode();
	void SetMeterMode(int mode);
	void SetMeterModeToPointDistance() {this->SetMeterMode(albaVMEComputeWrapping::POINT_DISTANCE);}
	// void SetMeterModeToLineDistance() {this->SetMeterMode(medVMEComputeWrapping::LINE_DISTANCE);}
	// void SetMeterModeToLineAngle() {this->SetMeterMode(medVMEComputeWrapping::LINE_ANGLE);}

	/** 
	Store the min and max distance to associate with colors.*/
	void SetDistanceRange(double min, double max);
	void SetDistanceRange(double range[2]) {SetDistanceRange(range[0],range[1]);};

	/** 
	Retrieve the distance range associated with colors.*/
	double *GetDistanceRange();
	void GetDistanceRange(double range[2]);

	/** 
	Color the meter with a range colors extracted by a LookupTable or in flat mode selected by material library. */
	void SetMeterColorMode(int mode);
	void SetMeterColorModeToOneColor() {this->SetMeterColorMode(albaVMEComputeWrapping::ONE_COLOR);}
	void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(albaVMEComputeWrapping::RANGE_COLOR);}

	/** 
	Get the color mode of the meter. */
	int GetMeterColorMode();

	/** 
	Set the measure type to absolute or relative to the initial measure. */
	void SetMeterMeasureType(int type);
	void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(albaVMEComputeWrapping::ABSOLUTE_MEASURE);}
	void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(albaVMEComputeWrapping::RELATIVE_MEASURE);}

	/** 
	Get the measure type. */
	int GetMeterMeasureType();

	/** 
	Represent the meter with a tube or as a line. */
	void SetMeterRepresentation(int representation);
	void SetMeterRepresentationToLine() {this->SetMeterRepresentation(albaVMEComputeWrapping::LINE_REPRESENTATION);}
	void SetMeterRepresentationToTube() {this->SetMeterRepresentation(albaVMEComputeWrapping::TUBE_REPRESENTATION);}

	/** 
	Get the representation mode of the meter. */
	int GetMeterRepresentation();

	/** 
	Represent the meter with a tube or as a line. */
	void SetMeterCapping(int capping);
	void SetMeterCappingOn() {this->SetMeterCapping(1);}
	void SetMeterCappingOff() {this->SetMeterCapping(0);}

	/** 
	Get the capping of tube representation of the meter. */
	int GetMeterCapping();

	/** 
	Enable-Disable event generation fo length threshold. */
	void SetGenerateEvent(int generate);
	void SetGenerateEventOn() {SetGenerateEvent(1);}
	void SetGenerateEventOff() {SetGenerateEvent(0);}

	/** 
	Get the length threshold event generation status. */
	int GetGenerateEvent();

	/** 
	Set the initial measure. */
	void SetInitMeasure(double init_measure);

	/** 
	Get the initial measure. */
	double GetInitMeasure();

	/** 
	Set the meter radius when represented as tube. */
	void SetMeterRadius(double radius);

	/** 
	Get the meter radius when represented as tube. */
	double GetMeterRadius();

	/** 
	Set the percentage elongation. */
	void SetDeltaPercent(int delta_percent);

	/** 
	Get the percentage elongation. */
	int GetDeltaPercent();

	/**
	return distance extracting it from the connected pipe. */
	double GetDistance();

	/**
	return angle extracting it from the connected pipe. */
	double GetAngle();

	/** return the meter's attributes */
	mmaMeter *GetMeterAttributes();

	albaVME *GetStartVME();
	albaVME *GetEnd1VME();
	
	albaVME *GetWrappedVME1();
	albaVME *GetWrappedVME2();
	albaVME *GetWrappedVME();
	

	albaVME *GetViaPointVME();
	std::vector<albaVME *> GetWrappedVMEList();


	/** Get the link to the surface.*/
	albaVME::albaLinksMap *GetMidPointsLinks();

	/** 
	Set links for the meter*/
	void SetMeterLink(const char *link_name, albaVME *n);

	void RemoveLink(const char *link_name);

	/** Return pointer to material attribute. */
	mmaMaterial *GetMaterial();

	/**Return Number of middle points*/
	int GetNumberMiddlePoints(){return m_MiddlePointList.size();};

	/**Get Coordinates of indexed middle point*/
	double *GetMiddlePointCoordinate(int index);

	double *GetStartPointCoordinate();
	double *GetEndPointCoordinate();

	/**Get Coordinates of Tangent 1 in wrapped geometry*/
	double *GetWrappedGeometryTangent1(){ return m_WrappedTangent1;};

	/**Get Coordinates of Tangent 2 in wrapped geometry*/
	double *GetWrappedGeometryTangent2(){ return m_WrappedTangent2;};

	/**Set\Get modality of wrapped vme (AUTOMATIC OR MANUAL)*/
	int GetWrappedMode1(){return m_WrappedMode1;};
	void SetWrappedMode1(int value){m_WrappedMode1 = value;};
	int GetWrappedMode2(){return m_WrappedMode2;};
	void SetWrappedMode2(int value){m_WrappedMode2 = value;};

	  int GetWrappedClass(){return m_WrappedClass;};
	  void SetWrappedClass(int value){m_WrappedClass = value;};

	/**Set\Get side of geometric wrapping: the side where search tangent over parametric surface*/
	int GetWrapSide(){return m_WrapSide;};
	void SetWrapSide(int value){m_WrapSide = value;};

	/**Set\Get reverse modality: reverse allows which direction follow for clip after finding tangent points.*/
	int GetWrapReverse(){return m_WrapReverse;};
	void SetWrapReverse(int value){m_WrapReverse = value;};

	void AddMidPoint(albaVME *node);
	double *GetExportPointCoordinate(int index);
	/**Return Number of export points*/
	int GetNumberExportPoints(){return m_ExportPointList.size();};
	double GetPointOnPlane(double zL ,double zH,double *point1,double *point2,double *point3,double *output);

	std::vector<int> m_OrderMiddlePointsVMEList; //order list of VME ID

protected:
	albaVMEComputeWrapping();

//-------------------old functions begin------------------
	/** update the output data structure */
	virtual void InternalUpdate();
	
	

	virtual ~albaVMEComputeWrapping();
	virtual int InternalStore(albaStorageElement *parent);
	virtual int InternalRestore(albaStorageElement *node);

	/** this creates the Material attribute at the right time... */
	virtual int InternalInitialize();

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();



	/** update the output data structure for manual wrapped mode */
	virtual void InternalUpdateManual();

	/** update the output data structure for automate wrapped mode with IOR customization*/
	virtual void InternalUpdateAutomated();

	/** update the output data structure for automate wrapped mode */
	virtual void InternalUpdateAutomatedIOR();


	/** Internally used to create a new instance of the GUI.*/
	virtual albaGUI *CreateGui();

	/*Control if the vme linked to wrapped meter are correctly represented in its gui*/
	bool MiddlePointsControl();

	/*Return albaVME pointer given a index representing index in ordered list*/
	albaVME* IndexToMiddlePointVME(int index);

	/*Save in file all points*/
	void SaveInFile();

	/*Syncronize the list of name before the creation of the gui*/
	void SyncronizeList();

	/**Push in Id Vector links id*/
	void PushIdVector(int id){m_OrderMiddlePointsVMEList.push_back(id);}

	/** Wrapping Core*/
	void WrappingCore(double *init, double *center, double *end,\
		bool IsStart, bool controlParallel,\
		vtkOBBTree *locator, vtkPoints *temporaryIntersection, vtkPoints *pointsIntersection,\
		double *versorY, double *versorZ, int nControl);

	void AvoidWrapping(double *local_start, double *local_end);
	void EventWraped(albaEvent *e);

	//-------------------new functions begin------------------
	void Dispatch();
	int PrepareData(int wrappedFlag,double *local_start,double *local_end,double *local_wrapped_center, vtkOBBTree *locator);
	bool PrepareData2();
	void SingleWrapAutomatedIOR(albaVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator);
	void GetTwoTangentPoint(albaVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator,vtkTransformPolyDataFilter *transformFirstData ,vtkPoints *pointsIntersection1,vtkPoints *pointsIntersection2);
	double DetValue(double *col1,double *col2,double *col3);
	void DoubleWrap(double *local_start,double *local_end,double *local_wrapped_center1,double *local_wrapped_center2,vtkOBBTree *locator1,vtkOBBTree *locator2,vtkTransformPolyDataFilter *transformFirstData);
	bool GetAcoordinateInLocal(double *Ap,double *Ap2,double *olCoord,double R,double angle);
	void GetUVWT(double *Ap,double *oCoord,double *iCoord,double & u,double & v,double & w,double & t);
	bool GetBcoordinate(double *Ap,double *oCoord,double *iCoord,double *mCoord,double Rm,double R,double *bCoord1,double *bCoord2);
	bool GetBcoordinateUpdate(double *cCoord ,double *bCoord1,double *bCoord2);
	bool GetCcoordinate(double *cCoord1,double *cCoord2);
	void GetTransFormMatrix(double cosA,double sinA,double cosB,double sinB,albaMatrix3x3 *mat);
	void GetAngleAandB(double & cosA,double & sinA,double & cosB,double & sinB);
	double GetPi();
	void GetGlobalSphereCenter(double *sphCoord);
	void GetGlobalCylinderCenter(double *cylCoord);
	void GetGlobalCylinderCenter(double *cylCoord,int objIdx);
	double GoldenSectionSearch(double a,double b,double x,double accuracy);
	//double getFunctionValue(double x,double *aCoord,double *bCoord);
	double ComputeDistanceOfLm(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *rCoord,double rm);
	double ComputeDistanceOfLm2(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double rm);
	void GetLocalCenterCoord(double *newCoord,double *oldCoord);
	void GetGlobalCenterCoord(double *globalCoord,double *localCoord);
	void GetLocalTransformedCoord(double *newCoord,double *oldCoord,albaMatrix3x3 *mat);
	void GetIntersectCircle(double *Ap,double *oCoord,double *iCoord,double *mCoord,double & Rm);
	void GetGlobalTransformedCoord(double *localCoord,double *globalCoord,albaMatrix3x3 *mat);
	double GetCutPlane();
	double GetSphereRadius();
	double GetCylinderRadius();
	double GetCylinderRadius(int objIdx);
	//double caculateHelix(vtkPolyData * hCurve);
	//double computeHelixCost(double tc,double to,double c,double k);
	//void changeXZ(double *dCoord ,double *scoord);
	double DrawHelix(double tc,double to,double c,double k,vtkCellArray * cells,vtkPoints *pts,bool drawFlag);
	double DrawHelix(double tc,double to,double c,double k,vtkCellArray * cells,vtkPoints *pts,bool drawFlag, int objIdx);
	void GetSphereCylinderWrap(const int step,double *viaPoint);
	void GetCylinderCylinderWrap(const int step);
	double GetPointLineDistance(double *point,double *Lpoint1,double *Lpoint2);

	double GetFunctionValue2(double x,double filterFlag,double *filterPlaneNormal,double *endPoint,double *outAPoint,double *outBPoint,double *outMPoint,double & Rm);
	double GetCutPlane2(double *aPoint,double *bPoint,double *mPoint,vtkClipPolyData *outCurve);
	double CaculateHelix2(vtkPolyData * hCurve,double *cCoord,double *vCoord,double drawFlag);
	double CaculateHelix2(vtkPolyData * hCurve,double *cCoord,double *vCoord,double drawFlag,int objIdx);
	bool GetCcoordinate2(double *bCoord,double *cCoord1,double *cCoord2);
  bool GetCcoordinate2(double *bCoord,double *cCoord1,double *cCoord2,int objIdx);
	bool GetBcoordinateUpdate2(double *aCoord,double *bCoord,double *cCoord ,double *bCoord1,double *bCoord2);
	double TestPlane(double *a,double *b,double *c,double *d);
	double ComputeHelixCost2(double tc,double to,double c,double k,double *cCoord,double *vCoord);
	double ComputeDistanceOfWholePath(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double *rCoord,double CIcurve,double rm);
	void FormatParameterForHelix(double  tc,double  to,double  Zc,double Zo,double & c ,double & k,double & tcRtn,double & toRtn);

	double GetCutPlaneForCI(double *bCoord,double *cCoord,vtkClipPolyData *clipData);
	//double getCutPlaneForCI(double *bCoord,double *cCoord,vtkCutter *clipData);


	void ProcessViaPoint();

	void GetWrapLocalTransform(double *inCoord,double *outCoord);
	void GetWrapLocalTransform(double *inCoord,double *outCoord,int objIdx);
	void GetWrapGlobalTransform(double *inCoord,double *outCoord);
	void GetWrapGlobalTransform(double *inCoord,double *outCoord,int objIdx);
	void GetWrapAngleAandB(double &cosA,double &sinA,double &cosB,double &sinB);
	// void getWrapMatrix(albaMatrix *inMx,albaMatrix *inMy);
	void GetWrapMatrix(albaMatrix &inMx,albaMatrix &inMy);
	void GetWrapMatrix(albaMatrix &inMx,albaMatrix &inMy,int objIdx);

	int GetViaPoint(double *viaPoint,bool isNearEndflag);
	void ComputeAngleForB(double *aCoord,double *bCoord1,double *bCoord2,double *bCoord);
	void DirectConnectSE();
	void Get90DegreePointOnCylinder(double *firstPoint,double *sphereCenter,double *secondPoint);
	double GetFilterFlag(double *outFilterPlaneNormal);
	void CopyPointValue(double *srcPoint,double *aimPoint);
	double ComputeCurveLength(double *aCoord,double *bCoord,double rm);
	void WrapCylinderOnly(const int step);
	void WrapSingleCylinder(double vId);//for double wrap cylinder
	void WrapSphereOnly(const int step,bool allowIntersectFlag);//allowIntersectFlag if allow intersect with cylinder

	//void wrapSphereCylinderSeparately(const int step,bool allowIntersectFlag);
	
	double ComputeDistanceOfLm3(double *start,double *cCoord,double *iCoord);
	double GetCutPlaneForCylinder(double *center,double *t1,double *t2,albaVME *wrapped_vme,vtkClipPolyData *clipData);
	double GoldenSectionSearch2(double a,double b,double x,double accuracy);
	void GetCcoordinateForCylinder(double *oCoord,double *outC1,double *outC2,int objIdx);
	bool IsEndPonintOnCylinder(double tolerance,double *endWrapLocal);
	bool IsLineInterSectObject(albaVME *wrapVME,double *point1,double *point2);

	double GoldenSectionSearch(double low,double high,double accuracy,double *A,double *b,double *c,double *firstC,int type);
	double GetAngleValue(double *A ,double *b,double *c);
	double GetPlaneValue(double *a,double *b,double *c,double *d);
	double GetDistanceValue(double *start,double *cCoord,double *end);
	double GetDistanceSumValue(double *start,double *cCoord,double *end);
	void GetSphereCylinderWrapAdvance(const int step);
	void TransformOutput(vtkPolyData *pData );
	void TransformOutputPoint( double *point);
	void TransformOutputClipData(vtkPolyData *clipData);
	bool CheckAlign();
	bool CheckTwoRadius();
	bool CheckNearEnough(double *firstPoint,double *secondPoint);

	albaGUI* CreateGuiForNewMeter( albaGUI *gui );
	albaGUI* CreateGuiForOldMeter( albaGUI *gui );
	void GetCylinderAxis(albaVME *wrapped_vme,int objIdx);
	void ChooseSameSidePoint(double *planePoint1,double *filterPoint,double *inputPoint1,double *inputPoint2,int cylinderIdx,double *rtnPoint);

	void GetOneSideTangentPointOnCylinder(double *startLocal,double *endLocal,int vmeIdx,int mode,double *p1Global,double *pointGlobal);
	                                  
	void GetOneSideTangentPointOnCylinderWithSamePlane(double *startLocal,double *endLocal,double *p1,int vmeIdx,double *p2Global);
	

	//------------------new function -------------------
//--------------test another way----------
	void WrapCylinderOnlyObstacleSet(int idx);
	double GetQTsegment(double R,double *Q,double *T);
	bool WrapCondition(double *Q,double *T,bool reverseFlag);
	void ComputeTangentXYQ(double *P,double R,double *rtn);
	void ComputeTangentXYT(double *S,double R, double *rtn);
	double ComputeQTLength(double *Q,double *T,double R);
	void ComputeQz(double *P,double *S,double *Q,double *T,double QTlength);
	void ComputeTz(double *P,double *S,double *Q,double *T,double QTlength);
//	void WrapCylinderOnlyObstacleSetBasic(double *Sg,double *Pg,int idx,double &segLength,double *Tout,double *Qout);
	void WrapCylinderOnlyObstacleSetBasic2(double *Sg,double *Pg,int idx,bool reverseFlag,double &segLength,double *Tout,double *Qout);
	void WrapCylinderCylinderObstacleSet();
//--------------test over----------------



	//--------------------new functions over-------------------


	double m_Distance;
	double m_Angle;
	double m_StartPoint[3];
	double m_EndPoint[3];
	double m_EndPoint2[3];
	double m_APoint[3];
	double m_BPoint[3];
	double m_CPoint[3];
	double m_Ipoint[3];
	double m_Mpoint[3];
	double m_Tolerance;

	double m_SphereWrapLocal[3],m_StartWrapLocal[3],m_ViaWrapLocal[3],m_EndWrapLocal[3];

	double m_WrappedVMECenter[3];
	double m_WrappedVMECenter1[3];
	double m_WrappedVMECenter2[3];
	double m_WrappedTangent1[3];
	double m_WrappedTangent2[3];
	double m_CylinderAxis1[3];
	double m_CylinderAxis2[3];
	double m_SphereAxis[3];

	double m_ViaPoint[3];
	double m_CosA;
	int m_PathNum;
	//albaString m_sPathNum;
	double m_Rm;
	double m_AbCurve;
	std::vector<Apoint *>  m_APointList;
	std::vector<Apoint *>  m_BPointList;
	std::vector<Apoint *>  m_CPointList;
	//std::vector<double[2]>
	double m_Alist[100][3];
	int m_Idx ;

	albaTransform *m_Transform;

	vtkLineSource     *m_LineSource;
	vtkLineSource     *m_LineSource2;
	//vtkLineSource     *m_LineSourceMiddle;
	vtkAppendPolyData *m_Goniometer;
	vtkPatchLine      *m_LinePatcher;
	albaTransform      *m_TmpTransform;
	albaTransform      *m_TmpTransform2;  

	std::vector<vtkLineSource *> m_LineSourceList; //list of PolyData

	std::vector<double *> m_MiddlePointList; // list of coordinates
	std::vector<double *> m_ExportPointList; // list of coordinates
	std::vector<albaString> m_OrderMiddlePointsNameVMEList; //order list of VME Name
	std::vector<albaString> m_TestList;
	

	int              m_OrderMiddlePointsVMEListNumberOfElements;

	albaString m_StartVmeName;
	albaString m_EndVme1Name;
	albaString m_WrappedVmeName1;
	albaString m_WrappedVmeName2;
	albaString m_WrappedVmeName;
	albaString m_ViaPointName;


	vtkALBASmartPointer<vtkPlaneSource> m_PlaneSource;
	vtkALBASmartPointer<vtkPlane> m_PlaneCutter;
	vtkALBASmartPointer<vtkCutter> m_Cutter;
	vtkALBASmartPointer<vtkPlane> m_PlaneClip;
	vtkALBASmartPointer<vtkClipPolyData> m_Clip;

	albaMatrix3x3 *m_Mat;
	albaMatrix3x3 *m_Imat;
	albaMatrix m_InMx2,m_InMy2,m_InMx1,m_InMy1;
	albaMatrix m_OutMx2,m_OutMy2,m_OutMx1,m_OutMy1;

	wxListBox *m_ListBox;
	int        m_WrappedMode1;
	int        m_WrappedMode2;
	int      m_WrappedClass;
	int        m_WrapSide;
	int			m_WrapReverseNew;
	int        m_WrapReverse;
	//---------------interface -------------
	wxBoxSizer* m_BSizer;
	albaGUI *m_GuiNewMeter;
	albaGUI *m_GuiOldMeter; 

	albaGUIRollOut *m_RollOutNewMeter;
	albaGUIRollOut *m_RollOutOldMeter;

	
private:
	albaVMEComputeWrapping(const albaVMEComputeWrapping&); // Not implemented
	void operator=(const albaVMEComputeWrapping&); // Not implemented

};
#endif
