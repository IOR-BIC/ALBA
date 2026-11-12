/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionAgent
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

#include "albaInteractionAgent.h"
#include "albaEvent.h"
#include "albaVME.h"

#include "vtkRenderer.h"
#include "vtkPoints.h"

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaInteractionAgent);

//------------------------------------------------------------------------------
albaInteractionAgent::albaInteractionAgent()
{
  m_View = NULL;
  m_Renderer = NULL;
}

//------------------------------------------------------------------------------
albaInteractionAgent::~albaInteractionAgent()
{
}

//------------------------------------------------------------------------------
void albaInteractionAgent::SetRendererAndView(vtkRenderer *ren, albaView *view)
{
  m_Renderer = ren;
  m_View = view;
}

//----------------------------------------------------------------------------
void albaInteractionAgent::ComputeDisplayToWorld(double x, double y, double z, double worldPt[4])
{
  if (!m_Renderer)
    return;

  m_Renderer->SetDisplayPoint(x, y, z);
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint(worldPt);
  if (worldPt[3])
  {
    worldPt[0] /= worldPt[3];
    worldPt[1] /= worldPt[3];
    worldPt[2] /= worldPt[3];
    worldPt[3] = 1.0;
  }
}

//----------------------------------------------------------------------------
void albaInteractionAgent::ComputeWorldToDisplay(double x, double y, double z, double displayPt[3])
{
  if (!m_Renderer)
    return;

  m_Renderer->SetWorldPoint(x, y, z, 1.0);
  m_Renderer->WorldToDisplay();
  m_Renderer->GetDisplayPoint(displayPt);
}

//----------------------------------------------------------------------------
void albaInteractionAgent::SendPickedPointEvent(int eventId, double pos[3], vtkIdType id, double scalar, albaVME *vme)
{
  vtkALBASmartPointer<vtkPoints> points;
  points->SetNumberOfPoints(1);
  points->SetPoint(0, pos);

  albaEvent pickEvent(this, eventId, (vtkObject *)points.GetPointer(), (long)id);
  pickEvent.SetDouble(scalar);
  pickEvent.SetVme(vme);
  albaEventMacro(pickEvent);
}
