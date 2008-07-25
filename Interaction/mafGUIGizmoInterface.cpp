/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIGizmoInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 08:44:32 $
  Version:   $Revision: 1.1 $
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


#include "mafGUIGizmoInterface.h"

//----------------------------------------------------------------------------
mafGUIGizmoInterface::mafGUIGizmoInterface(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_Gui = NULL;
}
//----------------------------------------------------------------------------
mafGUIGizmoInterface::~mafGUIGizmoInterface() 
//----------------------------------------------------------------------------
{

}
