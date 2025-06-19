/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterASCII
 Authors: Paolo Quadrani
 
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

#include "albaOpImporterASCII.h"
#include <albaGUIBusyInfo.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>

#include "albaEvent.h"
#include "albaGUI.h"

#include "albaASCIIImporterUtility.h"
#include "albaVME.h"
#include "albaVMEScalarMatrix.h"

#include "albaTagArray.h"

#include <algorithm>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterASCII);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterASCII::albaOpImporterASCII(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_Files.clear();

  m_ScalarOrder = 1;

  m_FileDir = albaGetLastUserFolder();
  m_ScalarData = NULL;
  m_ReadyToExecute = false;
}
//----------------------------------------------------------------------------
albaOpImporterASCII::~albaOpImporterASCII()
//----------------------------------------------------------------------------
{
  albaDEL(m_ScalarData);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterASCII::Copy()
//----------------------------------------------------------------------------
{
  albaOpImporterASCII *cp = new albaOpImporterASCII(m_Label);
  cp->m_Files	= m_Files;
  cp->m_FileDir = m_FileDir;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterASCII::SetParameters(void *param)
//----------------------------------------------------------------------------
{
  albaString t = "";
  wxString op_par = (char *)param;
  wxStringTokenizer tkz(op_par, " ");
  wxString order = tkz.GetNextToken();
  long so;
  if (order.ToLong(&so))
  {
    m_ScalarOrder = so;
  }
  else
  {
    t = order;
    if (t.FindFirst("*.") != -1)
    {
      FillFileList(t.GetCStr());
    }
    else
    {
      m_Files.insert(m_Files.end(), t.GetCStr());
    }
  }

  while (tkz.HasMoreTokens())
  {
    t = tkz.GetNextToken();
    if (t.FindFirst("*.") != -1)
    {
      FillFileList(t.GetCStr());
      break;
    }
    else
    {
      m_Files.insert(m_Files.end(), t.GetCStr());
    }
  }
  m_ReadyToExecute = true;
}
//----------------------------------------------------------------------------
void albaOpImporterASCII::FillFileList(const char *file_pattern)
//----------------------------------------------------------------------------
{
  wxArrayString files;
  m_Files.clear();
  wxDir::GetAllFiles(wxGetCwd(), &files, file_pattern);
  for (int f = 0; f < files.GetCount(); f++)
  {
    m_Files.insert(m_Files.end(), files[f].ToAscii());
  }
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum ASCII_IMPORTER_ID
{
  ID_ASCII_FILE = MINID,
  ID_ASCII_DATA_ORDER,
};
//----------------------------------------------------------------------------
void albaOpImporterASCII::OpRun()
//----------------------------------------------------------------------------
{
  wxString scalar_order[2] = {_("row"), _("columns")};
  
  m_Gui = new albaGUI(this);
  m_Gui->Combo(ID_ASCII_DATA_ORDER,_("order"),&m_ScalarOrder,2,scalar_order,_("Select the order of how are stored the scalars."));
  m_Gui->Button(ID_ASCII_FILE,_("ASCII data"),"",_("Choose single or multiple file ASCII."));
  m_Gui->OkCancel();
  
  if (m_ReadyToExecute)
  {
    int res = OP_RUN_CANCEL;
    if (ImportASCII() == ALBA_OK)
    {
      m_Output = m_ScalarData;
      res = OP_RUN_OK;
    }
    albaEventMacro(albaEvent(this,res));
    return;
  }

	m_Gui->Divider();
  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpImporterASCII::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_ASCII_FILE:
      {
        wxString wildc = "(*.*)|*.*";
        albaGetOpenMultiFiles(m_FileDir.GetCStr(),wildc.ToAscii(),m_Files);
      }
      break;
      case wxOK:
        if (ImportASCII() == ALBA_OK)
        {
          m_Output = m_ScalarData;
          OpStop(OP_RUN_OK);
        }
        else
        {
          albaMessage(_("Some errors occurs during the import phase! Data can not be imported."), _("Warning"));
          OpStop(OP_RUN_CANCEL);
        }
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
    }
  }
}
//----------------------------------------------------------------------------
int albaOpImporterASCII::ImportASCII()
//----------------------------------------------------------------------------
{
  albaGUIBusyInfo wait(_("Loading file/s: ..."),m_TestMode);
  albaNEW(m_ScalarData);
  m_ScalarData->SetName("scalar");

  int import_result = ALBA_ERROR;

  if (m_Files.size() > 1)
  {
    std::sort(m_Files.begin(),m_Files.end());
  }

  albaASCIIImporterUtility utility;
  for (int t=0; t<m_Files.size(); t++)
  {
    if(utility.ReadFile(m_Files[t].ToAscii()) == ALBA_OK)
    {
      import_result = ALBA_OK;
      m_ScalarData->SetData(utility.GetMatrix(),t);
    }
    else
    {
      import_result = ALBA_ERROR;
    }
    //Add the scalar array to the albaVMEScalarMatrix at the current time 't'.
  }

  m_ScalarData->SetScalarArrayOrientation(m_ScalarOrder);

  return import_result;
}
//----------------------------------------------------------------------------
void albaOpImporterASCII::SetFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.clear();
  AddFileName(file);
}
//----------------------------------------------------------------------------
void albaOpImporterASCII::AddFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.push_back(file);
}
