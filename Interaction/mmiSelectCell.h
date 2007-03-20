/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSelectCell.h,v $
  Language:  C++
  Date:      $Date: 2007-03-20 18:45:43 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiSelectCell_h
#define __mmiSelectCell_h

#include "mmiCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class mmiSelectCell : public mmiCameraMove
{
public:
  mafTypeMacro(mmiSelectCell,mmiCameraMove);

  /** redefined to set the picking iso-value flag */
  virtual void OnMouseMove();

  /** redefined to set the picking iso-value flag */
  virtual void OnLeftButtonDown(mafEventInteraction *e);
	
  /** redefined to set the picking iso-value flag and if it is true, call PickIsoValue() */
  virtual void OnButtonUp(mafEventInteraction *e);

protected:
  mmiSelectCell();
  virtual ~mmiSelectCell();
  
  /** send the picking informations to the listener */
  void PickCell(mafDevice *device);

  bool m_IsPicking;

private:
  mmiSelectCell(const mmiSelectCell&);  // Not implemented.
  void operator=(const mmiSelectCell&);  // Not implemented.
	void OnEvent(mafEventBase *event);
  //------------------------------------------------------------------------------;
};
#endif 
