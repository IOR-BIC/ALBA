/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoASCIIImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:29:00 $
  Version:   $Revision: 1.1 $
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

#include "mafTagArray.h"

//----------------------------------------------------------------------------
mmoASCIIImporter::mmoASCIIImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_Files.clear();
  m_DataOrder = DATA_ORDER_ROW;

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoASCIIImporter::~mmoASCIIImporter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoASCIIImporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoASCIIImporter *cp = new mmoASCIIImporter(m_Label);
  cp->m_Files			= m_Files;
  return cp;
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum ASCII_IMPORTER_ID
{
  ID_ASCII_FILE = MINID,
  ID_DATA_ORDER,
};
//----------------------------------------------------------------------------
void mmoASCIIImporter::OpRun()   
//----------------------------------------------------------------------------
{
  wxString order_choices[3] = {"row","column","matrix"};

  m_Gui = new mmgGui(this);
  m_Gui->Button(ID_ASCII_FILE,"ASCII data","","Choose single or multiple file ASCII.");
  m_Gui->Radio(ID_DATA_ORDER,"data order",&m_DataOrder,3,order_choices);
  m_Gui->OkCancel();
  m_Gui->Enable(ID_DATA_ORDER,m_Files.size()>0);
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
        mafGetOpenMultiFiles("",wildc.c_str(),m_Files);
        m_Gui->Enable(ID_DATA_ORDER,m_Files.size()>0);
      }
      break;
      case wxOK:
        ImportASCII();
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mmoASCIIImporter::ImportASCII()
//----------------------------------------------------------------------------
{
  bool success = false;
	wxBusyInfo wait("Loading file: ...");
  
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
