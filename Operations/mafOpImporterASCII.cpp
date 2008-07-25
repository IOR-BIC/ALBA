/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterASCII.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

#include "mafOpImporterASCII.h"
#include <wx/busyinfo.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>

#include "mafEvent.h"
#include "mafGUI.h"

#include "mafASCIIImporterUtility.h"
#include "mafVME.h"
#include "mafVMEScalarMatrix.h"

#include "mafTagArray.h"

#include <algorithm>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterASCII);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterASCII::mafOpImporterASCII(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_Files.clear();

  m_ScalarOrder = 1;

  m_FileDir = "";//mafGetApplicationDirectory().c_str();
  m_ScalarData = NULL;
  m_ReadyToExecute = false;
}
//----------------------------------------------------------------------------
mafOpImporterASCII::~mafOpImporterASCII()
//----------------------------------------------------------------------------
{
  mafDEL(m_ScalarData);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterASCII::Copy()
//----------------------------------------------------------------------------
{
  mafOpImporterASCII *cp = new mafOpImporterASCII(m_Label);
  cp->m_Files	= m_Files;
  cp->m_FileDir = m_FileDir;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterASCII::SetParameters(void *param)
//----------------------------------------------------------------------------
{
  mafString t = "";
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
    t = order.c_str();
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
    t = tkz.GetNextToken().c_str();
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
void mafOpImporterASCII::FillFileList(const char *file_pattern)
//----------------------------------------------------------------------------
{
  wxArrayString files;
  m_Files.clear();
  wxDir::GetAllFiles(wxGetWorkingDirectory(), &files, file_pattern);
  for (int f = 0; f < files.GetCount(); f++)
  {
    m_Files.insert(m_Files.end(), files[f].c_str());
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
void mafOpImporterASCII::OpRun()
//----------------------------------------------------------------------------
{
  wxString scalar_order[2] = {_("row"), _("columns")};
  
  m_Gui = new mafGUI(this);
  m_Gui->Combo(ID_ASCII_DATA_ORDER,_("order"),&m_ScalarOrder,2,scalar_order,_("Select the order of how are stored the scalars."));
  m_Gui->Button(ID_ASCII_FILE,_("ASCII data"),"",_("Choose single or multiple file ASCII."));
  m_Gui->OkCancel();
  
  if (m_ReadyToExecute)
  {
    int res = OP_RUN_CANCEL;
    if (ImportASCII() == MAF_OK)
    {
      m_Output = m_ScalarData;
      res = OP_RUN_OK;
    }
    mafEventMacro(mafEvent(this,res));
    return;
  }

	m_Gui->Divider();
  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpImporterASCII::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_ASCII_FILE:
      {
        wxString wildc = "(*.*)|*.*";
        mafGetOpenMultiFiles(m_FileDir.GetCStr(),wildc.c_str(),m_Files);
      }
      break;
      case wxOK:
        if (ImportASCII() == MAF_OK)
        {
          m_Output = m_ScalarData;
          OpStop(OP_RUN_OK);
        }
        else
        {
          mafMessage(_("Some errors occurs during the import phase! Data can not be imported."), _("Warning"));
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
int mafOpImporterASCII::ImportASCII()
//----------------------------------------------------------------------------
{
  if(!this->m_TestMode)
	  wxBusyInfo wait(_("Loading file/s: ..."));
  mafNEW(m_ScalarData);
  m_ScalarData->SetName("scalar");

  int import_result = MAF_ERROR;

  if (m_Files.size() > 1)
  {
    std::sort(m_Files.begin(),m_Files.end());
  }

  mafASCIIImporterUtility utility;
  for (int t=0; t<m_Files.size(); t++)
  {
    if(utility.ReadFile(m_Files[t].c_str()) == MAF_OK)
    {
      import_result = MAF_OK;
      m_ScalarData->SetData(utility.GetMatrix(),t);
    }
    else
    {
      import_result = MAF_ERROR;
    }
    //Add the scalar array to the mafVMEScalarMatrix at the current time 't'.
  }

  m_ScalarData->SetScalarArrayOrientation(m_ScalarOrder);

  return import_result;
}
//----------------------------------------------------------------------------
void mafOpImporterASCII::SetFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.clear();
  AddFileName(file);
}
//----------------------------------------------------------------------------
void mafOpImporterASCII::AddFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.push_back(file);
}
