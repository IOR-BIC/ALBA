/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterLandmark.cpp,v $
  Language:  C++
  Date:      $Date: 2010-07-28 13:35:58 $
  Version:   $Revision: 1.7.2.2 $
  Authors:   Daniele Giunchi, Simone Brazzale
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

#include "medOpImporterLandmark.h"
#include <wx/busyinfo.h>


#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <fstream>

const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
medOpImporterLandmark::medOpImporterLandmark(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  m_TypeSeparation = 0;
	
	m_VmeCloud		= NULL;
	m_TagFileFlag = false;
}
//----------------------------------------------------------------------------
medOpImporterLandmark::~medOpImporterLandmark( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterLandmark::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	medOpImporterLandmark *cp = new medOpImporterLandmark(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
  cp->m_TypeSeparation = m_TypeSeparation;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
  
	int result = OP_RUN_CANCEL;
	m_File = "";
	wxString pgd_wildc	= "Landmark (*.*)|*.*";
	
  
	mafString f = mafGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
	if(f != "")
	{
	  m_File = f;

    if (!m_TestMode)
    {
      m_Gui = new mafGUI(this);
      wxString choices[2] =  {_("Space"),_("Comma")};
      m_Gui->Radio(ID_TYPE_SEPARATION,"Separation",&m_TypeSeparation,2,choices,1,"");      
      m_Gui->Divider();
      m_Gui->Bool(ID_TYPE_FILE,"Tagged file",&m_TagFileFlag,0,"Check if the format is NAME x y z");
      m_Gui->Divider();
      m_Gui->OkCancel();
      m_Gui->Enable(ID_TYPE_SEPARATION,!m_TagFileFlag);
	    m_Gui->Update();
      ShowGui();
    }
	}
  else
  {
    mafEventMacro(mafEvent(this,result));
  }

}
//----------------------------------------------------------------------------
void medOpImporterLandmark::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      case ID_TYPE_FILE:
        {  
          if (m_TagFileFlag)
          {
            m_TypeSeparation = 0;
          }
          if (!m_TestMode)
          {
            m_Gui->Enable(ID_TYPE_SEPARATION,!m_TagFileFlag);
	          m_Gui->Update();
          }
        }
      default:
        mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpDo()   
//----------------------------------------------------------------------------
{
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");

  if(m_TagFileFlag == false)
	  ReadWithoutTag();
  else
    Read();

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_VmeCloud->SetName(name);
	
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_VmeCloud->GetTagArray()->SetTag(tag_Nature); //m_Vme->GetTagArray()->AddTag(tag_Nature);
  
	mafEventMacro(mafEvent(this,VME_ADD,m_VmeCloud));
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
/*
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpUndo()   

//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	//m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
	m_Vme = NULL;
}
*/
//----------------------------------------------------------------------------
void medOpImporterLandmark::Read()   
//----------------------------------------------------------------------------
{
  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(m_File);
  int numberOfLines = 0;
  char tmp[200];
  while(!landmarkNumberPromptFileStream.fail())
  {
    landmarkNumberPromptFileStream.getline(tmp,200);
    numberOfLines += 1;
  }
  landmarkNumberPromptFileStream.close();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Reading " << numberOfLines << " lines in landmark file" << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

	mafNEW(m_VmeCloud);
  m_VmeCloud->Open();

  std::ifstream  landmarkFileStream(m_File);
  char name[20];
  char time[6] = "0";
  char tx[20];
  char ty[20];
  char tz[20];

  double x = 0;
  double y = 0;
  double z = 0;
  int t = 0;

  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading landmark cloud");
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  long counter = 0;
  long progress = 0;

  while(!landmarkFileStream.fail())
  {
    landmarkFileStream >> name;
    if(name[0] == '#' || mafString(name) == "") 
    {
      //jump the comment or the blank line
      landmarkFileStream.getline(name,20);
      continue;
    }
    else if(mafString(name) == "Time")
    {
      landmarkFileStream >> time;
      counter = 0;
      continue;
    }
    else
    {
      landmarkFileStream >> tx;
      landmarkFileStream >> ty;
      landmarkFileStream >> tz;
      x = atof(tx);
      y = atof(ty);
      z = atof(tz);
      t = atoi(time);
      if(mafString(time) == "0")
        m_VmeCloud->AppendLandmark(name, false);
      m_VmeCloud->SetLandmark(counter,x,y,z,t);
      if(x == -9999 && y == -9999 && z == -9999 )
        m_VmeCloud->SetLandmarkVisibility(counter, 0, t);
      counter++;

      progress = counter * 100 / numberOfLines;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE, progress));

    }
  }
  m_VmeCloud->Close();
  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);

  landmarkFileStream.close();

  m_Output = m_VmeCloud;

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::ReadWithoutTag()   
//----------------------------------------------------------------------------
{
 	mafNEW(m_VmeCloud);
  m_VmeCloud->Open();

  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(m_File);
  int numberOfLines = 0;
  char tmp[200];
  while(!landmarkNumberPromptFileStream.fail())
  {
    landmarkNumberPromptFileStream.getline(tmp,200);
    numberOfLines += 1;
  }
  landmarkNumberPromptFileStream.close();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Reading " << numberOfLines - 1  << " landmarks" << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  // end  number of lm check

  std::ifstream  landmarkFileStream(m_File);

  double x = 0;
  double y = 0;
  double z = 0;

  char tx[20];
  char ty[20];
  char tz[20];
  
  long counter = 0; 
  long progress = 0;

  bool exception = FALSE;
  const wxChar comma = ',';

  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading landmark cloud");
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  while(!landmarkFileStream.fail())
  {
    switch (m_TypeSeparation)
    {
    case 0:
      { 
        landmarkFileStream >> tx;    
        if(mafString(tx) == "") 
        {
          //jump the the blank line
          landmarkFileStream.getline(tx,20);
          continue;
        }
        landmarkFileStream >> ty;
        landmarkFileStream >> tz;

        try {
          x = atof(tx);
          // workaround to see if the input format was wrong
          if (x<-10000 || x>10000)
            throw "Input file invalid!";
        }
        catch (char * err) {
          wxMessageBox(err,_("Warning"),wxOK | wxICON_ERROR);
          exception = TRUE;
          break;
        }
        y = atof(ty);
        z = atof(tz);
      }
      break;
    case 1:
      {
        landmarkFileStream.getline(tmp,200);
        if(mafString(tmp) == "") 
        {
          // jump the blank line
          landmarkFileStream.getline(tmp,200);
          continue;
        }
        wxString s = wxString(tmp);
        wxString t1 = s.BeforeFirst(comma);
        wxString t2 = s.AfterFirst(comma);
        wxString t3 = t2.BeforeFirst(comma);
        wxString t4 = t2.AfterFirst(comma);

        t1.ToDouble(&x);
        t3.ToDouble(&y);
        t4.ToDouble(&z);
      }
      break;
    }
    
    if (!exception)
    {
      // todo: optimize this append
      m_VmeCloud->AppendLandmark(mafString(counter), false);
      m_VmeCloud->SetLandmark(counter,x,y,z,0);
      if(x == -9999 && y == -9999 && z == -9999 )
        m_VmeCloud->SetLandmarkVisibility(counter, 0, 0);
    }
    else
    {
      break;
    }
    
    counter++;

    progress = counter * 100 / numberOfLines;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE, progress));

  }

  m_VmeCloud->Close();
  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);

  landmarkFileStream.close();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  m_Output = m_VmeCloud;
}
