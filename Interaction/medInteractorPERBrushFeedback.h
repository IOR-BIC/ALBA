/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPERBrushFeedback.h,v $
Language:  C++
Date:      $Date: 2011-05-10 15:08:19 $
Version:   $Revision: 1.1.2.1 $
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

  virtual void OnEvent(mafEventBase *event);

  /** Remove actor from render */
  void RemoveActor();

  /** Add actor to render */
  void AddActor();

  /** Enable/disable brush drawing */
  void EnableDrawing(bool enable) { m_EnableDrawing = enable; };

  /** Set the radius */
  void SetRadius(double radius);

  /***/
  void SetBrushShape(int shape);

protected:
  medInteractorPERBrushFeedback();
  virtual ~medInteractorPERBrushFeedback();
  
  void DrawEllipse(double x, double y);
  void DrawBox(double x, double y);

  int m_Radius;
  int m_CurrentShape;

  vtkCoordinate *m_Coordinate;
  vtkSphereSource *m_SphereSource;
  vtkCubeSource *m_CubeSource;
  vtkPolyDataMapper2D *m_BrushMapper;
  vtkActor2D *m_BrushActor;

  bool m_IsActorAdded;
  bool m_EnableDrawing;

  
private:
  medInteractorPERBrushFeedback(const medInteractorPERBrushFeedback&);  // Not implemented.
  void operator=(const medInteractorPERBrushFeedback&);  // Not implemented.
};
#endif 
