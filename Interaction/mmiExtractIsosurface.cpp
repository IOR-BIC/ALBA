/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiExtractIsosurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-02 10:29:09 $
  Version:   $Revision: 1.2 $
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
#include "mafRWIBase.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkAbstractPicker.h"
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

  mmdMouse *mouse = mmdMouse::SafeDownCast(m_Device);
  if( mouse && m_Renderer)
  {
    double pos_picked[3];
    if (mouse->GetRWI()->GetPicker()->Pick(x,y,0,m_Renderer))
    {
      mouse->GetRWI()->GetPicker()->GetPickPosition(pos_picked);
      vtkPoints *p = vtkPoints::New();
      p->SetNumberOfPoints(1);
      p->SetPoint(0,pos_picked);
      mafEventMacro(mafEvent(this,VME_PICKED,(vtkObject *)p));
      p->Delete();
    }
  }
}
