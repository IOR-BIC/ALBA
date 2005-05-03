/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCompositorMouse.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:36 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiCompositorMouse_h
#define __mmiCompositorMouse_h

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "mafInteractor.h"
#include "mafEvent.h"
#include "mmiGenericMouse.h"

#include <map>

#include "vtkMatrix4x4.h"
#include "mflTransform.h"
#include "vtkDOFMatrix.h"

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
  static mmiCompositorMouse *New();
  vtkTypeMacro(mmiCompositorMouse, mafInteractor);

  /** Start the interaction with the selected object; set CurrentCamera and Renderer ivar */
  virtual int StartInteraction(mmdMouse *mouse);

  /**  Process events coming from tracker */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  //----------------------------------------------------------------------------
  // Transform Enabling:
  //----------------------------------------------------------------------------

  /**
  Create a behavior given the activator*/
  mmiGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);

  /**
  Get a behavior given the activator*/
  mmiGenericMouse *GetBehavior(MMI_ACTIVATOR activator);


  //----------------------------------------------------------------------------
  // Listener stuff:
  //----------------------------------------------------------------------------
  /**
  Set the interactor listener*/
  void SetListener(mafEventListener *listener) {m_listener = listener;};
  mafEventListener *GetListener() {return m_listener;};
  
protected:

  mmiCompositorMouse();
  ~mmiCompositorMouse();

  virtual void OnButtonDown(mflEventInteraction *e);
  
  virtual void OnButtonUp(mflEventInteraction *e);

  /** mouse driven events*/
  void OnLeftButtonDown   (mflEventInteraction *e);
  void OnLeftButtonUp     (mflEventInteraction *e);
  void OnMiddleButtonDown (mflEventInteraction *e);
  void OnMiddleButtonUp   (mflEventInteraction *e);
  void OnMouseMove        ();
  void OnRightButtonDown  (mflEventInteraction *e);
  void OnRightButtonUp    (mflEventInteraction *e);

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
  std::map<int, mmiGenericMouse *> ActivatorMap;

  /** 
	init the interactor for the corresponding pressed button.
	*/
	void InitInteraction(int buttonPressed, mflEventInteraction *e);
	
	/**
  conversion from mouse-keyboard combo to the corresponding activator value ie
  the value in the MMI_ACTIVATOR enum, if no activator is found -1 is returned*/
  int MouseToActivator(int mouseButton, int shift, int ctrl);

  // the active behavior
  mmiGenericMouse *ActiveMMIGeneric;

private:

  mmiCompositorMouse(const mmiCompositorMouse&);  // Not implemented.
  void operator=(const mmiCompositorMouse&);   // Not implemented.

  int MousePose[2];
  int LastMousePose[2];
  int ButtonPressed;

  vtkCamera *CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  
};
#endif

