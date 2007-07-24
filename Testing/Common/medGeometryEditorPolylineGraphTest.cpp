/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGeometryEditorPolylineGraphTest.cpp,v $
Language:  C++
Date:      $Date: 2007-07-24 08:40:54 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGeometryEditorPolylineGraphTest.h"
#include "medGeometryEditorPolylineGraph.h"

#include "medVMEPolylineGraph.h"

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
void medGeometryEditorPolylineGraphTest::CreateExampleGraph()
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
void medGeometryEditorPolylineGraphTest::setUp()
//----------------------------------------------------------------------------
{
	m_Graph = vtkPolyData::New() ;
	CreateExampleGraph();
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::tearDown()
//----------------------------------------------------------------------------
{
	m_Graph->Delete() ;
	m_Graph = NULL ;
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestAddNewVertex1()
//----------------------------------------------------------------------------
{
	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
	double newVertex[3]={0,1,0};
	CPPUNIT_ASSERT(PolylineGraph->AddNewVertex(newVertex)==MAF_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==1);
	double newVertexOut[3];
	output->GetPoint(0,newVertexOut);
	CPPUNIT_ASSERT(newVertex[0]==newVertexOut[0]&&newVertex[1]==newVertexOut[1]&&newVertex[2]==newVertexOut[2]);

	vtkDEL(output);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestAddNewVertex2()
//----------------------------------------------------------------------------
{
	medVMEPolylineGraph *polyline;
	mafNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->GetOutput()->GetVTKData()->Update();
	polyline->Update();

	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double newVertex[3]={0,1,0};
	CPPUNIT_ASSERT(PolylineGraph->AddNewVertex(newVertex)==MAF_OK);

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
	mafDEL(polyline);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestAddBranch()
//----------------------------------------------------------------------------
{
	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,NULL,true);
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
	CPPUNIT_ASSERT(PolylineGraph->AddBranch(pointBranch1)==MAF_OK);

	PolylineGraph->SelectPoint(newVertex[0]);
	double pointBranch2[3]={0,2,0};
	CPPUNIT_ASSERT(PolylineGraph->AddBranch(pointBranch2)==MAF_ERROR);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==5);
	CPPUNIT_ASSERT(output->GetNumberOfCells()==2);

	vtkDEL(output);
	cppDEL(PolylineGraph);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestSelectPoint()
//----------------------------------------------------------------------------
{
	medVMEPolylineGraph *polyline;
	mafNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->GetOutput()->GetVTKData()->Update();
	polyline->Update();

	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double selectPointIn[3]={0.5,0.5,0.0};
	PolylineGraph->SelectPoint(selectPointIn);
	int id=PolylineGraph->GetVtkIdSelectedPoint();

	double selectPointOut[3];
	vtkPolyData *output=PolylineGraph->GetOutput();
	output->GetPoints()->GetPoint(id,selectPointOut);
	CPPUNIT_ASSERT(selectPointOut[0]==0.0 && selectPointOut[1]==0.0 && selectPointOut[2]==0.0);

	vtkDEL(output);
	cppDEL(PolylineGraph);
	mafDEL(polyline);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraphTest::TestDeletePoint()
//----------------------------------------------------------------------------
{
	medVMEPolylineGraph *polyline;
	mafNEW(polyline);
	polyline->SetData(m_Graph,0.0);
	polyline->GetOutput()->GetVTKData()->Update();
	polyline->Update();

	medGeometryEditorPolylineGraph *PolylineGraph = new medGeometryEditorPolylineGraph(NULL,NULL,polyline,true);
	double selectPointIn[3]={3.0,2.0,0.0};
	PolylineGraph->SelectPoint(selectPointIn);
	CPPUNIT_ASSERT(PolylineGraph->DeletePoint(selectPointIn)==MAF_OK);

	vtkPolyData *output=PolylineGraph->GetOutput();
	CPPUNIT_ASSERT(output->GetNumberOfPoints()==m_NumberOfPointsGraph-1);

	vtkDEL(output);
	cppDEL(PolylineGraph);
	mafDEL(polyline);
}