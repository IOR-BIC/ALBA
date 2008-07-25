/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoScale.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 08:44:32 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGizmoGuiScale_H__
#define __mafGizmoGuiScale_H__

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

class mafGUIGizmoScale : public mafGUIGizmoInterface
{
public:

  mafGUIGizmoScale(mafObserver *listener = NULL);
	~mafGUIGizmoScale(); 

  void OnEvent(mafEventBase *maf_event);

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
  void SetAbsScaling(mafMatrix *pose);

protected:

  void SendAbsScaling(mafEventBase *sourceEvent);

	/** Create the GUI*/
	void CreateGui();

  /**
  Gizmo abs pose*/
  double m_Scaling[3];

};
#endif
