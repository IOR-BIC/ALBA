/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractorPERBrushFeedback_h
#define __mafInteractorPERBrushFeedback_h

#include "mafDefines.h"
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
class MAF_EXPORT mafInteractorPERBrushFeedback : public mafInteractorPER
{
public: 
  
  enum BRUSH_SHAPES
  {
    CIRCLE_BRUSH_SHAPE = 0,
    SQUARE_BRUSH_SHAPE,
  };

  mafTypeMacro(mafInteractorPERBrushFeedback,mafInteractorPER);

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

  /** Set the target spacing to "align" volume and brush dimensions */
  void SetTargetVolumeSpacing(double spacing){m_TargetVolumeSpacing = spacing;};

  /** */
  double GetRealFeedBackShapeRadius(){return double(m_Radius * m_TargetVolumeSpacing);};

protected:
  /** constructor. */
  mafInteractorPERBrushFeedback();
  /** destructor. */
  virtual ~mafInteractorPERBrushFeedback();
  
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
  double m_TargetVolumeSpacing;

};
#endif 
