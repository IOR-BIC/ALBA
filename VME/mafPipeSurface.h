/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-10-17 13:07:15 $
  Version:   $Revision: 1.8 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeSurface_H_
#define _mafPipeSurface_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class mafAxes;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class mafPipeSurface : public mafPipe
{
public:
  mafTypeMacro(mafPipeSurface,mafPipe);

               mafPipeSurface();
  virtual     ~mafPipeSurface ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_SCALAR_VISIBILITY = Superclass::ID_LAST,
    ID_LAST
  };

  vtkTexture             *m_Texture;
  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;
	mafAxes                *m_Axes;

protected:
  int  m_ScalarVisibility;
//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  void UpdateProperty(bool fromTag = false);

  virtual mmgGui  *CreateGui();
};  
#endif // _mafPipeSurface_H_
