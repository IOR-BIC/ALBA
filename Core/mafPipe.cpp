/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.16 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
#include "mafNode.h"
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
	m_RenFront      = NULL;
	m_RenBack       = NULL;
  m_Gui           = NULL;
  m_Listener      = NULL;
	m_Selected = false;
}
//----------------------------------------------------------------------------
void mafPipe::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  m_Sg       = n->m_Sg;
  m_Vme			 = n->m_Vme->IsA("mafVME") ? ((mafVME*)n->m_Vme) : NULL;
  
  m_AssemblyFront = n->m_AssemblyFront;
  m_AssemblyBack	= n->m_AssemblyBack;
  m_RenFront      = n->m_RenFront;
  m_RenBack       = n->m_RenBack;
  
  m_Selected = false;
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
