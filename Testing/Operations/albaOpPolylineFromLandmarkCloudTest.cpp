/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpPolylineFromLandmarkCloudTest
 Authors: Gianluigi Crimi

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

#include <cppunit/config/SourcePrefix.h>
#include "albaOperationsTests.h"
#include "albaOpPolylineFromLandmarkCloudTest.h"

#include "albaOpPolylineFromLandmarkCloud.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGroup.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEPolyline.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"

#define TOLERANCE 1.0e-6

static double LandmarkPoints[3][3] = { { 0, 0, 0 }, { 1, 2, 3 }, { 4, 5, 6 } };

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::BeforeTest()
{
	m_Storage = albaVMEStorage::New();
	m_Storage->GetRoot()->SetName("root");
	m_Storage->GetRoot()->Initialize();

	albaNEW(m_Group);
	m_Group->SetName("group");
	m_Group->ReparentTo(m_Storage->GetRoot());
	albaMatrix groupMatrix;
	groupMatrix.SetElement(1, 3, 20);
	m_Group->SetAbsMatrix(groupMatrix);

	albaNEW(m_Cloud);
	m_Cloud->SetName("cloud");
	m_Cloud->TestModeOn();
	m_Cloud->ReparentTo(m_Group);
	albaMatrix cloudMatrix;
	cloudMatrix.SetElement(0, 3, 10);
	m_Cloud->SetMatrix(cloudMatrix);

	m_Cloud->AppendLandmark(LandmarkPoints[0], "A");
	m_Cloud->AppendLandmark(LandmarkPoints[1], "B");
	m_Cloud->AppendLandmark(LandmarkPoints[2], "C");
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::AfterTest()
{
	m_Group->ReparentTo(NULL);
	albaDEL(m_Cloud);
	albaDEL(m_Group);
	albaDEL(m_Storage);
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::TestDynamicAllocation()
{
	albaOpPolylineFromLandmarkCloud *op = new albaOpPolylineFromLandmarkCloud();
	cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::TestAccept()
{
	albaOpPolylineFromLandmarkCloud *op = new albaOpPolylineFromLandmarkCloud();

	CPPUNIT_ASSERT(op->Accept(m_Cloud));
	CPPUNIT_ASSERT(!op->Accept(m_Group));
	CPPUNIT_ASSERT(!op->Accept(m_Cloud->GetLandmark(0)));

	// At least two landmarks are needed for a polyline
	albaSmartPointer<albaVMELandmarkCloud> smallCloud;
	smallCloud->TestModeOn();
	smallCloud->ReparentTo(m_Group);
	CPPUNIT_ASSERT(!op->Accept(smallCloud));
	smallCloud->AppendLandmark(LandmarkPoints[0], "A");
	CPPUNIT_ASSERT(!op->Accept(smallCloud));
	smallCloud->AppendLandmark(LandmarkPoints[1], "B");
	CPPUNIT_ASSERT(op->Accept(smallCloud));
	smallCloud->ReparentTo(NULL);

	cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::TestCreatePolyData()
{
	vtkALBASmartPointer<vtkPolyData> poly;
	albaOpPolylineFromLandmarkCloud::CreatePolyData(m_Cloud, poly);

	// The points are the landmarks, in the order of the cloud
	CPPUNIT_ASSERT(poly->GetNumberOfPoints() == 3);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			CPPUNIT_ASSERT(fabs(poly->GetPoint(i)[j] - LandmarkPoints[i][j]) < TOLERANCE);

	// A single open line: 0-1-2, not closed on the first point
	CPPUNIT_ASSERT(poly->GetNumberOfLines() == 1);
	vtkALBASmartPointer<vtkIdList> ids;
	poly->GetLines()->InitTraversal();
	poly->GetLines()->GetNextCell(ids);
	CPPUNIT_ASSERT(ids->GetNumberOfIds() == 3);
	for (int i = 0; i < 3; i++)
		CPPUNIT_ASSERT(ids->GetId(i) == i);
}

//----------------------------------------------------------------------------
void albaOpPolylineFromLandmarkCloudTest::TestOpRun()
{
	albaOpPolylineFromLandmarkCloud *op = new albaOpPolylineFromLandmarkCloud();
	op->TestModeOn();
	CPPUNIT_ASSERT(op->Accept(m_Cloud));
	op->SetInput(m_Cloud);
	op->OpRun();
	op->OpDo();

	albaVMEPolyline *polyline = albaVMEPolyline::SafeDownCast(op->GetOutput());
	CPPUNIT_ASSERT(polyline != NULL);

	// Sibling of the cloud, with the expected name
	CPPUNIT_ASSERT(polyline->GetParent() == m_Group);
	CPPUNIT_ASSERT(albaString(polyline->GetName()).Equals("cloud Polyline"));

	// Same pose of the cloud: the absolute points of the polyline are the absolute landmarks
	CPPUNIT_ASSERT(polyline->GetOutput()->GetAbsMatrix()->Equals(m_Cloud->GetOutput()->GetAbsMatrix()));

	vtkPolyData *poly = (vtkPolyData *)polyline->GetOutput()->GetVTKData();
	CPPUNIT_ASSERT(poly->GetNumberOfPoints() == 3);

	albaSmartPointer<albaTransform> polylineAbs;
	polylineAbs->SetMatrix(*polyline->GetOutput()->GetAbsMatrix());
	for (int i = 0; i < 3; i++)
	{
		double polylinePoint[3], landmarkPoint[3], landmarkOrientation[3];
		polylineAbs->TransformPoint(poly->GetPoint(i), polylinePoint);
		m_Cloud->GetLandmark(i)->GetOutput()->GetAbsPose(landmarkPoint, landmarkOrientation);
		for (int j = 0; j < 3; j++)
			CPPUNIT_ASSERT(fabs(polylinePoint[j] - landmarkPoint[j]) < TOLERANCE);
	}

	// Expected absolute position of the second landmark: group (0,20,0) + cloud (10,0,0) + landmark (1,2,3)
	double expected[3] = { 11, 22, 3 }, secondPoint[3];
	polylineAbs->TransformPoint(poly->GetPoint(1), secondPoint);
	for (int j = 0; j < 3; j++)
		CPPUNIT_ASSERT(fabs(secondPoint[j] - expected[j]) < TOLERANCE);

	cppDEL(op);
}
