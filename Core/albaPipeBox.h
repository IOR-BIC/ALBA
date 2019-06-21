/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeBox
 Authors: Silvano Imboden, Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeBox_H__
#define __albaPipeBox_H__

#include "albaPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkOutlineSource;
class vtkPolyDataMapper;
class albaLODActor;
class vtkProperty;
class albaAxes;

//----------------------------------------------------------------------------
// albaPipeBox :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeBox : public albaPipe
{
public:
  albaTypeMacro(albaPipeBox,albaPipe);

               albaPipeBox();
  virtual     ~albaPipeBox ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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
  albaLODActor            *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  albaLODActor            *m_OutlineActor;
  albaAxes                *m_Axes;

  int m_BoundsMode;

  void UpdateProperty(bool fromTag = false);
  virtual albaGUI *CreateGui();
};  
#endif // __albaPipeBox_H__
