/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpApplyTrajectory
 Authors: Roberto Mucci, Simone Brazzale
 
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

#include "albaOpApplyTrajectory.h"
#include <albaGUIBusyInfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "albaDecl.h"

#include "albaGUI.h" 
#include "albaVMEGenericAbstract.h"
#include "albaTransform.h"
#include "albaMatrix.h"
#include "albaMatrixVector.h"
#include "albaSmartPointer.h"
#include "wx/filename.h"

#include <iostream>
#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpApplyTrajectory);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpApplyTrajectory::albaOpApplyTrajectory(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = (albaGetApplicationDirectory() + "/Data/");
  m_OriginalMatrix = NULL;
  m_VME = NULL;
}
//----------------------------------------------------------------------------
albaOpApplyTrajectory::~albaOpApplyTrajectory()
//----------------------------------------------------------------------------
{
  if (m_OriginalMatrix != NULL)
  {
    albaDEL(m_OriginalMatrix);
  }
}
//----------------------------------------------------------------------------
albaOp* albaOpApplyTrajectory::Copy()   
//----------------------------------------------------------------------------
{
	albaOpApplyTrajectory *cp = new albaOpApplyTrajectory(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool albaOpApplyTrajectory::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{  
  return !vme->IsAnimated() && !vme->IsA("albaVMERoot") 
    && !vme->IsA("albaVMEExternalData") && !vme->IsA("albaVMERefSys");
}
//----------------------------------------------------------------------------
bool albaOpApplyTrajectory::AcceptInputVME(albaVME* node)
//----------------------------------------------------------------------------
{
  if ( (node)->IsAnimated() && !node->IsA("albaVMERoot") 
    && !node->IsA("albaVMEExternalData") && !node->IsA("albaVMERefSys") )
  {
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::OpRun()   
//----------------------------------------------------------------------------
{
  // Create GUI
  if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::CreateGui()
//----------------------------------------------------------------------------
{
  wxString pgd_wildc	= "txt (*.txt)|*.txt";

  m_Gui = new albaGUI(this);

  m_Gui->Divider(0);
  m_Gui->Label("Load trajectories");
  m_Gui->FileOpen(ID_OPEN_FILE,"from file:", &m_File, pgd_wildc,"Choose text file");

  m_Gui->Divider(0);
  m_Gui->Divider(0);
  m_Gui->Button(ID_SELECT_VME,"Select","from VMEs:","Select VME"); 

  m_Gui->Divider(0);
  m_Gui->Divider(0);
  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::OpDo()   
//----------------------------------------------------------------------------
{
  // CASE 1: Read trajectories from a file
  if (m_VME==NULL)
  {
    wxString f = m_File;
    if(!f.IsEmpty() && wxFileExists(f))
	  {
      if (Read() != ALBA_OK)
      {
        if(!this->m_TestMode)
          albaMessage(_("Unsupported file format."), _("I/O Error"), wxICON_ERROR );
      }
	  }
    else
    {
      if(!this->m_TestMode)
        albaMessage(_("File empty or file not found."), _("I/O Error"), wxICON_ERROR );
    }
  }
  // CASE 2: Apply trajectories from a time-varying VME 
  else
  {
    if (ApplyTrajectoriesFromVME() != ALBA_OK)
    {
      if(!this->m_TestMode)
        albaMessage(_("An error occurred while applying trajectories"), _("I/O Error"), wxICON_ERROR );
    }
  }
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::OpUndo()
//----------------------------------------------------------------------------
{ 
  std::vector<albaTimeStamp> timestamps;
  albaVMEGenericAbstract *oldVme = albaVMEGenericAbstract::SafeDownCast(m_Input);

  oldVme->SetTimeStamp(m_OriginalMatrix->GetTimeStamp());

  albaMatrixVector *mv = oldVme->GetMatrixVector();
  if (mv)
  {
    mv->RemoveAllItems();
    mv->SetMatrix(m_OriginalMatrix);
  }

  oldVme->Modified();
 
	GetLogicManager()->VmeModified(m_Input);
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectory::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    
	case wxOK:
      {
        if (!m_VME && m_File.Compare("")==0)
        {
          albaMessage(_("No input has been selected."), _("I/O Error"), wxICON_ERROR );
          OpStop(OP_RUN_CANCEL);
        }
        else
        {
          OpStop(OP_RUN_OK);
        }
      }
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case ID_OPEN_FILE:
      {
        if (!m_TestMode)
        {
          if (m_File.Compare("")!=0)
          {
            m_Gui->Enable(ID_SELECT_VME,false);  
          }
        }
      }
      break;
    case ID_SELECT_VME:
      {
        albaString title = albaString("Select a VME:");
        albaEvent e(this,VME_CHOOSE);
        e.SetString(&title);
        e.SetPointer(&albaOpApplyTrajectory::AcceptInputVME); // accept only time-varying VME
        albaEventMacro(e);
        if (e.GetVme())
        {
          m_VME = e.GetVme();
        }

        if (!m_TestMode)
        {
          if (m_VME)
          {
            m_Gui->Enable(ID_OPEN_FILE,false);  
          }
        }
      }
      break;
    default:
      albaEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
int albaOpApplyTrajectory::Read()   
//----------------------------------------------------------------------------
{
	albaGUIBusyInfo wait("Working, please wait ..", m_TestMode);
  
  albaNEW(m_OriginalMatrix);
  m_OriginalMatrix->DeepCopy(m_Input->GetOutput()->GetAbsMatrix());

  wxString path, name, ext;
  wxFileName::SplitPath(m_File.GetCStr(),&path,&name,&ext);

  double time;
  double newPosition[3];
  double newOrientation[3];

  wxString s_file = m_File.GetCStr();
  wxFileInputStream inputFile( s_file );
  wxTextInputStream text( inputFile );
  
  wxString line;
  line = text.ReadLine(); //Ignore textual information
  do 
  {
    line = text.ReadLine();
    line.Replace(" ","\t");
    wxStringTokenizer tkz(line,wxT("\t"),wxTOKEN_RET_EMPTY_ALL);

    time = atof(tkz.GetNextToken());
    newPosition[0] = atof(tkz.GetNextToken());
    newPosition[1] = atof(tkz.GetNextToken());
    newPosition[2] = atof(tkz.GetNextToken());
    newOrientation[0] = atof(tkz.GetNextToken());
    newOrientation[1] = atof(tkz.GetNextToken());
    newOrientation[2] = atof(tkz.GetNextToken());
    
    wxString token = tkz.GetNextToken();
    if (!token.IsEmpty())
    {
      if(!this->m_TestMode)
        albaMessage(_("Error reading trajectory file. Incorrect number of parameter."), _("I/O Error"), wxICON_ERROR );
      return ALBA_ERROR;
    }

    albaSmartPointer<albaTransform> boxPose;
    boxPose->RotateY(newOrientation[1], POST_MULTIPLY);
    boxPose->RotateX(newOrientation[0], POST_MULTIPLY);
    boxPose->RotateZ(newOrientation[2], POST_MULTIPLY);
    boxPose->SetPosition(newPosition);

    m_Input->SetAbsMatrix(boxPose->GetMatrix(), time);

  } while (!inputFile.Eof());

  GetLogicManager()->VmeModified(m_Input);
	GetLogicManager()->CameraUpdate();

  return ALBA_OK;
}
//----------------------------------------------------------------------------
int albaOpApplyTrajectory::ApplyTrajectoriesFromVME()   
//----------------------------------------------------------------------------
{
  assert(m_VME);

	albaGUIBusyInfo wait("Please wait, working...", m_TestMode);
  
  albaNEW(m_OriginalMatrix);
  m_OriginalMatrix->DeepCopy(m_Input->GetOutput()->GetAbsMatrix());

  std::vector<albaTimeStamp> time_stamps;
  m_VME->GetTimeStamps(time_stamps);

  double time;

  for (int i=0; i<time_stamps.size(); i++)
  {
    time = time_stamps.at(i);

    albaMatrix boxPose;
    m_VME->GetOutput()->GetAbsMatrix(boxPose,time);
    m_Input->SetAbsMatrix(boxPose, time);
  }

  GetLogicManager()->VmeModified(m_Input);
	GetLogicManager()->CameraUpdate();
  return ALBA_OK;
}