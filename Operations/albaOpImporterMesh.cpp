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
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_ImporterType = 0;
  m_ImportedVmeMesh = NULL;
  m_NodesFileName = "";
  m_ElementsFileName = "";
}

//----------------------------------------------------------------------------
albaOpImporterMesh::~albaOpImporterMesh()
//----------------------------------------------------------------------------
{
  albaDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool albaOpImporterMesh::Accept(albaVME*node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterMesh::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterMesh *cp = new albaOpImporterMesh(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterMesh::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
int albaOpImporterMesh::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }
	
  albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(m_NodesFileName.GetCStr());
  reader->SetElementsFileName(m_ElementsFileName.GetCStr());
  reader->SetMode(albaVMEMeshAnsysTextImporter::GENERIC_MODE);

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
    m_ImportedVmeMesh->SetName("Imported Mesh");
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
  ID_Importer_Type,
  ID_NodesFileName,
  ID_ElementsFileName,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void albaOpImporterMesh::CreateGui()
//----------------------------------------------------------------------------
{
  albaString wildcard = "lis files (*.lis)|*.lis|All Files (*.*)|*.*";

  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Label("", true);

  //////////////////////////////////////////////////////////////////////////

  m_Gui->Label(_("Nodes file:"), true);
  m_Gui->FileOpen (ID_NodesFileName,	"",	&m_NodesFileName, wildcard);
  m_Gui->Divider();
 
  m_Gui->Label(_("Elements file:"), true);
  m_Gui->FileOpen (ID_ElementsFileName,	"",	&m_ElementsFileName, wildcard);
  m_Gui->Divider();

  m_Gui->Divider(2);
  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("");

  m_Gui->OkCancel();
  m_Gui->Label("");
  m_Gui->Label("");

  m_Gui->FitGui();
  m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaOpImporterMesh::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_NodesFileName:
        // this->SetNodesFileName(m_NodesFileName.GetCStr());       
      break;
      case ID_ElementsFileName:
        // this->SetElementsFileName(m_ElementsFileName.GetCStr());
      break;
      case wxOK:
      {
        this->Read();
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