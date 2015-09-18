/*=========================================================================

 Program: MAF2
 Module: mafOpExporterAnalogWS
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafOpExporterAnalogWS.h"

#include <wx/busyinfo.h>
#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafVMEOutputScalarMatrix.h"

#include <fstream>
#include <iostream>

#include <vnl\vnl_matrix.h>

using namespace std;

#define TAG_FORMAT "ANALOG"
#define FREQ 1.00

//----------------------------------------------------------------------------
mafOpExporterAnalogWS::mafOpExporterAnalogWS(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= "";

  m_Analog = NULL;
}
//----------------------------------------------------------------------------
mafOpExporterAnalogWS::~mafOpExporterAnalogWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterAnalogWS::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("mafVMEAnalog"));
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	mafOpExporterAnalogWS *cp = new mafOpExporterAnalogWS(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void mafOpExporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	wxString proposed = mafGetDocumentsDirectory().c_str();
	proposed += m_Input->GetName();
	proposed += ".csv";
	
  wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

	int result = OP_RUN_CANCEL;
	if(!f.IsEmpty())
	{
		m_File = f;
		Write();
		result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpExporterAnalogWS::Write()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
	  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  
  m_Analog = mafVMEAnalog::SafeDownCast(m_Input);
  mafTagItem *tag_sig = m_Analog->GetTagArray()->GetTag("SIGNALS_NAME");
  int n_sig = tag_sig->GetComponents()->size();

  mafString empty("");

  std::ofstream f_Out(m_File);
  if (!f_Out.bad())
  {
    // Add ANALOG tag
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

    //Add the third line containing the signal names
    f_Out << "FRAME,";
    f_Out << tag_sig->GetValue(0);
    for (int i=1;i<n_sig;i++)
    {
      if (!empty.Equals(tag_sig->GetValue(i)))
      {
        f_Out << "," << tag_sig->GetValue(i);
      }
    }
    f_Out << "\n";
    
    //Add a blank line 
    f_Out << "\n";

    //Add times and values; time is always the first row
    vnl_matrix<double> emgMatrix = m_Analog->GetScalarOutput()->GetScalarData();
    for (int i=0;i<emgMatrix.columns();i++)
    {
      // Add time
      double time = emgMatrix.get(0,i);
      f_Out << time << ",";
      for (int j=1;j<emgMatrix.rows()-1;j++)
      {
        // Add all values but last one
        f_Out << emgMatrix.get(j,i) << ",";
      }
      if (emgMatrix.rows()>0)
      {
        // Add last one
        if (i==emgMatrix.columns()-1)
        {
          f_Out << emgMatrix.get(emgMatrix.rows()-1,i);
        }
        else
        {
          f_Out << emgMatrix.get(emgMatrix.rows()-1,i) << "\n";
        }
      }
      if (!m_TestMode)
      {
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) emgMatrix.columns())*100.)));
      }
    }
    
    f_Out.close();
  }  

  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
  }
}