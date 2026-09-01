/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGeometryEditorPolylineGraphTest
 Authors: Matteo Giacomoni
 
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

#include "albaGeometryEditorPolylineGraphTest.h"
#include "albaGeometryEditorPolylineGraph.h"

#include "albaVMEPolylineGraph.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
//------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

												17
												|
												16
												|
												15 - 18 - 19 - 20 - 21
												|
												14
	 2										|
	/                     13
 /                      |                        
1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
|   |\
|   | \
0   5  4

*/  
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::CreateExampleGraph()
//------------------------------------------------------------------------------
{
	vtkPoints *points = vtkPoints::New() ;
	vtkCellArray *lines = vtkCellArray::New() ;

	int i ;

	// coordinates of vertices
	static double vertices[22][3] ={
		{0,0,0},
		{1,2,0},
		{2,4,0},
		{2,1,0},
		{3,1,0},
		{3,0,0},

		{3,2,0},
		{4,1,0},
		{5,2,0},
		{6,1,0},
		{7,2,0},
		{8,1,0},
		{9,2,0},

		{7,4,0},
		{9,6,0},
		{10,8,0},
		{13,10,0},
		{14,12,0},

		{11,9,0},
		{12,8,0},
		{13,8,0},
		{16,10,0}
	} ;

	// indices of simple lines and polylines
	static vtkIdType lineids[7][10] = {
		{0,1},
		{1,2},
		{3,4},
		{3,5},
		{1, 3, 6, 7, 8, 9, 10, 11, 12},
		{10, 13, 14, 15, 16, 17},
		{15, 18, 19, 20, 21}
	};

	// insert points
	for (i = 0 ;  i < 22 ;  i++)
		points->InsertNextPoint(vertices[i]) ;

	// insert lines and polylines
	lines->InsertNextCell(2, lineids[0]) ;
	lines->InsertNextCell(2, lineids[1]) ;
	lines->InsertNextCell(2, lineids[2]) ;
	lines->InsertNextCell(2, lineids[3]) ;
	lines->InsertNextCell(9, lineids[4]) ;
	lines->InsertNextCell(6, lineids[5]) ;
	lines->InsertNextCell(5, lineids[6]) ;

	// put points and cells in polydata
	m_Graph->SetPoints(points) ;
	m_Graph->SetLines(lines) ;

	// count how many vertices, edges and branches we should have got from the polydata
	m_NumberOfPointsGraph = m_Graph->GetPoints()->GetNumberOfPoints() ;
	m_NumberOfLinesGraph = m_Graph->GetNumberOfLines() ;
	for (i = 0, m_NumberOfEdgesGraph = 0, m_NumberOfBranchesGraph = 0 ;  i < m_NumberOfLinesGraph ;  i++){
		vtkCell *cell = m_Graph->GetCell(i) ;
		if (cell->GetCellType() == VTK_LINE){
			// each simple line in the polydata is an edge and a branch in the graph
			m_NumberOfBranchesGraph++ ;
			m_NumberOfEdgesGraph++ ;
		}
		else if(cell->GetCellType() == VTK_POLY_LINE){
			// each polyline in the polydata is a branch and a set of edges in the graph
			m_NumberOfBranchesGraph++ ;
			m_NumberOfEdgesGraph += cell->GetNumberOfPoints() - 1 ;
		}
		else
			CPPUNIT_ASSERT(false) ;
	}

	points->Delete() ;
	lines->Delete() ;

}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_Graph = vtkPolyData::New() ;
	CreateExampleGraph();
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
	albaDEL(m_Graph);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestAddNewVertex1()
//----------------------------------------------------------------------------
{
	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
	double newVertex[3]={0,1,0};
	CPPUNIT_ASSERT(PolylineGraph->AddNewVertex(newVertex)==ALBA_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==1);
	double newVertexOut[3];
	output->GetPoint(0,newVertexOut);
	CPPUNIT_ASSERT(newVertex[0]==newVertexOut[0]&&newVertex[1]==newVertexOut[1]&&newVertex[2]==newVertexOut[2]);

	vtkDEL(output);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestAddNewVertex2()
//----------------------------------------------------------------------------
{
	albaVMEPolylineGraph *polyline;
	albaNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->Update();

	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double newVertex[3]={0,1,0};
	CPPUNIT_ASSERT(PolylineGraph->AddNewVertex(newVertex)==ALBA_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==m_NumberOfPointsGraph+1);
	double newVertexOut[3];
	output->GetPoint(output->GetNumberOfPoints()-1,newVertexOut);
	CPPUNIT_ASSERT(newVertex[0]==newVertexOut[0]&&newVertex[1]==newVertexOut[1]&&newVertex[2]==newVertexOut[2]);

	vtkCell *cell=output->GetCell(0);
	cell->GetPoints()->GetPoint(2,newVertexOut);
	CPPUNIT_ASSERT(newVertex[0]==newVertexOut[0]&&newVertex[1]==newVertexOut[1]&&newVertex[2]==newVertexOut[2]);

	vtkDEL(output);
	cppDEL(PolylineGraph);
	albaDEL(polyline);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestAddBranch()
//----------------------------------------------------------------------------
{
	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
	double newVertex[4][3]={
		{0,1,0},
		{1,1,0},
		{2,2,0},
		{2,1,0}
	};
	for(int i=0;i<4;i++)
		PolylineGraph->AddNewVertex(newVertex[i]);

	PolylineGraph->SelectPoint(newVertex[1]);
	double pointBranch1[3]={1,2,0};
	CPPUNIT_ASSERT(PolylineGraph->AddBranch(pointBranch1)==ALBA_OK);

	PolylineGraph->SelectPoint(newVertex[0]);
	double pointBranch2[3]={0,2,0};
	CPPUNIT_ASSERT(PolylineGraph->AddBranch(pointBranch2)==ALBA_ERROR);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==5);
	CPPUNIT_ASSERT(output->GetNumberOfCells()==2);

	vtkDEL(output);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestSelectPoint()
//----------------------------------------------------------------------------
{
	albaVMEPolylineGraph *polyline;
	albaNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->Update();

	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double selectPointIn[3]={0.5,0.5,0.0};
	PolylineGraph->SelectPoint(selectPointIn);
	int pointId=PolylineGraph->GetVtkIdSelectedPoint();

	double selectPointOut[3];
	vtkPolyData *output=PolylineGraph->GetOutput();
	output->GetPoints()->GetPoint(pointId,selectPointOut);
	CPPUNIT_ASSERT(selectPointOut[0]==0.0 && selectPointOut[1]==0.0 && selectPointOut[2]==0.0);

	vtkDEL(output);
	cppDEL(PolylineGraph);
	albaDEL(polyline);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestDeletePoint()
//----------------------------------------------------------------------------
{
	albaVMEPolylineGraph *polyline;
	albaNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->Update();

	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double selectPointIn[3]={3.0,2.0,0.0};
	PolylineGraph->SelectPoint(selectPointIn);
	CPPUNIT_ASSERT(PolylineGraph->DeletePoint(selectPointIn)==ALBA_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==m_NumberOfPointsGraph-1);

	vtkDEL(output);
	cppDEL(PolylineGraph);
	albaDEL(polyline);
 }

//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestSelectBranch()
//----------------------------------------------------------------------------
{
	albaVMEPolylineGraph *polyline;
	albaNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->Update();

	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double selectPoint[3]={12.5,11.0,0.0};
	PolylineGraph->SelectBranch(selectPoint);
	vtkIdType ID=PolylineGraph->GetVtkIdSelectBranch();
	CPPUNIT_ASSERT(ID==5);

	cppDEL(PolylineGraph);
	albaDEL(polyline);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraphTest::TestInsertPoint()
//----------------------------------------------------------------------------
{
	albaVMEPolylineGraph *polyline;
	albaNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->Update();

	albaGeometryEditorPolylineGraph *PolylineGraph = new albaGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double insertPoint[3]={4.5,1.5,0.0};
	double selectPoint[3]={5,2,0};
	PolylineGraph->SelectPoint(selectPoint);
	CPPUNIT_ASSERT(PolylineGraph->InsertPoint(insertPoint)==ALBA_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetCell(4)->GetNumberOfPoints()==10);
	vtkIdList *pointList=output->GetCell(4)->GetPointIds();
	for(int i=0;i<output->GetCell(4)->GetNumberOfPoints();i++)
	{
		if(pointList->GetId(i)==7)
			CPPUNIT_ASSERT(pointList->GetId(i+1)==output->GetNumberOfPoints()-1);
	}

	vtkDEL(output);
	cppDEL(PolylineGraph);
	albaDEL(polyline);
}