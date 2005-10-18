/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi2DMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-10-18 13:45:44 $
  Version:   $Revision: 1.2 $
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
class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;

/**  mmi2DMeter
  @comments
  - Stefano 7-7-2005: Modified to work on multiple render windows while
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

  void SetMeasureType(int t) {MeasureType = t;};
  void SetMeasureTypeToDistanceBetweenPoints() {MeasureType = DISTANCE_BETWEEN_POINTS;};
  void SetMeasureTypeToDistanceBetweenLines()  {MeasureType = DISTANCE_BETWEEN_LINES;};
  void SetMeasureTypeToAngleBetweenLines()     {MeasureType = ANGLE_BETWEEN_LINES;};

  /**remove the meters from the render window*/
  void RemoveMeter();

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

  void DrawMeasureTool(double x, double y);
  void CalculateMeasure();

  vtkLineSource       *Line;
  vtkCoordinate       *Coordinate;
  vtkPolyDataMapper2D *LineMapper;
  vtkActor2D          *LineActor;
  vtkLineSource       *Line2;
  vtkPolyDataMapper2D *LineMapper2;
  vtkActor2D          *LineActor2;
//  vtkRenderWindow     *LastRenderWindow;
  /** the renderer used for the first line */
  vtkRenderer         *LastRenderer; 
  vtkRenderer         *CurrentRenderer;

	bool m_dragging_line;
  bool m_dragging_left;
  bool m_dragging;
  bool EndMeasure;
  int  MeasureType;
  
private:
  mmi2DMeter(const mmi2DMeter&);   // Not implemented.
  void operator=(const mmi2DMeter&);  // Not implemented.
};
#endif

