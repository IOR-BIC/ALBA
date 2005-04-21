/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:37:43 $
  Version:   $Revision: 1.2 $
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
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafPipe.h"
#include "mafNode.h"
#include "vtkMAFAssembly.h"
//----------------------------------------------------------------------------
mafPipe::mafPipe(mafSceneNode *n)
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
}
