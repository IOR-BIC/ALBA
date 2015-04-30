/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataMirrorTest
 Authors: Daniele Giunchi
 
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

#include "vtkMAFPolyDataMirror.h"
#include "vtkMAFPolyDataMirrorTest.h"

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
void vtkMAFPolyDataMirrorTest::setUp()
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
void vtkMAFPolyDataMirrorTest::tearDown()
//-------------------------------------------------------------------------
{
  m_TestPolyData->Delete();
}

//-------------------------------------------------------------------------
void vtkMAFPolyDataMirrorTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMAFPolyDataMirror *pdm;
	pdm = vtkMAFPolyDataMirror::New();
	pdm->Delete();
}

//-------------------------------------------------------------------------
void vtkMAFPolyDataMirrorTest::TestMirrorX()
//-------------------------------------------------------------------------
{
	vtkMAFPolyDataMirror *tqr = vtkMAFPolyDataMirror::New();
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
void vtkMAFPolyDataMirrorTest::TestMirrorY()
//-------------------------------------------------------------------------
{
	vtkMAFPolyDataMirror *tqr = vtkMAFPolyDataMirror::New();
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
void vtkMAFPolyDataMirrorTest::TestMirrorZ()
//-------------------------------------------------------------------------
{
	vtkMAFPolyDataMirror *tqr = vtkMAFPolyDataMirror::New();
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
void vtkMAFPolyDataMirrorTest::TestFlipNormals()
//-------------------------------------------------------------------------
{
	vtkMAFPolyDataMirror *tqr = vtkMAFPolyDataMirror::New();
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
