/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoTranslate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoGuiTranslate_H__
#define __albaGizmoGuiTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUI;

//----------------------------------------------------------------------------
/** gui class for translation gizmo

  @sa
 
  @todo
*/
class ALBA_EXPORT albaGUIGizmoTranslate : public albaGUIGizmoInterface
{
public:

  albaGUIGizmoTranslate(albaObserver *listener = NULL, bool testMode = false);
	~albaGUIGizmoTranslate(); 

  void OnEvent(albaEventBase *alba_event);

  /** Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

//----------------------------------------------------------------------------
//gui constants: 
//----------------------------------------------------------------------------

// this constants must be visible from the owner gizmo  

enum GUI_GIZMO_TRANSLATE_ID
{
    ID_TRANSLATE_X = MINID,
    ID_TRANSLATE_Y,
    ID_TRANSLATE_Z,
};

  /**
  Update Position ivar; only translation part is copied from pose */
  void SetAbsPosition(albaMatrix *pose);

protected:

  void SendAbsPosition(albaEventBase *sourceEvent);

	/** Create the GUI*/
	void CreateGui();

  /**
  Gizmo abs pose*/
  double m_Position[3];

  /** test friend */
  friend class albaGUIGizmoTranslateTest;

};
#endif
