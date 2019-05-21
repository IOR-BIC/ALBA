/*=========================================================================

 Program: MAF2
 Module: mafInteractorSegmentationPicker
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorSegmentationPicker_h
#define __mafInteractorSegmentationPicker_h

#include "mafDefines.h"
#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class MAF_EXPORT mafInteractorSegmentationPicker : public mafInteractorCameraMove
{
public:
 
  mafTypeMacro(mafInteractorSegmentationPicker,mafInteractorCameraMove);

	virtual void OnEvent(mafEventBase *event);
	
protected:
  /**	This function is called when on mouse button down */ 
  virtual void OnLeftButtonDown(mafEventInteraction *e);
  /**	This function is called when on mouse button release */ 
  virtual void OnLeftButtonUp();

  /** Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
	void SendPickingInformation(mafView *v, double *mouse_pos, long modifier, int eventID);

	/** constructor. */
  mafInteractorSegmentationPicker();

  /** destructor. */
  virtual ~mafInteractorSegmentationPicker();


	bool m_IsPicking;         //< Determine if interactor is picking
	double m_PickPosition[3];
};
#endif 
