/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-04-27 16:27:11 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeMeter_H_
#define _mafPipeMeter_H_

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
// mafPipeMeter :
//----------------------------------------------------------------------------
class mafPipeMeter : public mafPipe
{
public:
  mafTypeMacro(mafPipeMeter,mafPipe);

               mafPipeMeter();
  virtual     ~mafPipeMeter ();

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  vtkTubeFilter          *m_Tube;
  vtkPolyDataMapper	     *m_DataMapper;
  vtkActor               *m_DataActor;
  vtkOutlineCornerFilter *m_SelectionBox;
	vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

protected:
  void UpdateProperty(bool fromTag = false);
};
#endif // _mafPipeMeter_H_