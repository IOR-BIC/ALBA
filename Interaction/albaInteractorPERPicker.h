/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERPicker
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorPERPicker_h
#define __albaInteractorPERPicker_h

#include "albaInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVME;
class vtkCamera;

class ALBA_EXPORT albaInteractorPERPicker : public albaInteractorPER
{
public: 
  albaTypeMacro(albaInteractorPERPicker,albaInteractorPER);
	
protected:
  albaInteractorPERPicker();
  virtual ~albaInteractorPERPicker();
  
  /** 
    Perform a pick on start of interaction to find if the user pointed
    an object in the scene, and in case forward all the following 
    events to its behavior. if the user pointed to the background
    forward events to the camera interactor.*/
  virtual void OnLeftButtonUp(albaEventInteraction *e);

	virtual void OnMove(albaEventInteraction *e);
    
	void SendPickingInformation(albaView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, albaMatrix *tracker_pos = NULL, bool mouse_flag = true);

private:
  albaInteractorPERPicker(const albaInteractorPERPicker&);  // Not implemented.
  void operator=(const albaInteractorPERPicker&);  // Not implemented.

	bool m_MovedAfterMouseDown;
  /** test friend */
  friend class albaInteractorPERPickerTest;
};

#endif 
