/*=========================================================================

 Program: MAF2
 Module: medDeviceButtonsPadMouseDialog
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medDeviceButtonsPadMouseDialog_h
#define __medDeviceButtonsPadMouseDialog_h

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
class name : medDeviceButtonsPadMouseDialog
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
class MAF_EXPORT medDeviceButtonsPadMouseDialog : public  mafDeviceButtonsPadMouse
//----------------------------------------------------------------------------
{
public:
  /** RTTI macro */
  mafTypeMacro(medDeviceButtonsPadMouseDialog,mafDeviceButtonsPadMouse); 

  /** Set the selected view. */
  void SetView(mafView *view);

protected:
  /** constructor. */
  medDeviceButtonsPadMouseDialog();
  /** destructor. */
  virtual ~medDeviceButtonsPadMouseDialog();
};
#endif