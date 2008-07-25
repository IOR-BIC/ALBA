/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoRotate.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.3 $
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
#include "mafGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIButton;
class mafMatrix;

//----------------------------------------------------------------------------
/** gui class for rotation gizmo

  @sa
 
  @todo
*/
class mafGUIGizmoRotate : public mafGUIGizmoInterface
{
public:

  mafGUIGizmoRotate(mafObserver *listener);
  ~mafGUIGizmoRotate(); 

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
  double m_Orientation[3];

  /** 
  Send position to listener as vtkMatrix */
  void SendAbsOrientation(mafEventBase *sourceEvent);
};
#endif
