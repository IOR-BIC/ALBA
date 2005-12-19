/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.h,v $
  Language:  C++
  Date:      $Date: 2005-12-19 14:55:09 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipePolyline_H_
#define _mafPipePolyline_H_

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
#endif // _mafPipePolyline_H_
