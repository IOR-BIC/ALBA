/*=========================================================================

 Program: MAF2
 Module: mafPipeBox
 Authors: Silvano Imboden, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafPipeBox : public mafPipe
{
public:
  mafTypeMacro(mafPipeBox,mafPipe);

               mafPipeBox();
  virtual     ~mafPipeBox ();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select);

  void SetBoundsMode(int mode){if(mode>=BOUNDS_3D && mode<=BOUNDS_4D_SUBTREE)m_BoundsMode=mode;};
  int  GetBoundsMode(){return m_BoundsMode;};


  /** IDs for the GUI */
  enum PIPE_BOX_WIDGET_ID
  {
    ID_BOUNDS_MODE = Superclass::ID_LAST,
    ID_LAST
  };

  enum BOUNDS_MODE
  {
    BOUNDS_3D = 0,
    BOUNDS_4D,
    BOUNDS_3D_SUBTREE,
    BOUNDS_4D_SUBTREE
  };

protected:

  vtkOutlineSource       *m_Box;
  vtkPolyDataMapper	     *m_Mapper;
  mafLODActor            *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  mafLODActor            *m_OutlineActor;
  mafAxes                *m_Axes;

  int m_BoundsMode;

  void UpdateProperty(bool fromTag = false);
  virtual mafGUI *CreateGui();
};  
#endif // __mafPipeBox_H__
