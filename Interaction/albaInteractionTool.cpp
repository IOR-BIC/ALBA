/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionTool
 Authors: Gianluigi Crimi

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

#include "albaInteractionTool.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionTool);

//------------------------------------------------------------------------------
bool albaInteractionTool::OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnLeftButtonDownImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnLeftButtonUpImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnMiddleButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnMiddleButtonDownImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnMiddleButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnMiddleButtonUpImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnRightButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnRightButtonDownImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnRightButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnRightButtonUpImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnDragImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnMove(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnMoveImpl(rwi, ren, pos);
}

//------------------------------------------------------------------------------
bool albaInteractionTool::OnWheel(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation)
{
  if (!IsOnOperatingRWI(rwi))
    return false;
  return OnWheelImpl(rwi, ren, pos, rotation);
}
