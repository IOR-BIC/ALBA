/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi2DMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-11-21 18:21:49 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmi2DMeter_h
#define __mmi2DMeter_h

#include "mmiPER.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;

class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkXYPlotActor;

/**  Class implementing an interactor for creating 2D measure tool.
  This interactor build a line from picked points on VME and draw a 2D line on the renderwindow.
  If to the interactor is activated the flag to probe the scalar values of the VME then it generate also 
  the histogram of the probed points on VME. This interactor works in renderwindow with ParallelProjectionOn.

  @comments
  - Modified to work on multiple render windows while
  maintaining measure persistence at button up
*/
//----------------------------------------------------------------------------
class mmi2DMeter : public mmiPER
//----------------------------------------------------------------------------
{
public:
  mafTypeMacro(mmi2DMeter, mmiPER);

  enum METER_2D_MODALITY
  {
    DISTANCE_BETWEEN_POINTS = 0,
    DISTANCE_BETWEEN_LINES,
    ANGLE_BETWEEN_LINES,
    ID_RESULT_MEASURE,
    ID_RESULT_ANGLE,
  };

  /** 
  Set the type of the measure, selectable from DISTANCE_BETWEEN_POINTS, DISTANCE_BETWEEN_LINES and ANGLE_BETWEEN_LINES.*/
  void SetMeasureType(int t);
  /** 
  Set the type of the measure to DISTANCE_BETWEEN_POINTS.*/
  void SetMeasureTypeToDistanceBetweenPoints() {SetMeasureType(DISTANCE_BETWEEN_POINTS);};
  /** 
  Set the type of the measure to DISTANCE_BETWEEN_LINES.*/
  void SetMeasureTypeToDistanceBetweenLines()  {SetMeasureType(DISTANCE_BETWEEN_LINES);};
  /** 
  Set the type of the measure to ANGLE_BETWEEN_LINES.*/
  void SetMeasureTypeToAngleBetweenLines()     {SetMeasureType(ANGLE_BETWEEN_LINES);};

  /**
  Remove the meters from the render window*/
  void RemoveMeter();

  /** 
  Turn On/Off the creation of the histogram.*/
  void GenerateHistogram(bool generate);

  /** 
  Turn On the generation of the histogram from the measured line. It force the measure to be on DISTANCE_BETWEEN_POINTS.*/
  void GenerateHistogramOn() {GenerateHistogram(true);};

  /** 
  Turn Off the generation of the histogram.*/
  void GenerateHistogramOff() {GenerateHistogram(false);};

protected:
  mmi2DMeter();
  virtual ~mmi2DMeter();

  virtual void OnLeftButtonDown   (mafEventInteraction *e);
  virtual void OnLeftButtonUp     (mafEventInteraction *e);
  virtual void OnMiddleButtonDown (mafEventInteraction *e);
  virtual void OnMiddleButtonUp   (mafEventInteraction *e);
  virtual void OnRightButtonDown  (mafEventInteraction *e);
  virtual void OnRightButtonUp    (mafEventInteraction *e);
  virtual void OnButtonDown       (mafEventInteraction *e);
  virtual void OnMove             (mafEventInteraction *e);
  virtual void OnButtonUp         (mafEventInteraction *e); 
  virtual void OnChar             (mafEventInteraction *e) {};

  /** 
  Draw the measure tool according to the measure type selected.*/
  void DrawMeasureTool(double x, double y);
  /** 
  Calculate the measure according to the measure type selected*/
  void CalculateMeasure();
  /** 
  Create the histogram*/
  void CreateHistogram();

  vtkLineSource       *m_ProbingLine;
  vtkLineSource       *m_Line; ///< First line of the measure tool
  vtkCoordinate       *m_Coordinate;
  vtkPolyDataMapper2D *m_LineMapper;
  vtkActor2D          *m_LineActor;
  vtkLineSource       *m_Line2; ///< Second line of the measure tool (used for angle measures)
  vtkPolyDataMapper2D *m_LineMapper2;
  vtkActor2D          *m_LineActor2;
//  vtkRenderWindow     *LastRenderWindow;
  vtkRenderer         *m_LastRenderer; ///< Renderer used for the first line
  vtkRenderer         *m_CurrentRenderer;

	wxDialog  *m_HistogramDialog;
  mafRWI    *m_HistogramRWI;
  vtkXYPlotActor *m_PlotActor;

  bool m_GenerateHistogram;
  bool m_DraggingLine;
  bool m_DraggingLeft;
  bool m_DraggingMouse;
  bool m_EndMeasure;
  int  m_MeasureType;

  double  m_PickedPoint[3]; ///< 3D position of the picked point; used to assign position to the m_ProbingLine
  mafVME *m_ProbedVME; ///< VME probed by the m_ProbingLine
  
private:
  mmi2DMeter(const mmi2DMeter&);   // Not implemented.
  void operator=(const mmi2DMeter&);  // Not implemented.
};
#endif

