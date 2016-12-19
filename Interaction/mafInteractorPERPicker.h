/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERPicker
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractorPERPicker_h
#define __mafInteractorPERPicker_h

#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVME;
class vtkCamera;

class MAF_EXPORT mafInteractorPERPicker : public mafInteractorPER
{
public: 
  mafTypeMacro(mafInteractorPERPicker,mafInteractorPER);
	
protected:
  mafInteractorPERPicker();
  virtual ~mafInteractorPERPicker();
  
  /** 
    Perform a pick on start of interaction to find if the user pointed
    an object in the scene, and in case forward all the following 
    events to its behavior. if the user pointed to the background
    forward events to the camera interactor.*/
  virtual void OnLeftButtonUp(mafEventInteraction *e);

	virtual void OnMove(mafEventInteraction *e);
    
	void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

private:
  mafInteractorPERPicker(const mafInteractorPERPicker&);  // Not implemented.
  void operator=(const mafInteractorPERPicker&);  // Not implemented.

	bool m_MovedAfterMouseDown;
  /** test friend */
  friend class mafInteractorPERPickerTest;
};

#endif 
