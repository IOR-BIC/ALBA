/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: albaOpImporterEmgWS
 Authors: Roberto Mucci - Paolo Quadrani
 
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
#include "albaOpImporterEmgWS.h"

#include <albaGUIBusyInfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include "wx/filename.h"

#include "albaGUI.h"

#include "albaTagArray.h"
#include "albaVMEScalar.h"
#include "albaVMEGroup.h"

#include <iostream>

//----------------------------------------------------------------------------
albaOpImporterEmgWS::albaOpImporterEmgWS(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
  m_Group = NULL;
}
//----------------------------------------------------------------------------
albaOpImporterEmgWS::~albaOpImporterEmgWS()
//----------------------------------------------------------------------------
{
  albaDEL(m_Group);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterEmgWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpImporterEmgWS *cp = new albaOpImporterEmgWS(m_Label);
	cp->m_Listener = m_Listener;
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterEmgWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	albaString pgd_wildc	= "EMG File (*.*)|*.*";
  albaString fileDir = (albaGetApplicationDirectory() + "/Data/External/");
  albaString f = albaGetOpenFile(fileDir, pgd_wildc); 
	if(!f.IsEmpty() && wxFileExists(f.GetCStr()))
	{
	  m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpImporterEmgWS::Read()
//----------------------------------------------------------------------------
{
  albaGUIBusyInfo wait("Importing data, please wait...",m_TestMode);
  
  albaNEW(m_Group);
  wxString path, name, ext;
  wxFileName::SplitPath(m_File.GetCStr(),&path,&name,&ext);
  m_Group->SetName(name);

  albaTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");
  m_Group->GetTagArray()->SetTag(tag_Nature);

  wxFileInputStream inputFile( m_File.GetCStr() );
  wxTextInputStream text( inputFile );

  wxString line;
  
  /////////////////////////////////////////////////////////////
  // Check if file starts with the string "ANALOG"
  line = text.ReadLine(); 
  if (line.CompareTo(wxString("ANALOG")) != 0)
  {
    albaErrorMessage(_("Invalid file format!"));
    return;
  }

  ////////////////// Read sampling frequency //////////////////
  line = text.ReadLine();
  int comma = line.Find(wxT(','));
  wxString freq = line.SubString(0,comma - 1); 
  double freq_val = atof(freq.ToAscii());

  ////////////////// Read signals names //////////////////
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);

  //Count number of signals (columns)
  int num_tk = tkzName.CountTokens() - 1;  
  tkzName.GetNextToken(); //To skip "Sample #"
  
  std::vector<albaVMEScalar *> vmeVec;
  while (tkzName.HasMoreTokens())
  {
    albaVMEScalar *s = albaVMEScalar::New();
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
    t = atof(t_str.ToAscii()) / freq_val;
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
