/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-11 14:11:04 $
  Version:   $Revision: 1.3 $
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

#include "mmoLandmarkExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"

#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include <fstream>

//----------------------------------------------------------------------------
mmoLandmarkExporter::mmoLandmarkExporter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = "";
}
//----------------------------------------------------------------------------
mmoLandmarkExporter::~mmoLandmarkExporter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoLandmarkExporter::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  return node && node->IsMAFType(mafVMELandmarkCloud);
}
//----------------------------------------------------------------------------
mafOp* mmoLandmarkExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoLandmarkExporter *cp = new mmoLandmarkExporter(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mmoLandmarkExporter::OpRun()   
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
void mmoLandmarkExporter::ExportLandmark()
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
