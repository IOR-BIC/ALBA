/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMSF
 Authors: Paolo Quadrani
 
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

#include "mafOpExporterMSF.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEIterator.h"
#include "mafAbsLogicManager.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterMSF::mafOpExporterMSF(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_MSFFile    = "";
  m_MSFFileDir = mafGetLastUserFolder().c_str();

}
//----------------------------------------------------------------------------
mafOpExporterMSF::~mafOpExporterMSF()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExporterMSF::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc;
	const char *ext = GetLogicManager()->GetMsfFileExtension();
	
	wildc.Printf("Project File (*.%s)|*.%s", ext, ext);


	mafString f;
  if (m_MSFFile.IsEmpty())
  {
    f = mafGetSaveFile(m_MSFFileDir.GetCStr(),wildc.GetCStr()).c_str(); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.IsEmpty()) 
	{
    if (ExportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool mafOpExporterMSF::Accept(mafVME*vme)
//----------------------------------------------------------------------------
{
  return (vme != NULL) && (!vme->IsA("mafVMERoot"));
}
//----------------------------------------------------------------------------
int mafOpExporterMSF::ExportMSF()
//----------------------------------------------------------------------------
{					
  if (!m_TestMode)
  {
  	wxBusyInfo wait("Saving MSF: Please wait");
  }
  assert(m_MSFFile != "");

	if(!wxFileExists(wxString(m_MSFFile)))
	{
		wxString path, name, ext, dir2;
		wxSplitPath(m_MSFFile.GetCStr(),&path,&name,&ext);
		dir2 = path + "\\" + name;
		if(!wxDirExists(dir2))
			wxMkdir(dir2);
		m_MSFFile = dir2 + "\\" + name + "." + ext;
	}

  mafVMEStorage storage;
  storage.SetURL(m_MSFFile.GetCStr());
  storage.GetRoot()->SetName("root");
  storage.SetListener(this);
  storage.GetRoot()->Initialize();

  std::vector<idValues> values;

  mafVMEIterator *iter = m_Input->NewIterator();
  for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    idValues value;
    value.oldID = node->GetId();
    values.push_back(value);
  }
  iter->Delete();

	mafVME::CopyTree(m_Input,storage.GetRoot());

  iter = storage.GetRoot()->GetFirstChild()->NewIterator();
  int index = 0;
  for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    idValues value;
    values[index].newID = node->GetId();

    index++;

    
  }
  iter->Delete();

  std::vector<mafString> linkToEliminate;
  iter = storage.GetRoot()->GetFirstChild()->NewIterator();
  for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    linkToEliminate.clear();

    for (mafVME::mafLinksMap::iterator it=node->GetLinks()->begin();it!=node->GetLinks()->end();it++)
    {
      bool foundID = false;
      for (int i=0;i<values.size();i++)
      {
        int id = it->second.m_NodeId;
        if (it->second.m_NodeId == values[i].oldID)
        {
          it->second.m_NodeId = values[i].newID;
          foundID = true;
        }
      }

      if (!foundID)
      {
        linkToEliminate.push_back(it->first.GetCStr());
      }
    }

    for (int i=0;i<linkToEliminate.size();i++)
    {
      node->RemoveLink(linkToEliminate[i].GetCStr());
    }
  }
  iter->Delete();

  storage.GetRoot()->GetFirstChild()->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004
  if (storage.Store()!=0)
  {
    if (!m_TestMode)
    {
    	wxMessageBox("Error while exporting MSF");
    }
    return MAF_ERROR;
  }

  return MAF_OK;
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterMSF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterMSF *cp= new mafOpExporterMSF(m_Label);
  cp->m_Listener    = m_Listener;
  cp->m_MSFFile     = m_MSFFile;
  cp->m_MSFFileDir  = m_MSFFileDir;
  return cp;
}
