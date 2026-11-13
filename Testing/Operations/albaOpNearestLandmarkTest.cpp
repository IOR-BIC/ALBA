/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpNearestLandmarkTest
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
#include "albaOpNearestLandmarkTest.h"

#include "albaOpNearestLandmark.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMESurfaceParametric.h"
#include "albaMatrix.h"

#include "vtkALBASmartPointer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"

#define TOLERANCE 0.5

//----------------------------------------------------------------------------
// Observer answering the VME_CHOOSE requests of the operation:
// it gives the first candidate accepted by the operation's accept function
class NearestLandmarkChooser : public albaObserver
{
public:
	typedef bool(*AcceptFunction)(albaVME *);

	NearestLandmarkChooser(albaVME *landmark, albaVME *surface) { m_Landmark = landmark; m_Surface = surface; m_Choices = 0; }

	void OnEvent(albaEventBase *alba_event)
	{
		albaEvent *e = albaEvent::SafeDownCast(alba_event);
		if (e == NULL || e->GetId() != VME_CHOOSE)
			return;

		m_Choices++;
		AcceptFunction accept = (AcceptFunction)e->GetPointer();
		if (m_Landmark && accept(m_Landmark))
			e->SetVme(m_Landmark);
		else if (m_Surface && accept(m_Surface))
			e->SetVme(m_Surface);
		else
			e->SetVme(NULL);
	}

	albaVME *m_Landmark;
	albaVME *m_Surface;
	int m_Choices;
};

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::BeforeTest()
{
	m_Storage = albaVMEStorage::New();
	m_Storage->GetRoot()->SetName("root");
	m_Storage->GetRoot()->Initialize();

	albaNEW(m_Cloud);
	m_Cloud->SetName("cloud");
	m_Cloud->TestModeOn();
	m_Cloud->ReparentTo(m_Storage->GetRoot());
	m_Cloud->AppendLandmark(30, 0, 0, "LM");
	m_Landmark = m_Cloud->GetLandmark("LM");

	albaNEW(m_Surface);
	m_Surface->SetName("sphere");
	m_Surface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
	m_Surface->SetSphereRadius(10);
	m_Surface->SetSpherePhiRes(20);
	m_Surface->SetSphereTheRes(20);
	m_Surface->ReparentTo(m_Storage->GetRoot());

	// Center the sphere in (5,0,0) to check that the surface transform is used
	albaMatrix matrix;
	matrix.SetElement(0, 3, 5);
	m_Surface->SetAbsMatrix(matrix);
	m_Surface->Update();
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::AfterTest()
{
	m_Surface->ReparentTo(NULL);
	m_Cloud->ReparentTo(NULL);
	albaDEL(m_Surface);
	albaDEL(m_Cloud);
	albaDEL(m_Storage);
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestDynamicAllocation()
{
	albaOpNearestLandmark *op = new albaOpNearestLandmark();
	cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestAccept()
{
	albaOpNearestLandmark *op = new albaOpNearestLandmark();

	// Accepted on every VME: a non landmark input asks for the landmark in OpRun
	CPPUNIT_ASSERT(op->Accept(m_Landmark));
	CPPUNIT_ASSERT(op->Accept(m_Cloud));
	CPPUNIT_ASSERT(op->Accept(m_Surface));
	CPPUNIT_ASSERT(op->Accept(m_Storage->GetRoot()));

	cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestLandmarkAndSurfaceAccept()
{
	CPPUNIT_ASSERT(albaOpNearestLandmark::LandmarkAccept(m_Landmark));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::LandmarkAccept(m_Cloud));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::LandmarkAccept(m_Surface));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::LandmarkAccept(NULL));

	CPPUNIT_ASSERT(albaOpNearestLandmark::SurfaceAccept(m_Surface));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::SurfaceAccept(m_Landmark));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::SurfaceAccept(m_Cloud));
	CPPUNIT_ASSERT(!albaOpNearestLandmark::SurfaceAccept(NULL));
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestCreateClosestPoint()
{
	// Same transform applied by OpRun to bring the surface in absolute coordinates
	vtkALBASmartPointer<vtkTransform> transform;
	transform->SetMatrix(m_Surface->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	vtkALBASmartPointer<vtkTransformPolyDataFilter> filter;
	filter->SetInputData((vtkPolyData *)m_Surface->GetOutput()->GetVTKData());
	filter->SetTransform(transform);
	filter->Update();

	albaVMELandmarkCloud *cloud = albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface);

	CPPUNIT_ASSERT(cloud == m_Cloud);
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 2);
	CheckLastLandmark("Closest LM on sphere");
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestCreateClosestPointUniqueName()
{
	vtkALBASmartPointer<vtkTransform> transform;
	transform->SetMatrix(m_Surface->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	vtkALBASmartPointer<vtkTransformPolyDataFilter> filter;
	filter->SetInputData((vtkPolyData *)m_Surface->GetOutput()->GetVTKData());
	filter->SetTransform(transform);
	filter->Update();

	// The same landmark and surface (or two surfaces with the same name) must always create a new landmark
	albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface);
	CheckLastLandmark("Closest LM on sphere");

	albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface);
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 3);
	CheckLastLandmark("Closest LM on sphere (2)");

	albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface);
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 4);
	CheckLastLandmark("Closest LM on sphere (3)");
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestCreateClosestPointCustomName()
{
	vtkALBASmartPointer<vtkTransform> transform;
	transform->SetMatrix(m_Surface->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	vtkALBASmartPointer<vtkTransformPolyDataFilter> filter;
	filter->SetInputData((vtkPolyData *)m_Surface->GetOutput()->GetVTKData());
	filter->SetTransform(transform);
	filter->Update();

	// Name given by the caller (e.g. albaOpCreateCentroid), still made unique
	albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface, "sphere Centroid on surface");
	CheckLastLandmark("sphere Centroid on surface");

	albaOpNearestLandmark::CreateClosestPoint(filter->GetOutput(), m_Landmark, m_Surface, "sphere Centroid on surface");
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 3);
	CheckLastLandmark("sphere Centroid on surface (2)");
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestOpRunWithLandmarkInput()
{
	// Landmark selected: only the surface is asked
	RunOperation(m_Landmark, m_Landmark, m_Surface, 1);

	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 2);
	CheckLastLandmark("Closest LM on sphere");
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestOpRunWithOtherInput()
{
	// Surface selected: the landmark is asked first, then the surface
	RunOperation(m_Surface, m_Landmark, m_Surface, 2);

	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 2);
	CheckLastLandmark("Closest LM on sphere");
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::TestOpRunCancel()
{
	// No landmark chosen: the operation stops without asking the surface
	RunOperation(m_Surface, NULL, NULL, 1);
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 1);

	// No surface chosen: the operation stops without creating landmarks
	RunOperation(m_Landmark, m_Landmark, NULL, 1);
	CPPUNIT_ASSERT(m_Cloud->GetNumberOfLandmarks() == 1);
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::RunOperation(albaVME *input, albaVME *landmark, albaVME *surface, int expectedChoices)
{
	NearestLandmarkChooser chooser(landmark, surface);

	albaOpNearestLandmark *op = new albaOpNearestLandmark();
	op->TestModeOn();
	op->SetListener(&chooser);
	CPPUNIT_ASSERT(op->Accept(input));
	op->SetInput(input);
	op->OpRun();
	cppDEL(op);

	CPPUNIT_ASSERT(chooser.m_Choices == expectedChoices);
}

//----------------------------------------------------------------------------
void albaOpNearestLandmarkTest::CheckLastLandmark(const char *expectedName)
{
	int idx = m_Cloud->GetNumberOfLandmarks() - 1;
	CPPUNIT_ASSERT(albaString(m_Cloud->GetLandmarkName(idx)).Equals(expectedName));

	double point[3];
	m_Cloud->GetLandmark(idx, point);
	CPPUNIT_ASSERT(fabs(point[0] - 15) < TOLERANCE);
	CPPUNIT_ASSERT(fabs(point[1]) < TOLERANCE);
	CPPUNIT_ASSERT(fabs(point[2]) < TOLERANCE);
}
