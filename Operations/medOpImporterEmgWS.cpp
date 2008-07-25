/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterEmgWS.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 10:37:30 $
  Version:   $Revision: 1.3 $
  Authors:   Roberto Mucci - Paolo Quadrani
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
#include "medOpImporterEmgWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>

#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafVMEScalar.h"
#include "mafVMEGroup.h"

#include <iostream>

//----------------------------------------------------------------------------
medOpImporterEmgWS::medOpImporterEmgWS(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
  m_Group = NULL;
}
//----------------------------------------------------------------------------
medOpImporterEmgWS::~medOpImporterEmgWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterEmgWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterEmgWS *cp = new medOpImporterEmgWS(m_Label);
	cp->m_Listener = m_Listener;
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterEmgWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	mafString pgd_wildc	= "EMG File (*.*)|*.*";
  mafString fileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  mafString f = mafGetOpenFile(fileDir, pgd_wildc).c_str(); 
	if(!f.IsEmpty() && wxFileExists(f.GetCStr()))
	{
	  m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpImporterEmgWS::Read()
//----------------------------------------------------------------------------
{
  //if (!m_TestMode)
    wxBusyInfo wait("Importing data, please wait...");
  
  mafNEW(m_Group);
  wxString path, name, ext;
  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);
  m_Group->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");
  m_Group->GetTagArray()->SetTag(tag_Nature);

  wxFileInputStream inputFile( m_File.GetCStr() );
  wxTextInputStream text( inputFile );

  wxString line;
  
  /////////////////////////////////////////////////////////////
  // Check if file starts with the string "ANALOG"
  line = text.ReadLine(); 
  if (line.CompareTo("ANALOG") != 0)
  {
    mafErrorMessage(_("Invalid file format!"));
    return;
  }

  ////////////////// Read sampling frequency //////////////////
  line = text.ReadLine();
  int comma = line.Find(wxT(','));
  wxString freq = line.SubString(0,comma - 1); 
  double freq_val = atof(freq.c_str());

  ////////////////// Read signals names //////////////////
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);

  //Count number of signals (columns)
  int num_tk = tkzName.CountTokens() - 1;  
  tkzName.GetNextToken(); //To skip "Sample #"
  
  std::vector<mafVMEScalar *> vmeVec;
  while (tkzName.HasMoreTokens())
  {
    mafVMEScalar *s = mafVMEScalar::New();
    s->SetName(tkzName.GetNextToken());
    s->GetTagArray()->SetTag(tag_Nature);
    vmeVec.push_back(s);
    m_Group->AddChild(vmeVec[vmeVec.size()-1]);
  }

  ///////////////// Skip 'Units:' raw /////////////////
  line = text.ReadLine();
  
  ///////////////// Read file content and fill the VMEs /////////////////
  wxString t_str, scalar_str;
  double t, scalar_val;
  unsigned int i;
  while (!inputFile.Eof())
  {
    line = text.ReadLine();
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    /// First token is time stamp
    t_str = tkz.GetNextToken();
    t = atof(t_str.c_str()) / freq_val;
    i = 0;
    while (tkz.HasMoreTokens())
    {
      scalar_str = tkz.GetNextToken();
      scalar_val = atof(scalar_str);
      vmeVec[i++]->SetData(scalar_val, t);
    }
  }
  
  m_Output = m_Group;
}
