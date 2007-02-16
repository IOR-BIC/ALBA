/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkImporterTXT.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-16 13:36:36 $
  Version:   $Revision: 1.1 $
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

#include "mmoLandmarkImporterTXT.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>



#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <iostream>
#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum ID_LANDMARK_IMPORTER
{
  ID_TYPE_FILE = MINID,
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoLandmarkImporterTXT::mmoLandmarkImporterTXT(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	
	m_VmeCloud		= NULL;
  m_Start = 0;
}
//----------------------------------------------------------------------------
mmoLandmarkImporterTXT::~mmoLandmarkImporterTXT( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* mmoLandmarkImporterTXT::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	mmoLandmarkImporterTXT *cp = new mmoLandmarkImporterTXT(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterTXT::OpRun()   
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
      m_Gui->Integer(ID_TYPE_FILE,"Skip Col",&m_Start,0,MAXINT,"Number of column to skip");
      m_Gui->OkCancel();
	    m_Gui->Update();
      ShowGui();
    }
	}

}
//----------------------------------------------------------------------------
void mmoLandmarkImporterTXT::	OnEvent(mafEventBase *maf_event) 
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
        break;
      default:
        mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterTXT::OpDo()   
//----------------------------------------------------------------------------
{
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");

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
void mmoLandmarkImporterTXT::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
/*
//----------------------------------------------------------------------------
void mmoLandmarkImporterTXT::OpUndo()   

//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	//m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
	m_Vme = NULL;
}
*/
//----------------------------------------------------------------------------
void mmoLandmarkImporterTXT::Read()   
//----------------------------------------------------------------------------

{
	mafNEW(m_VmeCloud);
  m_VmeCloud->Open();
  m_VmeCloud->SetRadius(10);

  char name[20];
  wxString time;
  wxString skipc;
  wxString x;
  wxString y;
  wxString z;
  int index;
  
  std::vector<int> lm_idx;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;
  line=text.ReadLine(); //Ignore textual information
  line=text.ReadLine();
  line.Replace(" ","\t");

  wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
  int numland = (tkz.CountTokens()-1- m_Start)/3;
  mafString lm_name;
  for (int i=0;i<numland;i++)
  {
    lm_name = "lm_" + mafString(i);
    lm_idx.push_back(m_VmeCloud->AppendLandmark(lm_name));
  }

  do 
  {
    wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
    for (int c=0;c<m_Start;c++)
    {
      skipc=tkz.GetNextToken();
    }
    time=tkz.GetNextToken();
    
    long counter = 0;

    while (tkz.HasMoreTokens())
    {
      x=tkz.GetNextToken();
      y=tkz.GetNextToken();
      z=tkz.GetNextToken();

      if(x=="" && y=="" && z=="" )
      {
        m_VmeCloud->SetLandmark(lm_idx[counter],0,0,0,atof(time));
        m_VmeCloud->SetLandmarkVisibility(lm_idx[counter], 0, atof(time));
      }
      else
      {
        m_VmeCloud->SetLandmark(lm_idx[counter], atof(x), atof(y), atof(z), atof(time));
      }
      counter++;
    }
    line=text.ReadLine();
    line.Replace(" ","\t");
  } while (!inputFile.Eof());

  //MIO
  m_VmeCloud->Close();
  m_VmeCloud->Modified();

  m_Output = m_VmeCloud;
}
