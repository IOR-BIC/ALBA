/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteMouse.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:57 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdRemoteMouse_h
#define __mmdRemoteMouse_h

#ifdef __GNUG__
    #pragma interface "mmdRemoteMouse.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmdButtonsPad.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafView;
class vtkAssemblyPath;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkCellPicker;
class vtkRenderWindowInteractor;
class mafRWIBase;

/** Tracking 2D device, i.e. "Mouse".
  mmdRemoteMouse is a class implemnting interface for a Mouse. Current implemantation 
  recevices its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa mmdButtonsPad mafRWI
*/
class mmdRemoteMouse : public mmdButtonsPad
{
public:
  static mmdRemoteMouse *New() {return new mmdRemoteMouse;}
  vtkTypeMacro(mmdRemoteMouse,mmdButtonsPad); 

  // For factoring purpouses
  static vtkObject *NewObjectInstance() {return mmdRemoteMouse::New();}
  static const char *GetTypeName() {return "mmdRemoteMouse";}

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when a mouse is moved */
  MFL_EVT_DEC(MOUSE_2D_MOVE)
  /** @ingroup Events
      Issued when a keyboard char is pressed */
  MFL_EVT_DEC(MOUSE_CHAR_EVENT)
    
  /**  Reimplemented to manage fusion of move events. */
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  /**  Set the last mouse position and send a 2D MoveEvent. */
  void SetLastPosition(double x,double y,unsigned long modifiers = 0);

  /**  Return the last position */
  vtkGetVector2Macro(LastPosition,double);

  /**
  Perform mouse picking and return the selected assembly path picked, otherwise return NULL. */
  vtkAssemblyPath *Pick(int mouse_screen_pos[2]);

  /**
  Return the renderer of the selected view. */
  vtkRenderer *GetRenderer();

  /** 
  Return the selected view. */
  mafView *GetView();

  /** 
  Return the default mouse picker. */
  vtkAbstractPropPicker *GetPicker();

  /** 
  Return the Interactor. */
  vtkRenderWindowInteractor *GetInteractor();

protected:
  mmdRemoteMouse();
  virtual ~mmdRemoteMouse();

  //virtual int InternalInitialize();
  //virtual void InternalShutdown();

  /** add position to the event */ 
  virtual void SendButtonEvent(mflEventInteraction *event);

  /** Currently Mouse device doesn't have any settings */
  //virtual void CreateSettings();

  double   LastPosition[2];///< stores the last position

  mafView *SelectedView; ///< store the selected view to perform the mouse picking
  mafRWIBase *SelectedRWI;
  vtkCellPicker *Picker;

private:
  mmdRemoteMouse(const mmdRemoteMouse&);  // Not implemented.
  void operator=(const mmdRemoteMouse&);  // Not implemented.
};

#endif 

