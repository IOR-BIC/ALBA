/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPolyDataMirrorTest.cpp,v $
Language:  C++
Date:      $Date: 2007-12-28 12:53:27 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2007
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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkMEDPolyDataMirror.h"
#include "vtkMEDPolyDataMirrorTest.h"

#include "vtkMAFSmartPointer.h"
#include "mafString.h"
#include "vtkSphereSource.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"


#define EPSILON 0.01
//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::setUp()
//-------------------------------------------------------------------------
{
	m_TestPolyData = vtkPolyData::New();

	vtkPoints   *points = vtkPoints::New();
	vtkCellArray   *cellArray = vtkCellArray::New();
	m_TestPolyData->SetPoints(points);
	m_TestPolyData->SetPolys(cellArray);

	double zero = 0.0;
	int pointId[3];
	points->InsertNextPoint(zero,zero,zero);
	points->InsertNextPoint(0.5,0.5,0.5);
	points->InsertNextPoint(1.0,1.0,3.0);

	pointId[0] = 0;
	pointId[1] = 1;
	pointId[2] = 2;
	cellArray->InsertNextCell(3 , pointId);

	m_TestPolyData->Update();

	vtkDEL(points);
	vtkDEL(cellArray);
}
//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::tearDown()
//-------------------------------------------------------------------------
{
  m_TestPolyData->Delete();
}

//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataMirror *pdm;
	pdm = vtkMEDPolyDataMirror::New();
	pdm->Delete();
}

//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::TestMirrorX()
//-------------------------------------------------------------------------
{
	vtkMEDPolyDataMirror *tqr = vtkMEDPolyDataMirror::New();
	tqr->SetInput(m_TestPolyData);

	tqr->SetMirrorXCoordinate(0);
	CPPUNIT_ASSERT(tqr->GetMirrorXCoordinate() == 0);

	tqr->MirrorXCoordinateOff();
	CPPUNIT_ASSERT(tqr->GetMirrorXCoordinate() == 0);

	tqr->MirrorXCoordinateOn();
	CPPUNIT_ASSERT(tqr->GetMirrorXCoordinate() == 1);

	tqr->Update();

	
  CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[0] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[0] == -0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[0] == -1.0);

	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[1] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[1] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[1] == 1.0);
	
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[2] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[2] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[2] == 3.0);
	
	vtkDEL(tqr);
}

//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::TestMirrorY()
//-------------------------------------------------------------------------
{
	vtkMEDPolyDataMirror *tqr = vtkMEDPolyDataMirror::New();
	tqr->SetInput(m_TestPolyData);

	tqr->SetMirrorYCoordinate(0);
	CPPUNIT_ASSERT(tqr->GetMirrorYCoordinate() == 0);

	tqr->MirrorYCoordinateOff();
	CPPUNIT_ASSERT(tqr->GetMirrorYCoordinate() == 0);

	tqr->MirrorYCoordinateOn();
	CPPUNIT_ASSERT(tqr->GetMirrorYCoordinate() == 1);

	tqr->Update();


	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[0] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[0] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[0] == 1.0);

	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[1] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[1] == -0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[1] == -1.0);

	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[2] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[2] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[2] == 3.0);

	vtkDEL(tqr);
}

//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::TestMirrorZ()
//-------------------------------------------------------------------------
{
	vtkMEDPolyDataMirror *tqr = vtkMEDPolyDataMirror::New();
	tqr->SetInput(m_TestPolyData);

	tqr->SetMirrorZCoordinate(0);
	CPPUNIT_ASSERT(tqr->GetMirrorZCoordinate() == 0);

	tqr->MirrorZCoordinateOff();
	CPPUNIT_ASSERT(tqr->GetMirrorZCoordinate() == 0);

	tqr->MirrorZCoordinateOn();
	CPPUNIT_ASSERT(tqr->GetMirrorZCoordinate() == 1);

	tqr->Update();


	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[0] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[0] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[0] == 1.0);

	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[1] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[1] == 0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[1] == 1.0);

	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(0)[2] == 0.0);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(1)[2] == -0.5);
	CPPUNIT_ASSERT(tqr->GetOutput()->GetPoints()->GetPoint(2)[2] == -3.0);

	vtkDEL(tqr);
}

//-------------------------------------------------------------------------
void vtkMEDPolyDataMirrorTest::TestFlipNormals()
//-------------------------------------------------------------------------
{
	vtkMEDPolyDataMirror *tqr = vtkMEDPolyDataMirror::New();
	tqr->SetInput(m_TestPolyData);

	tqr->SetFlipNormals(0);
	CPPUNIT_ASSERT(tqr->GetFlipNormals() == 0);

	tqr->FlipNormalsOff();
	CPPUNIT_ASSERT(tqr->GetFlipNormals() == 0);

	tqr->FlipNormalsOn();
	CPPUNIT_ASSERT(tqr->GetFlipNormals() == 1);

	tqr->Update();

	double norm[3];
	tqr->GetOutput()->GetPointData()->GetNormals()->GetTuple(0, norm);
	CPPUNIT_ASSERT(norm[0] <= -0.707 + EPSILON && norm[0] >= -0.707 - EPSILON &&
		             norm[1] <= 0.707 + EPSILON && norm[1] >= 0.707 - EPSILON &&
								 norm[2] == 0.0
								 );

	tqr->GetOutput()->GetPointData()->GetNormals()->GetTuple(1, norm);
	CPPUNIT_ASSERT(norm[0] <= -0.707 + EPSILON && norm[0] >= -0.707 - EPSILON &&
		norm[1] <= 0.707 + EPSILON && norm[1] >= 0.707 - EPSILON &&
		norm[2] == 0.0
		);

	tqr->GetOutput()->GetPointData()->GetNormals()->GetTuple(2, norm);
	CPPUNIT_ASSERT(norm[0] <= -0.707 + EPSILON && norm[0] >= -0.707 - EPSILON &&
		norm[1] <= 0.707 + EPSILON && norm[1] >= 0.707 - EPSILON &&
		norm[2] == 0.0
		);
	

	vtkDEL(tqr);
}
