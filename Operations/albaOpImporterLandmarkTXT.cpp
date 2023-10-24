/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkTXT
 Authors: Roberto Mucci
 
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

#include "albaOpImporterLandmarkTXT.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>



#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaTagArray.h"
#include "albaSmartPointer.h"

#include <iostream>
#include <fstream>
#include "wx/filename.h"
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
albaOpImporterLandmarkTXT::albaOpImporterLandmarkTXT(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = albaGetLastUserFolder();
	
	m_VmeCloud		= NULL;
  m_Start = 1;
}
//----------------------------------------------------------------------------
albaOpImporterLandmarkTXT::~albaOpImporterLandmarkTXT()
//----------------------------------------------------------------------------
{
  albaDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterLandmarkTXT::Copy()   
//----------------------------------------------------------------------------
{
	albaOpImporterLandmarkTXT *cp = new albaOpImporterLandmarkTXT(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}

//----------------------------------------------------------------------------
void albaOpImporterLandmarkTXT::OpRun()   
//----------------------------------------------------------------------------
{

	int result = OP_RUN_CANCEL;
	m_File = "";
	wxString pgd_wildc	= "Landmark (*.*)|*.*";
  wxString f;
  if (!m_TestMode)
  {
    f = albaGetOpenFile(m_FileDir,pgd_wildc).char_str(); 
  }
	
	if(!f.IsEmpty() && wxFileExists(f))
	{
	  m_File = f;
    
    if (!m_TestMode)
    {
      m_Gui = new albaGUI(this);
      m_Gui->Integer(ID_TYPE_FILE,"Skip Col",&m_Start,0,MAXINT,"Number of column to skip");
      m_Gui->OkCancel();
	    m_Gui->Update();
      ShowGui();
    }
	}
  else
  {
    albaEventMacro(albaEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void albaOpImporterLandmarkTXT::	OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case wxOK:
        Read();
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      case ID_TYPE_FILE:
      break;
      default:
        albaEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void albaOpImporterLandmarkTXT::SetSkipColumn(int column)
//----------------------------------------------------------------------------
{
  m_Start = column;
}
//----------------------------------------------------------------------------
void albaOpImporterLandmarkTXT::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Please wait, working...");
  }
  albaNEW(m_VmeCloud);

  wxString path, name, ext;
  wxFileName::SplitPath(m_File.char_str(),&path,&name,&ext);
  m_VmeCloud->SetName(name);

  albaTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_VmeCloud->GetTagArray()->SetTag(tag_Nature);

  if (m_TestMode == true)
  {
    m_VmeCloud->TestModeOn();
  }

  m_VmeCloud->SetRadius(10);

  wxString skipc;
  albaString time, x, y, z;
  double xval, yval, zval, tval;
  
  std::vector<int> lm_idx;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;
  line = text.ReadLine(); //Ignore textual information
  line = text.ReadLine();
  line.Replace(" ","\t");

  wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
  int numland = (tkz.CountTokens()-1- m_Start)/3;
  albaString lm_name;
  for (int i=0;i<numland;i++)
  {
    lm_name = "lm_" + albaString(i);
    lm_idx.push_back(m_VmeCloud->AppendLandmark(lm_name));
  }

  do 
  {
    wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
    for (int c=0;c<m_Start;c++)
    {
      skipc=tkz.GetNextToken();
    }
    time = tkz.GetNextToken();
    
    long counter = 0;

    while (tkz.HasMoreTokens())
    {
      x = tkz.GetNextToken();
      y = tkz.GetNextToken();
      z = tkz.GetNextToken();
      xval = atof(x);
      yval = atof(y);
      zval = atof(z);
      tval = atof(time);

      if(x.IsEmpty() && y.IsEmpty() && z.IsEmpty() )
      {
        m_VmeCloud->SetLandmark(lm_idx[counter],0,0,0,tval);
        m_VmeCloud->SetLandmarkVisibility(lm_idx[counter], 0,tval);
      }
      else
      {
        m_VmeCloud->SetLandmark(lm_idx[counter],xval,yval,zval,tval);
      }
      counter++;
    }
    line = text.ReadLine();
    line.Replace(" ","\t");
  } while (!inputFile.Eof());

  m_VmeCloud->Modified();

  m_Output = m_VmeCloud;
}
