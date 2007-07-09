/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoEMGImporterWS.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-09 10:09:38 $
  Version:   $Revision: 1.6 $
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

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mmoEMGImporterWS.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "medVMEEmg.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"



#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoEMGImporterWS::mmoEMGImporterWS(wxString label) :
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
mmoEMGImporterWS::~mmoEMGImporterWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_EmgScalar);
}
//----------------------------------------------------------------------------
mafOp* mmoEMGImporterWS::Copy()   
//----------------------------------------------------------------------------
{
	mmoEMGImporterWS *cp = new mmoEMGImporterWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void mmoEMGImporterWS::OpRun()   
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
void mmoEMGImporterWS::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      default:
        mafEventMacro(*e);
    }
  }
}

//----------------------------------------------------------------------------
void mmoEMGImporterWS::Read()   
//----------------------------------------------------------------------------
{
  //if (!m_TestMode)
    wxBusyInfo wait("Please wait, working...");
  
  mafNEW(m_EmgScalar);
  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_EmgScalar->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  mafString time, scalar;
   
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  double emg_time; 
  double val_scalar;
  wxString line;

  line = text.ReadLine(); 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  int sizeMatrix;
  freq_val = atof(freq.c_str());
  
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line.Replace(","," ");

  int space, num_tk;
  unsigned i;
  wxString frame;

  do 
  {
    space = line.Find(' ');
    frame = line.SubString(0,space - 1);
    emg_time = atof(frame)/freq_val; 
    wxStringTokenizer tkz(line,wxT(' '),wxTOKEN_RET_EMPTY_ALL);

    tkz.GetNextToken(); //To skip the time value
    num_tk = tkz.CountTokens();  
    //num_tk < 1 ? sizeMatrix = 1 : sizeMatrix = num_tk - 1;
    if (num_tk > 0)
    {
      m_EmgMatrix.set_size(1, num_tk - 1);
    }
   
    i = 0;
    
    while (tkz.HasMoreTokens())
    {
      scalar = tkz.GetNextToken();
      val_scalar = atof(scalar);
     
      m_EmgMatrix.put(0,i,val_scalar); //Add scalar value to the vnl_matrix

      i++;     
    }

    m_EmgScalar->SetData(m_EmgMatrix, emg_time);
    //m_EmgMatrix.clear();

    line = text.ReadLine();
    line.Replace(","," ");

  } while (!inputFile.Eof());

  m_Output = m_EmgScalar;
}
