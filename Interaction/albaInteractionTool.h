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

class albaRWIBase;

/** Base class of the tools pushed on the stack of the PER.
  The mouse methods receive the render window, the renderer under the mouse and the 2D position
  in display coordinates; the view is rwi->GetView(), NULL for the render windows of the dialogs, and the
  modifier keys are GetCtrlDown(), GetAltDown() and GetShiftDown().
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

  /** Make the tool work only on the mouse events of rwi; NULL, the default, for the events of all the render windows.
    The pointer is only compared, never used. */
  void OperateOnlyOn(albaRWIBase *rwi) {m_OperatingRWI = rwi;}
  albaRWIBase *GetOperatingRWI() {return m_OperatingRWI;}

  /** The mouse events: the events of the render windows the tool does not operate on are not used (see OperateOnlyOn),
    the others go to the methods ...Impl of the subclasses. */
  bool OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnMiddleButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnMiddleButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnRightButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnRightButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnMove(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  bool OnWheel(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation);

  /** A key has been pressed or released */
  virtual bool OnKey(int keyCode, bool pressed) {return false;}

  /** Called when the tool is pushed on the stack */
  virtual void OnPush() {}

  /** Called when the tool is popped from the stack */
  virtual void OnPop() {}

protected:
  albaInteractionTool() {m_OperatingRWI = NULL;}
  virtual ~albaInteractionTool() {}

  /** The mouse events of the render windows the tool operates on: a method returns true if the tool used the event */
  virtual bool OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}
  virtual bool OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}
  virtual bool OnMiddleButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}
  virtual bool OnMiddleButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}
  virtual bool OnRightButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}
  virtual bool OnRightButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}

  /** The mouse moved with a button pressed: called only on the tool interacting with the mouse */
  virtual bool OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}

  /** The mouse moved without buttons pressed */
  virtual bool OnMoveImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return false;}

  /** The mouse wheel has been rotated */
  virtual bool OnWheelImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation) {return false;}

  /** Return true if the tool operates on the events of rwi (see OperateOnlyOn) */
  bool IsOnOperatingRWI(albaRWIBase *rwi) {return m_OperatingRWI == NULL || rwi == m_OperatingRWI;}

  albaRWIBase *m_OperatingRWI; ///< the only render window the tool operates on, NULL for all

private:
  albaInteractionTool(const albaInteractionTool&);  // Not implemented.
  void operator=(const albaInteractionTool&);  // Not implemented.
};

#endif
