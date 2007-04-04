/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMesh.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-04 16:38:14 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafVMEOutputMesh.h"
#include "mafVME.h"
#include "mafIndent.h"
#include "mafDataPipe.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "vtkUnstructuredGrid.h"
#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputMesh)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputMesh::mafVMEOutputMesh()
//-------------------------------------------------------------------------
{
  m_NumCells  = "0";
}

//-------------------------------------------------------------------------
mafVMEOutputMesh::~mafVMEOutputMesh()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkUnstructuredGrid *mafVMEOutputMesh::GetUnstructuredGridData()
//-------------------------------------------------------------------------
{
  return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
mmgGui* mafVMEOutputMesh::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  if (GetUnstructuredGridData())
  {
    this->Update();
    int num = GetUnstructuredGridData()->GetNumberOfCells();
    m_NumCells = num;
  }
  m_Gui->Label(_("cells: "), &m_NumCells, true);
  m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputMesh::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetUnstructuredGridData())
  {
    int num = GetUnstructuredGridData()->GetNumberOfCells();
    m_NumCells = num;
  }
  else
  {
    m_NumCells = _("0");
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
