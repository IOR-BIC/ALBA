/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMesh
 Authors: Simone Brazzale, Nicola Vanella
 
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

#include "albaOpExporterMesh.h"

#include "wx/busyinfo.h"

#include "albaDecl.h"

#include "albaVME.h"
#include "albaVMEMesh.h"
#include "albaVMEOutputMesh.h"

#include "albaVMEMeshAnsysTextExporter.h"

#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterMesh::albaOpExporterMesh(const wxString &label) :
	albaOpExporterFEMCommon(label)
{
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
}

//----------------------------------------------------------------------------
albaOpExporterMesh::~albaOpExporterMesh()
{
}

//----------------------------------------------------------------------------
bool albaOpExporterMesh::InternalAccept(albaVME*node)
{
  // Accept a albaVMEMesh as input
  return (node && node->GetOutput()->IsALBAType(albaVMEOutputMesh));;
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterMesh::Copy()   
{
  // Copy the operation
  albaOpExporterMesh *cp = new albaOpExporterMesh(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
void albaOpExporterMesh::OpRun()
{
	CreateGui();
}

//----------------------------------------------------------------------------
albaString albaOpExporterMesh::GetWildcard()
{
	return "lis files (*.txt)|*.txt|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void albaOpExporterMesh::OnOK()
{
	albaString wildcard = GetWildcard();

	m_FileDir = "";

	m_FileDir << this->m_Input->GetName();
	m_FileDir << ".txt";

	wxString f;
	f = albaGetSaveFile(m_FileDir, wildcard).ToAscii();
	if (!f.IsEmpty())
	{
		m_File = f;
	}

	// Generate the three output files
	wxString wxstr(m_File.GetCStr());
	wxString wxname = wxstr.BeforeLast('.');
	wxString wxextension = wxstr.AfterLast('.');

	m_NodesFileName << wxname.ToAscii() << "_NODES." << wxextension.ToAscii();
	m_ElementsFileName << wxname.ToAscii() << "_ELEMENTS." << wxextension.ToAscii();
	m_MaterialsFileName << wxname.ToAscii() << "_MATERIALS." << wxextension.ToAscii();

	// Write to output
	int result = Write();
}

//----------------------------------------------------------------------------
int albaOpExporterMesh::Write()
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Writing file ..."));
  }
	
  // Create the writer and pass the file name
  albaVMEMeshAnsysTextExporter *writer = new albaVMEMeshAnsysTextExporter;
  writer->SetInput((vtkUnstructuredGrid*)m_Input->GetOutput()->GetVTKData());

  writer->SetOutputNodesFileName(m_NodesFileName.GetCStr());
  writer->SetOutputElementsFileName(m_ElementsFileName.GetCStr());

	bool hasMaterials = HasMaterials();

	if (hasMaterials)
	{
		writer->SetMode(albaVMEMeshAnsysTextExporter::WITH_MAT_MODE);

		writer->SetOutputMaterialsFileName(m_MaterialsFileName.GetCStr());

		writer->SetMaterialData(GetMaterialData());
		writer->SetMatIdArray(GetMatIdArray());
	}
	else
	{
		writer->SetMode(albaVMEMeshAnsysTextExporter::WITHOUT_MAT_MODE);
	}
	
  // Write
	int returnValue = writer->Write();

  if (returnValue == ALBA_ERROR)
  {
    if (!m_TestMode)
    {
      albaMessage(_("Error writing output files! See log window for details..."),_("Error"));
    }
  }

  // free memory
  delete writer;
  return returnValue;
}