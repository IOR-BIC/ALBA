/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoTranslate.h,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:29 $
  Version:   $Revision: 1.1 $
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
#include "mafGuiGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgButton;
class mmgGui;

//----------------------------------------------------------------------------
/** gui class for translation gizmo

  @sa
 
  @todo
*/
class mafGuiGizmoTranslate : public mafGuiGizmoInterface
{
public:

  mafGuiGizmoTranslate(mafObserver *listener = NULL);
	~mafGuiGizmoTranslate(); 

  void OnEvent(mafEventBase *maf_event);

  /** Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

//----------------------------------------------------------------------------
//gui constants: 
//----------------------------------------------------------------------------

// this constants must be visible from the owner gizmo  

enum
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
  double Position[3];

};
#endif
