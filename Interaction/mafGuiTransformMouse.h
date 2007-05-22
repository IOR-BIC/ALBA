/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTransformMouse.h,v $
  Language:  C++
  Date:      $Date: 2007-05-22 10:55:04 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGuiTransformMouse_H__
#define __mafGuiTransformMouse_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGuiTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mmgButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class mafInteractor;
class mafVME;

//----------------------------------------------------------------------------
/** transform gui

  @sa
 
  @todo

*/
class mafGuiTransformMouse : public mafGuiTransformInterface
{
public:
  mafGuiTransformMouse(mafVME *input, mafObserver *listener = NULL);
	~mafGuiTransformMouse(); 

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
    SURFACE_SNAP
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

  void SetRotationConstraintId(int value){RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){TranslationConstraintId = value;};

protected:  
  mafInteractor* OldInteractor;
   
  /** Create interactors */
  void CreateISA();

  /** Override superclass */
  void CreateGui();

  mmiCompositorMouse *IsaCompositor;

  mmiGenericMouse *IsaRotate;
  mmiGenericMouse *IsaTranslate;
  mmiGenericMouse *IsaRoll;

  int RotationConstraintId;
	int TranslationConstraintId;
  
  /** Override superclass */;
  void RefSysVmeChanged();
  
};
#endif
