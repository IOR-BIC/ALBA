/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelect
 Authors: Gianluigi Crimi, from albaPositionalEventRouter

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

#include "albaToolSelect.h"
#include "albaDecl.h"
#include "albaEventBase.h"
#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaServiceLocator.h"
#include "albaAbsLogicManager.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolSelect);

//------------------------------------------------------------------------------
albaToolSelect::albaToolSelect()
{
  m_PickedVME = NULL;
  m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0;
  m_CanSelect = true;
  m_ShowContextMenu = false;
}

//------------------------------------------------------------------------------
void albaToolSelect::SetPickedVME(albaVME *vme, double pos[3])
{
  m_PickedVME = vme;
  for (int i = 0; i < 3; i++)
    m_PickedPosition[i] = pos ? pos[i] : 0;
}

//------------------------------------------------------------------------------
bool albaToolSelect::IsPickedVmeSelectable()
{
  return m_PickedVME && !m_PickedVME->IsA("albaVMEGizmo");
}

//------------------------------------------------------------------------------
bool albaToolSelect::Select()
{
  if (!m_CanSelect || !IsPickedVmeSelectable())
    return false;

  // a click on a landmark cloud selects its nearest landmark
  albaVME *vme = m_PickedVME;
  if (vme->IsA("albaVMELandmarkCloud"))
    vme = (albaVME *)((albaVMELandmarkCloud *)vme)->GetNearestLandmark(m_PickedPosition, true);

  if (albaServiceLocator::GetLogicManager())
    albaServiceLocator::GetLogicManager()->VmeSelect(vme);
  return true;
}

//------------------------------------------------------------------------------
bool albaToolSelect::OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  m_ShowContextMenu = true;
  return Select();
}

//------------------------------------------------------------------------------
bool albaToolSelect::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  m_ShowContextMenu = false;
  return false;
}

//------------------------------------------------------------------------------
bool albaToolSelect::OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (!m_ShowContextMenu)
    return false;

  m_ShowContextMenu = false;
  albaEventMacro(albaEventBase(this, SHOW_VIEW_CONTEXTUAL_MENU, m_PickedVME, MCH_UP));
  return true;
}
