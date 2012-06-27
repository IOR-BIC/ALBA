/*=========================================================================

 Program: MAF2
 Module: mafInteractorCompositorMouse
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorCompositorMouse_h
#define __mafInteractorCompositorMouse_h

#include "mafInteractor.h"
#include "mafEvent.h"
#include "mafInteractorGenericMouse.h"
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafSmartPointer.h"

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

/** Associates instances of mafInteractorGenericMouse to mouse buttons and ctrl/shift keys combinations.
  
  Associates instances of mafInteractorGenericMouse to mouse buttons and ctrl/shift keys combinations.
  For example:

  // Create the compositor:
  behaviorCompositor = mafInteractorCompositorMouse::New();

  mouseLeftBhavior = behaviorCompositor->CreateBehavior(MOUSE_LEFT);
  mouseMiddleShiftBehavior = behaviorCompositor->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  ...

  @sa
  - mafInteractorGenericMouse since mafInteractorCompositorMouse is working in pair with this class
  - mafGUITransformMouse component used by mafOpMAFTransform
  
*/
class MAF_EXPORT mafInteractorCompositorMouse : public mafInteractor
{
public:
  mafTypeMacro(mafInteractorCompositorMouse, mafInteractor);

  /** Start the interaction with the selected object; set m_CurrentCamera and Renderer ivar */
  virtual int StartInteraction(mafDeviceButtonsPadMouse *mouse);

  /**  Process events coming from the mouse */
  virtual void OnEvent(mafEventBase *event);

  /**
  Create a behavior given the activator*/
  mafInteractorGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);

  /**
  Get a behavior given the activator*/
  mafInteractorGenericMouse *GetBehavior(MMI_ACTIVATOR activator);
  
protected:

  mafInteractorCompositorMouse();
  ~mafInteractorCompositorMouse();

  virtual void OnButtonDown(mafEventInteraction *e);
  
  virtual void OnButtonUp(mafEventInteraction *e);

  /** mouse driven events*/
  void OnLeftButtonDown   (mafEventInteraction *e);
  void OnLeftButtonUp     (mafEventInteraction *e);
  void OnMiddleButtonDown (mafEventInteraction *e);
  void OnMiddleButtonUp   (mafEventInteraction *e);
  void OnMouseMove        ();
  void OnRightButtonDown  (mafEventInteraction *e);
  void OnRightButtonUp    (mafEventInteraction *e);

  typedef std::map<int, mafAutoPointer<mafInteractorGenericMouse> > mmuActivatorMap;

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
	void InitInteraction(int buttonPressed, mafEventInteraction *e);
	
	/**
  conversion from mouse-keyboard combo to the corresponding activator value ie
  the value in the MMI_ACTIVATOR enum, if no activator is found -1 is returned*/
  int MouseToActivator(int mouseButton, int shift, int ctrl);

  // the active behavior
  mafInteractorGenericMouse *m_ActiveMMIGeneric;

private:

  mafInteractorCompositorMouse(const mafInteractorCompositorMouse&);  // Not implemented.
  void operator=(const mafInteractorCompositorMouse&);   // Not implemented.

  int m_MousePose[2];
  int m_LastMousePose[2];
  int m_ButtonPressed;

  vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  
  /** friend test class */
  friend class mafInteractorCompositorMouseTest;

};
#endif

