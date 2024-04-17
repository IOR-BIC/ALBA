/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterGRFWSTest
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

#include "albaOpExporterGRFWSTest.h"
#include "albaOpExporterGRFWS.h"

#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include <vtkALBASmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "albaString.h"
#include "albaTagArray.h"
#include "albaVMEVector.h"
#include "albaVMESurface.h"
#include "albaVMEGroup.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpExporterGRFWS *Exporter = new albaOpExporterGRFWS("Exporter");
  cppDEL(Exporter);
}
//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpExporterGRFWS Exporter; 
}
//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestWrite()
//-----------------------------------------------------------
{
	albaOpExporterGRFWS *Exporter = new albaOpExporterGRFWS("Exporter");
	Exporter->TestModeOn();
	albaString filename = GET_TEST_DATA_DIR();
	filename << "/test_GRF.csv";
	Exporter->SetFileName(filename.GetCStr());

	albaVMEGroup* group;
	albaVMESurface* platform1;
	albaVMESurface* platform2;
	albaVMEVector* forceL;
	albaVMEVector* forceR;
	albaVMEVector* momentL;
	albaVMEVector* momentR;
	albaNEW(group);
	albaNEW(platform1);
	albaNEW(platform2);
	albaNEW(forceL);
	albaNEW(forceR);
	albaNEW(momentL);
	albaNEW(momentR);

	forceL->ReparentTo(platform1);
	momentL->ReparentTo(platform1);
	forceR->ReparentTo(platform2);
	momentR->ReparentTo(platform2);
	platform1->ReparentTo(group);
	platform2->ReparentTo(group);

	// Fill platforms
	vtkALBASmartPointer<vtkCubeSource> platformLeft;
	vtkALBASmartPointer<vtkCubeSource> platformRight;
	platformLeft->SetBounds(0, 10, 0, 50, 0, 5);
	platformRight->SetBounds(0, 10, 0, 50, 0, 5);
	platformLeft->Update();
	platformRight->Update();
	platform1->SetData(platformLeft->GetOutput(), 0);
	platform2->SetData(platformRight->GetOutput(), 0);

	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> cellArray;
	vtkALBASmartPointer<vtkPolyData> force;
	vtkIdType pointId[2];

	// Fill vectors
	for (int i = 0; i <= 10; i++)
	{
		points->InsertPoint(0, 0, 0, 0);
		points->InsertPoint(1, 10 + i, 10 + i, 10 + i);
		pointId[0] = 0;
		pointId[1] = 1;
		cellArray->InsertNextCell(2, pointId);
		force->SetPoints(points);
		force->SetLines(cellArray);

		forceL->SetData(force, i);
		forceL->Update();

		points->InsertPoint(1, 10 * (i + 1), 10 * (i + 1), 10 * (i + 1));

		forceR->SetData(force, i);
		forceR->Update();

		points->InsertPoint(1, 100 + i, 100 + i, 100 + i);

		momentL->SetData(force, i);
		momentL->Update();

		points->InsertPoint(1, 200 + i, 200 + i, 200 + i);

		momentR->SetData(force, i);
		momentR->Update();
	}

	// Execute Exporter
	Exporter->SetInput(group);
	Exporter->SetPlatforms(platform1, platform2);
	Exporter->SetForces(forceL, forceR);
	Exporter->SetMoments(momentL, momentR);
	Exporter->Write();
	Exporter->RemoveTempFiles();

	wxString file;
	file.append(filename.GetCStr());
	wxFileInputStream inputFile(file);
	wxTextInputStream text(inputFile);
	wxString line;

	// Check TAG
	line = text.ReadLine();
	CPPUNIT_ASSERT(line.Cmp("FORCE PLATES") == 0);

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check CORNERS
	line = text.ReadLine();
	int num_tk;
	wxStringTokenizer tkzName(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	num_tk = tkzName.CountTokens();
	tkzName.GetNextToken();
	tkzName.GetNextToken();
	wxString st = tkzName.GetNextToken();
	CPPUNIT_ASSERT((num_tk == 13) && (st.Cmp("50") == 0));

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check first row
	line = text.ReadLine();
	wxStringTokenizer tkzName2(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	num_tk = tkzName2.CountTokens();
	CPPUNIT_ASSERT((num_tk == 25));

	// Check fifth row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName3(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("4") == 0);
	st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);

	// Check last row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName4(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st4 = tkzName4.GetNextToken();
	CPPUNIT_ASSERT(st4.Cmp("9") == 0);
	for (int k = 0; k < 7; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("19") == 0);
	for (int k = 0; k < 17; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("209") == 0);

	albaDEL(group);
	albaDEL(platform1);
	albaDEL(platform2);
	albaDEL(forceL);
	albaDEL(forceR);
	albaDEL(momentL);
	albaDEL(momentR);
	cppDEL(Exporter);
}
//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestWriteFast()
//-----------------------------------------------------------
{
	albaOpExporterGRFWS *Exporter = new albaOpExporterGRFWS("Exporter");
	Exporter->TestModeOn();
	albaString filename = GET_TEST_DATA_DIR();
	filename << "/test_GRF_Fast.csv";
	Exporter->SetFileName(filename.GetCStr());

	albaVMEGroup* group;
	albaVMESurface* platform1;
	albaVMESurface* platform2;
	albaVMEVector* forceL;
	albaVMEVector* forceR;
	albaVMEVector* momentL;
	albaVMEVector* momentR;
	albaNEW(group);
	albaNEW(platform1);
	albaNEW(platform2);
	albaNEW(forceL);
	albaNEW(forceR);
	albaNEW(momentL);
	albaNEW(momentR);

	forceL->ReparentTo(platform1);
	momentL->ReparentTo(platform1);
	forceR->ReparentTo(platform2);
	momentR->ReparentTo(platform2);
	platform1->ReparentTo(group);
	platform2->ReparentTo(group);

	// Fill platforms
	vtkALBASmartPointer<vtkCubeSource> platformLeft;
	vtkALBASmartPointer<vtkCubeSource> platformRight;
	platformLeft->SetBounds(0, 10, 0, 50, 0, 5);
	platformRight->SetBounds(0, 10, 0, 50, 0, 5);
	platformLeft->Update();
	platformRight->Update();
	platform1->SetData(platformLeft->GetOutput(), 0);
	platform2->SetData(platformRight->GetOutput(), 0);

	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> cellArray;
	vtkALBASmartPointer<vtkPolyData> force;
	vtkIdType pointId[2];

	// Fill vectors
	for (int i = 0; i < 3; i++)
	{
		points->InsertPoint(0, 0, 0, 0);
		points->InsertPoint(1, 5 - (5 * i), 10, 10);
		pointId[0] = 0;
		pointId[1] = 1;
		cellArray->InsertNextCell(2, pointId);
		force->SetPoints(points);
		force->SetLines(cellArray);

		forceL->SetData(force, i);
		forceL->Update();

		points->InsertPoint(1, 10, 5 - (5 * i), 10);

		forceR->SetData(force, i);
		forceR->Update();

		points->InsertPoint(1, 10, 10, 5 - (5 * i));

		momentL->SetData(force, i);
		momentL->Update();

		points->InsertPoint(1, 200 + i, 200 + i, 200 + i);

		momentR->SetData(force, i);
		momentR->Update();

	}

	// Execute Exporter
	Exporter->SetInput(group);
	Exporter->SetPlatforms(platform1, platform2);
	Exporter->SetForces(forceL, forceR);
	Exporter->SetMoments(momentL, momentR);
	Exporter->WriteFast();
	Exporter->RemoveTempFiles();

	wxString file;
	file.append(filename.GetCStr());
	wxFileInputStream inputFile(file);
	wxTextInputStream text(inputFile);
	wxString line;

	// Check TAG
	line = text.ReadLine();
	CPPUNIT_ASSERT(line.Cmp("FORCE PLATES") == 0);

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check CORNERS
	line = text.ReadLine();
	int num_tk;
	wxStringTokenizer tkzName(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	num_tk = tkzName.CountTokens();
	tkzName.GetNextToken();
	tkzName.GetNextToken();
	wxString st = tkzName.GetNextToken();
	CPPUNIT_ASSERT((num_tk == 13) && (st.Cmp("50") == 0));

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check first row
	line = text.ReadLine();
	wxStringTokenizer tkzName2(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st2 = tkzName2.GetNextToken();
	CPPUNIT_ASSERT(st2.Cmp("0") == 0);
	for (int k = 0; k < 7; k++)
	{
		st2 = tkzName2.GetNextToken();
	}
	CPPUNIT_ASSERT(st2.Cmp("5") == 0);
	for (int k = 0; k < 5; k++)
	{
		st2 = tkzName2.GetNextToken();
	}
	CPPUNIT_ASSERT(st2.Cmp("5") == 0);
	for (int k = 0; k < 8; k++)
	{
		st2 = tkzName2.GetNextToken();
	}
	CPPUNIT_ASSERT(st2.Cmp("5") == 0);

	// Check second row
	line = text.ReadLine();
	wxStringTokenizer tkzName3(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("1") == 0);
	for (int k = 0; k < 7; k++)
	{
		st3 = tkzName3.GetNextToken();
	}
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);
	for (int k = 0; k < 5; k++)
	{
		st3 = tkzName3.GetNextToken();
	}
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);
	for (int k = 0; k < 8; k++)
	{
		st3 = tkzName3.GetNextToken();
	}
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);

	// Check third row
	line = text.ReadLine();
	wxStringTokenizer tkzName4(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st4 = tkzName4.GetNextToken();
	CPPUNIT_ASSERT(st4.Cmp("2") == 0);
	for (int k = 0; k < 7; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("-5") == 0);
	for (int k = 0; k < 5; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("-5") == 0);
	for (int k = 0; k < 8; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("-5") == 0);

	albaDEL(group);
	albaDEL(platform1);
	albaDEL(platform2);
	albaDEL(forceL);
	albaDEL(forceR);
	albaDEL(momentL);
	albaDEL(momentR);
	cppDEL(Exporter);
}
//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestWriteSingleVector()
//-----------------------------------------------------------
{
	albaOpExporterGRFWS *Exporter = new albaOpExporterGRFWS("Exporter");
	Exporter->TestModeOn();
	albaString filename = GET_TEST_DATA_DIR();
	filename << "/test_GRF_Single.csv";
	Exporter->SetFileName(filename.GetCStr());

	albaVMEVector* vector;
	albaNEW(vector);

	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> cellArray;
	vtkALBASmartPointer<vtkPolyData> force;
	vtkIdType pointId[2];

	// Fill vector
	for (int i = 0; i <= 10; i++)
	{
		points->InsertPoint(0, 0, 0, 0);
		points->InsertPoint(1, 10 + i, 10 + i, 10 + i);
		pointId[0] = 0;
		pointId[1] = 1;
		cellArray->InsertNextCell(2, pointId);
		force->SetPoints(points);
		force->SetLines(cellArray);

		vector->SetData(force, i);
		vector->Update();
	}

	// Execute Exporter
	Exporter->SetInput(vector);
	Exporter->SetForces(vector, NULL);
	Exporter->WriteSingleVector();

	wxString file;
	file.append(filename.GetCStr());
	wxFileInputStream inputFile(file);
	wxTextInputStream text(inputFile);
	wxString line;

	// Check TAG
	line = text.ReadLine();
	CPPUNIT_ASSERT(line.Cmp("VECTOR") == 0);

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check first row
	line = text.ReadLine();
	wxStringTokenizer tkzName2(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	int num_tk = tkzName2.CountTokens();
	CPPUNIT_ASSERT((num_tk == 10));

	// Check fifth row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName3(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("4") == 0);
	st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);

	// Check last row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName4(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st4 = tkzName4.GetNextToken();
	CPPUNIT_ASSERT(st4.Cmp("9") == 0);
	for (int k = 0; k < 7; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("19") == 0);

	albaDEL(vector);
	cppDEL(Exporter);
}
//-----------------------------------------------------------
void albaOpExporterGRFWSTest::TestWriteSingleVectorFast()
//-----------------------------------------------------------
{
	albaOpExporterGRFWS *Exporter = new albaOpExporterGRFWS("Exporter");
	Exporter->TestModeOn();
	albaString filename = GET_TEST_DATA_DIR();
	filename << "/test_GRF_Single_Fast.csv";
	Exporter->SetFileName(filename.GetCStr());

	albaVMEVector* vector;
	albaNEW(vector);

	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> cellArray;
	vtkALBASmartPointer<vtkPolyData> force;
	vtkIdType pointId[2];

	// Fill vector
	for (int i = 0; i <= 10; i++)
	{
		points->InsertPoint(0, 0, 0, 0);
		points->InsertPoint(1, 10 + i, 10 + i, 10 + i);
		pointId[0] = 0;
		pointId[1] = 1;
		cellArray->InsertNextCell(2, pointId);
		force->SetPoints(points);
		force->SetLines(cellArray);

		vector->SetData(force, i);
		vector->Update();
	}

	// Execute Exporter
	Exporter->SetInput(vector);
	Exporter->SetForces(vector, NULL);
	Exporter->WriteSingleVector();

	wxString file;
	file.append(filename.GetCStr());
	wxFileInputStream inputFile(file);
	wxTextInputStream text(inputFile);
	wxString line;

	// Check TAG
	line = text.ReadLine();
	CPPUNIT_ASSERT(line.Cmp("VECTOR") == 0);

	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();

	// Check first row
	line = text.ReadLine();
	wxStringTokenizer tkzName2(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	int num_tk = tkzName2.CountTokens();
	CPPUNIT_ASSERT((num_tk == 10));

	// Check fifth row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName3(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("4") == 0);
	st3 = tkzName3.GetNextToken();
	CPPUNIT_ASSERT(st3.Cmp("0") == 0);

	// Check last row
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	line = text.ReadLine();
	wxStringTokenizer tkzName4(line, wxT(','), wxTOKEN_RET_EMPTY_ALL);
	wxString st4 = tkzName4.GetNextToken();
	CPPUNIT_ASSERT(st4.Cmp("9") == 0);
	for (int k = 0; k < 7; k++)
	{
		st4 = tkzName4.GetNextToken();
	}
	CPPUNIT_ASSERT(st4.Cmp("19") == 0);

	albaDEL(vector);
	cppDEL(Exporter);
}