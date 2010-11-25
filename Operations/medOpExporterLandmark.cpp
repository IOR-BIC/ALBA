/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterLandmark.cpp,v $
  Language:  C++
  Date:      $Date: 2010-11-25 16:39:22 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefania Paperini , Daniele Giunchi, Simone Brazzale
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
#include <wx/dirdlg.h>

#include "mafDecl.h"
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
  m_LC_vector.clear();
}
//----------------------------------------------------------------------------
medOpExporterLandmark::~medOpExporterLandmark()
//----------------------------------------------------------------------------
{
  m_LC_vector.clear();
}
//----------------------------------------------------------------------------
bool medOpExporterLandmark::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  m_LC_vector.clear();
  bool result = node && node->IsMAFType(mafVMELandmarkCloud);
  
  // Accept a Landmark Cloud ..
  if (result==true)
  {
    m_LC_vector.push_back((mafVMELandmarkCloud*)node);
    return true;
  }
  // .. or a vme with many landmark clouds as children
  else
  {
    result = (node && FindLandmarkClouds(node)==1);
  }
}
//----------------------------------------------------------------------------
int medOpExporterLandmark::FindLandmarkClouds(mafNode* node)   
//----------------------------------------------------------------------------
{
  int result = 0;
  // Get input and store all the landmark clouds in the sub-tree
  const mafNode::mafChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    mafNode *child = children->at(i);
    if (child->IsA("mafVMELandmarkCloud"))
    {
      m_LC_vector.push_back((mafVMELandmarkCloud*)child);
      result = (FindLandmarkClouds(child) || 1);
    }
    else
    {
      result = (FindLandmarkClouds(child) || result);
    }
  }
  return result;
}
//----------------------------------------------------------------------------
mafOp* medOpExporterLandmark::Copy()   
//----------------------------------------------------------------------------
{
  medOpExporterLandmark *cp = new medOpExporterLandmark(m_Label);
  cp->m_File = m_File;
  cp->m_LC_vector = m_LC_vector;
  return cp;
}
//----------------------------------------------------------------------------
void medOpExporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  int errors = 0;
  wxString f;
  wxString proposed = mafGetApplicationDirectory().c_str();
  proposed += "/Data/External/";

  mafString info;
  info = "Exporting LC ";

  // For every LC stored in the vector execute the exporter
  for (int i=0; i<m_LC_vector.size();i++)
  {
    mafVMELandmarkCloud* cloud = m_LC_vector.at(i);

    // FIRST CASE: A single LC to export
    if (m_LC_vector.size()==1)
    {
      proposed += cloud->GetName();
	    proposed += ".txt";
      wxString wildc = "ascii file (*.txt)|*.txt";
	    f = mafGetSaveFile(proposed,wildc).c_str(); 
    }
    // SECOND CASE: Many LC to export
    else
    {
      if (i==0)
      {
        wxDirDialog dialog(NULL, "Choose directory where to save files:", proposed.c_str(), wxRESIZE_BORDER);
			  dialog.SetReturnCode(wxID_OK);
			  int ret_code = dialog.ShowModal();
			  if (ret_code == wxID_OK)
			  {
				  proposed = dialog.GetPath();
        }
      }
      f = proposed;
      f += "/LC_SET_";
      f += cloud->GetName();
      f += ".txt";
    }

	  if(!f.IsEmpty())
	  {
		  m_File = f;
		  ExportLandmark(cloud);
	  }
    else
    {
      errors++;
    }
	  
  }

  if (errors==0)
  {
    result = OP_RUN_OK;
  }

  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpExporterLandmark::ExportLandmark(mafVMELandmarkCloud* cloud)
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Saving landmark position: Please wait"));
  }

  if (cloud==NULL)
  {
    cloud = (mafVMELandmarkCloud*) m_Input;
  }

  std::ofstream f_Out(m_File);
  if (!f_Out.bad())
  {
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
