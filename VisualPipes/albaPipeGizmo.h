/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGizmo
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeGizmo_H__
#define __albaPipeGizmo_H__

#include "albaPipe.h"
#include "albaOBB.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkCaptionActor2D;
class vtkPolyDataMapper;
class albaGizmoAutoscaleHelper;
class albaGizmoInterface;

//----------------------------------------------------------------------------
// albaPipeGizmo :
//----------------------------------------------------------------------------
/** Visual pipe used to render a albaVMEGizmo. This show simply the polydata present into the VME.*/
class ALBA_EXPORT albaPipeGizmo : public albaPipe
{
public:
  albaTypeMacro(albaPipeGizmo,albaPipe);

               albaPipeGizmo();
  virtual     ~albaPipeGizmo ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select);

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Get assembly front/back */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};


protected:

  albaGizmoAutoscaleHelper *m_GizmoAutoscaleHelper;

  /** the master gizmo acting as mediator between its parts : see albaGizmoInterface*/
  albaGizmoInterface *m_Mediator;

  albaOBB m_OBB;

  void UpdatePipe();

  albaString m_Caption;

  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_GizmoActor;
  vtkActor *m_OutlineActor;
  vtkCaptionActor2D *m_CaptionActor;
};  
#endif // __albaPipeGizmo_H__
