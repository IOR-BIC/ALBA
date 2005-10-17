/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeBox.h,v $
  Language:  C++
  Date:      $Date: 2005-10-17 13:06:27 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeBox_H_
#define _mafPipeBox_H_

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkOutlineSource;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class mafAxes;

//----------------------------------------------------------------------------
// mafPipeBox :
//----------------------------------------------------------------------------
class mafPipeBox : public mafPipe
{
public:
  mafTypeMacro(mafPipeBox,mafPipe);

               mafPipeBox();
  virtual     ~mafPipeBox ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_BOX_WIDGET_ID
  {
    ID_BOUNDS_MODE = Superclass::ID_LAST,
    ID_LAST
  };

  vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;
  mafAxes                *m_Axes;

protected:
//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  int m_BoundsMode;

  void UpdateProperty(bool fromTag = false);
  virtual mmgGui *CreateGui();
};  
#endif // _mafPipeBox_H_
