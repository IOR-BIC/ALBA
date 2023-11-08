/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkWS
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
#include "albaOpExporterLandmarkWS.h"

#include <wx/busyinfo.h>

#include <fstream>
#include <iostream>

#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaProgressBarHelper.h"

using namespace std;

#define TAG_FORMAT "TRAJECTORIES"
#define FREQ 1.00

//----------------------------------------------------------------------------
albaOpExporterLandmarkWS::albaOpExporterLandmarkWS(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= "";

  m_Cloud = NULL;
}
//----------------------------------------------------------------------------
albaOpExporterLandmarkWS::~albaOpExporterLandmarkWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterLandmarkWS::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("albaVMELandmarkCloud"));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterLandmarkWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpExporterLandmarkWS *cp = new albaOpExporterLandmarkWS(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpExporterLandmarkWS::OpRun()   
//----------------------------------------------------------------------------
{
  wxString proposed = albaGetLastUserFolder();
  proposed += m_Input->GetName();
  proposed += "_TRAJECTORIES";
	proposed += ".csv";
	
  wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetSaveFile(proposed,wildc).ToAscii(); 

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
void albaOpExporterLandmarkWS::Write()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
	
  m_Cloud = albaVMELandmarkCloud::SafeDownCast(m_Input);

  std::vector<albaString> points_name;
  int numberLandmark = m_Cloud->GetNumberOfLandmarks();

  // Get points names
  for (int i=0; i<numberLandmark; i++)
  {
    points_name.push_back(m_Cloud->GetLandmarkName(i));
  }

  std::vector<albaTimeStamp> timeStamps;
  m_Cloud->GetTimeStamps(timeStamps);
  albaString lmName = "";
  double pos[3] = {0.0,0.0,0.0};
  double ori[3] = {0.0,0.0,0.0};
  double t;
  
  std::ofstream f_Out(m_File.ToAscii());
  if (!f_Out.bad())
  {
    // Add TRAJECTORIES tag
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",(DUMMY)\n";

    // Add the third line containing the points names
    f_Out << ","; // This dummy comma is necessary for compatibility with the Importer
    for (int i=0; i<numberLandmark; i++)
    {
      f_Out << points_name.at(i);
      if (i!=numberLandmark-1)
      {
        f_Out << ",,,"; // These dummy commas are necessary for compatibility with the Importer
      }
    }
    f_Out << "\n";

    // Add the line with user information
    f_Out << "Time";
    for (int i=0; i<numberLandmark-1; i++)
    {
      f_Out << ",X,Y,Z";
    }
    f_Out << "\n";

    for (int i=0; i<timeStamps.size(); i++)
    {
      t = timeStamps[i];
      f_Out << t;

      for(int j = 0; j < numberLandmark; j++)
      {
        m_Cloud->GetLandmark(j)->GetOutput()->GetAbsPose(pos,ori,t);
        if (pos[0]!=0 && pos[1]!=0 && pos[2]!=0)
        {
          f_Out << "," << pos[0] << "," << pos[1] << "," << pos[2];
        }
        else
        {
          f_Out << ",,,"; // These dummy commas are necessary for compatibility with the importer.
        }
      }
      f_Out << "\n";

      progressHelper.UpdateProgressBar(((double) i)/((double) timeStamps.size())*100.);
    }

    f_Out.close();
  }
}