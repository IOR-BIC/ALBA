/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdRemoteMouse.h,v $
  Language:  C++
  Date:      $Date: 2006-12-06 09:43:35 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdRemoteMouse_h
#define __mmdRemoteMouse_h

#include "mmdMouse.h"

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
  mmdRemoteMouse is a class implementing interface for a Mouse. Current implementation 
  receives its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa mmdButtonsPad mafRWI
*/
class mmdRemoteMouse : public mmdMouse
{
public:
  mafTypeMacro(mmdRemoteMouse, mmdMouse); 

  /**  Reimplemented to manage fusion of move events. */
  virtual void OnEvent(mafEventBase *event);

protected:
  mmdRemoteMouse();
  virtual ~mmdRemoteMouse();

private:
  mmdRemoteMouse(const mmdRemoteMouse&);  // Not implemented.
  void operator=(const mmdRemoteMouse&);  // Not implemented.
};
#endif 
