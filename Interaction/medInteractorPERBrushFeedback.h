/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPERBrushFeedback.h,v $
Language:  C++
Date:      $Date: 2012-02-08 13:13:21 $
Version:   $Revision: 1.1.2.6 $
Authors:   Eleonora Mambrini, Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medInteractorPERBrushFeedback_h
#define __medInteractorPERBrushFeedback_h

#include "mafInteractorPER.h"

class vtkActor2D;
class vtkImageMapper;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkLineSource;
class vtkCubeSource;
class vtkSphereSource;

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------


/** Segmentation Editing Interactor
*/
class medInteractorPERBrushFeedback : public mafInteractorPER
{
public: 
  
  enum BRUSH_SHAPES
  {
    CIRCLE_BRUSH_SHAPE = 0,
    SQUARE_BRUSH_SHAPE,
  };

  mafTypeMacro(medInteractorPERBrushFeedback,mafInteractorPER);

  /* Main event handler */
  virtual void OnEvent(mafEventBase *event);

  /** Remove actor from render */
  void RemoveActor();

  /** Add actor to render */
  void AddActor();

  /** Enable/disable brush drawing */
  void EnableDrawing(bool enable) { m_EnableDrawing = enable; };

  /** Set the radius */
  void SetRadius(double radius);

  /** Set the Shape of the brush*/
  void SetBrushShape(int shape);

protected:
  /** constructor. */
  medInteractorPERBrushFeedback();
  /** destructor. */
  virtual ~medInteractorPERBrushFeedback();
  
  /* draw an the Brush ellipse*/
  void DrawEllipse(double x, double y);
  /* draw an the Brush box*/
  void DrawBox(double x, double y);


 
  double m_Radius;
  int m_CurrentShape;

  vtkCoordinate *m_Coordinate;
  vtkSphereSource *m_SphereSource;
  vtkCubeSource *m_CubeSource;
  vtkPolyDataMapper2D *m_BrushMapper;
  vtkActor2D *m_BrushActor;

  bool m_IsActorAdded;
  bool m_EnableDrawing;
  bool m_MouseDown;
  double m_Count;

};
#endif 
