/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMesh
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

#include "albaOpImporterMesh.h" 

#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"

#include "albaVMEMeshAnsysTextImporter.h"

#include "vtkALBASmartPointer.h"

#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterMesh::albaOpImporterMesh(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;

	m_NodesFileName = "";
  m_ElementsFileName = "";
	m_MaterialsFileName = "";
	m_ImportMaterials = true;
}

//----------------------------------------------------------------------------
albaOpImporterMesh::~albaOpImporterMesh()
{
  albaDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool albaOpImporterMesh::InternalAccept(albaVME*node)
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterMesh::Copy()   
{
  albaOpImporterMesh *cp = new albaOpImporterMesh(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
void albaOpImporterMesh::OpRun()   
{
	CreateGui();
  ShowGui();
}

//----------------------------------------------------------------------------
int albaOpImporterMesh::Read()
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }
	
  albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(m_NodesFileName.GetCStr());
  reader->SetElementsFileName(m_ElementsFileName.GetCStr());
	
	if (!m_ImportMaterials)
	{
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITHOUT_MAT_MODE);
	}
	else
	{
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITH_MAT_MODE);
		reader->SetMaterialsFileName(m_MaterialsFileName.GetCStr());
	}

	int returnValue = reader->Read();

  if (returnValue == ALBA_ERROR)
  {
    if (!m_TestMode)
    {
      albaMessage(_("Error parsing input files! See log window for details..."),_("Error"));
    }
  } 
  else if (returnValue == ALBA_OK)
  {
    albaNEW(m_ImportedVmeMesh);
		albaString tmp, name;
		name = m_NodesFileName.BaseName();
		
		tmp = wxString(name.GetCStr()).MakeUpper().ToAscii();
		
		int first;
		first = tmp.FindFirst(".");
		if(tmp.FindFirst("NODES")>0)
			first = MIN(first, tmp.FindFirst("NODES"));
		if(tmp.FindFirst("_") >0)
			first = MIN(first, tmp.FindFirst("_"));
		
		name.Erase(first);
	
    m_ImportedVmeMesh->SetName(name.GetCStr());
	  m_ImportedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

    albaTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    m_ImportedVmeMesh->GetTagArray()->SetTag(tag_Nature);

    m_Output = m_ImportedVmeMesh;
  }

  delete reader;
  return returnValue;
}

//----------------------------------------------------------------------------
// Operation constants
//----------------------------------------------------------------------------
enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_ImportMaterials,
	ID_NodesFileName,
  ID_ElementsFileName,
	ID_MaterialsFileName,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void albaOpImporterMesh::CreateGui()
{
	albaString wildcard = "lis files (*.lis)|*.lis|txt files (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);

	m_Gui->Label("");
	m_Gui->Label(_("Nodes file:"),true);
  m_Gui->FileOpen (ID_NodesFileName,	"",	&m_NodesFileName, wildcard);
  m_Gui->Divider();
 
  m_Gui->Label(_("Elements file:"),true);
  m_Gui->FileOpen (ID_ElementsFileName,	"",	&m_ElementsFileName, wildcard);
  m_Gui->Divider();

	m_Gui->Bool(ID_ImportMaterials, "Import Materials", &m_ImportMaterials, true);
  
	m_Gui->Label(_("Materials file:"),true);
  m_Gui->FileOpen (ID_MaterialsFileName,	"",	&m_MaterialsFileName, wildcard);
  m_Gui->Divider(2);
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("");

	m_Gui->OkCancel();

	m_Gui->Enable(ID_MaterialsFileName, m_ImportMaterials);
	m_Gui->Enable(wxOK, false);

  
  m_Gui->FitGui();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpImporterMesh::OnEvent(albaEventBase *alba_event) 
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_NodesFileName:
      case ID_ElementsFileName:
			case ID_MaterialsFileName:
			case ID_ImportMaterials:
				m_Gui->Enable(ID_MaterialsFileName, m_ImportMaterials);
				m_Gui->Enable(wxOK, !m_NodesFileName.IsEmpty() && !m_ElementsFileName.IsEmpty() && (!m_ImportMaterials || !m_MaterialsFileName.IsEmpty()));
			break;
      case wxOK:
      {
        if(this->Read() == ALBA_OK)
					this->OpStop(OP_RUN_OK);
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

