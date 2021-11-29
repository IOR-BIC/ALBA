/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCompositorMouse
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorCompositorMouse_h
#define __albaInteractorCompositorMouse_h

#include "albaInteractor.h"
#include "albaEvent.h"
#include "albaInteractorGenericMouse.h"
#include "albaTransform.h"
#include "albaMatrix.h"
#include "albaSmartPointer.h"

#include <map>

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class vtkDoubleArray;
class vtkCellPicker;

//----------------------------------------------------------------------------
// combinations of mouse-buttons and key that may start Transformations:
// es: Translate may be initiated by MOUSE_MIDDLE | MOUSE_LEFT_SHIFT
//----------------------------------------------------------------------------
enum MMI_ACTIVATOR
{
    MOUSE_LEFT = 1,
    MOUSE_LEFT_SHIFT =2,
    MOUSE_LEFT_CONTROL = 4,
    
    MOUSE_MIDDLE = 8,
    MOUSE_MIDDLE_SHIFT = 16,
    MOUSE_MIDDLE_CONTROL = 32,
    
    MOUSE_RIGHT = 64,
    MOUSE_RIGHT_SHIFT = 128,
    MOUSE_RIGHT_CONTROL = 256,
};

//----------------------------------------------------------------------------
// pressed mouse button
//----------------------------------------------------------------------------
enum MOUSE_BUTTON
{
    NO_BUTTON_PRESSED,
    MOUSE_LEFT_PRESSED, 
    MOUSE_MIDDLE_PRESSED,
    MOUSE_RIGHT_PRESSED, 
};

/** Associates instances of albaInteractorGenericMouse to mouse buttons and ctrl/shift keys combinations.
  
  Associates instances of albaInteractorGenericMouse to mouse buttons and ctrl/shift keys combinations.
  For example:

  // Create the compositor:
  behaviorCompositor = albaInteractorCompositorMouse::New();

  mouseLeftBhavior = behaviorCompositor->CreateBehavior(MOUSE_LEFT);
  mouseMiddleShiftBehavior = behaviorCompositor->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  ...

  @sa
  - albaInteractorGenericMouse since albaInteractorCompositorMouse is working in pair with this class
  - albaGUITransformMouse component used by albaOpTransformOld
  
*/
class ALBA_EXPORT albaInteractorCompositorMouse : public albaInteractor
{
public:
  albaTypeMacro(albaInteractorCompositorMouse, albaInteractor);

  /** Start the interaction with the selected object; set m_CurrentCamera and Renderer ivar */
  virtual int StartInteraction(albaDeviceButtonsPadMouse *mouse);

  /**  Process events coming from the mouse */
  virtual void OnEvent(albaEventBase *event);

  /**
  Create a behavior given the activator*/
  virtual albaInteractorGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);

  /**
  Get a behavior given the activator*/
  albaInteractorGenericMouse *GetBehavior(MMI_ACTIVATOR activator);
  
protected:

  albaInteractorCompositorMouse();
  ~albaInteractorCompositorMouse();

  virtual void OnButtonDown(albaEventInteraction *e);
  
  virtual void OnButtonUp(albaEventInteraction *e);

  /** mouse driven events*/
  void OnLeftButtonDown   (albaEventInteraction *e);
  void OnLeftButtonUp     (albaEventInteraction *e);
  void OnMiddleButtonDown (albaEventInteraction *e);
  void OnMiddleButtonUp   (albaEventInteraction *e);
  void OnMouseMove        ();
  void OnRightButtonDown  (albaEventInteraction *e);
  void OnRightButtonUp    (albaEventInteraction *e);

  typedef std::map<int, albaAutoPointer<albaInteractorGenericMouse> > mmuActivatorMap;

  /**
                    ActivatorMap
  |-------------------------------------
  |    key activator     |   behavior
  |-------------------------------------
  |  MOUSE_LEFT          |   pMMI_1
  |  MOUSE_LEFT_SHIFT    |   pMMI_2
  |  MOUSE_LEFT_CTRL     |   pMMI_3 
  |   ...........        |   .......

  map holding activator<->behavior association*/
  mmuActivatorMap m_ActivatorMap;

  /** 
	init the interactor for the corresponding pressed button.
	*/
	void InitInteraction(int buttonPressed, albaEventInteraction *e);
	
	/**
  conversion from mouse-keyboard combo to the corresponding activator value ie
  the value in the MMI_ACTIVATOR enum, if no activator is found -1 is returned*/
  int MouseToActivator(int mouseButton, int shift, int ctrl);

  // the active behavior
  albaInteractorGenericMouse *m_ActiveMMIGeneric;

private:

  albaInteractorCompositorMouse(const albaInteractorCompositorMouse&);  // Not implemented.
  void operator=(const albaInteractorCompositorMouse&);   // Not implemented.

  int m_MousePose[2];
  int m_LastMousePose[2];
  int m_ButtonPressed;

  vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  
  /** friend test class */
  friend class albaInteractorCompositorMouseTest;

};
#endif

