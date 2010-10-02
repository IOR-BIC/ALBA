/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExporterGRFWSTest.cpp,v $
Language:  C++
Date:      $Date: 2010-10-02 09:35:01 $
Version:   $Revision: 1.1.2.2 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpExporterGRFWSTest.h"
#include "medOpExporterGRFWS.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include <vtkMAFSmartPointer.h>
#include <vtkCubeSource.h>
#include "vtkMAFSmartPointer.h"
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "mafString.h"
#include "mafTagArray.h"
#include "mafVMEVector.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void medOpExporterGRFWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpExporterGRFWS *Exporter = new medOpExporterGRFWS("Exporter");
  cppDEL(Exporter);
}
//-----------------------------------------------------------
void medOpExporterGRFWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpExporterGRFWS Exporter; 
}

//-----------------------------------------------------------
void medOpExporterGRFWSTest::TestWrite() 
//-----------------------------------------------------------
{
	medOpExporterGRFWS *Exporter = new medOpExporterGRFWS("Exporter");
	Exporter->TestModeOn();
	mafString filename=MED_DATA_ROOT;
  filename<<"/Test_ExporterGRFWS/test_GRF.csv";
	Exporter->SetFileName(filename.GetCStr());
  
  mafVMEGroup* group;
  mafVMESurface* platform1;
  mafVMESurface* platform2;
  mafVMEVector* forceL;
  mafVMEVector* forceR;
  mafVMEVector* momentL;
  mafVMEVector* momentR;
  mafNEW(group);
  mafNEW(platform1);
  mafNEW(platform2);
  mafNEW(forceL);
  mafNEW(forceR);
  mafNEW(momentL);
  mafNEW(momentR);

  forceL->ReparentTo(platform1);
  momentL->ReparentTo(platform1);
  forceR->ReparentTo(platform2);
  momentR->ReparentTo(platform2);
  platform1->ReparentTo(group);
  platform2->ReparentTo(group);

  // Fill platforms
  vtkMAFSmartPointer<vtkCubeSource> platformLeft;
  vtkMAFSmartPointer<vtkCubeSource> platformRight;
  platformLeft->SetBounds(0,10,0,50,0,5);
  platformRight->SetBounds(0,10,0,50,0,5);
  platform1->SetData(platformLeft->GetOutput(), 0);
  platform2->SetData(platformRight->GetOutput(), 0);

  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> cellArray;
  vtkMAFSmartPointer<vtkPolyData> force;
  int pointId[2];

  // Fill vectors
  for (int i=0; i<=10; i++)
  {
    points->InsertPoint(0, 0, 0, 0);
    points->InsertPoint(1, 10+i, 10+i, 10+i);
    pointId[0] = 0;
    pointId[1] = 1;
    cellArray->InsertNextCell(2, pointId);  
    force->SetPoints(points);
    force->SetLines(cellArray);
    force->Update();

    forceL->SetData(force, i);
    forceL->Update();
    forceL->GetOutput()->GetVTKData()->Update();

    points->InsertPoint(1, 10*(i+1), 10*(i+1), 10*(i+1));
    force->Update();

    forceR->SetData(force, i);
    forceR->Update();
    forceR->GetOutput()->GetVTKData()->Update();

    points->InsertPoint(1, 100+i, 100+i, 100+i);
    force->Update();

    momentL->SetData(force, i);
    momentL->Update();

    points->InsertPoint(1, 200+i, 200+i, 200+i);
    force->Update();

    momentL->GetOutput()->GetVTKData()->Update();
    momentR->SetData(force, i);
    momentR->Update();
    momentR->GetOutput()->GetVTKData()->Update();
  }

  // Execute Exporter
  Exporter->SetInput(group);
  Exporter->SetPlatforms(platform1,platform2);
  Exporter->SetForces(forceL,forceR);
  Exporter->SetMoments(momentL,momentR);
  Exporter->Write();
  Exporter->RemoveTempFiles();

  wxString file;
  file.append(filename.GetCStr());
  wxFileInputStream inputFile( file );
  wxTextInputStream text( inputFile );
  wxString line;

  // Check TAG
  line = text.ReadLine();
  CPPUNIT_ASSERT( line.Cmp("FORCE PLATES")==0);

  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();

  // Check CORNERS
  line = text.ReadLine();
  int num_tk;
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName.CountTokens();
  tkzName.GetNextToken(); 
  tkzName.GetNextToken();
  wxString st = tkzName.GetNextToken();
  CPPUNIT_ASSERT( (num_tk == 13) && (st.Cmp("50") == 0));

  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();

  // Check first row
  line = text.ReadLine();
  wxStringTokenizer tkzName2(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName2.CountTokens();
  CPPUNIT_ASSERT( (num_tk == 25) );

  // Check fifth row
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  wxStringTokenizer tkzName3(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  wxString st3 = tkzName3.GetNextToken();
  CPPUNIT_ASSERT( st3.Cmp("4") == 0);
  st3 = tkzName3.GetNextToken();
  CPPUNIT_ASSERT( st3.Cmp("0") == 0);

  // Check last row
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  wxStringTokenizer tkzName4(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  wxString st4 = tkzName4.GetNextToken();
  CPPUNIT_ASSERT( st4.Cmp("9") == 0);
  for (int k=0;k<7;k++)
  {
    st4 = tkzName4.GetNextToken();
  }
  CPPUNIT_ASSERT( st4.Cmp("19") == 0);
  for (int k=0;k<17;k++)
  {
    st4 = tkzName4.GetNextToken();
  }
  CPPUNIT_ASSERT( st4.Cmp("209") == 0);

  mafDEL(group);
  mafDEL(platform1);
  mafDEL(platform2);
  mafDEL(forceL);
  mafDEL(forceR);
  mafDEL(momentL);
  mafDEL(momentR);
  cppDEL(Exporter);
}