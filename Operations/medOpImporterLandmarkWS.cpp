/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterLandmarkWS.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:10:50 $
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

#include "medOpImporterLandmarkWS.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <iostream>

//----------------------------------------------------------------------------
medOpImporterLandmarkWS::medOpImporterLandmarkWS(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	m_VmeCloud		= NULL;
}
//----------------------------------------------------------------------------
medOpImporterLandmarkWS::~medOpImporterLandmarkWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterLandmarkWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterLandmarkWS *cp = new medOpImporterLandmarkWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterLandmarkWS::OpRun()   
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
     Read();
     result = OP_RUN_OK;
	 }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpImporterLandmarkWS::Read()   
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

  wxString skipc, line;
  mafString time, first_time, x, y, z;
  double xval, yval, zval, tval;
  std::vector<mafString> stringVec;
  
  std::vector<int> lm_idx;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  //check if file starts with the string "ANALOG"
  line = text.ReadLine(); //Ignore 4 lines of textual information
  if (line.CompareTo("TRAJECTORIES")!= 0)
  {
    mafErrorMessage("Invalid file format!");
    return;
  }

  line = text.ReadLine();

  //Read frequency 
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1);
  double freq_val;
  freq_val = atof(freq.c_str());

  //Put the signals names in a vector of string
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  
  tkzName.GetNextToken(); //To skip ","
  while (tkzName.HasMoreTokens())
  {
    stringVec.push_back(tkzName.GetNextToken()); 
    tkzName.GetNextToken(); //To skip ","
    tkzName.GetNextToken(); //To skip ","
  }
  
  line = text.ReadLine();
  line = text.ReadLine();

  wxStringTokenizer tkz_numAL(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  int numland = (tkz_numAL.CountTokens()-1)/3;

  mafString lm_name;
  int index;
  std::vector<int> indexSPlitOriginal;
  std::vector<int> indexSplitCopy;

  for (int i=0;i<numland;i++)
  {
    lm_name = stringVec.at(i);
    index = m_VmeCloud->FindLandmarkIndex(lm_name);
    if (index == -1)
    {
      lm_idx.push_back(m_VmeCloud->AppendLandmark(lm_name));
    }
    else
    {
      //To store index of LA split
     indexSPlitOriginal.push_back(index);
     indexSplitCopy.push_back(i);
    }
  }
  
  do 
  {
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    time = tkz.GetNextToken().c_str();
    long counter = 0;
    int counterAL = 0;
    int indexCounter = 0;
    tval = atof(time)/freq_val;

    while (tkz.HasMoreTokens())
    {
      x = tkz.GetNextToken().c_str();
      y = tkz.GetNextToken().c_str();
      z = tkz.GetNextToken().c_str();
      xval = atof(x);
      yval = atof(y);
      zval = atof(z);


      if (!indexSplitCopy.empty()) //true if AL is split in columns
      {
        if ( counter == indexSplitCopy[indexCounter]) //If TRUE this AL already exists
        {
          if(!x.IsEmpty() && !y.IsEmpty() && !z.IsEmpty() )
          {
            //Insert the values in the AL with the same name (idx)
            m_VmeCloud->SetLandmark(lm_idx[indexSPlitOriginal[indexCounter]],xval,yval,zval,tval);
          }
          indexCounter++;
          counter++;
        }
        else
        {
          if(x.IsEmpty() && y.IsEmpty() && z.IsEmpty() )
          {
            m_VmeCloud->SetLandmark(lm_idx[counterAL],0,0,0,tval);
            m_VmeCloud->SetLandmarkVisibility(lm_idx[counterAL], 0,tval);
          }
          else
          {
            m_VmeCloud->SetLandmark(lm_idx[counterAL],xval,yval,zval,tval);
          }
          counter++;
          counterAL++;
        }
      }
      else //AL is not spit in columns
      {
        if(x.IsEmpty() && y.IsEmpty() && z.IsEmpty() )
        {
          m_VmeCloud->SetLandmark(lm_idx[counterAL],0,0,0,tval);
          m_VmeCloud->SetLandmarkVisibility(lm_idx[counterAL], 0,tval);
        }
        else
        {
          m_VmeCloud->SetLandmark(lm_idx[counterAL],xval,yval,zval,tval);
        }
        counter++;
        counterAL++;
      }
 
    }
    line = text.ReadLine();

  } while (!inputFile.Eof());

  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);  
  m_Output = m_VmeCloud;
}
