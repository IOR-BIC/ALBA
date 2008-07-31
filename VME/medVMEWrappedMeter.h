/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2008-07-31 08:34:45 $
  Version:   $Revision: 1.8 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEWrappedMeter_h
#define __medVMEWrappedMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "medVMEOutputWrappedMeter.h"
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

/** medVMEWrappedMeter - 
*/
class MAF_EXPORT medVMEWrappedMeter : public mafVME
{
public:
  MAF_ID_DEC(LENGTH_THRESHOLD_EVENT);

  enum METER_MEASURE_TYPE_ID
  {
    POINT_DISTANCE=0,
  //  LINE_DISTANCE,
  //  LINE_ANGLE
  };
  enum METER_MODALITY_TYPE_ID
  {
    MANUAL_WRAP=0,
    AUTOMATED_WRAP,
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
  mafTypeMacro(medVMEWrappedMeter,mafVME);

  enum METER_WIDGET_ID
  {
    ID_START_METER_LINK = Superclass::ID_LAST,
    ID_END1_METER_LINK,
    ID_END2_METER_LINK,
    ID_WRAPPED_METER_LINK,
		ID_METER_MODE,
	  ID_WRAPPED_METER_MODE,
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
  virtual mafString GetVisualPipe() {return mafString("medPipeWrappedMeter");};

  /** return the right type of output */  
  medVMEOutputWrappedMeter *GetWrappedMeterOutput();

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
  void SetMeterModeToPointDistance() {this->SetMeterMode(medVMEWrappedMeter::POINT_DISTANCE);}
 // void SetMeterModeToLineDistance() {this->SetMeterMode(medVMEWrappedMeter::LINE_DISTANCE);}
 // void SetMeterModeToLineAngle() {this->SetMeterMode(medVMEWrappedMeter::LINE_ANGLE);}

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
  void SetMeterColorModeToOneColor() {this->SetMeterColorMode(medVMEWrappedMeter::ONE_COLOR);}
  void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(medVMEWrappedMeter::RANGE_COLOR);}

  /** 
  Get the color mode of the meter. */
  int GetMeterColorMode();

  /** 
  Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(medVMEWrappedMeter::ABSOLUTE_MEASURE);}
  void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(medVMEWrappedMeter::RELATIVE_MEASURE);}

  /** 
  Get the measure type. */
  int GetMeterMeasureType();

  /** 
  Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() {this->SetMeterRepresentation(medVMEWrappedMeter::LINE_REPRESENTATION);}
  void SetMeterRepresentationToTube() {this->SetMeterRepresentation(medVMEWrappedMeter::TUBE_REPRESENTATION);}

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
  mafVME *GetWrappedVME();

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
  medVMEWrappedMeter();
  virtual ~medVMEWrappedMeter();

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

  /** update the output data structure for automate wrapped mode */
  virtual void InternalUpdateAutomated();
  

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

  double m_Distance;
  double m_Angle;
  double m_StartPoint[3];
  double m_EndPoint[3];
  double m_EndPoint2[3];
  
  double m_WrappedVMECenter[3];
  double m_WrappedTangent1[3];
  double m_WrappedTangent2[3];


  mafTransform *m_Transform;

  vtkLineSource     *m_LineSource;
  vtkLineSource     *m_LineSource2;
  //vtkLineSource     *m_LineSourceMiddle;
  vtkAppendPolyData *m_Goniometer;
  mafTransform      *m_TmpTransform;
  
	std::vector<vtkLineSource *> m_LineSourceList; //list of PolyData
	
	std::vector<double *> m_MiddlePointList; // list of coordinates
	std::vector<int> m_OrderMiddlePointsVMEList; //order list of VME ID
	std::vector<mafString> m_OrderMiddlePointsNameVMEList; //order list of VME Name

	int              m_OrderMiddlePointsVMEListNumberOfElements;

  mafString m_StartVmeName;
  mafString m_EndVme1Name;
  mafString m_EndVme2Name;
  mafString m_WrappedVmeName;

  vtkMAFSmartPointer<vtkPlaneSource> m_PlaneSource;
  vtkMAFSmartPointer<vtkPlane> m_PlaneCutter;
  vtkMAFSmartPointer<vtkCutter> m_Cutter;
  vtkMAFSmartPointer<vtkPlane> m_PlaneClip;
  vtkMAFSmartPointer<vtkClipPolyData> m_Clip;

  wxListBox *m_ListBox;
	int        m_WrappedMode;
  int        m_WrapSide;
  int        m_WrapReverse;

private:
  medVMEWrappedMeter(const medVMEWrappedMeter&); // Not implemented
  void operator=(const medVMEWrappedMeter&); // Not implemented
};
#endif
