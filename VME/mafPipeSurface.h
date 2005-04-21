/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:36:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeSurface_H_
#define _mafPipeSurface_H_

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
//@@@class mafAxes;    //SIL. 20-5-2003 added line - 

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class mafPipeSurface : public mafPipe
{
public:
               mafPipeSurface(mafSceneNode *n,bool use_axes = true);
  virtual     ~mafPipeSurface ();
  //virtual void Show       (bool show); 
  virtual void Select     (bool select); 

  vtkPolyDataMapper	     *m_act_m;
  vtkActor               *m_act_a;
  vtkOutlineCornerFilter *m_sel_ocf;
	vtkPolyDataMapper      *m_sel_m;
  vtkProperty            *m_sel_p;
  vtkActor               *m_sel_a;
//@@@	mafAxes                *m_axes;     //SIL. 20-5-2003 added line - 

protected:
//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  void UpdateProperty(bool fromTag = false);
};  
#endif // _mafPipeSurface_H_
