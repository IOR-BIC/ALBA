/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeGizmo.h,v $
  Language:  C++
  Date:      $Date: 2005-07-08 13:29:33 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeGizmo_H_
#define _mafPipeGizmo_H_

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;

//----------------------------------------------------------------------------
// mafPipeGizmo :
//----------------------------------------------------------------------------
class mafPipeGizmo : public mafPipe
{
public:
  mafTypeMacro(mafPipeGizmo,mafPipe);

               mafPipeGizmo();
  virtual     ~mafPipeGizmo ();

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
#endif // _mafPipeGizmo_H_
