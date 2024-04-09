/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterGRFWS
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
#include "albaOpExporterGRFWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "albaGUI.h"
#include "albaTagArray.h"

#include <fstream>
#include <iostream>

#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkDataSet.h>
#include <vtkPolyData.h>

#include <albaTransformBase.h>
#include <albaMatrix.h>
#include <albaGUIRollOut.h>
#include "albaProgressBarHelper.h"

using namespace std;

#define TAG_FORMAT "FORCE PLATES"
#define TAG_FORMAT_V "VECTOR"
#define FREQ 1.00
#define DELTA 5.0

//----------------------------------------------------------------------------
albaOpExporterGRFWS::albaOpExporterGRFWS(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_EXPORTER;
	m_Canundo	= true;
	m_File		= "";

  m_AdvanceSettings = NULL;
  m_PlatformLeft = NULL;
  m_PlatformRight = NULL;
  m_ForceLeft = NULL;
  m_ForceRight = NULL;
  m_MomentLeft = NULL;
  m_MomentRight = NULL;
  m_Group = NULL;
  m_FastMethod = 0;
  m_Resolution = 100;
  m_Treshold.clear();
  m_Treshold.push_back(10);
  m_Treshold.push_back(10);
  m_Treshold.push_back(10);
  m_Treshold.push_back(10);
}
//----------------------------------------------------------------------------
albaOpExporterGRFWS::~albaOpExporterGRFWS()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::Clear()
//----------------------------------------------------------------------------
{
  m_PlatformLeft = NULL;
  m_PlatformRight = NULL;
  m_ForceLeft = NULL;
  m_ForceRight = NULL;
  m_MomentLeft = NULL;
  m_MomentRight = NULL;
  m_AdvanceSettings = NULL;
  m_Treshold.clear();
}
//----------------------------------------------------------------------------
bool albaOpExporterGRFWS::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  Clear();
  return (node && ( node->IsA("albaVMEVector") || LoadVMEs(node) ) );
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterGRFWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpExporterGRFWS *cp = new albaOpExporterGRFWS(m_Label);
	cp->m_File = m_File;
  cp->m_Resolution = m_Resolution;
  cp->m_FastMethod = m_FastMethod;
  for (int i=0;i<m_Treshold.size();i++)
  {
    (cp->m_Treshold).push_back(m_Treshold.at(i));
  }
	return cp;
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::OpRun()   
//----------------------------------------------------------------------------
{
  // Load inputs
  if (m_Input->IsA("albaVMEVector"))
  {
    m_ForceLeft = albaVMEVector::SafeDownCast(m_Input); 
  }
  else
  {
    LoadVMEs(m_Input);
  }
  // Create GUI
  if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
  CalculateTresholds();
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->SetOwnForegroundColour(wxColour(255,0,0));
  m_Gui->Label("This Operation may take seconds or hours,", false);
  m_Gui->Label("depending on your input MSF file size.", false);
  m_Gui->Label("Consider running this Op after an Importer", false);
  m_Gui->Label("(C3D or Force Plates); it will always take", false);
  m_Gui->Label("just few seconds!", false);
  m_Gui->Divider(0);
  m_Gui->Label("In any case, you can try the fast method which", false);
  m_Gui->Label("guesses the vector position time by time.", false);
  m_Gui->Label("This will reduce the time required by the Op,", false);
  m_Gui->Label("but works well only if the input vectors", false);
  m_Gui->Label("DO NOT FLIP DIRECTIONS TOO", true);
  m_Gui->Label("QUICKLY!", true);

  m_Gui->Divider(0);
  m_Gui->Bool(ID_FAST,"Try fast method",&m_FastMethod,1,"Fast method");
  m_Gui->Divider(0);
  m_Gui->Divider(1);
  m_Gui->Divider(0);

  m_AdvanceSettings = new albaGUI(this);
  m_AdvanceSettings->Divider(0);
  m_AdvanceSettings->Integer(ID_RES,"Resolution",&m_Resolution,0,1000,2,"Resolution");
  m_AdvanceSettings->Divider(0);
  m_AdvanceSettings->Label(" Tresholds");
  m_AdvanceSettings->Double(ID_FL,"Tr. FL",&(m_Treshold.at(0)),0,1.0e10,2,"Treshold");
  m_AdvanceSettings->Double(ID_ML,"Tr. ML",&(m_Treshold.at(1)),0,1.0e10,2,"Treshold");
  m_AdvanceSettings->Double(ID_FR,"Tr. FR",&(m_Treshold.at(2)),0,1.0e10,2,"Treshold");
  m_AdvanceSettings->Double(ID_MR,"Tr. MR",&(m_Treshold.at(3)),0,1.0e10,2,"Treshold");
  m_AdvanceSettings->Enable(ID_RES,false);
  m_AdvanceSettings->Enable(ID_FL,false);
  m_AdvanceSettings->Enable(ID_ML,false);
  m_AdvanceSettings->Enable(ID_FR,false);
  m_AdvanceSettings->Enable(ID_MR,false);
  m_AdvanceSettings->Update();
  albaGUIRollOut* roll = new albaGUIRollOut(m_Gui,"Advance Settings",m_AdvanceSettings,-1,false); // set an id

  m_Gui->Divider(0);
  m_Gui->OkCancel();
}

//----------------------------------------------------------------------------
void albaOpExporterGRFWS::OpDo()   
//----------------------------------------------------------------------------
{
  // INPUT is a VECTOR:
  if (m_Input->IsA("albaVMEVector"))
  {
    wxString proposed = albaGetLastUserFolder();
    proposed += m_Input->GetName();
    proposed += "_VECTOR";
    proposed += ".csv";

    wxString wildc = "ASCII CSV file (*.csv)|*.csv";
    wxString f = albaGetSaveFile(proposed,wildc).ToAscii(); 

    if(!f.IsEmpty())
    {
      SetFileName(f.ToAscii());
      if (!m_FastMethod)
      {
        WriteSingleVector();
      }
      else
      {
        WriteSingleVectorFast();
      }
    }

  }
  // INPUT is a FORCE PLATE: Load and Execute 
  else if (m_ForceLeft && m_ForceRight && m_MomentLeft && m_MomentRight)
  {
    wxString proposed = albaGetLastUserFolder();
    proposed += m_Input->GetName();
    proposed += "_FORCEPLATES";
    proposed += ".csv";

    wxString wildc = "ASCII CSV file (*.csv)|*.csv";
    wxString f = albaGetSaveFile(proposed,wildc).ToAscii(); 

    if(!f.IsEmpty())
    {
      SetFileName(f.ToAscii());
      if (!m_FastMethod)
      {
        Write();
      }
      else
      {
        WriteFast();
      }
    }
  }
  else
  {
    wxMessageBox("Need 2 PLATFORMS, each with a FORCE vector and a MOMENT vector!","GRF Exporter Warning",wxOK | wxICON_ERROR);
    albaEventMacro(albaEvent(this,OP_RUN_CANCEL));
  }
  RemoveTempFiles();
}

//----------------------------------------------------------------------------
void albaOpExporterGRFWS::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case ID_FAST:
      {
        if (!m_TestMode)
        {
          if (m_FastMethod)
          {
            m_AdvanceSettings->Enable(ID_RES,true);
            if (m_ForceLeft)
            {
              m_AdvanceSettings->Enable(ID_FL,true);
            }
            if (m_MomentLeft)
            {
              m_AdvanceSettings->Enable(ID_ML,true);
            }
            if (m_ForceRight)
            {
              m_AdvanceSettings->Enable(ID_FR,true);
            }
            if (m_MomentRight)
            {
              m_AdvanceSettings->Enable(ID_MR,true);
            }
          }
          else
          {
            m_AdvanceSettings->Enable(ID_RES,false);
            m_AdvanceSettings->Enable(ID_FL,false);
            m_AdvanceSettings->Enable(ID_ML,false);
            m_AdvanceSettings->Enable(ID_FR,false);
            m_AdvanceSettings->Enable(ID_MR,false);
          }
          m_AdvanceSettings->Update();
        }
      }
      break;
    case ID_RES:
      {
        CalculateTresholds();
      }
      break;
    default:
      albaEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
int albaOpExporterGRFWS::LoadVMEs(albaVME* node)   
//----------------------------------------------------------------------------
{
  int result = 0;
  // Get input
  const albaVME::albaChildrenVector* children = node->GetChildren();
  for(int i = 0; i < children->size(); i++)
  {
    albaVME *child = children->at(i);
    if (child->IsA("albaVMESurface"))
    {
      if (m_PlatformLeft==NULL)
      {
        m_PlatformLeft = albaVMESurface::SafeDownCast(child);
      }
      else if (m_PlatformRight==NULL)
      {
        m_PlatformRight = albaVMESurface::SafeDownCast(child);
      }
    }
    if (child->IsA("albaVMEVector"))
    {
      if (m_ForceLeft==NULL)
      {
        m_ForceLeft = albaVMEVector::SafeDownCast(child);
      }
      else if (m_MomentLeft==NULL)
      {
        m_MomentLeft = albaVMEVector::SafeDownCast(child);
      }
      else if (m_ForceRight==NULL)
      {
        m_ForceRight = albaVMEVector::SafeDownCast(child);
      }
      else if (m_MomentRight==NULL)
      {
        m_MomentRight = albaVMEVector::SafeDownCast(child);
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
void albaOpExporterGRFWS::Write()   
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait = NULL;
  albaString info = "Loading data from files";
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("This may take several minutes, please be patient...");
	progressHelper.SetBarText(info);
	 

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

  wxString file1 = m_File + "_tmp1";
  wxString file2 = m_File + "_tmp2";
  wxString file3 = m_File + "_tmp3";
  wxString file4 = m_File + "_tmp4";

  std::ofstream f_Out(m_File.ToAscii());
  std::ofstream f_Out1(file1.ToAscii());
  std::ofstream f_Out2(file2.ToAscii());
  std::ofstream f_Out3(file3.ToAscii());
  std::ofstream f_Out4(file4.ToAscii());

  std::vector<albaTimeStamp> kframes1;
  std::vector<albaTimeStamp> kframes2;
  m_ForceLeft->GetTimeStamps(kframes1);
  m_ForceRight->GetTimeStamps(kframes2);
  std::vector<albaTimeStamp> kframes = MergeTimeStamps(kframes1,kframes2);
  int size = kframes.size();

  //Add times and values; time is always the first row
  double copL[3][100000];   
  double copR[3][100000];
  // ---------------------
  // FORCE LEFT ----------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_ForceLeft->SetTimeStamp(time);
    m_ForceLeft->Update();
    m_ForceLeft->GetVTKOutput()->Update();
      
    vtkPolyData* polyFL = (vtkPolyData*)m_ForceLeft->GetOutput()->GetVTKData();
     
    double* p = polyFL->GetPoint(0);
    copL[0][i] = p[0];
    copL[1][i] = p[1];
    copL[2][i] = p[2];
    f_Out1 << copL[0][i] << "," << copL[1][i] << "," << copL[2][i] << "\n";

    p = polyFL->GetPoint(1);
    double fL[3];
    fL[0] = p[0];
    fL[1] = p[1];
    fL[2] = p[2];
    f_Out1 << fL[0]-copL[0][i] << "," << fL[1]-copL[1][i] << "," << fL[2]-copL[2][i] << "\n";

		progressHelper.UpdateProgressBar(((double) i)/((double) size)*25.);
  }
  // ---------------------
  // MOMENT LEFT ---------
  // ---------------------
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
    f_Out2 << mL[0]-copL[0][i] << "," << mL[1]-copL[1][i] << "," << mL[2]-copL[2][i] << "\n";

    progressHelper.UpdateProgressBar(25+((double) i)/((double) size)*25.);
  }  
  // ---------------------
  // FORCE RIGHT ---------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    m_ForceRight->SetTimeStamp(time);
    m_ForceRight->Update();
    m_ForceRight->GetVTKOutput()->Update();
     
    vtkPolyData* polyFR = (vtkPolyData*)m_ForceRight->GetOutput()->GetVTKData();
      
    double* p = polyFR->GetPoint(0);
    copR[0][i] = p[0];
    copR[1][i] = p[1];
    copR[2][i] = p[2];
    f_Out3 << copR[0][i] << "," << copR[1][i] << "," << copR[2][i] << "\n";
      
    p = polyFR->GetPoint(1);
    double fR[3];
    fR[0] = p[0];
    fR[1] = p[1];
    fR[2] = p[2];
    f_Out3 << fR[0]-copR[0][i] << "," << fR[1]-copR[1][i] << "," << fR[2]-copR[2][i] << "\n";
      
    progressHelper.UpdateProgressBar((50+((double) i)/((double) size)*25.));
  }
  // ---------------------
  // MOMENT RIGHT --------
  // ---------------------
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
    f_Out4 << mR[0]-copR[0][i] << "," << mR[1]-copR[1][i] << "," << mR[2]-copR[2][i] << "\n";

    progressHelper.UpdateProgressBar((75+((double) i)/((double) size)*25.));
  }  

  f_Out1.close();
  f_Out2.close();
  f_Out3.close();
  f_Out4.close();

  wxFileInputStream inputFile1( file1 );
  wxTextInputStream text1( inputFile1 );
  wxFileInputStream inputFile2( file2 );
  wxTextInputStream text2( inputFile2 );
  wxFileInputStream inputFile3( file3 );
  wxTextInputStream text3( inputFile3 );
  wxFileInputStream inputFile4( file4 );
  wxTextInputStream text4( inputFile4 );

  wxString line;

  progressHelper.ResetProgress();

  // Write to final file
  if (!f_Out.bad())
  {
    // Add TAG
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

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

      progressHelper.UpdateProgressBar(((double) i)/((double) size)*100.);
    }
    
    f_Out.close();
  }  

}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::WriteFast()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("This may take several minutes, please be patient...");
	

  // Must update VTK!
  m_PlatformLeft->Update();
  m_PlatformLeft->GetVTKOutput()->Update();
  m_PlatformRight->Update();
  m_PlatformRight->GetVTKOutput()->Update();

  double bounds1[6];
  bounds1[0] = 0;
  m_PlatformLeft->GetOutput()->GetVTKData()->GetBounds(bounds1);
  bounds1[4] = bounds1[4] + DELTA;
  double bounds2[6];
  bounds2[0] = 0;
  m_PlatformRight->GetOutput()->GetVTKData()->GetBounds(bounds2);
  bounds2[4] = bounds2[4] + DELTA;

  wxString file1 = m_File + "_tmp1";
  wxString file2 = m_File + "_tmp2";
  wxString file3 = m_File + "_tmp3";
  wxString file4 = m_File + "_tmp4";

  std::ofstream f_Out(m_File.ToAscii());
  std::ofstream f_Out1(file1.ToAscii());
  std::ofstream f_Out2(file2.ToAscii());
  std::ofstream f_Out3(file3.ToAscii());
  std::ofstream f_Out4(file4.ToAscii());

  std::vector<albaTimeStamp> kframes1;
  std::vector<albaTimeStamp> kframes2;
  m_ForceLeft->GetTimeStamps(kframes1);
  m_ForceRight->GetTimeStamps(kframes2);
  std::vector<albaTimeStamp> kframes = MergeTimeStamps(kframes1,kframes2);
  int size = kframes.size();

  // Get first data to set right input position
  m_ForceLeft->SetTimeStamp(kframes.at(0));
  m_ForceLeft->Update();
  m_MomentLeft->SetTimeStamp(kframes.at(0));
  m_MomentLeft->Update();
  m_ForceRight->SetTimeStamp(kframes.at(0));
  m_ForceRight->Update();
  m_MomentRight->SetTimeStamp(kframes.at(0));
  m_MomentRight->Update();

  double* init_posCOPL = m_ForceLeft->GetOutput()->GetVTKData()->GetPoint(0);
  double* init_posCOPR = m_ForceRight->GetOutput()->GetVTKData()->GetPoint(0);
  double* init_posML = m_MomentLeft->GetOutput()->GetVTKData()->GetPoint(1);
  double* init_posMR = m_MomentRight->GetOutput()->GetVTKData()->GetPoint(1);

  medGRFVector* vFL = new medGRFVector;
  medGRFVector* vFR = new medGRFVector;
  medGRFVector* vML = new medGRFVector;
  medGRFVector* vMR = new medGRFVector;
  for (int i=0;i<6;i++)
  {
    vFL->m_Bounds[i]=0;
    vFR->m_Bounds[i]=0;
    vML->m_Bounds[i]=0;
    vMR->m_Bounds[i]=0;
  }
  vFL->m_SwitchX = vFL->m_SwitchY = vFL->m_SwitchZ = 0;
  vFR->m_SwitchX = vFR->m_SwitchY = vFR->m_SwitchZ = 0;
  vML->m_SwitchX = vML->m_SwitchY = vML->m_SwitchZ = 0;
  vMR->m_SwitchX = vMR->m_SwitchY = vMR->m_SwitchZ = 0;

  // Generate values and store them in temp files
  double copL[3][100000];   
  double copR[3][100000];
  // ---------------------
  // FORCE LEFT ----------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    // Get bounds
    albaOBB obb;
    m_ForceLeft->GetOutput()->GetBounds(obb,time);
    vFL->m_Bounds[0] = (obb.m_Bounds)[0];
    vFL->m_Bounds[1] = (obb.m_Bounds)[1];
    vFL->m_Bounds[2] = (obb.m_Bounds)[2];
    vFL->m_Bounds[3] = (obb.m_Bounds)[3];
    vFL->m_Bounds[4] = (obb.m_Bounds)[4];
    vFL->m_Bounds[5] = (obb.m_Bounds)[5];

    // Switch direction if case
    int ind1 = ((vFL->m_SwitchX==0) ? 1 : 0);
    int ind2 = ((vFL->m_SwitchY==0) ? 2 : 3);
    int ind3 = ((vFL->m_SwitchZ==0) ? 4 : 5);
    CheckVectorToSwitch(i,ind1,ind2,ind3,vFL,init_posCOPL,0,m_ForceLeft,0,time);
    ind1 = ((vFL->m_SwitchX==0) ? 1 : 0);
    ind2 = ((vFL->m_SwitchY==0) ? 2 : 3);
    ind3 = ((vFL->m_SwitchZ==0) ? 4 : 5);
          
    // COP L
    copL[0][i] = vFL->m_Bounds[ind1];
    copL[1][i] = vFL->m_Bounds[ind2];
    copL[2][i] = vFL->m_Bounds[ind3];
    f_Out1 << copL[0][i] << "," << copL[1][i] << "," << copL[2][i] << "\n";

    // FORCE L
    ind1 = ((vFL->m_SwitchX==0) ? 0 : 1);
    ind2 = ((vFL->m_SwitchY==0) ? 3 : 2);
    ind3 = ((vFL->m_SwitchZ==0) ? 5 : 4);
    double fL[3];
    fL[0] =  vFL->m_Bounds[ind1];
    fL[1] =  vFL->m_Bounds[ind2];
    fL[2] =  vFL->m_Bounds[ind3];
    f_Out1 << fL[0]-copL[0][i] << "," << fL[1]-copL[1][i] << "," << fL[2]-copL[2][i] << "\n";

    progressHelper.UpdateProgressBar(((double) i)/((double) size)*25.);
  }  
  // ---------------------
  // MOMENT LEFT ---------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    // Get bounds
    albaOBB obb;
    m_MomentLeft->GetOutput()->GetBounds(obb,time);
    vML->m_Bounds[0] = (obb.m_Bounds)[0];
    vML->m_Bounds[1] = (obb.m_Bounds)[1];
    vML->m_Bounds[2] = (obb.m_Bounds)[2];
    vML->m_Bounds[3] = (obb.m_Bounds)[3];
    vML->m_Bounds[4] = (obb.m_Bounds)[4];
    vML->m_Bounds[5] = (obb.m_Bounds)[5];

    // Switch direction if case
    int ind1 = ((vML->m_SwitchX==0) ? 1 : 0);
    int ind2 = ((vML->m_SwitchY==0) ? 3 : 2);
    int ind3 = ((vML->m_SwitchZ==0) ? 5 : 4);
    CheckVectorToSwitch(i,ind1,ind2,ind3,vML,init_posML,1,m_MomentLeft,1,time);
    ind1 = ((vML->m_SwitchX==0) ? 1 : 0);
    ind2 = ((vML->m_SwitchY==0) ? 3 : 2);
    ind3 = ((vML->m_SwitchZ==0) ? 5 : 4);

    // MOMENT L
    double mL[3];
    mL[0] = vML->m_Bounds[ind1];
    mL[1] = vML->m_Bounds[ind2];
    mL[2] = vML->m_Bounds[ind3];
    f_Out2 << mL[0]-copL[0][i] << "," << mL[1]-copL[1][i] << "," << mL[2]-copL[2][i] << "\n";

    progressHelper.UpdateProgressBar(25+((double) i)/((double) size)*25.);
  }  
  // ---------------------
  // FORCE RIGHT ---------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    // Get bounds
    albaOBB obb;
    m_ForceRight->GetOutput()->GetBounds(obb,time);
    vFR->m_Bounds[0] = (obb.m_Bounds)[0];
    vFR->m_Bounds[1] = (obb.m_Bounds)[1];
    vFR->m_Bounds[2] = (obb.m_Bounds)[2];
    vFR->m_Bounds[3] = (obb.m_Bounds)[3];
    vFR->m_Bounds[4] = (obb.m_Bounds)[4];
    vFR->m_Bounds[5] = (obb.m_Bounds)[5];

    // Switch direction if case
    int ind1 = ((vFR->m_SwitchX==0) ? 0 : 1);
    int ind2 = ((vFR->m_SwitchY==0) ? 3 : 2);
    int ind3 = ((vFR->m_SwitchZ==0) ? 4 : 5);
    CheckVectorToSwitch(i,ind1,ind2,ind3,vFR,init_posCOPR,2,m_ForceRight,0,time);
    ind1 = ((vFR->m_SwitchX==0) ? 0 : 1);
    ind2 = ((vFR->m_SwitchY==0) ? 3 : 2);
    ind3 = ((vFR->m_SwitchZ==0) ? 4 : 5);

    // COP R
    copR[0][i] = vFR->m_Bounds[ind1];
    copR[1][i] = vFR->m_Bounds[ind2];
    copR[2][i] = vFR->m_Bounds[ind3];
    f_Out3 << copR[0][i] << "," << copR[1][i] << "," << copR[2][i] << "\n";
      
    // FORCE R
    ind1 = (vFR->m_SwitchX==0) ? 1 : 0;
    ind2 = (vFR->m_SwitchY==0) ? 2 : 3;
    ind3 = (vFR->m_SwitchZ==0) ? 5 : 4;
    double fR[3];
    fR[0] = vFR->m_Bounds[ind1];
    fR[1] = vFR->m_Bounds[ind2];
    fR[2] = vFR->m_Bounds[ind3];
    f_Out3 << fR[0]-copR[0][i] << "," << fR[1]-copR[1][i] << "," << fR[2]-copR[2][i] << "\n";
      
    progressHelper.UpdateProgressBar(50+((double) i)/((double) size)*25.);
  }  
  // ---------------------
  // MOMENT RIGHT --------
  // ---------------------
  for (int i=0;i<size;i++)
  {
    double time = kframes.at(i);

    // Get bounds
    albaOBB obb;
    m_MomentRight->GetOutput()->GetBounds(obb,time);
    vMR->m_Bounds[0] = (obb.m_Bounds)[0];
    vMR->m_Bounds[1] = (obb.m_Bounds)[1];
    vMR->m_Bounds[2] = (obb.m_Bounds)[2];
    vMR->m_Bounds[3] = (obb.m_Bounds)[3];
    vMR->m_Bounds[4] = (obb.m_Bounds)[4];
    vMR->m_Bounds[5] = (obb.m_Bounds)[5];

    // Switch direction if case
    int ind1 = ((vMR->m_SwitchX==0) ? 0 : 1);
    int ind2 = ((vMR->m_SwitchY==0) ? 3 : 2);
    int ind3 = ((vMR->m_SwitchZ==0) ? 5 : 4);
    CheckVectorToSwitch(i,ind1,ind2,ind3,vMR,init_posMR,3,m_MomentRight,1,time);
    ind1 = ((vMR->m_SwitchX==0) ? 0 : 1);
    ind2 = ((vMR->m_SwitchY==0) ? 3 : 2);
    ind3 = ((vMR->m_SwitchZ==0) ? 5 : 4);

    // MOMENT R
    double mR[3];
    mR[0] = vMR->m_Bounds[ind1];
    mR[1] = vMR->m_Bounds[ind2];
    mR[2] = vMR->m_Bounds[ind3];
    f_Out4 << mR[0]-copR[0][i] << "," << mR[1]-copR[1][i] << "," << mR[2]-copR[2][i] << "\n";

    progressHelper.UpdateProgressBar(75+((double) i)/((double) size)*25.);
  }  

  f_Out1.close();
  f_Out2.close();
  f_Out3.close();
  f_Out4.close();

  wxFileInputStream inputFile1( file1 );
  wxTextInputStream text1( inputFile1 );
  wxFileInputStream inputFile2( file2 );
  wxTextInputStream text2( inputFile2 );
  wxFileInputStream inputFile3( file3 );
  wxTextInputStream text3( inputFile3 );
  wxFileInputStream inputFile4( file4 );
  wxTextInputStream text4( inputFile4 );

  wxString line;

  progressHelper.ResetProgress();

  // Write to final file
  if (!f_Out.bad())
  {
    // Add TAG
    f_Out << TAG_FORMAT << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

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

      progressHelper.UpdateProgressBar(((double) i)/((double) size)*100.);
    }

    f_Out.close();
  } 

  delete vFL;
  delete vFR;
  delete vML;
  delete vMR;
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::WriteSingleVector()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("This may take several minutes, please be patient...");
  
  std::ofstream f_Out(m_File.ToAscii());

  std::vector<albaTimeStamp> kframes;
  m_ForceLeft->GetTimeStamps(kframes);
  int size = kframes.size();

  if (!f_Out.bad())
  {
    // Add TAG
    f_Out << TAG_FORMAT_V << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

    // Add a dummy line
    f_Out << "--- VECTOR SECTION ---\n";
    
    // Add a line containing the forces tag
    f_Out << "FRAME,COP1:X,COP1:Y,COP1:Z,Ref1:X,Ref1:Y,Ref1:Z,Force1:X,Force1:Y,Force1:Z\n";

    //Add a blank line 
    f_Out << "\n";

    double copL[3]; 
    // ---------------------
    // SINGLE VECTOR -------
    // ---------------------
    for (int i=0;i<size;i++)
    {
      double time = kframes.at(i);

      // TIME
      f_Out << time << ",";

      m_ForceLeft->SetTimeStamp(time);
      m_ForceLeft->Update();
        
      vtkPolyData* polyFL = (vtkPolyData*)m_ForceLeft->GetOutput()->GetVTKData();
       
      double* p = polyFL->GetPoint(0);
      copL[0] = p[0];
      copL[1] = p[1];
      copL[2] = p[2];

      // COP 
      f_Out << copL[0] << "," << copL[1] << "," << copL[2] << ",";

      p = polyFL->GetPoint(1);
      double fL[3];
      fL[0] = p[0];
      fL[1] = p[1];
      fL[2] = p[2];

      // REF 
      f_Out << 0 << "," << 0 << "," << 0 << ",";

      // FORCE
      f_Out << fL[0]-copL[0] << "," << fL[1]-copL[1] << "," << fL[2]-copL[2] << "\n";

      progressHelper.UpdateProgressBar(((double) i)/((double) size)*100.);
    }

    f_Out.close();
  }
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::WriteSingleVectorFast()   
//----------------------------------------------------------------------------
{  
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("This may take several minutes, please be patient...");

  std::ofstream f_Out(m_File.ToAscii());

  std::vector<albaTimeStamp> kframes;
  m_ForceLeft->GetTimeStamps(kframes);
  int size = kframes.size();

  // Get first data to set right input position
  m_ForceLeft->SetTimeStamp(kframes.at(0));
  m_ForceLeft->Update();
  double* init_posCOPL = m_ForceLeft->GetOutput()->GetVTKData()->GetPoint(0);
  medGRFVector* vFL = new medGRFVector;
  for (int i=0;i<6;i++)
  {
    vFL->m_Bounds[i]=0;
  }
  vFL->m_SwitchX = vFL->m_SwitchY = vFL->m_SwitchZ = 0;

  if (!f_Out.bad())
  {
    // Add TAG
    f_Out << TAG_FORMAT_V << "\n";

    // Add the first row containing the frequency
    f_Out << FREQ << ",\n";

    // Add a dummy line
    f_Out << "--- VECTOR SECTION ---\n";
    
    // Add a line containing the forces tag
    f_Out << "FRAME,COP1:X,COP1:Y,COP1:Z,Ref1:X,Ref1:Y,Ref1:Z,Force1:X,Force1:Y,Force1:Z\n";

    //Add a blank line 
    f_Out << "\n";

    // ---------------------
    // SINGLE VECTOR -------
    // ---------------------
    for (int i=0;i<size;i++)
    {
      double time = kframes.at(i);

      // TIME
      f_Out << time << ",";

      // Get bounds
      albaOBB obb;
      m_ForceLeft->GetOutput()->GetBounds(obb,time);
      vFL->m_Bounds[0] = (obb.m_Bounds)[0];
      vFL->m_Bounds[1] = (obb.m_Bounds)[1];
      vFL->m_Bounds[2] = (obb.m_Bounds)[2];
      vFL->m_Bounds[3] = (obb.m_Bounds)[3];
      vFL->m_Bounds[4] = (obb.m_Bounds)[4];
      vFL->m_Bounds[5] = (obb.m_Bounds)[5];

      // Switch direction if case
      int ind1 = ((vFL->m_SwitchX==0) ? 1 : 0);
      int ind2 = ((vFL->m_SwitchY==0) ? 2 : 3);
      int ind3 = ((vFL->m_SwitchZ==0) ? 4 : 5);
      CheckVectorToSwitch(i,ind1,ind2,ind3,vFL,init_posCOPL,0,m_ForceLeft,0,time);
      ind1 = ((vFL->m_SwitchX==0) ? 1 : 0);
      ind2 = ((vFL->m_SwitchY==0) ? 2 : 3);
      ind3 = ((vFL->m_SwitchZ==0) ? 4 : 5);
          
      // COP 
      double copL[3];
      copL[0] = vFL->m_Bounds[ind1];
      copL[1] = vFL->m_Bounds[ind2];
      copL[2] = vFL->m_Bounds[ind3];
      f_Out << copL[0] << "," << copL[1] << "," << copL[2] << ",";

      // REF 
      f_Out << 0 << "," << 0 << "," << 0 << ",";

      // FORCE 
      ind1 = ((vFL->m_SwitchX==0) ? 0 : 1);
      ind2 = ((vFL->m_SwitchY==0) ? 3 : 2);
      ind3 = ((vFL->m_SwitchZ==0) ? 5 : 4);
      double fL[3];
      fL[0] =  vFL->m_Bounds[ind1];
      fL[1] =  vFL->m_Bounds[ind2];
      fL[2] =  vFL->m_Bounds[ind3];
      f_Out << fL[0]-copL[0] << "," << fL[1]-copL[1] << "," << fL[2]-copL[2] << "\n";

			progressHelper.UpdateProgressBar(((double) i)/((double) size)*100.);      
    }

    f_Out.close();
  }

  delete vFL;
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::CalculateTresholds()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait = NULL;
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    wait = new wxBusyInfo("Calculating tresholds...");
  }
  // Calculate tresholds
  if (m_ForceLeft)
  {
    m_Treshold.at(0) = CalculateTreshold(m_ForceLeft);
  }
  if (m_MomentLeft)
  {
    m_Treshold.at(1) = CalculateTreshold(m_MomentLeft);
  }
  if (m_ForceRight)
  {
    m_Treshold.at(2) = CalculateTreshold(m_ForceRight);
  }
  if (m_MomentRight)
  {
    m_Treshold.at(3) = CalculateTreshold(m_MomentRight);
  }

  if (!m_TestMode)
  {
    m_AdvanceSettings->Update();  
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
    cppDEL(wait);
  }
}
//----------------------------------------------------------------------------
double albaOpExporterGRFWS::CalculateTreshold(albaVMEVector* v)
//----------------------------------------------------------------------------
{
  std::vector<double*> new_value;
  std::vector<double*> old_value;
  double max[3];
  max[0] = max[1] = max[2] = 0;
  if (v)
  {
    std::vector<albaTimeStamp> kframes;
    std::vector<albaTimeStamp> sframes;
    v->GetTimeStamps(kframes);
    int mod = (int)(kframes.size()/m_Resolution);
    for (int i=0;i<m_Resolution;i++)
    {
      if ((i*mod)<(kframes.size()))
      {
        sframes.push_back(kframes.at(i*mod));
      }
    }
    for (int i=0;i<sframes.size();i++)
    {
      albaTimeStamp t = sframes.at(i);
      
      albaOBB obb;
      v->GetOutput()->GetBounds(obb,t);
      double* p = new double[3];
      p[0] = (obb.m_Bounds)[1]-(obb.m_Bounds)[0];
      p[1] = (obb.m_Bounds)[3]-(obb.m_Bounds)[2];
      p[2] = (obb.m_Bounds)[5]-(obb.m_Bounds)[4];
      new_value.push_back(p);
      double maxx = 0;
      double maxy = 0; 
      double maxz = 0; 
      int s1 = old_value.size();
      int s2 = new_value.size();
      if (s1>0)
      {
        maxx = abs( (old_value.at(s1-1))[0] - (new_value.at(s2-1))[0] );
        maxy = abs( (old_value.at(s1-1))[1] - (new_value.at(s2-1))[1] );
        maxz = abs( (old_value.at(s1-1))[2] - (new_value.at(s2-1))[2] );
      }
      old_value.push_back(p);
      if (maxx > max[0])
      {
        max[0] = maxx;
      }
      if (maxy > max[1])
      {
        max[1] = maxy;
      }
      if (maxz > max[2])
      {
        max[2] = maxz;
      }
    }
  }
  double r = ((max[0]>max[1])?max[0]:max[1]);
  r = ((r>max[2])?r:max[2]);

  for (int i=0;i<new_value.size();i++)
  {
    cppDEL(new_value.at(i));
  }
  new_value.clear();
  old_value.clear();

  return r;
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::CheckVectorToSwitch(int frame, int coor1_ID, int coor2_ID, int coor3_ID, medGRFVector* v, double* original_pos, int tr, albaVMEVector* vVME, int pointID, albaTimeStamp time)
//----------------------------------------------------------------------------
{
  /* Check right input position:
  This method guesses frame by frame for each vector if the vector is going to change direction (by passing through its origin).
  In this case we must flip the vector coordinates.
  The vector direction are under control by cheking if its bounds, after decreasing, suddenly increase. This could be a signal the vector is changing
  direction and in this case the origina VTK data is loaded and the original position is controlled.
  THIS METHOD HAS BEEN IMPLEMENTED TO REDUCE SIGNIFICATIVELY THE TIME CONSUMPTION, BUT IT'S NOT SAFE, even if it has been tested to work 100% of the times.
  */

  // Flip check for corners position
  if (frame==0)
  {
    if (abs(v->m_Bounds[coor1_ID]-original_pos[0])>0.05)
    {
      v->m_SwitchX = !v->m_SwitchX;
    }
    if (abs(v->m_Bounds[coor2_ID]-original_pos[1])>0.05)
    {
      v->m_SwitchY = !v->m_SwitchY;
    }
    if (abs(v->m_Bounds[coor3_ID]-original_pos[2])>0.05)
    {
      v->m_SwitchZ = !v->m_SwitchZ;
    }
  }
  // Calculate distance
  else if (frame==1)
  {
    double distance[3];
    distance[0] = abs((v->m_Bounds[coor1_ID])-(v->m_Bounds[!coor1_ID]));
    distance[1] = abs((v->m_Bounds[coor2_ID])-(v->m_Bounds[5-coor2_ID]));
    distance[2] = abs((v->m_Bounds[coor3_ID])-(v->m_Bounds[9-coor3_ID]));
    for (int j=0;j<3;j++)
    {
      v->m_OldDistance[j] = distance[j];
    }
    // Must check ever!
    if (true)
    {
      vVME->SetTimeStamp(time);
      vVME->Update();
      double* pos = vVME->GetOutput()->GetVTKData()->GetPoint(pointID);
      CheckVectorToSwitch(0,coor1_ID,coor2_ID,coor3_ID,v,pos);
    }
  }
  // Calculate distance and growing flag
  else if (frame==2)
  {    
    double distance[3];
    distance[0] = abs((v->m_Bounds[coor1_ID])-(v->m_Bounds[!coor1_ID]));
    distance[1] = abs((v->m_Bounds[coor2_ID])-(v->m_Bounds[5-coor2_ID]));
    distance[2] = abs((v->m_Bounds[coor3_ID])-(v->m_Bounds[9-coor3_ID]));
    for (int j=0;j<3;j++)
    {
      if (distance[j]<=v->m_OldDistance[j]) 
      {
        v->m_IsDecreasing[j] = 1;
      }
      else
      {
        v->m_IsDecreasing[j] = 0;
      }
      v->m_OldDistance[j] = distance[j];
      v->m_WasDecreasing[j] = v->m_IsDecreasing[j];
    }
    // Must check ever!
    if (true)
    {
      vVME->SetTimeStamp(time);
      vVME->Update();
      double* pos = vVME->GetOutput()->GetVTKData()->GetPoint(pointID);
      CheckVectorToSwitch(0,coor1_ID,coor2_ID,coor3_ID,v,pos);
    }
  }
  // Check if the vector is changing direction. In case load VTK data and perform the flip check
  else
  {
    double distance[3];
    distance[0] = abs((v->m_Bounds[coor1_ID])-(v->m_Bounds[!coor1_ID]));
    distance[1] = abs((v->m_Bounds[coor2_ID])-(v->m_Bounds[5-coor2_ID]));
    distance[2] = abs((v->m_Bounds[coor3_ID])-(v->m_Bounds[9-coor3_ID]));
    bool switched = 0;
    for (int j=0;j<3;j++)
    {
      if (distance[j]<=v->m_OldDistance[j])
      {
        v->m_IsDecreasing[j] = 1;
      }
      else
      {
        v->m_IsDecreasing[j] = 0;
      }
      if (!switched && (distance[j]<m_Treshold[tr] && v->m_OldDistance[j]<m_Treshold[tr]) && (v->m_WasDecreasing[j] && !v->m_IsDecreasing[j]) )
      {
        vVME->SetTimeStamp(time);
        vVME->Update();
        double* pos = vVME->GetOutput()->GetVTKData()->GetPoint(pointID);
        CheckVectorToSwitch(0,coor1_ID,coor2_ID,coor3_ID,v,pos);
        switched = 1;
      }
      v->m_OldDistance[j] = distance[j];
      v->m_WasDecreasing[j] = v->m_IsDecreasing[j];
    }
  }
}
//----------------------------------------------------------------------------
std::vector<albaTimeStamp> albaOpExporterGRFWS::MergeTimeStamps(std::vector<albaTimeStamp> kframes1,std::vector<albaTimeStamp> kframes2)   
//----------------------------------------------------------------------------
{
  std::vector<albaTimeStamp> kframes;

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
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::SetFileName(const char *file_name)   
//----------------------------------------------------------------------------
{
  m_File = file_name;

  m_File_temp1 = m_File + "_tmp1";
  m_File_temp2 = m_File + "_tmp2";
  m_File_temp3 = m_File + "_tmp3";
  m_File_temp4 = m_File + "_tmp4";
}
//----------------------------------------------------------------------------
void albaOpExporterGRFWS::RemoveTempFiles()   
//----------------------------------------------------------------------------
{
  remove(m_File_temp1);
  remove(m_File_temp2);
  remove(m_File_temp3);
  remove(m_File_temp4);
}