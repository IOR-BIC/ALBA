/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeBox.h,v $
  Language:  C++
  Date:      $Date: 2006-07-24 08:52:58 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeBox_H__
#define __mafPipeBox_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkOutlineSource;
class vtkPolyDataMapper;
class mafLODActor;
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
  mafLODActor            *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
	vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  mafLODActor            *m_OutlineActor;
  mafAxes                *m_Axes;

protected:
//@@@  bool                    m_use_axes; //SIL. 22-5-2003 added line - 
  int m_BoundsMode;

  void UpdateProperty(bool fromTag = false);
  virtual mmgGui *CreateGui();
};  
#endif // __mafPipeBox_H__
