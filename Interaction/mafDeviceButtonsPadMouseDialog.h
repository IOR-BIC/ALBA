/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseDialog
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafDeviceButtonsPadMouseDialog_h
#define __mafDeviceButtonsPadMouseDialog_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafDeviceButtonsPadMouse;
class mafView;

/**
class name : mafDeviceButtonsPadMouseDialog
Add SetView method to mafDeviceButtonsPadMouse class to directly set a view to a mouse object.
This class avoid the following problem that occurs trying to use a mafDeviceButtonsPadMouse to
interact with a view plugged inside a modal dialog:
In a standard MAF application to set a view to a mouse object you must use the method
mafEventMacro(mafEvent(this, VIEW_SELECT, View); that rise a VIEW_SELECT event.
Logic's OnEvent method trap this event but ignore the view parameter and use ViewManager
to find the selected view;
Views plugged into modal dialogs are not listed inside the view manager so this method don't
set the desired view.

(An operation using this class is vph2OpAnnulusIdentification)
*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafDeviceButtonsPadMouseDialog : public  mafDeviceButtonsPadMouse
//----------------------------------------------------------------------------
{
public:
  /** RTTI macro */
  mafTypeMacro(mafDeviceButtonsPadMouseDialog,mafDeviceButtonsPadMouse); 

  /** Set the selected view. */
  void SetView(mafView *view);

protected:
  /** constructor. */
  mafDeviceButtonsPadMouseDialog();
  /** destructor. */
  virtual ~mafDeviceButtonsPadMouseDialog();
};
#endif