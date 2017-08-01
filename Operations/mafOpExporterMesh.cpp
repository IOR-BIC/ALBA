/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMesh
 Authors: Simone Brazzale, Nicola Vanella
 
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
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExporterMesh.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"

#include "mafVME.h"
#include "mafVMEMesh.h"
#include "mafVMEOutputMesh.h"

#include "mafVMEMeshAnsysTextExporter.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterMesh::mafOpExporterMesh(const wxString &label) :
	mafOpExporterFEMCommon(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
}

//----------------------------------------------------------------------------
mafOpExporterMesh::~mafOpExporterMesh()
{
}

//----------------------------------------------------------------------------
bool mafOpExporterMesh::Accept(mafVME*node)
{
  // Accept a mafVMEMesh as input
  return (node && node->GetOutput()->IsMAFType(mafVMEOutputMesh));;
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterMesh::Copy()   
{
  // Copy the operation
  mafOpExporterMesh *cp = new mafOpExporterMesh(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
void mafOpExporterMesh::OpRun()
{
	CreateGui();
}

//----------------------------------------------------------------------------
mafString mafOpExporterMesh::GetWildcard()
{
	return "lis files (*.lis)|*.lis|All Files (*.*)|*.*";
}

//----------------------------------------------------------------------------
void mafOpExporterMesh::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
			mafEventMacro(*e);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void mafOpExporterMesh::OnOK()
{
	mafString wildcard = GetWildcard();

	m_FileDir = "";

	m_FileDir << this->m_Input->GetName();
	m_FileDir << ".lis";

	wxString f;
	f = mafGetSaveFile(m_FileDir, wildcard).c_str();
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

	if (result == MAF_OK)
	{
		OpStop(OP_RUN_OK);
	}
	else
	{
		OpStop(OP_RUN_OK);
	}
}
//----------------------------------------------------------------------------
void mafOpExporterMesh::OpStop(int result)
{
	HideGui();
	mafEventMacro(mafEvent(this, result));
}

//----------------------------------------------------------------------------
void mafOpExporterMesh::CreateGui()
{
	Superclass::CreateGui();

	m_Gui->OkCancel();
	m_Gui->Divider();

	m_Gui->FitGui();
	m_Gui->Update();

	ShowGui();
}

//----------------------------------------------------------------------------
int mafOpExporterMesh::Write()
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Writing file ..."));
  }
	
  // Create the writer and pass the file name
  mafVMEMeshAnsysTextExporter *writer = new mafVMEMeshAnsysTextExporter;
  writer->SetInput((vtkUnstructuredGrid*)m_Input->GetOutput()->GetVTKData());

  writer->SetOutputNodesFileName(m_NodesFileName.GetCStr());
  writer->SetOutputElementsFileName(m_ElementsFileName.GetCStr());
  writer->SetOutputMaterialsFileName(m_MaterialsFileName.GetCStr());

	writer->SetMaterialData(GetMaterialData());
	writer->SetMatIdArray(GetMatIdArray());

  // Write
	int returnValue = writer->Write();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
      mafMessage(_("Error writing output files! See log window for details..."),_("Error"));
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