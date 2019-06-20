/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorPERBrushFeedback_h
#define __albaInteractorPERBrushFeedback_h

#include "albaDefines.h"
#include "albaInteractorPER.h"

class vtkActor2D;
class vtkImageMapper;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkLineSource;
class vtkCubeSource;
class vtkSphereSource;

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------


/** Segmentation Editing Interactor
*/
class ALBA_EXPORT albaInteractorPERBrushFeedback :  public albaInteractorPER
{
public: 
  
  albaTypeMacro(albaInteractorPERBrushFeedback,albaInteractorPER);

  /* Main event handler */
  virtual void OnEvent(albaEventBase *event);
  
protected:
  /** constructor. */
  albaInteractorPERBrushFeedback();
  /** destructor. */
  virtual ~albaInteractorPERBrushFeedback();
  
  bool m_MouseDown;
	double m_PickPosition[3];

};
#endif 
