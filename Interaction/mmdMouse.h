/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdMouse.h,v $
  Language:  C++
  Date:      $Date: 2005-11-02 10:39:28 $
  Version:   $Revision: 1.4 $
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
//class vtkAssemblyPath;
class vtkRenderer;
//class vtkAbstractPropPicker;
//class vtkCellPicker;
class vtkRenderWindowInteractor;


/** Tracking 2D device, i.e. "Mouse".
  mmdMouse is a class implemnting interface for a Mouse. Current implemantation 
  recevices its inputs by means of ProcessEvent and forward to all listeners. Any 
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
  Perform mouse picking and return the selected assembly path picked, otherwise return NULL. */
//  vtkAssemblyPath *Pick(int mouse_screen_pos[2]);
//  vtkAssemblyPath *Pick(int X, int Y);
  /**
  Return the renderer of the selected view. */
  vtkRenderer *GetRenderer();

  /** 
  Return the selected view. */
  mafView *GetView();

  /** 
  Return the default mouse picker. */
//  vtkAbstractPropPicker *GetPicker();

  /** 
  Return the Interactor. */
  vtkRenderWindowInteractor *GetInteractor();

  /** 
  Return the RenderWindowInteractor used by mouse device */
  mafRWIBase *GetRWI();

protected:
  mmdMouse();
  virtual ~mmdMouse();

  //virtual int InternalInitialize();
  //virtual void InternalShutdown();

  /** add position to the event */ 
  virtual void SendButtonEvent(mafEventInteraction *event);

  double          m_LastPosition[2];///< stores the last position

  mafView        *m_SelectedView;   ///< store the selected view to perform the mouse picking
  mafRWIBase     *m_SelectedRWI;
  //vtkCellPicker*  m_Picker;         ///< the picker used to pick the in the render window (to be moved in the view!)

private:
  mmdMouse(const mmdMouse&);  // Not implemented.
  void operator=(const mmdMouse&);  // Not implemented.
};

#endif 
