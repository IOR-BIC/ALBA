/*=========================================================================

 Program: MAF2
 Module: mafOpExporterLandmark
 Authors: Stefania Paperini , Daniele Giunchi, Simone Brazzale
 
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

#include "mafOpExporterLandmark.h"
#include "wx/busyinfo.h"
#include <wx/dirdlg.h>

#include "mafDecl.h"
#include "mafVMELandmark.h"

#include <fstream>

//----------------------------------------------------------------------------
mafOpExporterLandmark::mafOpExporterLandmark(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = "";
  m_FileDir = "";
  m_LC_names.clear();
  m_LC_vector.clear();
}
//----------------------------------------------------------------------------
mafOpExporterLandmark::~mafOpExporterLandmark()
//----------------------------------------------------------------------------
{
  m_LC_vector.clear();
  m_LC_names.clear();
}
//----------------------------------------------------------------------------
bool mafOpExporterLandmark::Accept(mafVME*node)   
//----------------------------------------------------------------------------
{ 
  m_LC_vector.clear();
  m_LC_names.clear();
  bool result = node && node->IsMAFType(mafVMELandmarkCloud);
  
  // Accept a Landmark Cloud ..
  if (result)
  {
    m_LC_vector.push_back((mafVMELandmarkCloud*)node);
    m_LC_names.push_back(node->GetName());    
  }
  // .. or a vme with many landmark clouds as children
  else
  {
    result = (node && FindLandmarkClouds(node)==1);
  }

	return result;
}
//----------------------------------------------------------------------------
int mafOpExporterLandmark::FindLandmarkClouds(mafVME* node)   
//----------------------------------------------------------------------------
{
  int result = 0;
  // Get input and store all the landmark clouds in the sub-tree
  const mafVME::mafChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    mafVME *child = children->at(i);
    if (child->IsA("mafVMELandmarkCloud"))
    {
      m_LC_vector.push_back((mafVMELandmarkCloud*)child);
      m_LC_names.push_back(child->GetName());
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
mafOp* mafOpExporterLandmark::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterLandmark *cp = new mafOpExporterLandmark(m_Label);
  cp->m_File = m_File;
  cp->m_FileDir = m_FileDir;
  cp->m_LC_vector = m_LC_vector;
  cp->m_LC_names = m_LC_names;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpExporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  int errors = 0;
  wxString f;
  wxString proposed = mafGetLastUserFolder().c_str();

  mafString info;
  info = "Exporting LC ";
  int copies = 1;

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
      if (m_FileDir.length()==0)
      {
        wxDirDialog dialog(NULL, "Choose directory where to save files:", proposed.c_str(), wxRESIZE_BORDER);
			  dialog.SetReturnCode(wxID_OK);
			  int ret_code = dialog.ShowModal();
			  if (ret_code == wxID_OK)
			  {
				  m_FileDir = dialog.GetPath();
        }
      }
      f = m_FileDir;
      f += "/LC_SET_";
      f += cloud->GetName();
      f += ".txt";
    }

    // manage case where there are more files with the same name
    int count = 0;
    for (std::vector<mafString>::iterator it = m_LC_names.begin(); it<m_LC_names.end(); it++)
    {
      mafString s(cloud->GetName());
        if (s.Compare(*it)==0)
        {
          count++;
        }
    }
    if (count>1)
    {
      std::stringstream out;
      out << copies;
      f = f.BeforeFirst('.');
      f += "_renamed_";
      f += out.str().c_str();
      f += ".txt";
      copies++;
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
void mafOpExporterLandmark::ExportLandmark(mafVMELandmarkCloud* cloud)
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
    int numberLandmark = cloud->GetNumberOfLandmarks();
    std::vector<mafTimeStamp> timeStamps;
    cloud->GetTimeStamps(timeStamps);
    mafString lmName = "";
    double pos[3] = {0.0,0.0,0.0};
    double ori[3] = {0.0,0.0,0.0};
    double t;
    for (unsigned int i = 0; i < timeStamps.size(); i++)
    {
			if (timeStamps.size() > 1)
			{
				t = timeStamps[i];
				f_Out << "Time" << "\t" << t << "\n";
			}

      for(unsigned int j = 0; j < numberLandmark; j++)
      {
        lmName = cloud->GetLandmarkName(j);
        //cloud->GetLandmarkPosition(j, pos, t);
        cloud->GetLandmark(j)->GetOutput()->GetAbsPose(pos,ori,t);
        f_Out << lmName << ",\t" << pos[0] << ",\t" << pos[1] << ",\t" << pos[2] <<",\n";
      }
    }

    f_Out.close();
  }
}
