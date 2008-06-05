/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeGizmo.h,v $
  Language:  C++
  Date:      $Date: 2008-06-05 14:06:26 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeGizmo_H__
#define __mafPipeGizmo_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkCaptionActor2D;

//----------------------------------------------------------------------------
// mafPipeGizmo :
//----------------------------------------------------------------------------
/** Visual pipe used to render a mafVMEGizmo. This show simply the polydata present into the VME.*/
class mafPipeGizmo : public mafPipe
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

protected:
  void UpdatePipe();

  mafString m_Caption;

  vtkActor *m_Actor;
  vtkActor *m_OutlineActor;
  vtkCaptionActor2D *m_CaptionActor;
};  
#endif // __mafPipeGizmo_H__
