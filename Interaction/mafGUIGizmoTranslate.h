/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoTranslate.h,v $
  Language:  C++
  Date:      $Date: 2009-07-15 14:26:49 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafGUIGizmoTranslate : public mafGUIGizmoInterface
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
