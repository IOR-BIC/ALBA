/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehavior
 Authors: Marco Petrone
 
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


#include "albaInteractionBehavior.h"
#include "albaVME.h"

#include "vtkProp3D.h"

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaInteractionBehavior);

//------------------------------------------------------------------------------
albaInteractionBehavior::albaInteractionBehavior()
{
  m_VME = NULL;
  m_Prop = NULL;
  m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0;
}

//------------------------------------------------------------------------------
albaInteractionBehavior::~albaInteractionBehavior()
{
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::SetVME(albaVME *vme)
{
  m_VME = vme;
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::SetProp(vtkProp3D *prop)
{
  m_Prop = prop;
}
