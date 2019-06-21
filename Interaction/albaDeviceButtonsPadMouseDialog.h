/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouseDialog
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDeviceButtonsPadMouseDialog_h
#define __albaDeviceButtonsPadMouseDialog_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaView;

/**
class name : albaDeviceButtonsPadMouseDialog
Add SetView method to albaDeviceButtonsPadMouse class to directly set a view to a mouse object.
This class avoid the following problem that occurs trying to use a albaDeviceButtonsPadMouse to
interact with a view plugged inside a modal dialog:
In a standard ALBA application to set a view to a mouse object you must use the method
albaEventMacro(albaEvent(this, VIEW_SELECT, View); that rise a VIEW_SELECT event.
Logic's OnEvent method trap this event but ignore the view parameter and use ViewManager
to find the selected view;
Views plugged into modal dialogs are not listed inside the view manager so this method don't
set the desired view.

(An operation using this class is vph2OpAnnulusIdentification)
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaDeviceButtonsPadMouseDialog : public  albaDeviceButtonsPadMouse
//----------------------------------------------------------------------------
{
public:
  /** RTTI macro */
  albaTypeMacro(albaDeviceButtonsPadMouseDialog,albaDeviceButtonsPadMouse); 

  /** Set the selected view. */
  void SetView(albaView *view);

protected:
  /** constructor. */
  albaDeviceButtonsPadMouseDialog();
  /** destructor. */
  virtual ~albaDeviceButtonsPadMouseDialog();
};
#endif