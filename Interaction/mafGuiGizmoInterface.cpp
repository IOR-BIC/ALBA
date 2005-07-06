/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiGizmoInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:27 $
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


#include "mafGuiGizmoInterface.h"

//----------------------------------------------------------------------------
mafGuiGizmoInterface::mafGuiGizmoInterface(mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_Gui = NULL;
}
//----------------------------------------------------------------------------
mafGuiGizmoInterface::~mafGuiGizmoInterface() 
//----------------------------------------------------------------------------
{

}
