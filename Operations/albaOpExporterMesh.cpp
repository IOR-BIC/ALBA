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
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
}

//----------------------------------------------------------------------------
albaOpExporterMesh::~albaOpExporterMesh()
{
}

//----------------------------------------------------------------------------
bool albaOpExporterMesh::Accept(albaVME*node)
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
	return "lis files (*.lis)|*.lis|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void albaOpExporterMesh::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case wxOK:
		{
			OnOK();
		}
		break;
		case wxCANCEL:
		{
			this->OpStop(OP_RUN_CANCEL);
		}
		break;
		default:
			albaEventMacro(*e);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExporterMesh::OnOK()
{
	albaString wildcard = GetWildcard();

	m_FileDir = "";

	m_FileDir << this->m_Input->GetName();
	m_FileDir << ".lis";

	wxString f;
	f = albaGetSaveFile(m_FileDir, wildcard).c_str();
	if (!f.IsEmpty())
	{
		m_File = f;
	}

	// Generate the three output files
	wxString wxstr(m_File.GetCStr());
	wxString wxname = wxstr.Before('.');
	wxString wxextension = wxstr.After('.');

	m_NodesFileName << wxname.c_str() << "_NODES." << wxextension.c_str();
	m_ElementsFileName << wxname.c_str() << "_ELEMENTS." << wxextension.c_str();
	m_MaterialsFileName << wxname.c_str() << "_MATERIALS." << wxextension.c_str();

	// Write to output
	int result = Write();

	if (result == ALBA_OK)
	{
		OpStop(OP_RUN_OK);
	}
	else
	{
		OpStop(OP_RUN_OK);
	}
}
//----------------------------------------------------------------------------
void albaOpExporterMesh::OpStop(int result)
{
	HideGui();
	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpExporterMesh::CreateGui()
{
	Superclass::CreateGui();

	m_Gui->OkCancel();
	m_Gui->Divider();

	m_Gui->FitGui();
	m_Gui->Update();

	ShowGui();
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
  writer->SetOutputMaterialsFileName(m_MaterialsFileName.GetCStr());

	writer->SetMaterialData(GetMaterialData());
	writer->SetMatIdArray(GetMatIdArray());

  // Write
	int returnValue = writer->Write();

  if (returnValue == ALBA_ERROR)
  {
    if (!m_TestMode)
    {
      albaMessage(_("Error writing output files! See log window for details..."),_("Error"));
    }
  }

  // Create a dummy file that will be used to check for duplicate files
  std::ofstream f_Out(m_File);
  if (!f_Out.bad())
  {
    f_Out << "FILE LIST:" << "\n";
    f_Out << m_NodesFileName.GetCStr() << "\n";
    f_Out << m_ElementsFileName.GetCStr() << "\n";
    f_Out << m_MaterialsFileName.GetCStr() << "\n";
  }
  f_Out.close();

  // free memeory
  delete writer;
  return returnValue;
}