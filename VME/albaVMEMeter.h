/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeter
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEMeter_h
#define __albaVMEMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEVolumeGray.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMeter;
class mmaMaterial;
class vtkLineSource;
class vtkLineSource;
class vtkAppendPolyData;

class vtkXYPlotActor;
class albaRWI;
class albaGUIDialogPreview;


/** albaVMEMeter - 
*/
class ALBA_EXPORT albaVMEMeter : public albaVME
{
public:
  ALBA_ID_DEC(LENGTH_THRESHOLD_EVENT);

  enum METER_MEASURE_TYPE_ID
  {
    POINT_DISTANCE=0,
    LINE_DISTANCE,
    LINE_ANGLE
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
  albaTypeMacro(albaVMEMeter,albaVME);

  enum METER_WIDGET_ID
  {
    ID_START_METER_LINK = Superclass::ID_LAST,
    ID_END1_METER_LINK,
    ID_END2_METER_LINK,
    ID_PLOTTED_VME_LINK,
	  ID_METER_MODE,
    ID_PLOT_PROFILE,
    ID_LAST
  };

  static bool VMEAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVME));};
  static bool VolumeAccept(albaVME*node) {return(node != NULL && node->IsALBAType(albaVMEVolumeGray));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Meter into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-Meter. */
  virtual bool Equals(albaVME *vme);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeMeter");};

  /** return the right type of output */  
  albaVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

  /** Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /**Get/Set the kind of measure  performed: distance between points (POINT) or
  distance between a point and a line. */
  int GetMeterMode();
  void SetMeterMode(int mode);
  void SetMeterModeToPointDistance() {this->SetMeterMode(albaVMEMeter::POINT_DISTANCE);}
  void SetMeterModeToLineDistance() {this->SetMeterMode(albaVMEMeter::LINE_DISTANCE);}
  void SetMeterModeToLineAngle() {this->SetMeterMode(albaVMEMeter::LINE_ANGLE);}

  /** Store the min and max distance to associate with colors.*/
  void SetDistanceRange(double min, double max);
  void SetDistanceRange(double range[2]) {SetDistanceRange(range[0],range[1]);};

  /** Retrieve the distance range associated with colors.*/
  double *GetDistanceRange();
  void GetDistanceRange(double range[2]);

  /** Color the meter with a range colors extracted by a LookupTable or in flat mode selected by material library. */
  void SetMeterColorMode(int mode);
  void SetMeterColorModeToOneColor() {this->SetMeterColorMode(albaVMEMeter::ONE_COLOR);}
  void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(albaVMEMeter::RANGE_COLOR);}

  /** Get the color mode of the meter. */
  int GetMeterColorMode();

  /** Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(albaVMEMeter::ABSOLUTE_MEASURE);}
  void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(albaVMEMeter::RELATIVE_MEASURE);}

  /** Get the measure type. */
  int GetMeterMeasureType();

  /** Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() {this->SetMeterRepresentation(albaVMEMeter::LINE_REPRESENTATION);}
  void SetMeterRepresentationToTube() {this->SetMeterRepresentation(albaVMEMeter::TUBE_REPRESENTATION);}

  /** Get the representation mode of the meter. */
  int GetMeterRepresentation();

  /** Represent the meter with a tube or as a line. */
  void SetMeterCapping(int capping);
  void SetMeterCappingOn() {this->SetMeterCapping(1);}
  void SetMeterCappingOff() {this->SetMeterCapping(0);}

  /** Get the capping of tube representation of the meter. */
  int GetMeterCapping();

  /** Enable-Disable event generation of length threshold. */
  void SetGenerateEvent(int generate);
  void SetGenerateEventOn() {SetGenerateEvent(1);}
  void SetGenerateEventOff() {SetGenerateEvent(0);}

  /** Get the length threshold event generation status. */
  int GetGenerateEvent();

  /** Set the initial measure. */
  void SetInitMeasure(double init_measure);

  /** Get the initial measure. */
  double GetInitMeasure();

  /** Set the meter radius when represented as tube. */
  void SetMeterRadius(double radius);

  /** Get the meter radius when represented as tube. */
  double GetMeterRadius();

  /** Set the percentage elongation. */
  void SetDeltaPercent(int delta_percent);

  /** Get the percentage elongation. */
  int GetDeltaPercent();

  /** return distance extracting it from the connected pipe. */
  double GetDistance();

  /** return angle extracting it from the connected pipe. */
  double GetAngle();

  /** return the meter's attributes */
  mmaMeter *GetMeterAttributes();

  albaVME *GetStartVME();
  albaVME *GetEnd1VME();
  albaVME *GetEnd2VME();
  albaVME *GetPlottedVME();

  /** Set links for the meter*/
  virtual void SetMeterLink(const char *link_name, albaVME *n);

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Turn On/Off the creation of the histogram.*/
  void GenerateHistogram(int generate);

  /** Turn On the generation of the histogram from the measured line. It force the measure to be on DISTANCE_BETWEEN_POINTS.*/
  void GenerateHistogramOn() {GenerateHistogram(1);};

  /** Turn Off the generation of the histogram.*/
  void GenerateHistogramOff() {GenerateHistogram(0);};

  /** Retrieve StartPoint coordinates*/
  double *GetStartPointCoordinate(){return m_StartPoint;};

  /** Retrieve EndPoint1 coordinates*/
  double *GetEndPointCoordinate(){return m_EndPoint;};

  /** Retrieve EndPoint coordinates*/
  double *GetEndPoint2Coordinate(){return m_EndPoint2;};

protected:
  albaVMEMeter();
  virtual ~albaVMEMeter();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** this creates the Material attribute at the right time... */
  virtual int InternalInitialize();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** Update the names for the meter's links. */
  void UpdateLinks();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  /** 
  Create the histogram*/
  void CreateHistogram();

  double m_Distance;
  double m_Angle;
  double m_StartPoint[3];
  double m_EndPoint[3];
  double m_EndPoint2[3];

  albaTransform *m_Transform;

  vtkLineSource     *m_LineSource;
  vtkLineSource     *m_LineSource2;
  vtkAppendPolyData *m_Goniometer;
  vtkPolyData       *m_PolyData;
  albaTransform      *m_TmpTransform;

  albaString m_StartVmeName;
  albaString m_EndVme1Name;
  albaString m_EndVme2Name;
  albaString m_ProbeVmeName;

  vtkLineSource       *m_ProbingLine;

  albaGUIDialogPreview  *m_HistogramDialog;
  albaRWI    *m_HistogramRWI;
  vtkXYPlotActor *m_PlotActor;

  int m_GenerateHistogram;
  albaVMEVolumeGray *m_ProbedVME; ///< VME probed by the m_ProbingLine

private:
  albaVMEMeter(const albaVMEMeter&); // Not implemented
  void operator=(const albaVMEMeter&); // Not implemented
};
#endif
