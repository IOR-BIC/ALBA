/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterGRFWS.cpp,v $
  Language:  C++
  Date:      $Date: 2010-09-09 15:19:21 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
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
#include "medOpExporterGRFWS.h"

#include <wx/busyinfo.h>
#include "mafGUI.h"

#include "mafTagArray.h"

#include <fstream>
#include <iostream>

#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkDataSet.h>
#include <vtkPolyData.h>

#include <mafTransformBase.h>
#include <mafMatrix.h>

using namespace std;

#define TAG_FORMAT "FORCE PLATES"
#define FREQ 1.00
#define DELTA 5.0

//----------------------------------------------------------------------------
medOpExporterGRFWS::medOpExporterGRFWS(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= "";

  m_PlatformLeft = NULL;
  m_PlatformRight = NULL;
  m_ForceLeft = NULL;
  m_ForceRight = NULL;
  m_MomentLeft = NULL;
  m_MomentRight = NULL;
  m_Group = NULL;

}
//----------------------------------------------------------------------------
medOpExporterGRFWS::~medOpExporterGRFWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool medOpExporterGRFWS::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  bool is_group = (node && node->IsA("mafVMEGroup"));
  if (!is_group)
  {
    return false;
  }
  int n_of_platforms = 0;
  int n_of_vectors = 0;
  const mafNode::mafChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    mafNode *child = children->at(i);
    if (child->IsA("mafVMESurface"))
    {
      n_of_platforms++;
      if (child->GetNumberOfChildren()==2)
      {
        if (child->GetChild(0)->IsA("mafVMEVector"))
        {
          n_of_vectors++;
        }
        if (child->GetChild(1)->IsA("mafVMEVector"))
        {
          n_of_vectors++;
        }
      }
    }
  }
  return (is_group && n_of_platforms==2 && (n_of_vectors==4));
}
//----------------------------------------------------------------------------
mafOp* medOpExporterGRFWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpExporterGRFWS *cp = new medOpExporterGRFWS(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void medOpExporterGRFWS::OpRun()   
//----------------------------------------------------------------------------
{
  // Get input
  const mafNode::mafChildrenVector* children = m_Input->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    mafNode *child = children->at(i);
    if (child->IsA("mafVMESurface"))
    {
      if (m_PlatformLeft==NULL)
      {
        m_PlatformLeft = mafVMESurface::SafeDownCast(child);
      }
      else
      {
        m_PlatformRight = mafVMESurface::SafeDownCast(child);
      }
      if (child->GetNumberOfChildren()==2)
      {
        if (child->GetChild(0)->IsA("mafVMEVector"))
        {
          if (m_ForceLeft==NULL)
          {
            m_ForceLeft = mafVMEVector::SafeDownCast(child->GetChild(0));
          }
          else
          {
            m_ForceRight = mafVMEVector::SafeDownCast(child->GetChild(0));
          }
        }
        if (child->GetChild(1)->IsA("mafVMEVector"))
        {
          if (m_MomentLeft==NULL)
          {
            m_MomentLeft = mafVMEVector::SafeDownCast(child->GetChild(1));
          }
          else
          {
            m_MomentRight = mafVMEVector::SafeDownCast(child->GetChild(1));
          }
        }
      }
    }
  }

  int result = OP_RUN_CANCEL;

  // Execute
  if (m_PlatformLeft && m_PlatformRight && m_ForceLeft && m_ForceRight && m_ForceRight && m_MomentRight)
  {
    wxString proposed = mafGetApplicationDirectory().c_str();
    proposed += "/Data/External/";
	  proposed += m_Input->GetName();
    proposed += "_FORCEPLATES";
	  proposed += ".csv";
  	
    wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	  wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

	  if(!f.IsEmpty())
	  {
		  m_File = f;
		  Write();
		  result = OP_RUN_OK;
	  }
  }
  else
  {
    wxMessageBox("Need 2 PLATFORMS, each with a FORCE vector and a MOMENT vector!","Warning",wxOK | wxICON_ERROR);
  }

	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpExporterGRFWS::Write()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
	  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  
  std::ofstream f_Out(m_File);
  if (!f_Out.bad())
  {
    // Add TAG
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",Hz\n";

    // Add a dummy line
    f_Out << "--- PLATES CORNERS SECTION ---\n";

    // Add the fourth line containing the plates tag
    f_Out << "Plate#,X1,Y1,Z1,X2,Y2,Z2,X3,Y3.Z3,X4,Y4,Z4\n";

    // Add the fifth and sixth line containing the corner values
    f_Out << "1,";
    double bounds[6];
    bounds[0] = 0;
    m_PlatformLeft->GetOutput()->GetVTKData()->GetBounds(bounds);
    bounds[4] = bounds[4] + DELTA;
    f_Out << bounds[0] << "," << bounds[3] << "," << bounds[4] << "," << bounds[1] << "," << bounds[3] << "," << bounds[4] << ","
        << bounds[1] << "," << bounds[2] << "," << bounds[4] << "," << bounds[0] << "," << bounds[2] << "," << bounds[4] << "\n";
    f_Out << "2,";
    m_PlatformRight->GetOutput()->GetVTKData()->GetBounds(bounds);
    bounds[4] = bounds[4] + DELTA;
    f_Out << bounds[0] << "," << bounds[3] << "," << bounds[4] << "," << bounds[1] << "," << bounds[3] << "," << bounds[4] << ","
        << bounds[1] << "," << bounds[2] << "," << bounds[4] << "," << bounds[0] << "," << bounds[2] << "," << bounds[4] << "\n";

    //Add a blank line 
    f_Out << "\n";

    // Add a dummy line
    f_Out << "--- FORCES VECTOR SECTION (1=PLATE1,2=PLATE2) ---\n";

    // Add a line containing the forces tag
    f_Out << "TIME,COP1:X,COP1:Y,COP1:Z,Ref1:X,Ref1:Y,Ref1:Z,Force1:X,Force1:Y,Force1:Z,Moment1:X,Moment1:Y,Moment1:Z,COP2:X,COP2:Y,COP2:Z,Ref2:X,Ref2:Y,Ref2:Z,Force2:X,Force2:Y,Force2:Z,Moment2:X,Moment2:Y,Moment2:Z\n";

    //Add a blank line 
    f_Out << "\n";

    //Add times and values; time is always the first row
    std::vector<mafTimeStamp> kframes1;
    std::vector<mafTimeStamp> kframes2;
    m_ForceLeft->GetTimeStamps(kframes1);
    m_ForceRight->GetTimeStamps(kframes2);
    std::vector<mafTimeStamp> kframes = mergeTimeStamps(kframes1,kframes2);
    int size = kframes.size();
    for (int i=0;i<size;i++)
    {
      double time = kframes.at(i);

      m_PlatformLeft->SetTimeStamp(time);
      m_PlatformLeft->GetOutput()->GetVTKData()->Update();
      m_ForceLeft->SetTimeStamp(time);
      m_ForceLeft->GetOutput()->GetVTKData()->Update();
      m_MomentLeft->SetTimeStamp(time);
      m_MomentLeft->GetOutput()->GetVTKData()->Update();
      m_PlatformRight->SetTimeStamp(time);
      m_PlatformRight->GetOutput()->GetVTKData()->Update();
      m_ForceRight->SetTimeStamp(time);
      m_ForceRight->GetOutput()->GetVTKData()->Update();
      m_MomentRight->SetTimeStamp(time);
      m_MomentRight->GetOutput()->GetVTKData()->Update();

      vtkPolyData* polyFL = (vtkPolyData*)m_ForceLeft->GetOutput()->GetVTKData();
      vtkPolyData* polyML = (vtkPolyData*)m_MomentLeft->GetOutput()->GetVTKData();
      vtkPolyData* polyFR = (vtkPolyData*)m_ForceRight->GetOutput()->GetVTKData();
      vtkPolyData* polyMR = (vtkPolyData*)m_MomentRight->GetOutput()->GetVTKData();

      // TIME
      f_Out << time << ",";
      
      // COP L
      double* p = polyFL->GetPoint(0);
      double copL[3];
      copL[0] = p[0];
      copL[1] = p[1];
      copL[2] = p[2];
      f_Out << copL[0] << "," << copL[1] << "," << copL[2] << ",";
      
      // REF L
      f_Out << 0 << "," << 0 << "," << 0 << ",";
      
      // FORCE L
      p = polyFL->GetPoint(1);
      double fL[3];
      fL[0] = p[0];
      fL[1] = p[1];
      fL[2] = p[2];
      f_Out << fL[0]-copL[0] << "," << fL[1]-copL[1] << "," << fL[2]-copL[2] << ",";

      // MOMENT L
      p = polyML->GetPoint(1);
      double mL[3];
      mL[0] = p[0];
      mL[1] = p[1];
      mL[2] = p[2];
      f_Out << mL[0]-copL[0] << "," << mL[1]-copL[1] << "," << mL[2]-copL[2] << ",";

      // COP R
      p = polyFR->GetPoint(0);
      double copR[3];
      copR[0] = p[0];
      copR[1] = p[1];
      copR[2] = p[2];
      f_Out << copR[0] << "," << copR[1] << "," << copR[2] << ",";
      
      // REF R
      f_Out << 0 << "," << 0 << "," << 0 << ",";
      
      // FORCE R
      p = polyFR->GetPoint(1);
      double fR[3];
      fR[0] = p[0];
      fR[1] = p[1];
      fR[2] = p[2];
      f_Out << fR[0]-copR[0] << "," << fR[1]-copR[1] << "," << fR[2]-copR[2] << ",";

      // MOMENT R
      p = polyMR->GetPoint(1);
      double mR[3];
      mR[0] = p[0];
      mR[1] = p[1];
      mR[2] = p[2];
      f_Out << mR[0]-copR[0] << "," << mR[1]-copR[1] << "," << mR[2]-copR[2] << "\n";

      if (!m_TestMode)
      {
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) size)*100.)));
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
//----------------------------------------------------------------------------
std::vector<mafTimeStamp> medOpExporterGRFWS::mergeTimeStamps(std::vector<mafTimeStamp> kframes1,std::vector<mafTimeStamp> kframes2)   
//----------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> kframes;

  int i = 0;
  int j = 0;

  do
  {
    if (kframes1.at(i)<=kframes2.at(j))
    {
      kframes.push_back(kframes1.at(i));
      if (kframes1.at(i)==kframes2.at(j))
      {
        j++;
      }
      i++;
    }
    else
    {
      kframes.push_back(kframes2.at(j));
      j++;
    }
  } while (i!=(kframes1.size()-1) && j!=(kframes2.size()-1));

  return kframes;
}