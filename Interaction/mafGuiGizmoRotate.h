/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoRotate.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:28 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGizmoGuiRotate_H__
#define __mafGizmoGuiRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGuiGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgButton;
class mafMatrix;

//----------------------------------------------------------------------------
/** gui class for rotation gizmo

  @sa
 
  @todo
*/
class mafGuiGizmoRotate : public mafGuiGizmoInterface
{
public:

  mafGuiGizmoRotate(mafObserver *listener);
  ~mafGuiGizmoRotate(); 

  void OnEvent(mafEventBase *maf_event);

  /** Enable-Disable the GUI's widgets */
  void EnableWidgets(bool enable);

//----------------------------------------------------------------------------
//gui constants: 
//----------------------------------------------------------------------------

// this constants must be visible from the owner gizmo  
enum GIZMO_ROTATE_ID
{
    ID_ROTATE_X = MINID,
    ID_ROTATE_Y,
    ID_ROTATE_Z,
};

  /**
  Set RotAngles ivar, only rotation part is copied from pose*/
  void SetAbsOrientation(mafMatrix *pose);
  
protected:

  /** Create the GUI*/
  void CreateGui();

  /**
  Gui rot angles*/
  double Orientation[3];

  /** 
  Send position to listener as vtkMatrix */
  void SendAbsOrientation(mafEventBase *sourceEvent);
};
#endif
