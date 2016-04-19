/*=========================================================================

 Program: MAF2
 Module: mafPipe
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
//----------------------------------------------------------------------------


#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafGUI.h"

#include "mafPipe.h"
#include "mafVME.h"
#include "vtkMAFAssembly.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipe);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipe::mafPipe()
//----------------------------------------------------------------------------
{
	m_Sg            = NULL;
  m_Vme			      = NULL;
	m_AssemblyFront	= NULL;
	m_AssemblyBack	= NULL;
	m_AlwaysVisibleAssembly = NULL;
	m_RenFront      = NULL;
	m_RenBack       = NULL;
	m_AlwaysVisibleRenderer = NULL;
  m_Gui           = NULL;
  m_Listener      = NULL;
	m_Selected = false;
}
//----------------------------------------------------------------------------
void mafPipe::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  if (n != NULL)
  {
	  m_Sg       = n->GetSceneGraph();
	  m_Vme			 = n->GetVme();
	  
	  m_AssemblyFront = n->GetAssemblyFront();
	  m_AssemblyBack	= n->GetAssemblyBack();
	  m_AlwaysVisibleAssembly = n->GetAlwaysVisibleAssembly();
	  m_RenFront      = n->GetRenFront();
	  m_RenBack       = n->GetRenBack();
	  m_AlwaysVisibleRenderer = n->GetAlwaysVisibleRenderer();
	
	  m_Selected = false;
  }
}
//----------------------------------------------------------------------------
mafPipe::~mafPipe()
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
void mafPipe::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mafGUI* mafPipe::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  mafString type = "mafPipe" ;
  m_Gui->Label("type :", type);
	m_Gui->Divider();
  return m_Gui;
}
