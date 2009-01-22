/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEComputeWrapping.H,v $
Language:  C++
Date:      $Date: 2009-01-22 20:50:44 $
Version:   $Revision: 1.1.2.5 $
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
#include "mafVME.h"
#include "medVMEOutputComputeWrapping.h"
#include "mafEvent.h"
#include "mafVMESurfaceParametric.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkCutter.h"
#include "vtkClipPolyData.h"

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
class mafMatrix3x3;
class vtkTransformPolyDataFilter;
class vtkCellArray;
/** medVMEComputeWrapping - 
*/
class MAF_EXPORT medVMEComputeWrapping : public mafVME
{


public:
	MAF_ID_DEC(LENGTH_THRESHOLD_EVENT);
	enum INTERSECT_MODE{
		NON_WRAP = 0,
		SINGLE_SPHERE = 1,
		SINGLE_CYLINDER = 2,
		SINGLE_CYLINDER2 = 3,
		WRAP_CYLINDER_CYLINDER = 4,
		WRAP_SPHERE_CYLINDER = 5,
		WRAP_SPHERE_CYLINDER_2 = 6,
		INSIDE_FLAG = -1
	};
	enum WRAP_MODE_ID{
		SPHERE_CYLINDER = 0,
		DOUBLE_CYLINDER = 1,
		CYLINDER_ONLY = 2,
		SPHERE_ONLY = 3

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
	enum METER_MODALITY_TYPE_ID
	{

		IOR_AUTOMATED_WRAP=0,
		//MANUAL_WRAP,
		//AUTOMATED_WRAP,
	};
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
	mafTypeMacro(medVMEComputeWrapping,mafVME);

	enum METER_WIDGET_ID
	{
		ID_START_METER_LINK = Superclass::ID_LAST,
		ID_END1_METER_LINK,
		ID_END2_METER_LINK,
		ID_WRAPPED_METER_LINK,
		ID_WRAPPED_METER_LINK2,
		ID_VIA_POINT,
		ID_METER_MODE,
		ID_WRAPPED_METER_MODE,
		ID_WRAPPED_METER_TYPE,
		ID_WRAPPED_METER_NUM,
		ID_WRAPPED_SIDE,
		ID_WRAPPED_REVERSE,
		ID_LISTBOX,
		ID_ADD_POINT,
		ID_REMOVE_POINT,
		ID_UP,
		ID_DOWN,
		ID_SAVE_FILE_BUTTON,
		ID_LAST
	};
	class Apoint
	{
	public:
		double  x;
		double  y;
		double  z;
	};

	static bool VMEAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVME));};
	static bool VMESurfaceParametricAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMESurfaceParametric));};

	/** Precess events coming from other objects */ 
	virtual void OnEvent(mafEventBase *maf_event);

	/** print a dump of this object */
	virtual void Print(std::ostream& os, const int tabs=0);

	/** Copy the contents of another VME-Meter into this one. */
	virtual int DeepCopy(mafNode *a);

	/** Compare with another VME-Meter. */
	virtual bool Equals(mafVME *vme);

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual mafString GetVisualPipe() {return mafString("medPipeComputeWrapping");};

	/** return the right type of output */  
	medVMEOutputComputeWrapping *GetWrappedMeterOutput();

	/**
	Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
	set or get the Pose for a specified time. When setting, if the time does not exist
	the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
	interpolates on the fly according to the matrix interpolator.*/
	virtual void SetMatrix(const mafMatrix &mat);

	/**
	Return the list of timestamps for this VME. Timestamps list is 
	obtained merging timestamps for matrixes and VME items*/
	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

	/** return always false since (currently) the slicer is not an animated VME (position 
	is the same for all timestamps). */
	virtual bool IsAnimated();

	/** return an xpm-icon that can be used to represent this node */
	//static char ** GetIcon();

	void EnableManualModeWidget(bool flag);

	/**
	Get/Set the kind of measure  performed: distance between points (POINT) or
	distance between a point and a line. */
	int GetMeterMode();
	void SetMeterMode(int mode);
	void SetMeterModeToPointDistance() {this->SetMeterMode(medVMEComputeWrapping::POINT_DISTANCE);}
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
	void SetMeterColorModeToOneColor() {this->SetMeterColorMode(medVMEComputeWrapping::ONE_COLOR);}
	void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(medVMEComputeWrapping::RANGE_COLOR);}

	/** 
	Get the color mode of the meter. */
	int GetMeterColorMode();

	/** 
	Set the measure type to absolute or relative to the initial measure. */
	void SetMeterMeasureType(int type);
	void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(medVMEComputeWrapping::ABSOLUTE_MEASURE);}
	void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(medVMEComputeWrapping::RELATIVE_MEASURE);}

	/** 
	Get the measure type. */
	int GetMeterMeasureType();

	/** 
	Represent the meter with a tube or as a line. */
	void SetMeterRepresentation(int representation);
	void SetMeterRepresentationToLine() {this->SetMeterRepresentation(medVMEComputeWrapping::LINE_REPRESENTATION);}
	void SetMeterRepresentationToTube() {this->SetMeterRepresentation(medVMEComputeWrapping::TUBE_REPRESENTATION);}

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

	mafVME *GetStartVME();
	mafVME *GetEnd1VME();
	mafVME *GetEnd2VME();

	mafVME *GetWrappedVME1();
	mafVME *GetWrappedVME2();
	mafVME *GetViaPointVME();
	std::vector<mafVME *> GetWrappedVMEList();

	/** Get the link to the surface.*/
	mafNode::mafLinksMap *GetMidPointsLinks();

	/** 
	Set links for the meter*/
	void SetMeterLink(const char *link_name, mafNode *n);

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
	int GetWrappedMode(){return m_WrappedMode;};
	void SetWrappedMode(int value){m_WrappedMode = value;};

	/**Set\Get side of geometric wrapping: the side where search tangent over parametric surface*/
	int GetWrapSide(){return m_WrapSide;};
	void SetWrapSide(int value){m_WrapSide = value;};

	/**Set\Get reverse modality: reverse allows which direction follow for clip after finding tangent points.*/
	int GetWrapReverse(){return m_WrapReverse;};
	void SetWrapReverse(int value){m_WrapReverse = value;};

	void AddMidPoint(mafNode *node);

protected:
	medVMEComputeWrapping();
	virtual ~medVMEComputeWrapping();

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** this creates the Material attribute at the right time... */
	virtual int InternalInitialize();

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

	/** update the output data structure for manual wrapped mode */
	virtual void InternalUpdateManual();

	/** update the output data structure for automate wrapped mode with IOR customization*/
	virtual void InternalUpdateAutomated();

	/** update the output data structure for automate wrapped mode */
	virtual void InternalUpdateAutomatedIOR();


	/** Internally used to create a new instance of the GUI.*/
	virtual mafGUI *CreateGui();

	/*Control if the vme linked to wrapped meter are correctly represented in its gui*/
	bool MiddlePointsControl();

	/*Return mafVME pointer given a index representing index in ordered list*/
	mafNode* IndexToMiddlePointVME(int index);

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
	void eventWraped(mafEvent *e);

	//-------------------new functions begin------------------
	void dispatch();
	int prepareData(int wrappedFlag,double *local_start,double *local_end,double *local_wrapped_center, vtkOBBTree *locator);
	bool prepareData2();
	void singleWrapAutomatedIOR(mafVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator);
	void getTwoTangentPoint(mafVME * wrapped_vme,double *local_start,double *local_end,double *local_wrapped_center,vtkOBBTree *locator,vtkTransformPolyDataFilter *transformFirstData ,vtkPoints *pointsIntersection1,vtkPoints *pointsIntersection2);
	double detValue(double *col1,double *col2,double *col3);
	void doubleWrap(double *local_start,double *local_end,double *local_wrapped_center1,double *local_wrapped_center2,vtkOBBTree *locator1,vtkOBBTree *locator2,vtkTransformPolyDataFilter *transformFirstData);
	bool getAcoordinateInLocal(double *Ap,double *Ap2,double *olCoord,double R,double angle);
	void getUVWT(double *Ap,double *oCoord,double *iCoord,double & u,double & v,double & w,double & t);
	bool getBcoordinate(double *Ap,double *oCoord,double *iCoord,double *mCoord,double Rm,double R,double *bCoord1,double *bCoord2);
	bool getBcoordinateUpdate(double *cCoord ,double *bCoord1,double *bCoord2);
	bool getCcoordinate(double *cCoord1,double *cCoord2);
	void getTransFormMatrix(double cosA,double sinA,double cosB,double sinB,mafMatrix3x3 *mat);
	void getAngleAandB(double & cosA,double & sinA,double & cosB,double & sinB);
	double getPi();
	void getGlobalSphereCenter(double *sphCoord);
	void getGlobalCylinderCenter(double *cylCoord);
	double goldenSectionSearch(double a,double b,double x,double accuracy);
	//double getFunctionValue(double x,double *aCoord,double *bCoord);
	double computeDistanceOfLm(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *rCoord,double rm);
	double computeDistanceOfLm2(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double rm);
	void getLocalCenterCoord(double *newCoord,double *oldCoord);
	void getGlobalCenterCoord(double *globalCoord,double *localCoord);
	void getLocalTransformedCoord(double *newCoord,double *oldCoord,mafMatrix3x3 *mat);
	void getIntersectCircle(double *Ap,double *oCoord,double *iCoord,double *mCoord,double & Rm);
	void getGlobalTransformedCoord(double *localCoord,double *globalCoord,mafMatrix3x3 *mat);
	double getCutPlane();
	double getSphereRadius();
	double getCylinderRadius();
	//double caculateHelix(vtkPolyData * hCurve);
	//double computeHelixCost(double tc,double to,double c,double k);
	//void changeXZ(double *dCoord ,double *scoord);
	double drawHelix(double tc,double to,double c,double k,vtkCellArray * cells,vtkPoints *pts,bool drawFlag);
	void getSphereCylinderWrap(const int step);

	double getFunctionValue2(double x,double filterFlag,double *filterPlaneNormal,double *endPoint,double *outAPoint,double *outBPoint,double *outMPoint,double & Rm);
	double getCutPlane2(double *aPoint,double *bPoint,double *mPoint,vtkClipPolyData *outCurve);
	double caculateHelix2(vtkPolyData * hCurve,double *cCoord,double *vCoord,double drawFlag);
	bool getCcoordinate2(double *bCoord,double *cCoord1,double *cCoord2);
	bool getBcoordinateUpdate2(double *aCoord,double *bCoord,double *cCoord ,double *bCoord1,double *bCoord2);
	double testPlane(double *a,double *b,double *c,double *d);
	double computeHelixCost2(double tc,double to,double c,double k,double *cCoord,double *vCoord);
	double computeDistanceOfWholePath(double *oCoord,double *iCoord,double *aCoord,double *bCoord,double *cCoord,double *rCoord,double CIcurve,double rm);
	void formatParameterForHelix(double  tc,double  to,double  Zc,double Zo,double & c ,double & k,double & tcRtn,double & toRtn);

	double getCutPlaneForCI(double *bCoord,double *cCoord,vtkClipPolyData *clipData);
	//double getCutPlaneForCI(double *bCoord,double *cCoord,vtkCutter *clipData);


	void processViaPoint();

	void getWrapLocalTransform(double *inCoord,double *outCoord);
	void getWrapGlobalTransform(double *inCoord,double *outCoord);
	void getWrapAngleAandB(double &cosA,double &sinA,double &cosB,double &sinB);
	// void getWrapMatrix(mafMatrix *inMx,mafMatrix *inMy);
	void getWrapMatrix(mafMatrix &inMx,mafMatrix &inMy);

	int GetViaPoint(double *viaPoint,bool isNearEndflag);
	void computeAngleForB(double *aCoord,double *bCoord1,double *bCoord2,double *bCoord);
	void directConnectSE();
	void get90DegreePointOnCylinder(double *firstPoint,double *sphereCenter,double *secondPoint);
	double getFilterFlag(double *outFilterPlaneNormal);
	void copyPointValue(double *srcPoint,double *aimPoint);
	double computeCurveLength(double *aCoord,double *bCoord,double rm);
	void wrapCylinderOnly(const int step);
	void wrapSingleCylinder(double vId);//for double wrap cylinder
	void wrapSphereOnly(const int step);
	double computeDistanceOfLm3(double *start,double *cCoord,double *iCoord);
	double getCutPlaneForCylinder(double *center,double *t1,double *t2,mafVME *wrapped_vme,vtkClipPolyData *clipData);
	double goldenSectionSearch2(double a,double b,double x,double accuracy);
	void getCcoordinateForCylinder(double *oCoord,double *outC1,double *outC2);
	bool isEndPonintOnCylinder(double tolerance,double *endWrapLocal);
	bool isLineInterSectObject(mafVME *wrapVME,double *point1,double *point2);

	double goldenSectionSearch(double low,double high,double accuracy,double *A,double *b,double *c,double *firstC,int type);
	double getAngleValue(double *A ,double *b,double *c);
	double getPlaneValue(double *a,double *b,double *c,double *d);
	double getDistanceValue(double *start,double *cCoord,double *end);
	void getSphereCylinderWrapAdvance(const int step);
	void transformOutput(vtkPolyData *pData );
	void transformOutputPoint( double *point);
	void transformOutputClipData(vtkPolyData *clipData);
	bool checkAlign();
	bool checkTwoRadius();
	//------------------new function -------------------



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
	double m_CylinderAxis[3];

	double m_ViaPoint[3];
	double m_CosA;
	int m_PathNum;
	//mafString m_sPathNum;
	double m_Rm;
	double m_AbCurve;
	std::vector<Apoint *>  m_APointList;
	std::vector<Apoint *>  m_BPointList;
	std::vector<Apoint *>  m_CPointList;
	//std::vector<double[2]>
	double m_Alist[100][3];
	int m_Idx ;

	mafTransform *m_Transform;

	vtkLineSource     *m_LineSource;
	vtkLineSource     *m_LineSource2;
	//vtkLineSource     *m_LineSourceMiddle;
	vtkAppendPolyData *m_Goniometer;
	mafTransform      *m_TmpTransform;
	mafTransform      *m_TmpTransform2;  

	std::vector<vtkLineSource *> m_LineSourceList; //list of PolyData

	std::vector<double *> m_MiddlePointList; // list of coordinates
	std::vector<int> m_OrderMiddlePointsVMEList; //order list of VME ID
	std::vector<mafString> m_OrderMiddlePointsNameVMEList; //order list of VME Name

	int              m_OrderMiddlePointsVMEListNumberOfElements;

	mafString m_StartVmeName;
	mafString m_EndVme1Name;
	mafString m_EndVme2Name;
	mafString m_WrappedVmeName;
	mafString m_WrappedVmeName2;
	mafString m_ViaPointName;


	vtkMAFSmartPointer<vtkPlaneSource> m_PlaneSource;
	vtkMAFSmartPointer<vtkPlane> m_PlaneCutter;
	vtkMAFSmartPointer<vtkCutter> m_Cutter;
	vtkMAFSmartPointer<vtkPlane> m_PlaneClip;
	vtkMAFSmartPointer<vtkClipPolyData> m_Clip;

	mafMatrix3x3 *m_Mat;
	mafMatrix3x3 *m_Imat;
	mafMatrix m_InMx,m_InMy;
	mafMatrix m_OutMx,m_OutMy;

	wxListBox *m_ListBox;
	int        m_WrappedMode;
	int        m_WrapSide;
	int        m_WrapReverse;

private:
	medVMEComputeWrapping(const medVMEComputeWrapping&); // Not implemented
	void operator=(const medVMEComputeWrapping&); // Not implemented

};
#endif
