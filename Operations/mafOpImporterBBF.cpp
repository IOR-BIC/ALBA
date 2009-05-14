/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterBBF.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-14 15:04:48 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Hui Wei
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

#include "mafOpImporterBBF.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMEGeneric.h"

#include "mafTagArray.h"
//#include "vtkDataSet.h"
#include "../BES_Beta/IO/mafVolumeLargeReader.h"
#include "mafVMEVolumeLarge.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterBBF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterBBF::mafOpImporterBBF(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";
  m_VmeLarge = NULL;
  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterBBF::~mafOpImporterBBF()
//----------------------------------------------------------------------------
{  
  mafDEL(m_VmeLarge);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterBBF::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterBBF *cp = new mafOpImporterBBF(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterBBF::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "bbf Data (*.bbf)|*.bbf";
  mafString f;
  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir, wildc, _("Choose .bbf file")).c_str();
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    if (ImportBBF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        mafMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int mafOpImporterBBF::ImportBBF()
//----------------------------------------------------------------------------
{  
	if(!this->m_TestMode)
		wxBusyInfo wait(_("Loading file: ..."));
  
  size_t idx1 = m_File.find_last_of("_");
  size_t idx2 = m_File.find_last_of(".");
  size_t idx3 = m_File.find_last_of("\\");

  wxString nFileName = m_File.Mid(0,idx1)+m_File.Mid(idx2);
  size_t idx4 = nFileName.find_last_of(".");
  wxString showName = m_File.Mid(idx3+1,idx4-idx3-1);
  mafVolumeLargeReader *reader = mafVolumeLargeReader::New();
  reader->SetFileName(nFileName);
  reader->Update();
	
	mafNEW(m_VmeLarge); 
  m_VmeLarge->SetFileName("");
  if (m_VmeLarge->SetLargeData(reader) == MAF_OK)
  {
	  m_Output = m_VmeLarge;
    m_Output->ReparentTo(m_Input);
    m_Output->SetName(showName);
    return MAF_OK;
  }
  else
  {
    if(!this->m_TestMode) {
      mafMessage(_("Error reading VTK file."), 
        _("I/O Error"), wxICON_ERROR );
    }
    return MAF_ERROR;
  }  
}
