/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterEmgWS.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-27 13:04:57 $
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
#include "medOpImporterEmgWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mmgGui.h"
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
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();

}
//----------------------------------------------------------------------------
medOpImporterEmgWS::~medOpImporterEmgWS()
//----------------------------------------------------------------------------
{
  if (m_Group != NULL) 
  {
    mafDEL(m_Group);
  }
}
//----------------------------------------------------------------------------
mafOp* medOpImporterEmgWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterEmgWS *cp = new medOpImporterEmgWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
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
void medOpImporterEmgWS::Read()   
//----------------------------------------------------------------------------
{
  //if (!m_TestMode)
    wxBusyInfo wait("Please wait, working...");
  
  mafNEW(m_Group);
  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_Group->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  mafString time, scalar;
   
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  double emg_time; 
  double val_scalar;
  wxString line;
  int num_tk;
  int rowNumber = 0;
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

  //Read frequency 
  wxString freq = line.SubString(0,comma - 1); 
  double freq_val;
  freq_val = atof(freq.c_str());

  //Put the signals names in a vector of string
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);

  //Count number of signals (columns)
  num_tk = tkzName.CountTokens()-1;  
  tkzName.GetNextToken(); //To skip "Sample #"
  while (tkzName.HasMoreTokens())
  {
    stringVec.push_back(tkzName.GetNextToken());    
  }

  line = text.ReadLine();
  line = text.ReadLine();
  line.Replace(","," ");

  //count number of timestamps (rows)
  do  
  {
    line = text.ReadLine();
    rowNumber++;
  } while (!inputFile.Eof());

  vnl_matrix<double> emgMatrix;
  emgMatrix.set_size(rowNumber , num_tk);
  std::vector<mafTimeStamp> timeVect;

  wxFileInputStream inputFile1( m_File );
  wxTextInputStream text1( inputFile1 );

  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line.Replace(","," ");

  unsigned i;
  wxString frame;

  //Fill the matrix 
  for (int n = 0; n < rowNumber; n++)
  {
    i = 0;
    wxStringTokenizer tkz(line,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
    frame = tkz.GetNextToken(); //To skip the time value
    emg_time = atof(frame)/freq_val; 
    timeVect.push_back(emg_time);

    while (tkz.HasMoreTokens())
    {
      scalar = tkz.GetNextToken();
      val_scalar = atof(scalar);

      emgMatrix.put(n,i,val_scalar); //Add scalar value to the vnl_matrix 
      i++;
    }
    line = text1.ReadLine();
    line.Replace(","," ");
  }
  
  mafEventMacro(mafEvent(this, PROGRESSBAR_SHOW));
  double scalarValue;
  long progress = 0;
  mafTimeStamp timeValue;
  int colNumber = emgMatrix.cols();
  for (int c = 0; c < colNumber ; c++)
  {
    mafSmartPointer<mafVMEScalar> emgScalar;
    emgScalar->SetName(stringVec.at(c));

    for (int n = 0; n < emgMatrix.rows(); n++)
    {
      scalarValue = emgMatrix.get(n,c);
      timeValue = timeVect.at(n);
      emgScalar->SetData(scalarValue, timeValue);
    }

    progress = (c * 100)/colNumber;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));

    m_Group->AddChild(emgScalar);
    m_Group->Update();
  }
  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  m_Output = m_Group;
}
