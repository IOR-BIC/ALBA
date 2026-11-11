/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionTool
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractionTool_h
#define __albaInteractionTool_h

#include "albaInteractionAgent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaEventInteraction;

/** Base class of the tools pushed on the stack of the PER.
  The PER offers each event to the tools from the top of the stack to the bottom:
  a method returns true if the tool used the event, and then the event stops there.
  If no tool uses it, the PER applies its default behavior (VME behavior, selection, camera).
  A tool handles only the buttons it redefines: the others return false.
  The tool using a button down interacts with the device until that button is released:
  it receives the drags and the down and up of the other buttons in the meantime. */
class ALBA_EXPORT albaInteractionTool : public albaInteractionAgent
{
public:
  albaTypeMacro(albaInteractionTool, albaInteractionAgent);

  virtual bool OnLeftButtonDown(albaEventInteraction *e) {return false;}
  virtual bool OnLeftButtonUp(albaEventInteraction *e) {return false;}
  virtual bool OnMiddleButtonDown(albaEventInteraction *e) {return false;}
  virtual bool OnMiddleButtonUp(albaEventInteraction *e) {return false;}
  virtual bool OnRightButtonDown(albaEventInteraction *e) {return false;}
  virtual bool OnRightButtonUp(albaEventInteraction *e) {return false;}

  /** The device moved with a button pressed: called only on the tool interacting with the device */
  virtual bool OnDrag(albaEventInteraction *e) {return false;}

  /** The device moved without buttons pressed */
  virtual bool OnMove(albaEventInteraction *e) {return false;}

  /** The mouse wheel has been rotated */
  virtual bool OnWheel(albaEventInteraction *e) {return false;}

  /** A key has been pressed or released */
  virtual bool OnKey(int keyCode, bool pressed) {return false;}

  /** Called when the tool is pushed on the stack */
  virtual void OnPush() {}

  /** Called when the tool is popped from the stack */
  virtual void OnPop() {}

protected:
  albaInteractionTool() {}
  virtual ~albaInteractionTool() {}

private:
  albaInteractionTool(const albaInteractionTool&);  // Not implemented.
  void operator=(const albaInteractionTool&);  // Not implemented.
};

#endif
