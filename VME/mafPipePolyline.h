/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.h,v $
  Language:  C++
  Date:      $Date: 2006-06-03 11:04:18 $
  Version:   $Revision: 1.3 $
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

  /** IDs for the GUI */
  enum PIPE_POLYLINE_WIDGET_ID
  {
    ID_POLYLINE_REPRESENTATION = Superclass::ID_LAST,
    ID_TUBE_RADIUS,
    ID_TUBE_CAPPING,
    ID_LAST
  };

protected:
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

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipePolyline_H__
