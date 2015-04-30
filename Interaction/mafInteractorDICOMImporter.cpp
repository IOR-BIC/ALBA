/*=========================================================================

 Program: MAF2
 Module: mafInteractorDICOMImporter
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"

#include "mafInteractorDICOMImporter.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafInteractor.h"

#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkPoints.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkRenderer.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorDICOMImporter)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractorDICOMImporter::mafInteractorDICOMImporter()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
mafInteractorDICOMImporter::~mafInteractorDICOMImporter()
//------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------
int mafInteractorDICOMImporter::StartInteraction(mafDeviceButtonsPadMouse *mouse)
//------------------------------------------------------------------------------
{
  return Superclass::StartInteraction(mouse);
}
//----------------------------------------------------------------------------
void mafInteractorDICOMImporter::OnMouseMove() 
//----------------------------------------------------------------------------
{
  SendCropPosition(MOUSE_MOVE);
  Superclass::OnMouseMove();
}
//----------------------------------------------------------------------------
void mafInteractorDICOMImporter::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  if (e == NULL)
  {
	  return;
  }

  SendCropPosition(MOUSE_DOWN);
  Superclass::OnLeftButtonDown(e);
}
//----------------------------------------------------------------------------
void mafInteractorDICOMImporter::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  SendCropPosition(MOUSE_UP);
  Superclass::OnLeftButtonUp();
}
//----------------------------------------------------------------------------
void mafInteractorDICOMImporter::SendCropPosition(int event_id)
//----------------------------------------------------------------------------
{
  double wp[4];
  ComputeDisplayToWorld(m_LastMousePose[0],m_LastMousePose[1],-1,wp);

  vtkMAFSmartPointer<vtkPoints> position_point; 
  position_point->SetNumberOfPoints(1);
  position_point->SetPoint(0,wp[0],wp[1],wp[2]);

  mafEventMacro(mafEvent(this, event_id, position_point.GetPointer()));
}
