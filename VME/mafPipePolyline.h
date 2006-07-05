/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.h,v $
  Language:  C++
  Date:      $Date: 2006-07-05 11:13:19 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipePolyline_H__
#define __mafPipePolyline_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkTubeFilter;
class vtkGlyph3D;
class vtkSphereSource;

//----------------------------------------------------------------------------
// mafPipePolyline :
//----------------------------------------------------------------------------
class mafPipePolyline : public mafPipe
{
public:
  mafTypeMacro(mafPipePolyline,mafPipe);

               mafPipePolyline();
  virtual     ~mafPipePolyline ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  /** Set the visual representation of the polyline.
  Acceptable values are 0 (POLYLINE), 1 (TUBE) or 2 (SPHERE GLYPHED).*/
  void SetRepresentation(int representation);

  /** Set the polyline representation as simple polyline.*/
  void SetRepresentationToPolyline() {SetRepresentation(POLYLINE);};

  /** Set the polyline representation as tube.*/
  void SetRepresentationToTube() {SetRepresentation(TUBE);};

  /** Set the polyline representation as sphere glyphed polyline.*/
  void SetRepresentationToGlyph() {SetRepresentation(GLYPH);};

  /** Set The Radius */
  void SetRadius(double radius);

  /** IDs for the GUI */
  enum PIPE_POLYLINE_WIDGET_ID
  {
    ID_POLYLINE_REPRESENTATION = Superclass::ID_LAST,
    ID_TUBE_RADIUS,
    ID_TUBE_RESOLUTION,
    ID_TUBE_CAPPING,
    ID_SPHERE_RADIUS,
    ID_SPHERE_RESOLUTION,
    ID_LAST
  };

  enum POLYLINE_REPRESENTATION
  {
    POLYLINE = 0,
    TUBE,
    GLYPH
  };

protected:
  vtkSphereSource        *m_Sphere;
  vtkGlyph3D             *m_Glyph;
  vtkTubeFilter          *m_Tube;
  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;

  int m_Representation;
  int m_Capping;
  double m_TubeRadius;
  double m_SphereRadius;
  double m_TubeResolution;
  double m_SphereResolution;

  /** Initialize representation, capping, radius and resolution variables.*/
  void InitializeFromTag();

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipePolyline_H__
