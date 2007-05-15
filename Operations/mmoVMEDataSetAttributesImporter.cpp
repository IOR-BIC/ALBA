/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVMEDataSetAttributesImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-05-15 16:29:33 $
  Version:   $Revision: 1.3 $
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

#include "mmoVMEDataSetAttributesImporter.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMEDataSetAttributesImporter.h"
#include "mafVMEGeneric.h"

#include "mmgGui.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoVMEDataSetAttributesImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoVMEDataSetAttributesImporter::mmoVMEDataSetAttributesImporter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  
  m_FileName = "";
  m_FilePrefix = "";
  m_TimeVarying = 0;
  m_TSFileName = "";
  m_UseTSFile = 0;
  m_AttributeType = mafVMEDataSetAttributesImporter::POINT_DATA;
  
  m_InputPreserving = false;
  m_Input = NULL;
}

//----------------------------------------------------------------------------
mmoVMEDataSetAttributesImporter::~mmoVMEDataSetAttributesImporter( ) 
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool mmoVMEDataSetAttributesImporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* mmoVMEDataSetAttributesImporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoVMEDataSetAttributesImporter *cp = new mmoVMEDataSetAttributesImporter(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void mmoVMEDataSetAttributesImporter::OpRun()   
{
  CreateGui();
  ShowGui();
}


int mmoVMEDataSetAttributesImporter::Read()
{
  
  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading file: ...");  
  }

  mafVMEGeneric *input = mafVMEGeneric::SafeDownCast(m_Input);

  mafVMEDataSetAttributesImporter* attributesImporter = NULL;
  
  attributesImporter = new mafVMEDataSetAttributesImporter;
  attributesImporter->SetInput(input);
	attributesImporter->SetFileName(m_FileName.GetCStr());
  attributesImporter->SetTimeVaryingFlag(m_TimeVarying == 1 ? true : false);
  attributesImporter->SetUseTSFile(m_UseTSFile ? true : false);
  attributesImporter->SetTSFileName(m_TSFileName.GetCStr());
  attributesImporter->SetFilePrefix(m_FilePrefix.GetCStr());
  attributesImporter->SetAttributeType(m_AttributeType);

  int returnValue = attributesImporter->Read();

  if (returnValue == MAF_ERROR)
  {
    if (!m_TestMode)
    {
      wxMessageBox("Error parsing input files! See log window for details...");  
    }
  } 
  else
  {
    m_Output = attributesImporter->GetOutput();
  }

  cppDEL(attributesImporter);

  return returnValue;
}

void mmoVMEDataSetAttributesImporter::OpStop(int result)
{
  if (result == OP_RUN_OK)
  {
    this->Read();
  } 
  
  HideGui();

	mafEventMacro(mafEvent(this,result));  	   
}

enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_AttributeType,
  ID_FileName,
  ID_FilePrefix,
  ID_UseTSFile,
  ID_TSFileName,
  ID_TimeVarying,
  ID_OK,
  ID_CANCEL,
};

void mmoVMEDataSetAttributesImporter::CreateGui()
{
  mafString wildcard = "txt files (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);
  
  m_Gui->Label(" ");
  m_Gui->Label("attributes file:",true);
  m_Gui->FileOpen (ID_FileName,	"",	&m_FileName, wildcard);
  
  m_Gui->Label("time varying");
  m_Gui->Bool(ID_TimeVarying,"",&m_TimeVarying);
  m_Gui->Divider();

  m_Gui->Label("file prefix:",true);
  m_Gui->String(ID_FilePrefix,	"",	&m_FilePrefix, wildcard);
  m_Gui->Divider();

  m_Gui->Bool(ID_UseTSFile,"use TS file:",&m_UseTSFile);
  m_Gui->Divider();

  m_Gui->Label("time stamps file:",true);
  m_Gui->FileOpen (ID_TSFileName,	"",	&m_TSFileName, wildcard);
  m_Gui->Divider();

  wxString attributeType[2]={"point data","cell data"};
  m_Gui->Label("Importer type",true);
  m_Gui->Combo(ID_AttributeType,"",&m_AttributeType,2,attributeType);
  m_Gui->Divider(2);

  m_Gui->Divider();
  m_Gui->OkCancel();

  m_Gui->Update();

  m_TimeVarying ? EnableTimeVaryingGui(true) : EnableTimeVaryingGui(false);
}
  

void mmoVMEDataSetAttributesImporter::EnableTimeVaryingGui( bool enable )
{
  m_Gui->Enable(ID_FilePrefix, enable);
  m_Gui->Enable(ID_UseTSFile, enable);
  m_Gui->Enable(ID_TSFileName, enable && m_UseTSFile);
}
void mmoVMEDataSetAttributesImporter::OnEvent(mafEventBase *maf_event) 
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    
    case ID_FileName:
    break;

    case ID_FilePrefix:
    break;

    case ID_TSFileName:
    break;

    case ID_TimeVarying:
      m_TimeVarying ? EnableTimeVaryingGui(true) : EnableTimeVaryingGui(false);
    break;

    case ID_UseTSFile:
      m_Gui->Enable(ID_TSFileName,m_UseTSFile ? true : false);
    break;

    case wxOK:
    {
      this->OpStop(OP_RUN_OK);
      return;
    }
    break;

    case wxCANCEL:
    {
      this->OpStop(OP_RUN_CANCEL);
      return;
    }
    break;
    
    default:
      mafEventMacro(*e);
    break;
    }	
  }
}

void mmoVMEDataSetAttributesImporter::SetFileName( const char *filename )
{
	m_FileName = filename;
}

void mmoVMEDataSetAttributesImporter::SetFilePrefix( const char *filePrefix )
{
	m_FilePrefix = filePrefix;
}

void mmoVMEDataSetAttributesImporter::SetTSFileName( const char *tsFileName )
{
	m_TSFileName = tsFileName;
}

void mmoVMEDataSetAttributesImporter::SetUseTSFile( bool useTSFile )
{
  m_UseTSFile = useTSFile;
}

void mmoVMEDataSetAttributesImporter::SetDataTypeToPointData()
{
  m_AttributeType = mafVMEDataSetAttributesImporter::POINT_DATA;
}

void mmoVMEDataSetAttributesImporter::SetDataTypeToCellData()
{
  m_AttributeType = mafVMEDataSetAttributesImporter::CELL_DATA;	
}

void mmoVMEDataSetAttributesImporter::TimeVaryingOn()
{
  m_TimeVarying = true	;
}

void mmoVMEDataSetAttributesImporter::TimeVaryingOff()
{
	m_TimeVarying = false;
}