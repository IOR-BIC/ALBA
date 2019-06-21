/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoRotate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoGuiRotate_H__
#define __albaGizmoGuiRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIButton;
class albaMatrix;

//----------------------------------------------------------------------------
/** gui class for rotation gizmo

  @sa
 
  @todo
*/
class ALBA_EXPORT albaGUIGizmoRotate : public albaGUIGizmoInterface
{
public:

  albaGUIGizmoRotate(albaObserver *listener, bool testMode = false);
  ~albaGUIGizmoRotate(); 

  void OnEvent(albaEventBase *alba_event);

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
  void SetAbsOrientation(albaMatrix *pose);
  
protected:

  /** Create the GUI*/
  void CreateGui();

  /**
  Gui rot angles*/
  double m_Orientation[3];

  /** 
  Send position to listener as vtkMatrix */
  void SendAbsOrientation(albaEventBase *sourceEvent);

  /** friend test */
  friend class albaGUIGizmoRotateTest;

};
#endif
