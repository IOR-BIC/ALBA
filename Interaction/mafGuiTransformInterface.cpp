/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTransformInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-23 09:17:16 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGuiTransformInterface.h"
#include "mafDecl.h"

#include "mmgGui.h"
#include "mmgButton.h"

#include "mmiGenericMouse.h"
#include "mmiCompositorMouse.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGuiTransformInterface::mafGuiTransformInterface()
//----------------------------------------------------------------------------
{
  m_CurrentTime = -1;
  m_Listener = NULL;
  m_InputVME = NULL;
  m_Gui = NULL;
  m_RefSysVME = m_InputVME;
}
//----------------------------------------------------------------------------
mafGuiTransformInterface::~mafGuiTransformInterface() 
//----------------------------------------------------------------------------
{   
 
}

//----------------------------------------------------------------------------
void mafGuiTransformInterface::SetRefSys(mafVME* refSysVme)
//----------------------------------------------------------------------------
{ 
  if (refSysVme == NULL)
  return;

  m_RefSysVME = refSysVme;
  RefSysVmeChanged();
}
