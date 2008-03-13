/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeBox.h,v $
  Language:  C++
  Date:      $Date: 2008-03-13 16:51:43 $
  Version:   $Revision: 1.7 $
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

  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

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
  virtual mmgGui *CreateGui();
};  
#endif // __mafPipeBox_H__
