/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.h,v $
  Language:  C++
  Date:      $Date: 2008-02-01 12:50:52 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeScalar_H__
#define __mafPipeScalar_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkCubeAxesActor2D;
class vtkActor;

//----------------------------------------------------------------------------
// mafPipeScalar :
//----------------------------------------------------------------------------
/** Visual pipe that visually represents the VTK representation of mafVMEScalar.*/
class mafPipeScalar : public mafPipe
{
public:
  mafTypeMacro(mafPipeScalar,mafPipe);

               mafPipeScalar();
  virtual     ~mafPipeScalar ();

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_SCALAR_WIDGET_ID
  {
    ID_RADIUS = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  vtkCubeAxesActor2D *m_CubeAxes;
  vtkActor *m_Actor;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false);
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeScalar_H__
