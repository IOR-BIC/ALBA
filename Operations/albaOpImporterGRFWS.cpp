/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterGRFWS
 Authors: Roberto Mucci, Simone Brazzale
 
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

#include "albaOpImporterGRFWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "albaVMEVector.h"
#include "albaVMESurface.h"
#include "albaVMEGroup.h"

#include "albaGUI.h"

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkALBASmartPointer.h"
#include "vtkCellArray.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <iostream>
#include "albaProgressBarHelper.h"
#include "wx/filename.h"


#define DELTA 5.0

//----------------------------------------------------------------------------
albaOpImporterGRFWS::albaOpImporterGRFWS(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType       	= OPTYPE_IMPORTER;
	m_Canundo	      = true;
	m_File		      = "";
	m_FileDir       = albaGetLastUserFolder();
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
albaOpImporterGRFWS::~albaOpImporterGRFWS()
//----------------------------------------------------------------------------
{
  albaDEL(m_PlatformLeft);
  albaDEL(m_PlatformRight);
  albaDEL(m_ForceLeft);
  albaDEL(m_ForceRight);
  albaDEL(m_MomentLeft);
  albaDEL(m_MomentRight);
  albaDEL(m_Group);
}

//----------------------------------------------------------------------------
void albaOpImporterGRFWS::OpUndo()
//----------------------------------------------------------------------------
{   
  if(m_PlatformLeft != NULL)
    GetLogicManager()->VmeRemove(m_PlatformLeft);
  if(m_PlatformRight != NULL)
    GetLogicManager()->VmeRemove(m_PlatformRight);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterGRFWS::Copy()   
//----------------------------------------------------------------------------
{
	albaOpImporterGRFWS *cp = new albaOpImporterGRFWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterGRFWS::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  wxString pgd_wildc	= "GRF File (*.*)|*.*";
  wxString f;
  f = albaGetOpenFile(m_FileDir,pgd_wildc); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterGRFWS::Read()   
//----------------------------------------------------------------------------
{
  wxFileInputStream inputFile( m_File );
  wxTextInputStream text( inputFile );

  wxString line;
  line = text.ReadLine(); 

  if (line.CompareTo(wxString("FORCE PLATES"))== 0)
  {
    ReadForcePlates();
  }
  else if (line.CompareTo(wxString("VECTOR"))== 0)
  {
    ReadSingleVector();
  }
  else
  {
    albaErrorMessage("Invalid file format!");
    return;
  }
}
//----------------------------------------------------------------------------
void albaOpImporterGRFWS::ReadForcePlates()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();


  wxString path, name, ext;
  wxFileName::SplitPath(m_File,&path,&name,&ext);

  albaTagItem tag_Nature;
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

  albaString platform1St[12];
  albaString platform2St[12];

  double platform1[12];
  double platform2[12];

  line = text.ReadLine(); 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  freq_val = atof(freq.ToAscii());

  line = text.ReadLine(); //Skip textual lines
  line = text.ReadLine();
   
  //Get values of the corners of the first platforms
  line = text.ReadLine();
  wxStringTokenizer corners1(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  albaString junk = corners1.GetNextToken(); //Value to ignore
  for (int i = 0 ; i < 12 ; i++)
  {
    platform1St[i] = corners1.GetNextToken();
    platform1[i] = atof(platform1St[i]);
  }
  
  //Get values of the corners of the second platforms
  line = text.ReadLine();
  wxStringTokenizer corners2(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  junk = corners2.GetNextToken(); //Value to ignore
  for (int i = 0 ; i < 12 ; i++)
  {
    platform2St[i] = corners2.GetNextToken();
    platform2[i] = atof(platform2St[i]);
  }

  vtkALBASmartPointer<vtkCubeSource> platformLeft;
  vtkALBASmartPointer<vtkCubeSource> platformRight;

  //Get values for platforms
   albaNEW(m_PlatformLeft);
   albaNEW(m_PlatformRight);

   albaString PlatNameLeft = name;
   albaString platNameRight = name;
   PlatNameLeft << "_PLATFORM_1";
   platNameRight << "_PLATFORM_2";

   m_PlatformLeft->SetName(PlatNameLeft);
   m_PlatformRight->SetName(platNameRight);

   albaNEW(m_Group);
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
  
  albaString timeSt;
  albaTimeStamp time;

  albaString cop1StX,cop1StY,cop1StZ,ref1StX,ref1StY,ref1StZ,force1StX,force1StY,force1StZ,moment1StX,moment1StY,moment1StZ;
  albaString cop2StX,cop2StY,cop2StZ,ref2StX,ref2StY,ref2StZ,force2StX,force2StY,force2StZ,moment2StX,moment2StY,moment2StZ;

  vtkALBASmartPointer<vtkPolyData> force1;
  vtkALBASmartPointer<vtkPolyData> force2;
  vtkALBASmartPointer<vtkPolyData> moment1;
  vtkALBASmartPointer<vtkPolyData> moment2;
  vtkALBASmartPointer<vtkPoints> pointsf1;
  vtkALBASmartPointer<vtkPoints> pointsf2;
  vtkALBASmartPointer<vtkCellArray> cellArrayf1;
  vtkALBASmartPointer<vtkCellArray> cellArrayf2;
  vtkALBASmartPointer<vtkPoints> pointsm1;
  vtkALBASmartPointer<vtkPoints> pointsm2;
  vtkALBASmartPointer<vtkCellArray> cellArraym1;
  vtkALBASmartPointer<vtkCellArray> cellArraym2;
	vtkIdType pointId1[2];
	vtkIdType pointId2[2];

  albaNEW(m_ForceLeft);
  albaNEW(m_ForceRight);
  albaNEW(m_MomentLeft);
  albaNEW(m_MomentRight);

  albaString alLeft = name;
  albaString alRight = name;
  alLeft << "_GRF_1";
  alRight << "_GRF_2";
  
  m_ForceLeft->SetName(alLeft);
  m_ForceRight->SetName(alRight);

  albaString almLeft = name;
  albaString almRight = name;
  almLeft << "_MOMENT_1";
  almRight << "_MOMENT_2";
  
  m_MomentLeft->SetName(almLeft);
  m_MomentRight->SetName(almRight);

  int count = 0;
  do 
  {
    line = text.ReadLine();
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    timeSt = tkz.GetNextToken();
    time = atof(timeSt)/freq_val; 
   
    //Values of the first platform
    cop1StX = tkz.GetNextToken();
    cop1StY = tkz.GetNextToken();
    cop1StZ = tkz.GetNextToken();
    ref1StX = tkz.GetNextToken();
    ref1StY = tkz.GetNextToken();
    ref1StZ = tkz.GetNextToken();
    force1StX = tkz.GetNextToken();
    force1StY = tkz.GetNextToken();
    force1StZ = tkz.GetNextToken();
    moment1StX = tkz.GetNextToken();
    moment1StY = tkz.GetNextToken();
    moment1StZ = tkz.GetNextToken();

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

      vtkALBASmartPointer<vtkTransformPolyDataFilter> transfForL;
      vtkALBASmartPointer<vtkTransform> transff;
     
      transff->Translate(cop1X, cop1Y, cop1Z);
      transfForL->SetTransform(transff);
      transfForL->SetInputData(force1);
      transfForL->Update();

      m_ForceLeft->SetData(transfForL->GetOutput(), time);
      m_ForceLeft->Modified();
      m_ForceLeft->Update();

      // MOMENT
      pointsm1->InsertPoint(0, 0, 0, 0);
      pointsm1->InsertPoint(1, moment1X, moment1Y, moment1Z);
      pointId1[0] = 0;
      pointId1[1] = 1;
      cellArraym1->InsertNextCell(2, pointId1);  
      moment1->SetPoints(pointsm1);
      moment1->SetLines(cellArraym1);

      vtkALBASmartPointer<vtkTransformPolyDataFilter> transfMomL;
      vtkALBASmartPointer<vtkTransform> transfm;
     
      transfm->Translate(cop1X, cop1Y, cop1Z);
      transfMomL->SetTransform(transfm);
      transfMomL->SetInputData(moment1);
      transfMomL->Update();

      m_MomentLeft->SetData(transfMomL->GetOutput(), time);
      m_MomentLeft->Modified();
      m_MomentLeft->Update();
    }

    //Values of the second platform
    cop2StX = tkz.GetNextToken();
    cop2StY = tkz.GetNextToken();
    cop2StZ = tkz.GetNextToken();
    ref2StX = tkz.GetNextToken();
    ref2StY = tkz.GetNextToken();
    ref2StZ = tkz.GetNextToken();
    force2StX = tkz.GetNextToken();
    force2StY = tkz.GetNextToken();
    force2StZ = tkz.GetNextToken();
    moment2StX = tkz.GetNextToken();
    moment2StY = tkz.GetNextToken();
    moment2StZ = tkz.GetNextToken();

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

      vtkALBASmartPointer<vtkTransformPolyDataFilter> transfForR;
      vtkALBASmartPointer<vtkTransform> transffr;
      
      transffr->Translate(cop2X, cop2Y, cop2Z);
      transfForR->SetTransform(transffr);
      transfForR->SetInputData(force2);
      transfForR->Update();
 
      m_ForceRight->SetData(transfForR->GetOutput(), time);
      m_ForceRight->Modified();
      m_ForceRight->Update();

      // MOMENT
      pointsm2->InsertPoint(0, 0, 0, 0);
      pointsm2->InsertPoint(1, moment2X, moment2Y, moment2Z);
      pointId2[0] = 0;
      pointId2[1] = 1;
      cellArraym2->InsertNextCell(2, pointId2);  
      moment2->SetPoints(pointsm2);
      moment2->SetLines(cellArraym2);

      vtkALBASmartPointer<vtkTransformPolyDataFilter> transfMomR;
      vtkALBASmartPointer<vtkTransform> transfmr;
      
      transfmr->Translate(cop2X, cop2Y, cop2Z);
      transfMomR->SetTransform(transfmr);
      transfMomR->SetInputData(moment2);
      transfMomR->Update();
 
      m_MomentRight->SetData(transfMomR->GetOutput(), time);
      m_MomentRight->Modified();
      m_MomentRight->Update();
    }

    count++;
    progressHelper.UpdateProgressBar(((double) count)/((double) totlines)*100.);
    

  }while (!inputFile.Eof());

  //Create the albaVMESurface for the platforms
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
void albaOpImporterGRFWS::ReadSingleVector()   
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();

  wxString path, name, ext;
  wxFileName::SplitPath(m_File,&path,&name,&ext);

  albaTagItem tag_Nature;
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
  freq_val = atof(freq.ToAscii());

  line = text.ReadLine(); 
  line = text.ReadLine();
  line = text.ReadLine(); 

  //Read vector data
  albaString timeSt;
  albaTimeStamp time;

  albaString cop1StX,cop1StY,cop1StZ,ref1StX,ref1StY,ref1StZ,force1StX,force1StY,force1StZ;

  vtkALBASmartPointer<vtkPolyData> force1;
  vtkALBASmartPointer<vtkPoints> pointsf1;
  vtkALBASmartPointer<vtkCellArray> cellArrayf1;
	vtkIdType pointId1[2];

  albaNEW(m_ForceLeft);

  albaString alLeft = name;
  alLeft << "_VECTOR";
  
  m_ForceLeft->SetName(alLeft);

  int count = 0;
  do 
  {
    line = text.ReadLine();
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    timeSt = tkz.GetNextToken();
    time = atof(timeSt)/freq_val; 
   
    //Values of the first platform
    cop1StX = tkz.GetNextToken();
    cop1StY = tkz.GetNextToken();
    cop1StZ = tkz.GetNextToken();
    ref1StX = tkz.GetNextToken();
    ref1StY = tkz.GetNextToken();
    ref1StZ = tkz.GetNextToken();
    force1StX = tkz.GetNextToken();
    force1StY = tkz.GetNextToken();
    force1StZ = tkz.GetNextToken();

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

      vtkALBASmartPointer<vtkTransformPolyDataFilter> transfForL;
      vtkALBASmartPointer<vtkTransform> transff;
     
      transff->Translate(cop1X, cop1Y, cop1Z);
      transfForL->SetTransform(transff);
      transfForL->SetInputData(force1);
      transfForL->Update();

      m_ForceLeft->SetData(transfForL->GetOutput(), time);
      m_ForceLeft->Modified();
      m_ForceLeft->Update();
    }

    count++;
    progressHelper.UpdateProgressBar(((double) count)/((double) totlines)*100.);
    

  }while (!inputFile.Eof());

  m_Output = m_ForceLeft;
  m_Output->ReparentTo(m_Input);
}