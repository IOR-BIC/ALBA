/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmark
 Authors: Stefania Paperini , Daniele Giunchi, Simone Brazzale
 
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

#include "albaOpExporterLandmark.h"
#include "wx/busyinfo.h"
#include <wx/dirdlg.h>

#include "albaDecl.h"
#include "albaVMELandmark.h"

#include <fstream>

//----------------------------------------------------------------------------
albaOpExporterLandmark::albaOpExporterLandmark(const wxString &label) :
albaOp(label)
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
albaOpExporterLandmark::~albaOpExporterLandmark()
//----------------------------------------------------------------------------
{
  m_LC_vector.clear();
  m_LC_names.clear();
}
//----------------------------------------------------------------------------
bool albaOpExporterLandmark::InternalAccept(albaVME*node)   
//----------------------------------------------------------------------------
{ 
  m_LC_vector.clear();
  m_LC_names.clear();
  bool result = node && node->IsALBAType(albaVMELandmarkCloud);
  
  // Accept a Landmark Cloud ..
  if (result)
  {
    m_LC_vector.push_back((albaVMELandmarkCloud*)node);
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
int albaOpExporterLandmark::FindLandmarkClouds(albaVME* node)   
//----------------------------------------------------------------------------
{
  int result = 0;
  // Get input and store all the landmark clouds in the sub-tree
  const albaVME::albaChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    albaVME *child = children->at(i);
    if (child->IsA("albaVMELandmarkCloud"))
    {
      m_LC_vector.push_back((albaVMELandmarkCloud*)child);
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
albaOp* albaOpExporterLandmark::Copy()   
//----------------------------------------------------------------------------
{
  albaOpExporterLandmark *cp = new albaOpExporterLandmark(m_Label);
  cp->m_File = m_File;
  cp->m_FileDir = m_FileDir;
  cp->m_LC_vector = m_LC_vector;
  cp->m_LC_names = m_LC_names;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpExporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  int errors = 0;
  wxString f;
  wxString proposed = albaGetLastUserFolder();

  albaString info;
  info = "Exporting LC ";
  int copies = 1;

  // For every LC stored in the vector execute the exporter
  for (int i=0; i<m_LC_vector.size();i++)
  {
    albaVMELandmarkCloud* cloud = m_LC_vector.at(i);

    // FIRST CASE: A single LC to export
    if (m_LC_vector.size()==1)
    {
      proposed += cloud->GetName();
	    proposed += ".txt";
      wxString wildc = "ascii file (*.txt)|*.txt";
	    f = albaGetSaveFile(proposed,wildc).ToAscii(); 
    }
    // SECOND CASE: Many LC to export
    else
    {
      if (m_FileDir.length()==0)
      {
        wxDirDialog dialog(NULL, "Choose directory where to save files:", proposed.ToAscii(), wxRESIZE_BORDER);
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
    for (std::vector<albaString>::iterator it = m_LC_names.begin(); it<m_LC_names.end(); it++)
    {
      albaString s(cloud->GetName());
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

  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpExporterLandmark::ExportLandmark(albaVMELandmarkCloud* cloud)
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait(_("Saving landmark position: Please wait"));
  }

  if (cloud==NULL)
  {
    cloud = (albaVMELandmarkCloud*) m_Input;
  }

  std::ofstream f_Out(m_File.ToAscii());
  if (!f_Out.bad())
  {
    int numberLandmark = cloud->GetNumberOfLandmarks();
    std::vector<albaTimeStamp> timeStamps;
    cloud->GetTimeStamps(timeStamps);
    albaString lmName = "";
    double pos[3] = {0.0,0.0,0.0};
    double ori[3] = {0.0,0.0,0.0};
    double t=0;
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
        f_Out << lmName << ",\t" << pos[0] << ",\t" << pos[1] << ",\t" << pos[2] <<"\n";
      }
    }

    f_Out.close();
  }
}
