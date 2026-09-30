/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformTest
 Authors: Nicola Vanella
 
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
#include "albaOpTransformTest.h"
#include "albaOpTransform.h"

#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMESurface.h"
#include "vtkALBASmartPointer.h"
#include "albaVMERoot.h"
#include "albaVME.h"
#include "albaVMEExternalData.h"

#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"
#include "albaTransform.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
//#include "vtkCellData.h"
//#include "vtkPolyData.h"

//-----------------------------------------------------------
void albaOpTransformTest::TestDynamicAllocation()
{
	albaOpTransform *transform = new albaOpTransform();
  albaDEL(transform);
}
//-----------------------------------------------------------
void albaOpTransformTest::TestAccept()
{
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMEPolylineGraph> polyline;
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEExternalData> external;

	albaOpTransform *transform = new albaOpTransform();
	transform->TestModeOn();

  // Try with accepted data type
  CPPUNIT_ASSERT(transform->Accept(group) );
  CPPUNIT_ASSERT(transform->Accept(volume) );
  CPPUNIT_ASSERT(transform->Accept(polyline) );
  CPPUNIT_ASSERT(transform->Accept(surface) );

  // Try with unwanted data type
  CPPUNIT_ASSERT( !transform->Accept(root) );
  CPPUNIT_ASSERT( !transform->Accept(external) );

  albaDEL(transform);
}
//-----------------------------------------------------------
void albaOpTransformTest::TestResetMove()
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  albaMatrix* old_matrix;
  albaNEW(old_matrix);
  old_matrix->DeepCopy((albaMatrix*)surface->GetOutput()->GetAbsMatrix());
  double value = old_matrix->GetElement(0,0);  

	albaOpTransform *transform = new albaOpTransform();
	transform->TestModeOn();
	transform->SetInput(surface);
	transform->OpRun();

  surface->GetOutput()->GetAbsMatrix()->SetElement(0,0,value*2);
  CPPUNIT_ASSERT( !((albaMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

	transform->Reset();
  CPPUNIT_ASSERT( ((albaMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  albaDEL(old_matrix);
  albaDEL(transform);
}

//-----------------------------------------------------------
void albaOpTransformTest::TestResetScale()
{
	vtkALBASmartPointer<vtkCubeSource> cube;
	cube->Update();

	double startPoint[3];
	cube->GetOutput()->GetPoint(0, startPoint);

	albaSmartPointer<albaVMESurface> surfaceInput;

	surfaceInput->SetData(cube->GetOutput(), 0.0);
	surfaceInput->GetOutput()->Update();
	surfaceInput->Update();

	albaOpTransform *transform = new albaOpTransform();
	transform->TestModeOn();
	transform->SetInput(surfaceInput);

	albaMatrix m;
	double scaleFactorX, scaleFactorY, scaleFactorZ;
	scaleFactorX = 2.0;
	scaleFactorY = 3.0;
	scaleFactorZ = 1.0;
	albaTransform::Scale(m, scaleFactorX, scaleFactorY, scaleFactorZ, 0);

	transform->OpRun();
	transform->SetNewAbsMatrix(m);
	transform->Reset();

	albaVMESurface *surfaceOutput = albaVMESurface::SafeDownCast(transform->GetInput());
	surfaceOutput->GetOutput()->Update();
	surfaceOutput->Update();

	vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());

	double endPoint[3];
	cubeOutput->GetPoint(0, endPoint);

	CPPUNIT_ASSERT(endPoint[0] == startPoint[0]);
	CPPUNIT_ASSERT(endPoint[1] == startPoint[1]);
	CPPUNIT_ASSERT(endPoint[2] == startPoint[2]);

	albaDEL(transform);
}

//-----------------------------------------------------------
void albaOpTransformTest::TestTransform()
{
	double value1, value2, value3;
	value1 = 1.0;
	value2 = 2.0;
	value3 = 3.0;

	vtkALBASmartPointer<vtkCubeSource> cube;
	cube->Update();

	double startPoint[3];
	cube->GetOutput()->GetPoint(0, startPoint);

	albaSmartPointer<albaVMERoot> root;
	albaSmartPointer<albaVMESurface> surfaceInput;
	surfaceInput->ReparentTo(root);

	surfaceInput->SetData(cube->GetOutput(), 0.0);
	surfaceInput->GetOutput()->Update();
	surfaceInput->Update();

	albaOpTransform *opTransform = new albaOpTransform();
	opTransform->TestModeOn();
	opTransform->SetInput(surfaceInput);
	opTransform->OpRun();

	//
	albaMatrix m;
	albaMatrix *opM;

	// Scale
	albaTransform::Scale(m, value1, value2, value3, POST_MULTIPLY);
	opTransform->Scale(value1, value2, value3);

	opM = opTransform->GetAbsMatrix();

	CPPUNIT_ASSERT(m.Equals(opM));

	// Rotate
	albaTransform::RotateX(m, value1, POST_MULTIPLY);
	albaTransform::RotateY(m, value2, POST_MULTIPLY);
	albaTransform::RotateZ(m, value3, POST_MULTIPLY);

	opTransform->Rotate(value1, value2, value3);

	opM = opTransform->GetAbsMatrix();

	CPPUNIT_ASSERT(m.Equals(opM));

	// Translate
	albaTransform::Translate(m, value1, value2, value3, POST_MULTIPLY);
	opTransform->Translate(value1, value2, value3);

	opM = opTransform->GetAbsMatrix();
	
	CPPUNIT_ASSERT(m.Equals(opM));

	albaDEL(opTransform);
}