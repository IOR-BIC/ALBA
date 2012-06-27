/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseRemote
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafDeviceButtonsPadMouseRemote_h
#define __mafDeviceButtonsPadMouseRemote_h

#include "mafDeviceButtonsPadMouse.h"

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
  mafDeviceButtonsPadMouseRemote is a class implementing interface for a Mouse. Current implementation 
  receives its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa mafDeviceButtonsPad mafRWI
*/
class MAF_EXPORT mafDeviceButtonsPadMouseRemote : public mafDeviceButtonsPadMouse
{
public:
  mafTypeMacro(mafDeviceButtonsPadMouseRemote, mafDeviceButtonsPadMouse); 

  /**  Reimplemented to manage fusion of move events. */
  virtual void OnEvent(mafEventBase *event);

protected:
  mafDeviceButtonsPadMouseRemote();
  virtual ~mafDeviceButtonsPadMouseRemote();

private:
  mafDeviceButtonsPadMouseRemote(const mafDeviceButtonsPadMouseRemote&);  // Not implemented.
  void operator=(const mafDeviceButtonsPadMouseRemote&);  // Not implemented.
};
#endif 
