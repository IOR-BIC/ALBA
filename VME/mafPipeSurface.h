/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-04-22 20:02:53 $
  Version:   $Revision: 1.2 $
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
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface ();

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

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
