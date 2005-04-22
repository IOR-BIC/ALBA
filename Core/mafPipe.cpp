/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-22 20:01:05 $
  Version:   $Revision: 1.3 $
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
#include "mafPipe.h"
#include "mafNode.h"
#include "vtkMAFAssembly.h"
#include "mmgGui.h"


mafCxxTypeMacro(mafPipe);
//----------------------------------------------------------------------------
mafPipe::mafPipe()
//----------------------------------------------------------------------------
{
	m_sg       = NULL;
  m_mafnode  = NULL;
  m_vme			 = NULL;
	m_asm1		 = NULL;
	m_asm2		 = NULL;
	m_ren1     = NULL;
	m_ren2     = NULL;
  m_Gui      = NULL;
	m_selected = false;
}
//----------------------------------------------------------------------------
void mafPipe::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  m_sg       = n->m_sg;
  m_mafnode  = n->m_vme;
  m_vme			 = m_mafnode->IsA("mafVME") ? ((mafVME*)m_mafnode) : NULL;
  m_asm1		 = n->m_asm1;
  m_asm2		 = n->m_asm2;
  m_ren1     = n->m_ren1;
  m_ren2     = n->m_ren2;
  m_selected = false;
}
//----------------------------------------------------------------------------
mafPipe::~mafPipe( ) 
//----------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mmgGui *mafPipe::GetGui()
//-------------------------------------------------------------------------
{
  if (m_Gui==NULL) CreateGui();
  assert(m_Gui);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafPipe::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mmgGui* mafPipe::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(NULL); // replace NULL with 'this' ....  //SIL. 22-4-2005: 

  wxString type = "mafPipe" ;
  m_Gui->Label("type :", type);

  return m_Gui;
}
