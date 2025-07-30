/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVMEDataSetAttributes
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

#include "albaOpImporterVMEDataSetAttributes.h"

#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"
#include "albaVMEDataSetAttributesImporter.h"
#include "albaVMEGeneric.h"

#include "albaGUI.h"

#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterVMEDataSetAttributes);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterVMEDataSetAttributes::albaOpImporterVMEDataSetAttributes(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  
  m_FileName = "";
  m_FilePrefix = "";
  m_TimeVarying = 0;
  m_TSFileName = "";
  m_UseTSFile = 0;
  m_AttributeType = albaVMEDataSetAttributesImporter::POINT_DATA;
  
  m_InputPreserving = false;
  m_Input = NULL;

  m_UseIdArray = false;
  m_IdArrayName = "Id";
}

//----------------------------------------------------------------------------
albaOpImporterVMEDataSetAttributes::~albaOpImporterVMEDataSetAttributes( ) 
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
bool albaOpImporterVMEDataSetAttributes::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterVMEDataSetAttributes::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterVMEDataSetAttributes *cp = new albaOpImporterVMEDataSetAttributes(m_Label);
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterVMEDataSetAttributes::OpRun()   
{
  CreateGui();
  ShowGui();
}


int albaOpImporterVMEDataSetAttributes::Read()
{
  albaGUIBusyInfo wait("Reading file: ...",m_TestMode);  
  
  albaVMEGeneric *input = albaVMEGeneric::SafeDownCast(m_Input);

  albaVMEDataSetAttributesImporter* attributesImporter = NULL;
  
  attributesImporter = new albaVMEDataSetAttributesImporter;
  attributesImporter->SetInput(input);
	attributesImporter->SetFileName(m_FileName.GetCStr());
  attributesImporter->SetTimeVaryingFlag(m_TimeVarying == 1 ? true : false);
  attributesImporter->SetUseTSFile(m_UseTSFile ? true : false);
  attributesImporter->SetTSFileName(m_TSFileName.GetCStr());
  attributesImporter->SetFilePrefix(m_FilePrefix.GetCStr());
  attributesImporter->SetAttributeType(m_AttributeType);
  attributesImporter->SetUseIdArrayFlag(m_UseIdArray);
  attributesImporter->SetIdArrayName(m_IdArrayName.GetCStr());

  int returnValue = attributesImporter->Read();

  if (returnValue == ALBA_ERROR)
  {
    if (!m_TestMode)
    {
      albaMessage(_("Error parsing input files! See log window for details..."),_("Error"));
    }
  } 
  else
  {
    m_Output = attributesImporter->GetOutput();
  }

  cppDEL(attributesImporter);

  return returnValue;
}

void albaOpImporterVMEDataSetAttributes::OpStop(int result)
{
  if (result == OP_RUN_OK)
  {
    this->Read();
  } 
  
  HideGui();

	albaEventMacro(albaEvent(this,result));  	   
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
  ID_UseIdArray,
  ID_IdArrayName,
  ID_OK,
  ID_CANCEL,
};

void albaOpImporterVMEDataSetAttributes::CreateGui()
{
  albaString wildcard = "txt files (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new albaGUI(this);
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

  m_Gui->Label("use id map array");
  m_Gui->Bool(ID_UseIdArray,"",&m_UseIdArray);
  m_Gui->Divider();
  m_Gui->Label("id map array name:",true);
  m_Gui->String(ID_IdArrayName,	"",	&m_IdArrayName);
  m_Gui->Enable(ID_IdArrayName, m_UseIdArray ? true : false);
  m_Gui->Divider();

  m_Gui->Divider();
  m_Gui->OkCancel();

  m_Gui->Update();

  m_TimeVarying ? EnableTimeVaryingGui(true) : EnableTimeVaryingGui(false);
}
  

void albaOpImporterVMEDataSetAttributes::EnableTimeVaryingGui( bool enable )
{
  m_Gui->Enable(ID_FilePrefix, enable);
  m_Gui->Enable(ID_UseTSFile, enable);
  m_Gui->Enable(ID_TSFileName, enable && m_UseTSFile);
}
void albaOpImporterVMEDataSetAttributes::OnEvent(albaEventBase *alba_event) 
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
    
    case ID_UseIdArray:
      m_Gui->Enable(ID_IdArrayName, m_UseIdArray ? true : false);
    break;

    case wxOK:
    {
      this->OpStop(OP_RUN_OK);

      //WORKAROUND CODE 
      //referred to bug 933
      if (m_Output != NULL)
      {
				GetLogicManager()->VmeVisualModeChanged(m_Output);
      }
      //END WORKAROUND CODE 

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
      albaEventMacro(*e);
    break;
    }	
  }
}

void albaOpImporterVMEDataSetAttributes::SetFileName( const char *filename )
{
	m_FileName = filename;
}

void albaOpImporterVMEDataSetAttributes::SetFilePrefix( const char *filePrefix )
{
	m_FilePrefix = filePrefix;
}

void albaOpImporterVMEDataSetAttributes::SetTSFileName( const char *tsFileName )
{
	m_TSFileName = tsFileName;
}

void albaOpImporterVMEDataSetAttributes::SetUseTSFile( bool useTSFile )
{
  m_UseTSFile = useTSFile;
}

void albaOpImporterVMEDataSetAttributes::SetDataTypeToPointData()
{
  m_AttributeType = albaVMEDataSetAttributesImporter::POINT_DATA;
}

void albaOpImporterVMEDataSetAttributes::SetDataTypeToCellData()
{
  m_AttributeType = albaVMEDataSetAttributesImporter::CELL_DATA;	
}

void albaOpImporterVMEDataSetAttributes::TimeVaryingOn()
{
  m_TimeVarying = true	;
}

void albaOpImporterVMEDataSetAttributes::TimeVaryingOff()
{
	m_TimeVarying = false;
}
