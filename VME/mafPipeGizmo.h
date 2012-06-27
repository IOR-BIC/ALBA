/*=========================================================================

 Program: MAF2
 Module: mafPipeGizmo
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeGizmo_H__
#define __mafPipeGizmo_H__

#include "mafPipe.h"
#include "mafOBB.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkCaptionActor2D;
class vtkPolyDataMapper;
class mafGizmoAutoscaleHelper;
class mafGizmoInterface;

//----------------------------------------------------------------------------
// mafPipeGizmo :
//----------------------------------------------------------------------------
/** Visual pipe used to render a mafVMEGizmo. This show simply the polydata present into the VME.*/
class MAF_EXPORT mafPipeGizmo : public mafPipe
{
public:
  mafTypeMacro(mafPipeGizmo,mafPipe);

               mafPipeGizmo();
  virtual     ~mafPipeGizmo ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select);

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};


protected:

  mafGizmoAutoscaleHelper *m_GizmoAutoscaleHelper;

  /** the master gizmo acting as mediator between its parts : see mafGizmoInterface*/
  mafGizmoInterface *m_Mediator;

  mafOBB m_OBB;

  void UpdatePipe();

  mafString m_Caption;

  vtkPolyDataMapper *m_Mapper;
  vtkActor *m_GizmoActor;
  vtkActor *m_OutlineActor;
  vtkCaptionActor2D *m_CaptionActor;
};  
#endif // __mafPipeGizmo_H__
