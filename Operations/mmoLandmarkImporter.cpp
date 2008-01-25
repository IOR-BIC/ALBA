/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-25 13:39:09 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi
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

#include "mmoLandmarkImporter.h"
#include <wx/busyinfo.h>


#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <fstream>

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum ID_LANDMARK_IMPORTER
{
  ID_TYPE_FILE = MINID,
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoLandmarkImporter::mmoLandmarkImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	
	m_VmeCloud		= NULL;
	m_TagFileFlag = false;
}
//----------------------------------------------------------------------------
mmoLandmarkImporter::~mmoLandmarkImporter( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* mmoLandmarkImporter::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	mmoLandmarkImporter *cp = new mmoLandmarkImporter(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}
//----------------------------------------------------------------------------
void mmoLandmarkImporter::OpRun()   
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
      m_Gui = new mmgGui(this);
      m_Gui->Bool(ID_TYPE_FILE,"Tagged file",&m_TagFileFlag,0,"Check if the format is NAME x y z");
      m_Gui->OkCancel();
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
void mmoLandmarkImporter::	OnEvent(mafEventBase *maf_event) 
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
      default:
        mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void mmoLandmarkImporter::OpDo()   
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
void mmoLandmarkImporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
/*
//----------------------------------------------------------------------------
void mmoLandmarkImporter::OpUndo()   

//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	//m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
	m_Vme = NULL;
}
*/
//----------------------------------------------------------------------------
void mmoLandmarkImporter::Read()   
//----------------------------------------------------------------------------
{
	mafNEW(m_VmeCloud);
  m_VmeCloud->Open();

  std::ifstream  landmarkFileStream(m_File);
  char name[20];
  char time[6] = "0";
  char x[10];
  char y[10];
  char z[10];

  long counter = 0;
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
      landmarkFileStream >> x;
      landmarkFileStream >> y;
      landmarkFileStream >> z;
      /*mafString t;
      t << name;
      t << " ";
      t << x;
      t << " ";
      t << y;
      t << " ";
      t << z;
      mafLogMessage(t);*/
      if(mafString(time) == "0")
        m_VmeCloud->AppendLandmark(name);
      m_VmeCloud->SetLandmark(counter, atof(x) ,atof(y) ,atof(z),atoi(time));
      //m_VmeCloud->GetLandmark(counter)->SetRadius(5);
      if(atof(x) == -9999 && atof(y) == -9999 && atof(z) == -9999 )
        m_VmeCloud->SetLandmarkVisibility(counter, 0, atof(time));
      counter++;
    }
  }
  m_VmeCloud->Close();
  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);

  landmarkFileStream.close();

  m_Output = m_VmeCloud;
}
//----------------------------------------------------------------------------
void mmoLandmarkImporter::ReadWithoutTag()   
//----------------------------------------------------------------------------
{
	mafNEW(m_VmeCloud);
  m_VmeCloud->Open();

  std::ifstream  landmarkFileStream(m_File);

  char x[10];
  char y[10];
  char z[10];

  long counter = 0;
  while(!landmarkFileStream.fail())
  {

    landmarkFileStream >> x;
    
    if(mafString(x) == "") 
    {
      //jump the comment or the blank line
      landmarkFileStream.getline(x,20);
      continue;
    }


    landmarkFileStream >> y;
    landmarkFileStream >> z;
    
    m_VmeCloud->AppendLandmark(mafString(counter));
    m_VmeCloud->SetLandmark(counter, atof(x) ,atof(y) ,atof(z),0);
    //m_VmeCloud->GetLandmark(counter)->SetRadius(5);
    if(atof(x) == -9999 && atof(y) == -9999 && atof(z) == -9999 )
      m_VmeCloud->SetLandmarkVisibility(counter, 0, 0);
    counter++;
  }
  m_VmeCloud->Close();
  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);

  landmarkFileStream.close();

  m_Output = m_VmeCloud;
}