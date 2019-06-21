/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformInterface
 Authors: Stefano Perticoni
 
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


#include "albaGUITransformInterface.h"
#include "albaDecl.h"

#include "albaGUI.h"
#include "albaGUIButton.h"

#include "albaInteractorGenericMouse.h"
#include "albaInteractorCompositorMouse.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
albaGUITransformInterface::albaGUITransformInterface()
//----------------------------------------------------------------------------
{
  m_CurrentTime = -1;
  m_Listener = NULL;
  m_InputVME = NULL;
  m_Gui = NULL;
  m_RefSysVME = m_InputVME;
  m_TestMode = false;
}
//----------------------------------------------------------------------------
albaGUITransformInterface::~albaGUITransformInterface() 
//----------------------------------------------------------------------------
{   
 
}

//----------------------------------------------------------------------------
void albaGUITransformInterface::SetRefSys(albaVME* refSysVme)
//----------------------------------------------------------------------------
{ 
  if (refSysVme == NULL)
  return;

  m_RefSysVME = refSysVme;
  RefSysVmeChanged();
}
