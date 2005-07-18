/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiExtractIsosurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-18 14:16:33 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mmiExtractIsosurface.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor.h"

#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkCellPicker.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiExtractIsosurface)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiExtractIsosurface::mmiExtractIsosurface()
//------------------------------------------------------------------------------
{
  m_PickValue = false;
  m_MousePicker = NULL;
}

//------------------------------------------------------------------------------
mmiExtractIsosurface::~mmiExtractIsosurface()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int mmiExtractIsosurface::StartInteraction(mmdMouse *mouse)
//------------------------------------------------------------------------------
{
  m_MousePicker = (vtkCellPicker *)mouse->GetPicker();
  return Superclass::StartInteraction(mouse);
}

//----------------------------------------------------------------------------
void mmiExtractIsosurface::OnMouseMove() 
//----------------------------------------------------------------------------
{ 
  m_PickValue = false;
  Superclass::OnMouseMove();
}
//----------------------------------------------------------------------------
void mmiExtractIsosurface::OnLeftButtonDown(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_PickValue = true;
  Superclass::OnLeftButtonDown(e);
}
//----------------------------------------------------------------------------
void mmiExtractIsosurface::OnLeftButtonUp()
//----------------------------------------------------------------------------
{
	if(m_PickValue)
    this->PickIsoValue();
    
  Superclass::OnLeftButtonUp();
}
//----------------------------------------------------------------------------
void mmiExtractIsosurface::PickIsoValue()
//----------------------------------------------------------------------------
{
  int x = m_LastMousePose[0];
  int y = m_LastMousePose[1];

  if( m_MousePicker && m_Renderer && m_MousePicker->Pick(x,y,0,m_Renderer) )
  {
    double pos_picked[3];
    m_MousePicker->GetPickPosition(pos_picked);
		vtkPoints *p = vtkPoints::New();
		p->SetNumberOfPoints(1);
		p->SetPoint(0,pos_picked);
		mafEventMacro(mafEvent(this,VME_PICKED,(vtkObject *)p));
		p->Delete();
  }
}
