/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoScale
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoGuiScale_H__
#define __albaGizmoGuiScale_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIButton;
class albaObserver;
class albaGUI;

//----------------------------------------------------------------------------
/** GUI class for scaling gizmo

  @sa
 
  @todo
*/

class ALBA_EXPORT albaGUIGizmoScale : public albaGUIGizmoInterface
{
public:

  albaGUIGizmoScale(albaObserver *listener = NULL, bool testMode = false);
	~albaGUIGizmoScale(); 

  void OnEvent(albaEventBase *alba_event);

  /** Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner gizmo  

  enum GIZMO_SCALE_ID
  {
      ID_SCALE_X = MINID,
      ID_SCALE_Y,
      ID_SCALE_Z,
  };

  /**
  Update Scaling ivar; only scaling part is copied from pose */
  void SetAbsScaling(albaMatrix *pose);

protected:

  void SendAbsScaling(albaEventBase *sourceEvent);

	/** Create the GUI*/
	void CreateGui();

  /**
  Gizmo abs pose*/
  double m_Scaling[3];

  /** test friend */
  friend class albaGUIGizmoScaleTest;

};
#endif
