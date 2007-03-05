/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkImporterWS.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-05 16:42:49 $
  Version:   $Revision: 1.2 $
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

#include "mmoLandmarkImporterWS.h"
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
mmoLandmarkImporterWS::mmoLandmarkImporterWS(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	
	m_VmeCloud		= NULL;
  m_Start = 0;
}
//----------------------------------------------------------------------------
mmoLandmarkImporterWS::~mmoLandmarkImporterWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* mmoLandmarkImporterWS::Copy()   
//----------------------------------------------------------------------------
{
	mmoLandmarkImporterWS *cp = new mmoLandmarkImporterWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	wxString pgd_wildc	= "Landmark (*.*)|*.*";
  wxString f;
  if (!m_TestMode)
    {
      f = mafGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
    }
	if(!f.IsEmpty() && wxFileExists(f))
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
  else
  {
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterWS::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterWS::SetSkipColumn(int column)
//----------------------------------------------------------------------------
{
  m_Start = column;
}
//----------------------------------------------------------------------------
void mmoLandmarkImporterWS::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Please wait, working...");
  }
  mafNEW(m_VmeCloud);
  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_VmeCloud->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_VmeCloud->GetTagArray()->SetTag(tag_Nature);
  m_VmeCloud->Open();
  m_VmeCloud->SetRadius(10);

  wxString skipc;
  mafString time, first_time, x, y, z;
  double xval, yval, zval, tval;
  
  std::vector<int> lm_idx;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;
  line = text.ReadLine(); //Ignore 4 lines of textual information
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1);
  double freq_val;
  freq_val = atof(freq.c_str());
  
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  //line.Replace(" ","\t");

  wxStringTokenizer tkz_numAL(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  int numland = (tkz_numAL.CountTokens()-1- m_Start)/3;
  mafString lm_name;
  for (int i=0;i<numland;i++)
  {
    lm_name = "lm_" + mafString(i);
    lm_idx.push_back(m_VmeCloud->AppendLandmark(lm_name));
  }

  for (int c=0;c<m_Start;c++)
  {
    skipc=tkz_numAL.GetNextToken();
  }

  do 
  {
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    time = tkz.GetNextToken().c_str();

    long counter = 0;

    while (tkz.HasMoreTokens())
    {
      x = tkz.GetNextToken().c_str();
      y = tkz.GetNextToken().c_str();
      z = tkz.GetNextToken().c_str();
      xval = atof(x);
      yval = atof(y);
      zval = atof(z);
      tval = atof(time)/freq_val;

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
    

    for (int c=0;c<m_Start;c++)
    {
      skipc=tkz.GetNextToken();
    }

  } while (!inputFile.Eof());

  m_VmeCloud->Modified();

  m_Output = m_VmeCloud;
}
