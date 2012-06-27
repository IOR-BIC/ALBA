/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoTranslate
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGizmoGuiTranslate_H__
#define __mafGizmoGuiTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGUIGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIButton;
class mafGUI;

//----------------------------------------------------------------------------
/** gui class for translation gizmo

  @sa
 
  @todo
*/
class MAF_EXPORT mafGUIGizmoTranslate : public mafGUIGizmoInterface
{
public:

  mafGUIGizmoTranslate(mafObserver *listener = NULL, bool testMode = false);
	~mafGUIGizmoTranslate(); 

  void OnEvent(mafEventBase *maf_event);

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
  void SetAbsPosition(mafMatrix *pose);

protected:

  void SendAbsPosition(mafEventBase *sourceEvent);

	/** Create the GUI*/
	void CreateGui();

  /**
  Gizmo abs pose*/
  double m_Position[3];

  /** test friend */
  friend class mafGUIGizmoTranslateTest;

};
#endif
