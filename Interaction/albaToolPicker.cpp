/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolPicker
 Authors: Nicola Vanella
 
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

#include "albaToolPicker.h"
#include "albaRWIBase.h"
#include "albaPickHelper.h"
#include "albaVME.h"

#include <algorithm>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolPicker)

//------------------------------------------------------------------------------
albaToolPicker::albaToolPicker()
{
  m_ContinuousPicking = false;
  m_Picking = false;
  m_Dragged = false;
}

//----------------------------------------------------------------------------
void albaToolPicker::AddTargetVME(albaVME *vme)
{
  if (vme && std::find(m_TargetVMEs.begin(), m_TargetVMEs.end(), vme) == m_TargetVMEs.end())
    m_TargetVMEs.push_back(vme);
}

//----------------------------------------------------------------------------
void albaToolPicker::RemoveTargetVME(albaVME *vme)
{
  m_TargetVMEs.erase(std::remove(m_TargetVMEs.begin(), m_TargetVMEs.end(), vme), m_TargetVMEs.end());
}

//----------------------------------------------------------------------------
bool albaToolPicker::IsPickable(albaVME *vme)
{
  if (m_TargetVMEs.empty())
    return !vme->IsA("albaVMEGizmo");
  return std::find(m_TargetVMEs.begin(), m_TargetVMEs.end(), vme) != m_TargetVMEs.end();
}

//----------------------------------------------------------------------------
bool albaToolPicker::OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  m_Dragged = false;
  m_Picking = false;

  // SHIFT leaves the button to the PER
  if (m_ShiftDown)
    return false;

  // without targets and continuous picking the click is checked at the button up
  if (m_TargetVMEs.empty() && !m_ContinuousPicking)
    return true;

  // the button down on a VME that is not picked is left to the PER
  double pickedPos[3], scalar;
  albaVME *pickedVme;
  vtkIdType pointId;
  if (!albaPickHelper::PickPoint(rwi->GetView(), pos, pickedPos, pickedVme, pointId, scalar) || !IsPickable(pickedVme))
    return false;

  if (m_ContinuousPicking)
  {
    m_Picking = true;
    SendPickedPointEvent(VME_PICKED, pickedPos, pointId, scalar, pickedVme);
  }
  return true;
}

//----------------------------------------------------------------------------
bool albaToolPicker::OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  m_Dragged = true;
  if (!m_Picking)
    return false;

  double pickedPos[3], scalar;
  albaVME *pickedVme;
  vtkIdType pointId;
  if (albaPickHelper::PickPoint(rwi->GetView(), pos, pickedPos, pickedVme, pointId, scalar) && IsPickable(pickedVme))
    SendPickedPointEvent(VME_PICKING, pickedPos, pointId, scalar, pickedVme);
  return true;
}

//----------------------------------------------------------------------------
bool albaToolPicker::OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (m_Dragged && !m_Picking)
    return true;
  m_Picking = false;

  double pickedPos[3], scalar;
  albaVME *pickedVme;
  vtkIdType pointId;
  if (albaPickHelper::PickPoint(rwi->GetView(), pos, pickedPos, pickedVme, pointId, scalar) && IsPickable(pickedVme))
    SendPickedPointEvent(VME_PICKED, pickedPos, pointId, scalar, pickedVme);
  return true;
}
