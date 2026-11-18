/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCompositorMouseBehavior
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCompositorMouseBehavior_h
#define __albaCompositorMouseBehavior_h

#include "albaInteractionBehavior.h"
#include "albaEvent.h"
#include "albaTransformBehavior.h"
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

/** Associates instances of albaTransformBehavior to mouse buttons and ctrl/shift keys combinations.
  
  Associates instances of albaTransformBehavior to mouse buttons and ctrl/shift keys combinations.
  For example:

  // Create the compositor:
  behaviorCompositor = albaCompositorMouseBehavior::New();

  mouseLeftBhavior = behaviorCompositor->CreateBehavior(MOUSE_LEFT);
  mouseMiddleShiftBehavior = behaviorCompositor->CreateBehavior(MOUSE_MIDDLE_SHIFT);
  ...

  @sa
  - albaTransformBehavior since albaCompositorMouseBehavior is working in pair with this class
  - albaTransformMouseBehavior, used through albaTransformMouseGUI by albaOpTransformOld
  
*/
class ALBA_EXPORT albaCompositorMouseBehavior : public albaInteractionBehavior
{
public:
  albaTypeMacro(albaCompositorMouseBehavior, albaInteractionBehavior);

  /** The button starts the transform behavior associated with the button and the modifier keys, if any */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return StartTransform(MOUSE_LEFT_PRESSED, view, ren, pos);}
  virtual bool OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return StartTransform(MOUSE_MIDDLE_PRESSED, view, ren, pos);}
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return StartTransform(MOUSE_RIGHT_PRESSED, view, ren, pos);}

  /** The release of the button stops the active transform behavior */
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return StopTransform();}
  virtual bool OnMiddleButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return StopTransform();}
  virtual bool OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return StopTransform();}

  /** The drags go to the active transform behavior */
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);

  /**
  Create a behavior given the activator*/
  virtual albaTransformBehavior *CreateBehavior(MMI_ACTIVATOR activator);

  /**
  Get a behavior given the activator*/
  albaTransformBehavior *GetBehavior(MMI_ACTIVATOR activator);
  
protected:

  albaCompositorMouseBehavior();
  ~albaCompositorMouseBehavior();

  /** Create the transform behavior used by CreateBehavior: subclasses can create a specialized one */
  virtual albaTransformBehavior *NewTransformBehavior();

  typedef std::map<int, albaAutoPointer<albaTransformBehavior> > mmuActivatorMap;

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

  /** Start the transform behavior associated with the button and the modifier keys; return false if there is none */
  bool StartTransform(int buttonPressed, albaView *view, vtkRenderer *ren, double pos[2]);

  /** Stop the active transform behavior; return false if there is none */
  bool StopTransform();
	
	/**
  conversion from mouse-keyboard combo to the corresponding activator value ie
  the value in the MMI_ACTIVATOR enum, if no activator is found -1 is returned*/
  int MouseToActivator(int mouseButton, int shift, int ctrl);

  // the active behavior
  albaTransformBehavior *m_ActiveTransform;

private:

  albaCompositorMouseBehavior(const albaCompositorMouseBehavior&);  // Not implemented.
  void operator=(const albaCompositorMouseBehavior&);   // Not implemented.

  int m_MousePose[2];
  int m_LastMousePose[2];

  vtkCamera *m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  
  /** friend test class */
  friend class albaCompositorMouseBehaviorTest;

};
#endif

