/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipe
 Authors: Silvano Imboden
 
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
//----------------------------------------------------------------------------


#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaGUI.h"

#include "albaPipe.h"
#include "albaVME.h"
#include "vtkALBAAssembly.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipe);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipe::albaPipe()
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
void albaPipe::Create(albaSceneNode *n)
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

		n->SetPipe(this);
	
	  m_Selected = false;
  }
}
//----------------------------------------------------------------------------
albaPipe::~albaPipe()
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
void albaPipe::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
albaGUI* albaPipe::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);

  albaString type = "albaPipe" ;
  m_Gui->Label("type :", type);
	m_Gui->Divider();
  return m_Gui;
}
