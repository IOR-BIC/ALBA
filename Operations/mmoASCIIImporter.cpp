/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoASCIIImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-12 10:02:22 $
  Version:   $Revision: 1.5 $
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

#include "mmoASCIIImporter.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafASCIIImporterUtility.h"
#include "mafVME.h"
#include "mafVMEScalar.h"

#include "mafTagArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoASCIIImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoASCIIImporter::mmoASCIIImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_Files.clear();

  m_FileDir = mafGetApplicationDirectory().c_str();
  m_ScalarData = NULL;
}
//----------------------------------------------------------------------------
mmoASCIIImporter::~mmoASCIIImporter()
//----------------------------------------------------------------------------
{
  mafDEL(m_ScalarData);
}
//----------------------------------------------------------------------------
mafOp* mmoASCIIImporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoASCIIImporter *cp = new mmoASCIIImporter(m_Label);
  cp->m_Files	= m_Files;
  cp->m_FileDir = m_FileDir;
  return cp;
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
void mmoASCIIImporter::OpRun()
//----------------------------------------------------------------------------
{
  wxString scalar_order[2] = {"row", "columns"};
  
  m_Gui = new mmgGui(this);
  m_Gui->Button(ID_ASCII_FILE,"ASCII data","","Choose single or multiple file ASCII.");
  m_Gui->Combo(ID_ASCII_DATA_ORDER,"order",&m_ScalarOrder,2,scalar_order,"Select the order of how are stored the scalars.");
  m_Gui->OkCancel();
  
  ShowGui();
}
//----------------------------------------------------------------------------
void mmoASCIIImporter::OnEvent(mafEventBase *maf_event)
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
          wxMessageBox("Some errors occurs during the import phase! Data can not be imported.", "Warning");
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
int mmoASCIIImporter::ImportASCII()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Loading file/s: ...");
  mafNEW(m_ScalarData);
  m_ScalarData->SetName("scalar");

  int import_result = MAF_ERROR;

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
    //Add the scalar array to the mafVMEScalar at the current time 't'.
  }

  mafTagItem item;
  item.SetName("SCALAR_ORDER");
  item.SetValue(m_ScalarOrder);
  m_ScalarData->GetTagArray()->SetTag(item);

  return import_result;
}
//----------------------------------------------------------------------------
void mmoASCIIImporter::SetFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.clear();
  AddFileName(file);
}
//----------------------------------------------------------------------------
void mmoASCIIImporter::AddFileName(std::string &file)
//----------------------------------------------------------------------------
{
  m_Files.push_back(file);
}
