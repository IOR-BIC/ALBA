/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUITransformMouse.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:47:19 $
  Version:   $Revision: 1.1.2.5 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUITransformMouse_H__
#define __mafGUITransformMouse_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGUITransformInterface.h"
#include "mafInteractorCompositorMouse.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mafInteractorGenericMouse;
class mafInteractor;
class mafVME;

//----------------------------------------------------------------------------
/** transform gui

  @sa
 
  @todo

*/
class mafGUITransformMouse : public mafGUITransformInterface
{
public:
  mafGUITransformMouse(mafVME *input, mafObserver *listener = NULL);
	~mafGUITransformMouse(); 

  // constraints enum
  enum TRANSFORM_MOUSE_WIDGET_ID
  {
    X_AXIS = 0,
    Y_AXIS, 
    Z_AXIS,
    VIEW_PLANE, 
    NORMAL_VIEW_PLANE,
    XY_PLANE, 
    XZ_PLANE, 
    YZ_PLANE,
    SURFACE_SNAP,
    NORMAL_SURFACE,
  };

  void OnEvent(mafEventBase *maf_event);

  /** Override superclass */
	void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum GUI_TRANSFORM_MOUSE_ID
  {
      ID_ROTATION_AXES = MINID,
      ID_TRASLATION_AXES,
  };

  /** Start and stop interaction through this object isa */
  void AttachInteractorToVme();
  void DetachInteractorFromVme();

  void SetRotationConstraintId(int value){m_RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){m_TranslationConstraintId = value;};

  mafInteractorGenericMouse* CreateBehavior(MMI_ACTIVATOR activator);

protected:  
  mafInteractor* m_OldInteractor;
   
  /** Create interactors */
  void CreateISA();

  /** Override superclass */
  void CreateGui();

  mafInteractorCompositorMouse *m_IsaCompositor;

  mafInteractorGenericMouse *m_IsaRotate;
  mafInteractorGenericMouse *m_IsaTranslate;
  mafInteractorGenericMouse *m_IsaRoll;

  int m_RotationConstraintId;
	int m_TranslationConstraintId;
  
  /** Override superclass */;
  void RefSysVmeChanged();
  
};
#endif
