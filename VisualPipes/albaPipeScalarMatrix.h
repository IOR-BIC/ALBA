/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeScalarMatrix
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeScalarMatrix_H__
#define __albaPipeScalarMatrix_H__

#include "albaPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkCubeAxesActor2D;
class vtkActor;

//----------------------------------------------------------------------------
// albaPipeScalarMatrix :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeScalarMatrix : public albaPipe
{
public:
  albaTypeMacro(albaPipeScalarMatrix,albaPipe);

               albaPipeScalarMatrix();
  virtual     ~albaPipeScalarMatrix ();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n);
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
