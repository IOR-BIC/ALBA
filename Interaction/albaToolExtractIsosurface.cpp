/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolExtractIsosurface
 Authors: Gianluigi Crimi, from albaInteractorExtractIsosurface by Paolo Quadrani & Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaToolExtractIsosurface.h"
#include "albaView.h"
#include "albaRWIBase.h"

#include "vtkAbstractPicker.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolExtractIsosurface);

//------------------------------------------------------------------------------
albaToolExtractIsosurface::albaToolExtractIsosurface()
{
  m_PickValue = false;
}

//------------------------------------------------------------------------------
bool albaToolExtractIsosurface::OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (ren == NULL)
    return false;

  m_PickValue = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaToolExtractIsosurface::OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  m_PickValue = false;
  return false;
}

//------------------------------------------------------------------------------
bool albaToolExtractIsosurface::OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (ren == NULL)
    return true;

  if (m_PickValue)
  {
    if (rwi->GetPicker()->Pick((int)pos[0], (int)pos[1], 0, ren))
    {
      double pickedPos[3];
      rwi->GetPicker()->GetPickPosition(pickedPos);
      SendPickedPointEvent(VME_PICKED, pickedPos);
    }
  }
  m_PickValue = false;

  ren->GetRenderWindow()->SetDesiredUpdateRate(0.001);
  ren->GetRenderWindow()->Render();
  return true;
}
