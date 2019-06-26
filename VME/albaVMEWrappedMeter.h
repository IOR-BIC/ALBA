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
#ifndef __albaVMEWrappedMeter_h
#define __albaVMEWrappedMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaVMEOutputWrappedMeter.h"
#include "albaEvent.h"
#include "albaVMESurfaceParametric.h"
#include "vtkALBASmartPointer.h"
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

/** albaVMEWrappedMeter - 
*/
class ALBA_EXPORT albaVMEWrappedMeter : public albaVME
{
public:
  ALBA_ID_DEC(LENGTH_THRESHOLD_EVENT);

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
    IOR_AUTOMATED_WRAP,
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
  albaTypeMacro(albaVMEWrappedMeter,albaVME);

  enum METER_WIDGET_ID
  {
    ID_START_METER_LINK = Superclass::ID_LAST,
    ID_END1_METER_LINK,
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
  virtual albaString GetVisualPipe() {return albaString("albaPipeWrappedMeter");};

  /** return the right type of output */  
  albaVMEOutputWrappedMeter *GetWrappedMeterOutput();

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

  void EnableManualModeWidget(bool flag);

  /**
  Get/Set the kind of measure  performed: distance between points (POINT) or
  distance between a point and a line. */
  int GetMeterMode();
  void SetMeterMode(int mode);
  void SetMeterModeToPointDistance() {this->SetMeterMode(albaVMEWrappedMeter::POINT_DISTANCE);}
 // void SetMeterModeToLineDistance() {this->SetMeterMode(albaVMEWrappedMeter::LINE_DISTANCE);}
 // void SetMeterModeToLineAngle() {this->SetMeterMode(albaVMEWrappedMeter::LINE_ANGLE);}

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
  void SetMeterColorModeToOneColor() {this->SetMeterColorMode(albaVMEWrappedMeter::ONE_COLOR);}
  void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(albaVMEWrappedMeter::RANGE_COLOR);}

  /** 
  Get the color mode of the meter. */
  int GetMeterColorMode();

  /** 
  Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(albaVMEWrappedMeter::ABSOLUTE_MEASURE);}
  void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(albaVMEWrappedMeter::RELATIVE_MEASURE);}

  /** 
  Get the measure type. */
  int GetMeterMeasureType();

  /** 
  Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() {this->SetMeterRepresentation(albaVMEWrappedMeter::LINE_REPRESENTATION);}
  void SetMeterRepresentationToTube() {this->SetMeterRepresentation(albaVMEWrappedMeter::TUBE_REPRESENTATION);}

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
  albaVME *GetWrappedVME();

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
  int GetWrappedMode(){return m_WrappedMode;};
  void SetWrappedMode(int value){m_WrappedMode = value;};

  /**Set\Get side of geometric wrapping: the side where search tangent over parametric surface*/
  int GetWrapSide(){return m_WrapSide;};
  void SetWrapSide(int value){m_WrapSide = value;};

  /**Set\Get reverse modality: reverse allows which direction follow for clip after finding tangent points.*/
  int GetWrapReverse(){return m_WrapReverse;};
  void SetWrapReverse(int value){m_WrapReverse = value;};

  void AddMidPoint(albaVME *node);

protected:
  albaVMEWrappedMeter();
  virtual ~albaVMEWrappedMeter();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

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


  double m_Distance;
  double m_Angle;
  double m_StartPoint[3];
  double m_EndPoint[3];
  double m_EndPoint2[3];
  
  double m_WrappedVMECenter[3];
  double m_WrappedTangent1[3];
  double m_WrappedTangent2[3];


  albaTransform *m_Transform;

  vtkLineSource     *m_LineSource;
  vtkLineSource     *m_LineSource2;
  //vtkLineSource     *m_LineSourceMiddle;
  vtkAppendPolyData *m_Goniometer;
  albaTransform      *m_TmpTransform;
  
	std::vector<vtkLineSource *> m_LineSourceList; //list of PolyData
	
	std::vector<double *> m_MiddlePointList; // list of coordinates
	std::vector<int> m_OrderMiddlePointsVMEList; //order list of VME ID
	std::vector<albaString> m_OrderMiddlePointsNameVMEList; //order list of VME Name

	int              m_OrderMiddlePointsVMEListNumberOfElements;

  albaString m_StartVmeName;
  albaString m_EndVme1Name;
  albaString m_WrappedVmeName;

  vtkALBASmartPointer<vtkPlaneSource> m_PlaneSource;
  vtkALBASmartPointer<vtkPlane> m_PlaneCutter;
  vtkALBASmartPointer<vtkCutter> m_Cutter;
  vtkALBASmartPointer<vtkPlane> m_PlaneClip;
  vtkALBASmartPointer<vtkClipPolyData> m_Clip;

  wxListBox *m_ListBox;
	int        m_WrappedMode;
  int        m_WrapSide;
  int        m_WrapReverse;

private:
  albaVMEWrappedMeter(const albaVMEWrappedMeter&); // Not implemented
  void operator=(const albaVMEWrappedMeter&); // Not implemented
};
#endif
