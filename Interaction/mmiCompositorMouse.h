/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCompositorMouse.h,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:19:57 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiCompositorMouse_h
#define __mmiCompositorMouse_h

#include "mafInteractor.h"
#include "mafEvent.h"
#include "mmiGenericMouse.h"
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafSmartPointer.h"

#include <map>

//#include "vtkMatrix4x4.h"
//#include "vtkDOFMatrix.h"

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

/** Associates instances of mmiGenericMouse to mouse buttons and ctrl/shift keys combinations.
  
  Associates instances of mmiGenericMouse to mouse buttons and ctrl/shift keys combinations.
  @sa
  - mmoTransformGeneric for examples on how to use this class
  - mmiGenericMouse since mmiCompositorMouse is working in pair with this class
*/
class mmiCompositorMouse : public mafInteractor
{
public:
  mafTypeMacro(mmiCompositorMouse, mafInteractor);

  /** Start the interaction with the selected object; set m_CurrentCamera and Renderer ivar */
  virtual int StartInteraction(mmdMouse *mouse);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  //----------------------------------------------------------------------------
  // Transform Enabling:
  //----------------------------------------------------------------------------

  /**
  Create a behavior given the activator*/
  mmiGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);

  /**
  Get a behavior given the activator*/
  mmiGenericMouse *GetBehavior(MMI_ACTIVATOR activator);
  
protected:

  mmiCompositorMouse();
  ~mmiCompositorMouse();

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

  typedef std::map<int, mafAutoPointer<mmiGenericMouse> > mmuActivatorMap;

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
  mmiGenericMouse *m_ActiveMMIGeneric;

private:

  mmiCompositorMouse(const mmiCompositorMouse&);  // Not implemented.
  void operator=(const mmiCompositorMouse&);   // Not implemented.

  int m_MousePose[2];
  int m_LastMousePose[2];
  int m_ButtonPressed;

  vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  
};
#endif

