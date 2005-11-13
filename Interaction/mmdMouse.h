/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdMouse.h,v $
  Language:  C++
  Date:      $Date: 2005-11-13 14:55:51 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdMouse_h
#define __mmdMouse_h

#include "mmdButtonsPad.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafView;
class mafRWIBase;
class vtkRenderer;
class vtkRenderWindowInteractor;

/** Tracking 2D device, i.e. "Mouse".
  mmdMouse is a class implementing interface for a Mouse. Current implementation 
  receive its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa mmdButtonsPad mafRWI
*/
class mmdMouse : public mmdButtonsPad
{
public:
  mafTypeMacro(mmdMouse,mmdButtonsPad); 

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when a mouse is moved */
  MAF_ID_DEC(MOUSE_2D_MOVE)
  /** @ingroup Events
      Issued when a keyboard char is pressed */
  MAF_ID_DEC(MOUSE_CHAR_EVENT)
    
  /**  Reimplemented to manage fusion of move events. */
  virtual void OnEvent(mafEventBase *event);

  /**  Set the last mouse position and send a 2D MoveEvent. */
  void SetLastPosition(double x,double y,unsigned long modifiers = 0);

  /**  Return the last position */
  const double *GetLastPosition() {return m_LastPosition;}
  void GetLastPosition(double pos[2]) {pos[0]=m_LastPosition[0];pos[1]=m_LastPosition[1];}

  /**
  Return the renderer of the selected view. */
  vtkRenderer *GetRenderer();

  /** 
  Return the selected view. */
  mafView *GetView();

  /** 
  Return the Interactor. */
  vtkRenderWindowInteractor *GetInteractor();

  /** 
  Return the RenderWindowInteractor used by mouse device */
  mafRWIBase *GetRWI();

  /** 
  Used to set the flag for updating the m_SelectedRWI during mouse motion and not only on ViewSelected event.*/
  void SetUpdateRWIDuringMotion(bool update_on_motion) {m_UpdateRwiInOnMoveFlag = update_on_motion;};
  void UpdateRWIDuringMotionOff() {m_UpdateRwiInOnMoveFlag = true;};
  void UpdateRWIDuringMotionOn() {m_UpdateRwiInOnMoveFlag = true;};

  /** 
  Return the status of the m_UpdateRwiInOnMoveFlag flag.*/
  bool IsUpdateRWIDuringMotion() {return m_UpdateRwiInOnMoveFlag;};

protected:
  mmdMouse();
  virtual ~mmdMouse();

  /** add position to the event */ 
  virtual void SendButtonEvent(mafEventInteraction *event);

  double      m_LastPosition[2];///< stores the last position
  bool        m_UpdateRwiInOnMoveFlag; ///< If this flag true the m_SelectedRWI is updated in when the mouse move inside the view (used in compounded views)

  mafView    *m_SelectedView;   ///< store the selected view to perform the mouse picking
  mafRWIBase *m_SelectedRWI;

private:
  mmdMouse(const mmdMouse&);  // Not implemented.
  void operator=(const mmdMouse&);  // Not implemented.
};
#endif 
