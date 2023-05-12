/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterAnalogWS
 Authors: Simone Brazzale
 
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
#include "albaOpExporterAnalogWS.h"

#include <wx/busyinfo.h>
#include "albaGUI.h"

#include "albaTagArray.h"
#include "albaVMEOutputScalarMatrix.h"

#include <fstream>
#include <iostream>

#include <vnl\vnl_matrix.h>
#include "albaProgressBarHelper.h"

using namespace std;

#define TAG_FORMAT "ANALOG"
#define FREQ 1.00

//----------------------------------------------------------------------------
albaOpExporterAnalogWS::albaOpExporterAnalogWS(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= "";

  m_Analog = NULL;
}
//----------------------------------------------------------------------------
albaOpExporterAnalogWS::~albaOpExporterAnalogWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterAnalogWS::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("albaVMEAnalog"));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpExporterAnalogWS *cp = new albaOpExporterAnalogWS(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpExporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	wxString proposed = albaGetLastUserFolder().c_str();
	proposed += m_Input->GetName();
	proposed += ".csv";
	
  wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetSaveFile(proposed,wildc).c_str(); 

	int result = OP_RUN_CANCEL;
	if(!f.IsEmpty())
	{
		m_File = f;
		Write();
		result = OP_RUN_OK;
	}
	albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpExporterAnalogWS::Write()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
  
  m_Analog = albaVMEAnalog::SafeDownCast(m_Input);
  albaTagItem *tag_sig = m_Analog->GetTagArray()->GetTag("SIGNALS_NAME");
  int n_sig = tag_sig->GetComponents()->size();

  albaString empty("");

  std::ofstream f_Out(m_File.c_str());
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
			progressHelper.UpdateProgressBar(((double) i)/((double) emgMatrix.columns())*100.);
    }
    
    f_Out.close();
  }  
}