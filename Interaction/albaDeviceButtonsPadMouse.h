/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouse
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDeviceButtonsPadMouse_h
#define __albaDeviceButtonsPadMouse_h

#include "albaDeviceButtonsPad.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;
class albaRWIBase;
class vtkRenderer;
class vtkRenderWindowInteractor;

/** Tracking 2D device, i.e. "Mouse".
  albaDeviceButtonsPadMouse is a class implementing interface for a Mouse. Current implementation 
  receive its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa albaDeviceButtonsPad albaRWI
*/
class ALBA_EXPORT albaDeviceButtonsPadMouse : public albaDeviceButtonsPad
{
public:
  albaTypeMacro(albaDeviceButtonsPadMouse,albaDeviceButtonsPad); 

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when a mouse is moved */
  // ALBA_ID_DEC(MOUSE_2D_MOVE)
  /** @ingroup Events
      Issued when a keyboard char is pressed */
  // ALBA_ID_DEC(MOUSE_CHAR_EVENT)
  /** @ingroup Events
      Issued on double click mouse button */
  // ALBA_ID_DEC(MOUSE_DCLICK)

  /** return the mouse 2d move id */
  static albaID GetMouse2DMoveId();

  /** return the mouse char event id */
  static albaID GetMouseCharEventId();

  /** return the mouse D click id */
  static albaID GetMouseDClickId();
    
  /** Reimplemented to manage fusion of move events. */
  virtual void OnEvent(albaEventBase *event);

  /** Set the last mouse position and send a 2D MoveEvent. */
  void SetLastPosition(double x,double y,unsigned long modifiers = 0);

  /**  Return the last position */
  const double *GetLastPosition() {return m_LastPosition;}
  void GetLastPosition(double pos[2]) {pos[0]=m_LastPosition[0];pos[1]=m_LastPosition[1];}

  /** Return the renderer of the selected view. */
  vtkRenderer *GetRenderer();

  /** Return the selected view. */
  albaView *GetView();

  /** Return the Interactor. */
  vtkRenderWindowInteractor *GetInteractor();

  /** Return the RenderWindowInteractor used by mouse device */
  albaRWIBase *GetRWI();

  void DisplayToNormalizedDisplay(double display[2]);
  void NormalizedDisplayToDisplay(double normalized[2]);

protected:
  albaDeviceButtonsPadMouse();
  virtual ~albaDeviceButtonsPadMouse();

  /** add position to the event */ 
  virtual void SendButtonEvent(albaEventInteraction *event);

  double      m_LastPosition[2];///< stores the last position
  bool        m_ButtonPressed;

  albaView    *m_SelectedView;   ///< store the selected view to perform the mouse picking
  albaRWIBase *m_SelectedRWI;

private:
  albaDeviceButtonsPadMouse(const albaDeviceButtonsPadMouse&);  // Not implemented.
  void operator=(const albaDeviceButtonsPadMouse&);  // Not implemented.
};
#endif 
