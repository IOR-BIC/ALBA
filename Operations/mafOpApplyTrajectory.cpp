/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpApplyTrajectory.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-19 11:52:42 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Roberto Mucci
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

#include "mafOpApplyTrajectory.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafDecl.h"

#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMEGenericAbstract.h"
#include "mafTransform.h"
#include "mafMatrixVector.h"
#include "mafSmartPointer.h"

#include <iostream>
#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpApplyTrajectory);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpApplyTrajectory::mafOpApplyTrajectory(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/").c_str();
  m_OriginalMatrix = NULL;
}
//----------------------------------------------------------------------------
mafOpApplyTrajectory::~mafOpApplyTrajectory()
//----------------------------------------------------------------------------
{
  if (m_OriginalMatrix != NULL)
  {
    mafDEL(m_OriginalMatrix);
  }
}
//----------------------------------------------------------------------------
mafOp* mafOpApplyTrajectory::Copy()   
//----------------------------------------------------------------------------
{
	mafOpApplyTrajectory *cp = new mafOpApplyTrajectory(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	return cp;
}

//----------------------------------------------------------------------------
bool mafOpApplyTrajectory::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{  
  return !((mafVME *)vme)->IsAnimated();
}

//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpRun()   
//----------------------------------------------------------------------------
{
	m_File = "";
	wxString pgd_wildc	= "txt (*.txt)|*.txt";
  wxString f;
  if (!m_TestMode)
  {
    f = mafGetOpenFile(m_FileDir,pgd_wildc, _("Choose txt file")).c_str(); 
  }
	
  int result = OP_RUN_CANCEL;
	if(!f.IsEmpty() && wxFileExists(f))
	{
	  m_File = f;

    if (Read() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        mafMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
	}
  else
  {
    if(!this->m_TestMode)
      mafMessage(_("File empty or file not found."), _("I/O Error"), wxICON_ERROR );
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpApplyTrajectory::OpUndo()
//----------------------------------------------------------------------------
{ 
  std::vector<mafTimeStamp> timestamps;
  mafVMEGenericAbstract *oldVme = mafVMEGenericAbstract::SafeDownCast(m_Input);

  oldVme->SetTimeStamp(m_OriginalMatrix->GetTimeStamp());

  mafMatrixVector *mv = oldVme->GetMatrixVector();
  if (mv)
  {
    mv->RemoveAllItems();
    mv->SetMatrix(m_OriginalMatrix);
  }

  oldVme->Modified();
  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
int mafOpApplyTrajectory::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Please wait, working...");
  }

  mafNEW(m_OriginalMatrix);
  m_OriginalMatrix->DeepCopy(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

  double time;
  double newPosition[3];
  double newOrientation[3];

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );
  
  wxString line;
  line = text.ReadLine(); //Ignore textual information
  do 
  {
    line = text.ReadLine();
    line.Replace(" ","\t");
    wxStringTokenizer tkz(line,wxT("\t"),wxTOKEN_RET_EMPTY_ALL);

    time = atof(tkz.GetNextToken().c_str());
    newPosition[0] = atof(tkz.GetNextToken().c_str());
    newPosition[1] = atof(tkz.GetNextToken().c_str());
    newPosition[2] = atof(tkz.GetNextToken().c_str());
    newOrientation[0] = atof(tkz.GetNextToken().c_str());
    newOrientation[1] = atof(tkz.GetNextToken().c_str());
    newOrientation[2] = atof(tkz.GetNextToken().c_str());
    
    wxString token = tkz.GetNextToken().c_str();
    if (!token.IsEmpty())
    {
      if(!this->m_TestMode)
        mafMessage(_("Error reading trajectory file. Incorrect number of parameter."), _("I/O Error"), wxICON_ERROR );
      return MAF_ERROR;
    }

    mafSmartPointer<mafTransform> boxPose;
    boxPose->RotateY(newOrientation[1], POST_MULTIPLY);
    boxPose->RotateX(newOrientation[0], POST_MULTIPLY);
    boxPose->RotateZ(newOrientation[2], POST_MULTIPLY);
    boxPose->SetPosition(newPosition);

    ((mafVME *)m_Input)->SetAbsMatrix(boxPose->GetMatrix(), time);

  } while (!inputFile.Eof());

  mafEventMacro(mafEvent(this, VME_MODIFIED, m_Input));
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  return MAF_OK;
}
