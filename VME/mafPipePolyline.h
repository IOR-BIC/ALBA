/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.h,v $
  Language:  C++
  Date:      $Date: 2006-03-02 22:00:51 $
  Version:   $Revision: 1.2 $
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

//----------------------------------------------------------------------------
// mafPipePolyline :
//----------------------------------------------------------------------------
class mafPipePolyline : public mafPipe
{
public:
  mafTypeMacro(mafPipePolyline,mafPipe);

               mafPipePolyline();
  virtual     ~mafPipePolyline ();

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;

protected:
};  
#endif // __mafPipePolyline_H__
