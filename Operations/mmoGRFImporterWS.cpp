/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoGRFImporterWS.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-13 07:37:01 $
  Version:   $Revision: 1.7 $
  Authors:   Roberto Mucci
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

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkMAFSmartPointer.h"
#include "vtkCellArray.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "mmoGRFImporterWS.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEScalar.h"
#include "mafVMEVector.h"
#include "mafVMESurface.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <iostream>
#include <fstream>

#define DELTA 10.0



//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoGRFImporterWS::mmoGRFImporterWS(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType       	= OPTYPE_IMPORTER;
	m_Canundo	      = true;
	m_File		      = "";
	m_FileDir       = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  m_Output        = NULL;
  m_PlatformLeft  = NULL;
  m_PlatformRight = NULL;
  m_VectorLeft    = NULL;
  m_VectorRight   = NULL;
  m_AlCop        = NULL;
}
//----------------------------------------------------------------------------
mmoGRFImporterWS::~mmoGRFImporterWS()
//----------------------------------------------------------------------------
{
//  mafDEL(m_GrfScalar);
  mafDEL(m_PlatformLeft);
  mafDEL(m_PlatformRight);
  mafDEL(m_VectorLeft);
  mafDEL(m_VectorRight);
  mafDEL(m_AlCop);
  


}
//----------------------------------------------------------------------------
void mmoGRFImporterWS::OpDo()
//----------------------------------------------------------------------------
{
  if(m_PlatformLeft != NULL)
  {
    mafEventMacro(mafEvent(this,VME_ADD,m_PlatformLeft));
    m_VectorLeft->ReparentTo(m_PlatformLeft);
  }

  if(m_PlatformRight != NULL)
  {
    mafEventMacro(mafEvent(this,VME_ADD,m_PlatformRight));
    m_VectorRight->ReparentTo(m_PlatformRight);
  }

  if(m_PlatformLeft != NULL && m_PlatformRight != NULL)
  {
    mafEventMacro(mafEvent(this,VME_ADD,m_AlCop));
  }


}
//----------------------------------------------------------------------------
void mmoGRFImporterWS::OpUndo()
//----------------------------------------------------------------------------
{   
  if(m_PlatformLeft != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_PlatformLeft));
  if(m_PlatformRight != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_PlatformRight));
}
//----------------------------------------------------------------------------
mafOp* mmoGRFImporterWS::Copy()   
//----------------------------------------------------------------------------
{
	mmoGRFImporterWS *cp = new mmoGRFImporterWS(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;

	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
void mmoGRFImporterWS::OpRun()   
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;
  m_File = "";
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
void mmoGRFImporterWS::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    default:
      mafEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void mmoGRFImporterWS::Read()   
//----------------------------------------------------------------------------
{
  //if (!m_TestMode)
    wxBusyInfo wait("Please wait, working...");

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

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
   m_PlatformLeft->SetName("Left_Platform");
   m_PlatformRight->SetName("Right_Platform");
  
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

  vtkMAFSmartPointer<vtkPolyData> vector1;
  vtkMAFSmartPointer<vtkPolyData> vector2;
  vtkMAFSmartPointer<vtkPoints> points1;
  vtkMAFSmartPointer<vtkPoints> points2;
  vtkMAFSmartPointer<vtkCellArray> cellArray1;
  vtkMAFSmartPointer<vtkCellArray> cellArray2;
  int pointId1[2];
  int pointId2[2];

  mafString alLeft = "Left";
  mafString alRight = "Right";

  mafNEW(m_VectorLeft);
  mafNEW(m_VectorRight);
  mafNEW(m_AlCop);
  
  m_VectorLeft->SetName("Left_vector");
  m_VectorRight->SetName("Right_vector");
  m_AlCop->SetName("COP");
  m_AlCop->Open();
  m_AlCop->SetRadius(10);
  m_AlCop->AppendLandmark(alLeft);
  m_AlCop->AppendLandmark(alRight);
  
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
      points1->InsertPoint(0, 0, 0, 0);
      points1->InsertPoint(1, force1X, force1Y, force1Z);
      pointId1[0] = 0;
      pointId1[1] = 1;
      cellArray1->InsertNextCell(2, pointId1);  
      vector1->SetPoints(points1);
      vector1->SetLines(cellArray1);
      vector1->Update;

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfVecL;
      vtkMAFSmartPointer<vtkTransform> transf;
     
      transf->Translate(cop1X, cop1Y, cop1Z);
      transfVecL->SetTransform(transf);
      transfVecL->SetInput(vector1);
      transfVecL->Update();


      m_VectorLeft->SetData(transfVecL->GetOutput(), time);

      m_VectorLeft->Modified();
      m_VectorLeft->Update();
      m_VectorLeft->GetOutput()->GetVTKData()->Update();

      m_AlCop->SetLandmark(alLeft, cop1X, cop1Y, cop1Z, time);
      m_AlCop->SetLandmarkVisibility(alLeft, 1, time);
    }
    else
    {
      m_AlCop->SetLandmarkVisibility(alLeft, 0, time);
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
      points2->InsertPoint(0, 0, 0, 0);
      points2->InsertPoint(1, force2X, force2Y, force2Z);
      pointId2[0] = 0;
      pointId2[1] = 1;
      cellArray2->InsertNextCell(2, pointId2);  
      vector2->SetPoints(points2);
      vector2->SetLines(cellArray2);
      vector2->Update;

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfVecR;
      vtkMAFSmartPointer<vtkTransform> transf;
      
      transf->Translate(cop2X, cop2Y, cop2Z);
      transfVecR->SetTransform(transf);
      transfVecR->SetInput(vector2);
      transfVecR->Update();
 
      m_VectorRight->SetData(transfVecR->GetOutput(), time);

      m_VectorRight->Modified();
      m_VectorRight->Update();
      m_VectorRight->GetOutput()->GetVTKData()->Update();

      m_AlCop->SetLandmark(alRight, cop2X, cop2Y, cop2Z, time);
      m_AlCop->SetLandmarkVisibility(alRight, 1, time);
    }
    else
    {
      m_AlCop->SetLandmarkVisibility(alRight, 0, time);
    }

  }while (!inputFile.Eof());

  //Create the mafVMESurface for the platforms
  m_PlatformLeft->SetData(platformLeft->GetOutput(), 0);
  m_PlatformRight->SetData(platformRight->GetOutput(), 0);
}