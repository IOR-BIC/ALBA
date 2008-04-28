/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterLandmark.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:37:17 $
  Version:   $Revision: 1.1 $
  Authors:   Stefania Paperini , Daniele Giunchi (porting MAf 2.0)
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

#include "medOpExporterLandmark.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"

#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include <fstream>

//----------------------------------------------------------------------------
medOpExporterLandmark::medOpExporterLandmark(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = "";
}
//----------------------------------------------------------------------------
medOpExporterLandmark::~medOpExporterLandmark()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool medOpExporterLandmark::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  return node && node->IsMAFType(mafVMELandmarkCloud);
}
//----------------------------------------------------------------------------
mafOp* medOpExporterLandmark::Copy()   
//----------------------------------------------------------------------------
{
  medOpExporterLandmark *cp = new medOpExporterLandmark(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void medOpExporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
	wxString proposed = mafGetApplicationDirectory().c_str();
  proposed += "/Data/External/";
	proposed += m_Input->GetName();
	proposed += ".txt";
	
  wxString wildc = "ascii file (*.txt)|*.txt";
	wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

	int result = OP_RUN_CANCEL;
	if(!f.IsEmpty())
	{
		m_File = f;
		ExportLandmark();
		result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpExporterLandmark::ExportLandmark()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Saving landmark position: Please wait"));
  }

  std::ofstream f_Out(m_File);
  if (!f_Out.bad())
  {
    mafVMELandmarkCloud *cloud = (mafVMELandmarkCloud *)m_Input;
    bool statusOpen = cloud->IsOpen();
    if (!statusOpen)
    {
      cloud->Open();
    }
    int numberLandmark = cloud->GetNumberOfLandmarks();
    std::vector<mafTimeStamp> timeStamps;
    cloud->GetTimeStamps(timeStamps);
    mafString lmName = "";
    double pos[3] = {0.0,0.0,0.0};
    double ori[3] = {0.0,0.0,0.0};
    double t;
    for (unsigned int i = 0; i < timeStamps.size(); i++)
    {
      t = timeStamps[i];
      f_Out << "Time" << "\t" << t << "\n";

      for(unsigned int j = 0; j < numberLandmark; j++)
      {
        lmName = cloud->GetLandmarkName(j);
        //cloud->GetLandmarkPosition(j, pos, t);
        cloud->GetLandmark(j)->GetOutput()->GetAbsPose(pos,ori,t);
        f_Out << lmName << "\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] <<"\n";
      }
    }
    if (!statusOpen)
    {
      cloud->Close();
    }
    f_Out.close();
  }
}
