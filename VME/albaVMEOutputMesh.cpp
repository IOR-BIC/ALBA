/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMesh
 Authors: Stefano Perticoni
 
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
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaVMEOutputMesh.h"
#include "albaVME.h"
#include "albaIndent.h"
#include "albaDataPipe.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "vtkUnstructuredGrid.h"
#include "vtkImageData.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputMesh)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputMesh::albaVMEOutputMesh()
//-------------------------------------------------------------------------
{
  m_NumNodes = m_NumCells  = "0";

	m_Material = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputMesh::~albaVMEOutputMesh()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkUnstructuredGrid *albaVMEOutputMesh::GetUnstructuredGridData()
//-------------------------------------------------------------------------
{
  return vtkUnstructuredGrid::SafeDownCast(GetVTKData());
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputMesh::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();
  m_Gui->Divider();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputMesh::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  if (GetUnstructuredGridData())
  {
    m_NumCells = GetUnstructuredGridData()->GetNumberOfCells();
    m_NumNodes = GetUnstructuredGridData()->GetNumberOfPoints();
  }
  else
  {
    m_NumNodes=m_NumCells = _("0");
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputMesh::GetMaterial()
//-------------------------------------------------------------------------
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? mmaMaterial::SafeDownCast(GetVME()->GetAttribute("MaterialAttributes")) : NULL;
}

//-------------------------------------------------------------------------
void albaVMEOutputMesh::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
	m_Material = material;
}
