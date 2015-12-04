/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMesh
 Authors: Stefano Perticoni
 
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

#include "mafOpImporterMesh.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"

#include "mafVMEMeshAnsysTextImporter.h"

#include "vtkMAFSmartPointer.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMesh::mafOpImporterMesh(const wxString &label) :
mafOp(label)
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
mafOpImporterMesh::~mafOpImporterMesh()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedVmeMesh);
}
//----------------------------------------------------------------------------
bool mafOpImporterMesh::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMesh::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterMesh *cp = new mafOpImporterMesh(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMesh::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
int mafOpImporterMesh::Read()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Loading file: ..."));
  }
	
  mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(m_NodesFileName.GetCStr());
  reader->SetElementsFileName(m_ElementsFileName.GetCStr());
  reader->SetMode(mafVMEMeshAnsysTextImporter::GENERIC_MODE);

	int returnValue = reader->Read();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
      mafMessage(_("Error parsing input files! See log window for details..."),_("Error"));
    }
  } 
  else if (returnValue == MAF_OK)
  {
    mafNEW(m_ImportedVmeMesh);
    m_ImportedVmeMesh->SetName("Imported Mesh");
	  m_ImportedVmeMesh->SetDataByDetaching(reader->GetOutput()->GetUnstructuredGridOutput()->GetVTKData(),0);

    mafTagItem tag_Nature;
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
void mafOpImporterMesh::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = "lis files (*.lis)|*.lis|All Files (*.*)|*.*";

  m_Gui = new mafGUI(this);
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
void mafOpImporterMesh::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
      break;
    }	
  }
}
