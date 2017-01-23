/*=========================================================================

 Program: MAF2
 Module: mafOpImporterGRFWS
 Authors: Roberto Mucci, Simone Brazzale
 
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

#include "mafOpImporterGRFWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafVMEVector.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"

#include "mafGUI.h"

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkMAFSmartPointer.h"
#include "vtkCellArray.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <iostream>
#include "mafProgressBarHelper.h"

#define DELTA 5.0

//----------------------------------------------------------------------------
mafOpImporterGRFWS::mafOpImporterGRFWS(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType       	= OPTYPE_IMPORTER;
	m_Canundo	      = true;
	m_File		      = "";
	m_FileDir       = mafGetLastUserFolder().c_str();
  m_Output        = NULL;
  m_PlatformLeft  = NULL;
  m_PlatformRight = NULL;
  m_ForceLeft    = NULL;
  m_ForceRight   = NULL;
  m_MomentLeft    = NULL;
  m_MomentRight   = NULL;
  m_Group         = NULL;
}
//----------------------------------------------------------------------------
mafOpImporterGRFWS::~mafOpImporterGRFWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_PlatformLeft);
  mafDEL(m_PlatformRight);
  mafDEL(m_ForceLeft);
  mafDEL(m_ForceRight);
  mafDEL(m_MomentLeft);
  mafDEL(m_MomentRight);
  mafDEL(m_Group);
}

//----------------------------------------------------------------------------
void mafOpImporterGRFWS::OpUndo()
//----------------------------------------------------------------------------
{   
  if(m_PlatformLeft != NULL)
    GetLogicManager()->VmeRemove(m_PlatformLeft);
  if(m_PlatformRight != NULL)
    GetLogicManager()->VmeRemove(m_PlatformRight);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterGRFWS::Copy()   
//----------------------------------------------------------------------------
{
	mafOpImporterGRFWS *cp = new mafOpImporterGRFWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterGRFWS::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  wxString pgd_wildc	= "GRF File (*.*)|*.*";
  wxString f;
  f = mafGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterGRFWS::Read()   
//----------------------------------------------------------------------------
{
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;
  line = text.ReadLine(); 

  if (line.CompareTo("FORCE PLATES")== 0)
  {
    ReadForcePlates();
  }
  else if (line.CompareTo("VECTOR")== 0)
  {
    ReadSingleVector();
  }
  else
  {
    mafErrorMessage("Invalid file format!");
    return;
  }
}
//----------------------------------------------------------------------------
void mafOpImporterGRFWS::ReadForcePlates()   
//----------------------------------------------------------------------------
{
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();


  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  wxFileInputStream inputCountFile( m_File );
  wxTextInputStream textCount( inputCountFile );

  wxString line_count;

  int totlines = 0;
  do {
    line_count = textCount.ReadLine();
    totlines++;
  } while (!inputCountFile.Eof());
  totlines = totlines - 10;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;

  mafString platform1St[12];
  mafString platform2St[12];

  double platform1[12];
  double platform2[12];

  line = text.ReadLine(); 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  freq_val = atof(freq.c_str());

  line = text.ReadLine(); //Skip textual lines
  line = text.ReadLine();
   
  //Get values of the corners of the first platforms
  line = text.ReadLine();
  wxStringTokenizer corners1(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  mafString junk = corners1.GetNextToken(); //Value to ignore
  for (int i = 0 ; i < 12 ; i++)
  {
    platform1St[i] = corners1.GetNextToken().c_str();
    platform1[i] = atof(platform1St[i]);
  }
  
  //Get values of the corners of the second platforms
  line = text.ReadLine();
  wxStringTokenizer corners2(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  junk = corners2.GetNextToken(); //Value to ignore
  for (int i = 0 ; i < 12 ; i++)
  {
    platform2St[i] = corners2.GetNextToken().c_str();
    platform2[i] = atof(platform2St[i]);
  }

  vtkMAFSmartPointer<vtkCubeSource> platformLeft;
  vtkMAFSmartPointer<vtkCubeSource> platformRight;

  //Get values for platforms
   mafNEW(m_PlatformLeft);
   mafNEW(m_PlatformRight);

   mafString PlatNameLeft = name;
   mafString platNameRight = name;
   PlatNameLeft << "_PLATFORM_1";
   platNameRight << "_PLATFORM_2";

   m_PlatformLeft->SetName(PlatNameLeft);
   m_PlatformRight->SetName(platNameRight);

   mafNEW(m_Group);
   m_Group->SetName(name);
  
  double thickness1 = platform1[2]-DELTA;
  double thickness2 = platform2[2]-DELTA;

  platformLeft->SetBounds(platform1[0],platform1[3],platform1[7],platform1[1],thickness1,platform1[2]);
  platformRight->SetBounds(platform2[0],platform2[3],platform2[7],platform2[1],thickness2,platform2[2]);

  line = text.ReadLine(); //Ignore lines
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();

  //Read vector data
  
  mafString timeSt;
  mafTimeStamp time;

  mafString cop1StX,cop1StY,cop1StZ,ref1StX,ref1StY,ref1StZ,force1StX,force1StY,force1StZ,moment1StX,moment1StY,moment1StZ;
  mafString cop2StX,cop2StY,cop2StZ,ref2StX,ref2StY,ref2StZ,force2StX,force2StY,force2StZ,moment2StX,moment2StY,moment2StZ;

  vtkMAFSmartPointer<vtkPolyData> force1;
  vtkMAFSmartPointer<vtkPolyData> force2;
  vtkMAFSmartPointer<vtkPolyData> moment1;
  vtkMAFSmartPointer<vtkPolyData> moment2;
  vtkMAFSmartPointer<vtkPoints> pointsf1;
  vtkMAFSmartPointer<vtkPoints> pointsf2;
  vtkMAFSmartPointer<vtkCellArray> cellArrayf1;
  vtkMAFSmartPointer<vtkCellArray> cellArrayf2;
  vtkMAFSmartPointer<vtkPoints> pointsm1;
  vtkMAFSmartPointer<vtkPoints> pointsm2;
  vtkMAFSmartPointer<vtkCellArray> cellArraym1;
  vtkMAFSmartPointer<vtkCellArray> cellArraym2;
  int pointId1[2];
  int pointId2[2];

  mafNEW(m_ForceLeft);
  mafNEW(m_ForceRight);
  mafNEW(m_MomentLeft);
  mafNEW(m_MomentRight);

  mafString alLeft = name;
  mafString alRight = name;
  alLeft << "_GRF_1";
  alRight << "_GRF_2";
  
  m_ForceLeft->SetName(alLeft);
  m_ForceRight->SetName(alRight);

  mafString almLeft = name;
  mafString almRight = name;
  almLeft << "_MOMENT_1";
  almRight << "_MOMENT_2";
  
  m_MomentLeft->SetName(almLeft);
  m_MomentRight->SetName(almRight);

  int count = 0;
  do 
  {
    line = text.ReadLine();
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    timeSt = tkz.GetNextToken().c_str();
    time = atof(timeSt)/freq_val; 
   
    //Values of the first platform
    cop1StX = tkz.GetNextToken().c_str();
    cop1StY = tkz.GetNextToken().c_str();
    cop1StZ = tkz.GetNextToken().c_str();
    ref1StX = tkz.GetNextToken().c_str();
    ref1StY = tkz.GetNextToken().c_str();
    ref1StZ = tkz.GetNextToken().c_str();
    force1StX = tkz.GetNextToken().c_str();
    force1StY = tkz.GetNextToken().c_str();
    force1StZ = tkz.GetNextToken().c_str();
    moment1StX = tkz.GetNextToken().c_str();
    moment1StY = tkz.GetNextToken().c_str();
    moment1StZ = tkz.GetNextToken().c_str();

    double cop1X = atof(cop1StX);
    double cop1Y = atof(cop1StY);
    double cop1Z = atof(cop1StZ);

    double ref1X = atof(ref1StX);
    double ref1Y = atof(ref1StY);
    double ref1Z = atof(ref1StZ);
   
    double force1X = atof(force1StX);
    double force1Y = atof(force1StY);
    double force1Z = atof(force1StZ);

    double moment1X = atof(moment1StX);
    double moment1Y = atof(moment1StY);
    double moment1Z = atof(moment1StZ);

    if (cop1X != NULL || cop1Y != NULL || cop1Z != NULL)
    {
      // FORCE
      pointsf1->InsertPoint(0, 0, 0, 0);
      pointsf1->InsertPoint(1, force1X, force1Y, force1Z);
      pointId1[0] = 0;
      pointId1[1] = 1;
      cellArrayf1->InsertNextCell(2, pointId1);  
      force1->SetPoints(pointsf1);
      force1->SetLines(cellArrayf1);
      force1->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfForL;
      vtkMAFSmartPointer<vtkTransform> transff;
     
      transff->Translate(cop1X, cop1Y, cop1Z);
      transfForL->SetTransform(transff);
      transfForL->SetInput(force1);
      transfForL->Update();

      m_ForceLeft->SetData(transfForL->GetOutput(), time);
      m_ForceLeft->Modified();
      m_ForceLeft->Update();
      m_ForceLeft->GetOutput()->GetVTKData()->Update();

      // MOMENT
      pointsm1->InsertPoint(0, 0, 0, 0);
      pointsm1->InsertPoint(1, moment1X, moment1Y, moment1Z);
      pointId1[0] = 0;
      pointId1[1] = 1;
      cellArraym1->InsertNextCell(2, pointId1);  
      moment1->SetPoints(pointsm1);
      moment1->SetLines(cellArraym1);
      moment1->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfMomL;
      vtkMAFSmartPointer<vtkTransform> transfm;
     
      transfm->Translate(cop1X, cop1Y, cop1Z);
      transfMomL->SetTransform(transfm);
      transfMomL->SetInput(moment1);
      transfMomL->Update();

      m_MomentLeft->SetData(transfMomL->GetOutput(), time);
      m_MomentLeft->Modified();
      m_MomentLeft->Update();
      m_MomentLeft->GetOutput()->GetVTKData()->Update();
    }

    //Values of the second platform
    cop2StX = tkz.GetNextToken().c_str();
    cop2StY = tkz.GetNextToken().c_str();
    cop2StZ = tkz.GetNextToken().c_str();
    ref2StX = tkz.GetNextToken().c_str();
    ref2StY = tkz.GetNextToken().c_str();
    ref2StZ = tkz.GetNextToken().c_str();
    force2StX = tkz.GetNextToken().c_str();
    force2StY = tkz.GetNextToken().c_str();
    force2StZ = tkz.GetNextToken().c_str();
    moment2StX = tkz.GetNextToken().c_str();
    moment2StY = tkz.GetNextToken().c_str();
    moment2StZ = tkz.GetNextToken().c_str();

    double cop2X = atof(cop2StX);
    double cop2Y = atof(cop2StY);
    double cop2Z = atof(cop2StZ);

    double ref2X = atof(ref2StX);
    double ref2Y = atof(ref2StY);
    double ref2Z = atof(ref2StZ);

    double force2X = atof(force2StX);
    double force2Y = atof(force2StY);
    double force2Z = atof(force2StZ);

    double moment2X = atof(moment2StX);
    double moment2Y = atof(moment2StY);
    double moment2Z = atof(moment2StZ);

    if (cop2X != NULL || cop2Y != NULL || cop2Z != NULL)
    {
      // FORCE
      pointsf2->InsertPoint(0, 0, 0, 0);
      pointsf2->InsertPoint(1, force2X, force2Y, force2Z);
      pointId2[0] = 0;
      pointId2[1] = 1;
      cellArrayf2->InsertNextCell(2, pointId2);  
      force2->SetPoints(pointsf2);
      force2->SetLines(cellArrayf2);
      force2->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfForR;
      vtkMAFSmartPointer<vtkTransform> transffr;
      
      transffr->Translate(cop2X, cop2Y, cop2Z);
      transfForR->SetTransform(transffr);
      transfForR->SetInput(force2);
      transfForR->Update();
 
      m_ForceRight->SetData(transfForR->GetOutput(), time);
      m_ForceRight->Modified();
      m_ForceRight->Update();
      m_ForceRight->GetOutput()->GetVTKData()->Update();

      // MOMENT
      pointsm2->InsertPoint(0, 0, 0, 0);
      pointsm2->InsertPoint(1, moment2X, moment2Y, moment2Z);
      pointId2[0] = 0;
      pointId2[1] = 1;
      cellArraym2->InsertNextCell(2, pointId2);  
      moment2->SetPoints(pointsm2);
      moment2->SetLines(cellArraym2);
      moment2->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfMomR;
      vtkMAFSmartPointer<vtkTransform> transfmr;
      
      transfmr->Translate(cop2X, cop2Y, cop2Z);
      transfMomR->SetTransform(transfmr);
      transfMomR->SetInput(moment2);
      transfMomR->Update();
 
      m_MomentRight->SetData(transfMomR->GetOutput(), time);
      m_MomentRight->Modified();
      m_MomentRight->Update();
      m_MomentRight->GetOutput()->GetVTKData()->Update();
    }

    count++;
    progressHelper.UpdateProgressBar(((double) count)/((double) totlines)*100.);
    

  }while (!inputFile.Eof());

  //Create the mafVMESurface for the platforms
  m_PlatformLeft->SetData(platformLeft->GetOutput(), 0);
  m_PlatformRight->SetData(platformRight->GetOutput(), 0);

  if(m_PlatformLeft != NULL)
  {
    m_PlatformLeft->ReparentTo(m_Group);
    m_ForceLeft->ReparentTo(m_PlatformLeft);
    m_MomentLeft->ReparentTo(m_PlatformLeft);
  }

  if(m_PlatformRight != NULL)
  {
    m_PlatformRight->ReparentTo(m_Group);
    m_ForceRight->ReparentTo(m_PlatformRight);
    m_MomentRight->ReparentTo(m_PlatformRight);
  }

  m_Output = m_Group;
  m_Output->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void mafOpImporterGRFWS::ReadSingleVector()   
//----------------------------------------------------------------------------
{
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  wxFileInputStream inputCountFile( m_File );
  wxTextInputStream textCount( inputCountFile );

  wxString line_count;

  int totlines = 0;
  do {
    line_count = textCount.ReadLine();
    totlines++;
  } while (!inputCountFile.Eof());
  totlines = totlines - 5;

  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;

  line = text.ReadLine(); 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  freq_val = atof(freq.c_str());

  line = text.ReadLine(); 
  line = text.ReadLine();
  line = text.ReadLine(); 

  //Read vector data
  mafString timeSt;
  mafTimeStamp time;

  mafString cop1StX,cop1StY,cop1StZ,ref1StX,ref1StY,ref1StZ,force1StX,force1StY,force1StZ;

  vtkMAFSmartPointer<vtkPolyData> force1;
  vtkMAFSmartPointer<vtkPoints> pointsf1;
  vtkMAFSmartPointer<vtkCellArray> cellArrayf1;
  int pointId1[2];

  mafNEW(m_ForceLeft);

  mafString alLeft = name;
  alLeft << "_VECTOR";
  
  m_ForceLeft->SetName(alLeft);

  int count = 0;
  do 
  {
    line = text.ReadLine();
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    timeSt = tkz.GetNextToken().c_str();
    time = atof(timeSt)/freq_val; 
   
    //Values of the first platform
    cop1StX = tkz.GetNextToken().c_str();
    cop1StY = tkz.GetNextToken().c_str();
    cop1StZ = tkz.GetNextToken().c_str();
    ref1StX = tkz.GetNextToken().c_str();
    ref1StY = tkz.GetNextToken().c_str();
    ref1StZ = tkz.GetNextToken().c_str();
    force1StX = tkz.GetNextToken().c_str();
    force1StY = tkz.GetNextToken().c_str();
    force1StZ = tkz.GetNextToken().c_str();

    double cop1X = atof(cop1StX);
    double cop1Y = atof(cop1StY);
    double cop1Z = atof(cop1StZ);

    double ref1X = atof(ref1StX);
    double ref1Y = atof(ref1StY);
    double ref1Z = atof(ref1StZ);
   
    double force1X = atof(force1StX);
    double force1Y = atof(force1StY);
    double force1Z = atof(force1StZ);

    if (cop1X != NULL || cop1Y != NULL || cop1Z != NULL)
    {
      // FORCE
      pointsf1->InsertPoint(0, 0, 0, 0);
      pointsf1->InsertPoint(1, force1X, force1Y, force1Z);
      pointId1[0] = 0;
      pointId1[1] = 1;
      cellArrayf1->InsertNextCell(2, pointId1);  
      force1->SetPoints(pointsf1);
      force1->SetLines(cellArrayf1);
      force1->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfForL;
      vtkMAFSmartPointer<vtkTransform> transff;
     
      transff->Translate(cop1X, cop1Y, cop1Z);
      transfForL->SetTransform(transff);
      transfForL->SetInput(force1);
      transfForL->Update();

      m_ForceLeft->SetData(transfForL->GetOutput(), time);
      m_ForceLeft->Modified();
      m_ForceLeft->Update();
      m_ForceLeft->GetOutput()->GetVTKData()->Update();
    }

    count++;
    progressHelper.UpdateProgressBar(((double) count)/((double) totlines)*100.);
    

  }while (!inputFile.Eof());

  m_Output = m_ForceLeft;
  m_Output->ReparentTo(m_Input);
}