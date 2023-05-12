/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterAnalogWS
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
#include "albaOpImporterAnalogWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include "albaGUI.h"

#include "albaTagArray.h"
#include "albaVMEAnalog.h"

#include <iostream>
#include "albaProgressBarHelper.h"

//----------------------------------------------------------------------------
albaOpImporterAnalogWS::albaOpImporterAnalogWS(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = albaGetLastUserFolder().c_str();

  m_EmgScalar = NULL;
}
//----------------------------------------------------------------------------
albaOpImporterAnalogWS::~albaOpImporterAnalogWS()
//----------------------------------------------------------------------------
{
  albaDEL(m_EmgScalar);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpImporterAnalogWS *cp = new albaOpImporterAnalogWS(m_Label);
	cp->m_File = m_File;
  cp->m_FileDir = m_FileDir;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	wxString pgd_wildc	= "EMG File (*.*)|*.*";
  wxString f;
  f = albaGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
	if(!f.IsEmpty() && wxFileExists(f))
	{
	  m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpImporterAnalogWS::Read()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
  
  albaNEW(m_EmgScalar);
  wxString path, name, ext;
  wxFileName::SplitPath(m_File.c_str(),&path,&name,&ext);
  m_EmgScalar->SetName(name);

  albaTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_EmgScalar->GetTagArray()->SetTag(tag_Nature);

  albaString time, scalar;
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  double emg_time; 
  double val_scalar;
  wxString line;
  std::vector<albaString> stringVec;

  //check if file starts with the string "ANALOG"
  line = text.ReadLine(); 
  if (line.CompareTo("ANALOG")!= 0)
  {
    albaErrorMessage("Invalid file format!");
    return;
  }
 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  freq_val = atof(freq.c_str());

  //Put the signals names in a vector of string
  int num_tk;
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName.CountTokens();

  tkzName.GetNextToken(); //To skip ","
  while (tkzName.HasMoreTokens())
  {
    stringVec.push_back(tkzName.GetNextToken()); 
  }

  line = text.ReadLine();

  int space;
  unsigned i;
  unsigned c = 0;
  int rowNumber = 0;
  wxString frame;

  do  
  {
    line = text.ReadLine();
    rowNumber++;
  } while (!inputFile.Eof());

  vnl_matrix<double> emgMatrix;
  emgMatrix.set_size(rowNumber , num_tk);

  wxFileInputStream inputFile1( m_File );
  wxTextInputStream text1( inputFile1 );
  
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line.Replace(","," ");
    
  for (int n = 0; n < rowNumber; n++)
  {
    i = 0;
    space = line.Find(' ');
    frame = line.SubString(0,space - 1);
    emg_time = atof(frame)/freq_val; 
    emgMatrix.put(n,i, emg_time); //Add scalar value to the vnl_matrix

    wxStringTokenizer tkz(line,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
    tkz.GetNextToken(); //To skip the time value

    while (tkz.HasMoreTokens())
    {
      i++;
      scalar = tkz.GetNextToken();
      val_scalar = atof(scalar);
    
      emgMatrix.put(n,i,val_scalar); //Add scalar value to the vnl_matrix 
    }
    line = text1.ReadLine();
    line.Replace(","," ");
    progressHelper.UpdateProgressBar(((double) n)/((double) rowNumber-1)*100.);
    
  } 

  vnl_matrix<double> emgMatrixTranspose = emgMatrix.transpose();

  m_EmgScalar->SetData(emgMatrixTranspose, 0);

  albaTagItem tag_Sig;
  tag_Sig.SetName("SIGNALS_NAME");
  tag_Sig.SetNumberOfComponents(num_tk - 1);
  m_EmgScalar->GetTagArray()->SetTag(tag_Sig);

  albaTagItem *tag_Signals = m_EmgScalar->GetTagArray()->GetTag("SIGNALS_NAME");
  for (int n = 0; n < stringVec.size(); n++)
  {
    tag_Signals->SetValue(stringVec[n], n);
  }

  m_Output = m_EmgScalar;
  m_Output->ReparentTo(m_Input);
}
