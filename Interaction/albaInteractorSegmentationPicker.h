/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSegmentationPicker
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorSegmentationPicker_h
#define __albaInteractorSegmentationPicker_h

#include "albaDefines.h"
#include "albaInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class ALBA_EXPORT albaInteractorSegmentationPicker : public albaInteractorCameraMove
{
public:
 
  albaTypeMacro(albaInteractorSegmentationPicker,albaInteractorCameraMove);

	virtual void OnEvent(albaEventBase *event);
	
protected:
  /**	This function is called when on mouse button down */ 
  virtual void OnLeftButtonDown(albaEventInteraction *e);
  /**	This function is called when on mouse button release */ 
  virtual void OnLeftButtonUp();

  /** Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
	void SendPickingInformation(albaView *v, double *mouse_pos, long modifier, int eventID);

	/** constructor. */
  albaInteractorSegmentationPicker();

  /** destructor. */
  virtual ~albaInteractorSegmentationPicker();


	bool m_IsPicking;         //< Determine if interactor is picking
	double m_PickPosition[3];
};
#endif 
