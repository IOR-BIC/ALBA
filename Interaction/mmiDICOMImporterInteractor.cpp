/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiDICOMImporterInteractor.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-10 13:06:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mmiDICOMImporterInteractor.h"
#include "mmdMouse.h"
#include "mafInteractor.h"

#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkPoints.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkRenderer.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiDICOMImporterInteractor)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiDICOMImporterInteractor::mmiDICOMImporterInteractor()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmiDICOMImporterInteractor::~mmiDICOMImporterInteractor()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int mmiDICOMImporterInteractor::StartInteraction(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  return Superclass::StartInteraction(mouse);
}
//----------------------------------------------------------------------------
void mmiDICOMImporterInteractor::OnMouseMove() 
//----------------------------------------------------------------------------
{
  SendCropPosition(MOUSE_MOVE);
  Superclass::OnMouseMove();
}
//----------------------------------------------------------------------------
void mmiDICOMImporterInteractor::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  SendCropPosition(MOUSE_DOWN);
  Superclass::OnLeftButtonDown(e);
}
//----------------------------------------------------------------------------
void mmiDICOMImporterInteractor::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
  SendCropPosition(MOUSE_UP);
  Superclass::OnLeftButtonUp();
}
//----------------------------------------------------------------------------
void mmiDICOMImporterInteractor::SendCropPosition(int event_id)
//----------------------------------------------------------------------------
{
  double wp[4];
  ComputeDisplayToWorld(m_LastMousePose[0],m_LastMousePose[1],-1,wp);

  vtkMAFSmartPointer<vtkPoints> position_point; 
  position_point->SetNumberOfPoints(1);
  position_point->SetPoint(0,wp[0],wp[1],wp[2]);

  mafEventMacro(mafEvent(this, event_id, position_point.GetPointer()));
}
