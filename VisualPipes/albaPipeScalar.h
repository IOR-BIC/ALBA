/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeScalar
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeScalar_H__
#define __albaPipeScalar_H__

#include "albaPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkCubeAxesActor2D;
class vtkActor;

//----------------------------------------------------------------------------
// albaPipeScalar :
//----------------------------------------------------------------------------
/** Visual pipe that visually represents the VTK representation of albaVMEScalar.*/
class ALBA_EXPORT albaPipeScalar : public albaPipe
{
public:
  albaTypeMacro(albaPipeScalar,albaPipe);

               albaPipeScalar();
  virtual     ~albaPipeScalar ();

  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

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
  virtual albaGUI  *CreateGui();
};  
#endif // __albaPipeScalar_H__
