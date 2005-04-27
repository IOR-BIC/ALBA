/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-04-27 16:27:49 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEMeter_h
#define __mafVMEMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMeter;
class vtkLineSource;
class vtkLineSource;
class vtkAppendPolyData;

/** mafVMEMeter - 
*/
class MAF_EXPORT mafVMEMeter : public mafVME
{
public:
  MAF_ID_DEC(LengthThresholdEvent);

  enum {
    POINT_DISTANCE=0,
    LINE_DISTANCE,
    LINE_ANGLE
  };
  enum {
    ONE_COLOR=0,
    RANGE_COLOR
  };
  enum {
    LINE_REPRESENTATION=0,
    TUBE_REPRESENTATION
  };
  enum {
    ABSOLUTE_MEASURE=0,
    RELATIVE_MEASURE
  };
  mafTypeMacro(mafVMEMeter,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputPolyline *GetPolylineOutput();

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

  /**
  Get/Set the kind of measure  performed: distance between points (POINT) or
  distance between a point and a line. */
  int GetMeterMode();
  void SetMeterMode(int mode);
  void SetMeterModeToPointDistance() {this->SetMeterMode(mafVMEMeter::POINT_DISTANCE);}
  void SetMeterModeToLineDistance() {this->SetMeterMode(mafVMEMeter::LINE_DISTANCE);}
  void SetMeterModeToLineAngle() {this->SetMeterMode(mafVMEMeter::LINE_ANGLE);}

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
  void SetMeterColorModeToOneColor() {this->SetMeterColorMode(mafVMEMeter::ONE_COLOR);}
  void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(mafVMEMeter::RANGE_COLOR);}

  /** 
  Get the color mode of the meter. */
  int GetMeterColorMode();

  /** 
  Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(mafVMEMeter::ABSOLUTE_MEASURE);}
  void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(mafVMEMeter::RELATIVE_MEASURE);}

  /** 
  Get the measure type. */
  int GetMeterMeasureType();

  /** 
  Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() {this->SetMeterRepresentation(mafVMEMeter::LINE_REPRESENTATION);}
  void SetMeterRepresentationToTube() {this->SetMeterRepresentation(mafVMEMeter::TUBE_REPRESENTATION);}

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
  void SetGenerateEventOn() {this->SetGenerateEvent(1);}
  void SetGenerateEventOff() {this->SetGenerateEvent(0);}

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

protected:
  mafVMEMeter();
  virtual ~mafVMEMeter();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** return the meter's attributes */
  mmaMeter *GetMeterAttributes();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  double m_Distance;
  double m_Angle;
  double m_StartPoint[3];
  double m_EndPoint[3];
  double m_EndPoint2[3];

  mafTransform *m_Transform;
  mmaMeter     *m_MeterAttributes;

  vtkLineSource     *m_LineSource;
  vtkLineSource     *m_LineSource2;
  vtkAppendPolyData *m_Goniometer;
  mafTransform      *m_TmpTransform;

private:
  mafVMEMeter(const mafVMEMeter&); // Not implemented
  void operator=(const mafVMEMeter&); // Not implemented
};
#endif