/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractorPERBrushFeedback_h
#define __mafInteractorPERBrushFeedback_h

#include "mafDefines.h"
#include "mafInteractorPER.h"

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
class MAF_EXPORT mafInteractorPERBrushFeedback :  public mafInteractorPER
{
public: 
  
  mafTypeMacro(mafInteractorPERBrushFeedback,mafInteractorPER);

  /* Main event handler */
  virtual void OnEvent(mafEventBase *event);
  
protected:
  /** constructor. */
  mafInteractorPERBrushFeedback();
  /** destructor. */
  virtual ~mafInteractorPERBrushFeedback();
  
  bool m_MouseDown;
	double m_PickPosition[3];

};
#endif 
