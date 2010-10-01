/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterGRFWS.cpp,v $
  Language:  C++
  Date:      $Date: 2010-10-01 19:08:22 $
  Version:   $Revision: 1.1.2.6 $
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
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include "mafGUI.h"

#include "mafTagArray.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkDataSet.h>
#include <vtkPolyData.h>

#include <mafTransformBase.h>
#include <mafMatrix.h>

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
void medOpExporterGRFWS::Clear()
//----------------------------------------------------------------------------
{
  m_PlatformLeft = NULL;
  m_PlatformRight = NULL;
  m_ForceLeft = NULL;
  m_ForceRight = NULL;
  m_MomentLeft = NULL;
  m_MomentRight = NULL;
}
//----------------------------------------------------------------------------
bool medOpExporterGRFWS::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  Clear();
  return (node && LoadVMEs(node));
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
  int result = OP_RUN_CANCEL;

  // Load and Execute
  if (LoadVMEs(m_Input))
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
      m_File_temp1 = m_File + "_tmp1";
      m_File_temp2 = m_File + "_tmp2";
      m_File_temp3 = m_File + "_tmp3";
      m_File_temp4 = m_File + "_tmp4";

		  Write();

      remove(m_File_temp1);
      remove(m_File_temp2);
      remove(m_File_temp3);
      remove(m_File_temp4);

		  result = OP_RUN_OK;
	  }
  }
  else
  {
    wxMessageBox("Need 2 PLATFORMS, each with a FORCE vector and a MOMENT vector!","GRF Exporter Warning",wxOK | wxICON_ERROR);
  }

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int medOpExporterGRFWS::LoadVMEs(mafNode* node)   
//----------------------------------------------------------------------------
{
  int result = 0;
  // Get input
  const mafNode::mafChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    mafNode *child = children->at(i);
    if (child->IsA("mafVMESurface"))
    {
      if (m_PlatformLeft==NULL)
      {
        m_PlatformLeft = mafVMESurface::SafeDownCast(child);
      }
      else if (m_PlatformRight==NULL)
      {
        m_PlatformRight = mafVMESurface::SafeDownCast(child);
      }
    }
    if (child->IsA("mafVMEVector"))
    {
      if (m_ForceLeft==NULL)
      {
        m_ForceLeft = mafVMEVector::SafeDownCast(child);
      }
      else if (m_MomentLeft==NULL)
      {
        m_MomentLeft = mafVMEVector::SafeDownCast(child);
      }
      else if (m_ForceRight==NULL)
      {
        m_ForceRight = mafVMEVector::SafeDownCast(child);
      }
      else if (m_MomentRight==NULL)
      {
        m_MomentRight = mafVMEVector::SafeDownCast(child);
      }
    }
    if (m_PlatformLeft==NULL || m_PlatformRight==NULL || m_ForceLeft==NULL || m_ForceRight==NULL || m_MomentLeft==NULL || m_MomentRight==NULL)
    {
      result =  LoadVMEs(child);
    }
    else
    {
      return 1;
    }
  }
  return result;
}
//----------------------------------------------------------------------------
void medOpExporterGRFWS::Write()   
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait = NULL;
  mafString info = "Loading data from files";
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&info));
	  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    wait = new wxBusyInfo("This may take several minutes, please be patient...");
  }

  // Must update VTK!
  m_PlatformLeft->Update();
  m_PlatformLeft->GetVTKOutput()->Update();
  m_PlatformRight->Update();
  m_PlatformRight->GetVTKOutput()->Update();
  m_ForceLeft->Update();
  m_ForceLeft->GetVTKOutput()->Update();
  m_MomentLeft->Update();
  m_MomentLeft->GetVTKOutput()->Update();
  m_ForceRight->Update();
  m_ForceRight->GetVTKOutput()->Update();
  m_MomentRight->Update();
  m_MomentRight->GetVTKOutput()->Update();

  double bounds1[6];
  bounds1[0] = 0;
  m_PlatformLeft->GetOutput()->GetVTKData()->GetBounds(bounds1);
  bounds1[4] = bounds1[4] + DELTA;
  double bounds2[6];
  bounds2[0] = 0;
  m_PlatformRight->GetOutput()->GetVTKData()->GetBounds(bounds2);
  bounds2[4] = bounds2[4] + DELTA;

  std::ofstream f_Out(m_File);
  std::ofstream f_Out1(m_File_temp1);
  std::ofstream f_Out2(m_File_temp2);
  std::ofstream f_Out3(m_File_temp3);
  std::ofstream f_Out4(m_File_temp4);

  std::vector<mafTimeStamp> kframes1;
  std::vector<mafTimeStamp> kframes2;
  m_ForceLeft->GetTimeStamps(kframes1);
  m_ForceRight->GetTimeStamps(kframes2);
  std::vector<mafTimeStamp> kframes = MergeTimeStamps(kframes1,kframes2);
  int size = kframes.size();

  // Pre-calculate values and save them in temp files.
  // THIS SHOULD SPEED UP THE PROCESS!! That otherwise is infinite in case the user has just opened a MSF file.
  double copL[3];   
  double copR[3];
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_ForceLeft->SetTimeStamp(time);
    m_ForceLeft->Update();
    m_ForceLeft->GetVTKOutput()->Update();
      
    vtkPolyData* polyFL = (vtkPolyData*)m_ForceLeft->GetOutput()->GetVTKData();
     
    double* p = polyFL->GetPoint(0);
    copL[0] = p[0];
    copL[1] = p[1];
    copL[2] = p[2];
    f_Out1 << copL[0] << "," << copL[1] << "," << copL[2] << "\n";

    p = polyFL->GetPoint(1);
    double fL[3];
    fL[0] = p[0];
    fL[1] = p[1];
    fL[2] = p[2];
    f_Out1 << fL[0]-copL[0] << "," << fL[1]-copL[1] << "," << fL[2]-copL[2] << "\n";

    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) size)*25.)));
    }
  }
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_MomentLeft->SetTimeStamp(time);
    m_MomentLeft->Update();
    m_MomentLeft->GetVTKOutput()->Update();
      
    vtkPolyData* polyML = (vtkPolyData*)m_MomentLeft->GetOutput()->GetVTKData();
      
    double* p = polyML->GetPoint(1);
    double mL[3];
    mL[0] = p[0];
    mL[1] = p[1];
    mL[2] = p[2];
    f_Out2 << mL[0]-copL[0] << "," << mL[1]-copL[1] << "," << mL[2]-copL[2] << "\n";

    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(25+((double) i)/((double) size)*25.)));
    }
  }
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_ForceRight->SetTimeStamp(time);
    m_ForceRight->Update();
    m_ForceRight->GetVTKOutput()->Update();
     
    vtkPolyData* polyFR = (vtkPolyData*)m_ForceRight->GetOutput()->GetVTKData();
      
    double* p = polyFR->GetPoint(0);
    copR[0] = p[0];
    copR[1] = p[1];
    copR[2] = p[2];
    f_Out3 << copR[0] << "," << copR[1] << "," << copR[2] << "\n";
      
    p = polyFR->GetPoint(1);
    double fR[3];
    fR[0] = p[0];
    fR[1] = p[1];
    fR[2] = p[2];
    f_Out3 << fR[0]-copR[0] << "," << fR[1]-copR[1] << "," << fR[2]-copR[2] << "\n";
      
    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(50+((double) i)/((double) size)*25.)));
    }
  }
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_MomentRight->SetTimeStamp(time);
    m_MomentRight->Update();
    m_MomentRight->GetVTKOutput()->Update();

    vtkPolyData* polyMR = (vtkPolyData*)m_MomentRight->GetOutput()->GetVTKData();
      
    double* p = polyMR->GetPoint(1);
    double mR[3];
    mR[0] = p[0];
    mR[1] = p[1];
    mR[2] = p[2];
    f_Out4 << mR[0]-copR[0] << "," << mR[1]-copR[1] << "," << mR[2]-copR[2] << "\n";

    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(75+((double) i)/((double) size)*25.)));
    }
  }  

  f_Out1.close();
  f_Out2.close();
  f_Out3.close();
  f_Out4.close();

  wxFileInputStream inputFile1( m_File_temp1 );
  wxTextInputStream text1( inputFile1 );
  wxFileInputStream inputFile2( m_File_temp2 );
  wxTextInputStream text2( inputFile2 );
  wxFileInputStream inputFile3( m_File_temp3 );
  wxTextInputStream text3( inputFile3 );
  wxFileInputStream inputFile4( m_File_temp4 );
  wxTextInputStream text4( inputFile4 );

  wxString line;

  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)0.0));
  }

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
    
    f_Out << bounds1[0] << "," << bounds1[3] << "," << bounds1[4] << "," << bounds1[1] << "," << bounds1[3] << "," << bounds1[4] << ","
        << bounds1[1] << "," << bounds1[2] << "," << bounds1[4] << "," << bounds1[0] << "," << bounds1[2] << "," << bounds1[4] << "\n";
    f_Out << "2,";

    f_Out << bounds2[0] << "," << bounds2[3] << "," << bounds2[4] << "," << bounds2[1] << "," << bounds2[3] << "," << bounds2[4] << ","
        << bounds2[1] << "," << bounds2[2] << "," << bounds2[4] << "," << bounds2[0] << "," << bounds2[2] << "," << bounds2[4] << "\n";

    //Add a blank line 
    f_Out << "\n";

    // Add a dummy line
    f_Out << "--- FORCES VECTOR SECTION (1=PLATE1,2=PLATE2) ---\n";

    // Add a line containing the forces tag
    f_Out << "FRAME,COP1:X,COP1:Y,COP1:Z,Ref1:X,Ref1:Y,Ref1:Z,Force1:X,Force1:Y,Force1:Z,Moment1:X,Moment1:Y,Moment1:Z,COP2:X,COP2:Y,COP2:Z,Ref2:X,Ref2:Y,Ref2:Z,Force2:X,Force2:Y,Force2:Z,Moment2:X,Moment2:Y,Moment2:Z\n";

    //Add a blank line 
    f_Out << "\n";

    // Add time and values. Time is always the first row.
    for (int i=0;i<size;i++)
    {
      double time = kframes.at(i);

      // TIME
      f_Out << time << ",";
        
      // COP L
      line = text1.ReadLine();
      f_Out << line << ",";
        
      // REF L
      f_Out << 0 << "," << 0 << "," << 0 << ",";
        
      // FORCE L
      line = text1.ReadLine();
      f_Out << line << ",";

      // MOMENT L
      line = text2.ReadLine();
      f_Out << line << ",";
              
      // COP R
      line = text3.ReadLine();
      f_Out << line << ",";
        
      // REF R
      f_Out << 0 << "," << 0 << "," << 0 << ",";
        
      // FORCE R
      line = text3.ReadLine();
      f_Out << line << ",";
      
      // MOMENT R
      line = text4.ReadLine();
      f_Out << line << "\n";

      if (!m_TestMode)
      {
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) size)*100.)));
      }

    }
    
    f_Out.close();
  }  
  
  info = "";
  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&info));
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
    cppDEL(wait);
  }
}
//----------------------------------------------------------------------------
std::vector<mafTimeStamp> medOpExporterGRFWS::MergeTimeStamps(std::vector<mafTimeStamp> kframes1,std::vector<mafTimeStamp> kframes2)   
//----------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> kframes;

  int i = 0;
  int j = 0;

  if (kframes1.size()==0 && kframes2.size()==0)
  {
    kframes.resize(0);
    return kframes;
  }

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
  } while (i<(kframes1.size()) && j<(kframes2.size()));

  return kframes;
}