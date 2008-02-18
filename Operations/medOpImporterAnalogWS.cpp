/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterAnalogWS.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-18 15:14:59 $
  Version:   $Revision: 1.4 $
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
#include "medOpImporterAnalogWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include "mmgGui.h"

#include "mafTagArray.h"
#include "medVMEAnalog.h"

#include <iostream>

//----------------------------------------------------------------------------
medOpImporterAnalogWS::medOpImporterAnalogWS(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();

  m_EmgScalar = NULL;
}
//----------------------------------------------------------------------------
medOpImporterAnalogWS::~medOpImporterAnalogWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_EmgScalar);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterAnalogWS *cp = new medOpImporterAnalogWS(m_Label);
	cp->m_File = m_File;
  cp->m_FileDir = m_FileDir;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	wxString pgd_wildc	= "EMG File (*.*)|*.*";
  wxString f;
  f = mafGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
	if(!f.IsEmpty() && wxFileExists(f))
	{
	  m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpImporterAnalogWS::Read()   
//----------------------------------------------------------------------------
{
  //if (!m_TestMode)
    wxBusyInfo wait("Please wait, working...");
  
  mafNEW(m_EmgScalar);
  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_EmgScalar->SetName(name);
  m_EmgScalar->ReparentTo(m_Input);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_EmgScalar->GetTagArray()->SetTag(tag_Nature);

  mafString time, scalar;
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  double emg_time; 
  double val_scalar;
  wxString line;
  std::vector<mafString> stringVec;

  //check if file starts with the string "ANALOG"
  line = text.ReadLine(); 
  if (line.CompareTo("ANALOG")!= 0)
  {
    mafErrorMessage("Invalid file format!");
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

    /*if (rowNumber == 10)
    {
      line.Replace(","," ");
      wxStringTokenizer tkzCols(line,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
      num_tk = tkzCols.CountTokens();  
    }*/
  } while (!inputFile.Eof());

  vnl_matrix<double> emgMatrix;
  emgMatrix.set_size(rowNumber-1 , num_tk);

  wxFileInputStream inputFile1( m_File );
  wxTextInputStream text1( inputFile1 );
  
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line.Replace(","," ");
  
  for (int n = 0; n < rowNumber-1; n++)
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
  }

  vnl_matrix<double> emgMatrixTranspose = emgMatrix.transpose();

  m_EmgScalar->SetData(emgMatrixTranspose, 0);

  mafTagItem tag_Sig;
  tag_Sig.SetName("SIGNALS_NAME");
  tag_Sig.SetNumberOfComponents(num_tk - 1);
  m_EmgScalar->GetTagArray()->SetTag(tag_Sig);

  mafTagItem *tag_Signals = m_EmgScalar->GetTagArray()->GetTag("SIGNALS_NAME");
  for (int n = 0; n < stringVec.size(); n++)
  {
    tag_Signals->SetValue(stringVec[n], n);
  }

  
  m_Output = m_EmgScalar;
}
