/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-06-06 10:59:10 $
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


#include "mafGizmoInterface.h"
#include "mafGuiGizmoInterface.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
mafGizmoInterface::mafGizmoInterface()
//----------------------------------------------------------------------------
{ 
  m_InputVME = NULL;
  m_Listener = NULL;
  m_Modality = G_LOCAL;
  m_RefSysVME = NULL;
  m_Visibility = false;
}
//----------------------------------------------------------------------------
mafGizmoInterface::~mafGizmoInterface() 
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------  
void mafGizmoInterface::SendTransformMatrix(mafMatrix* matrix, int eventId, long arg)
//----------------------------------------------------------------------------
{
  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(matrix);
  e2s.SetId(eventId);
  e2s.SetArg(arg);

  mafEventMacro(e2s);
}
