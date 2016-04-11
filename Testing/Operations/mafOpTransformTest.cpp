/*=========================================================================

 Program: MAF2
 Module: mafOpTransformTest
 Authors: Nicola Vanella
 
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
#include "mafOpTransformTest.h"
#include "mafOpTransform.h"

#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPolylineGraph.h"
#include "mafVMESurface.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMERoot.h"
#include "mafVME.h"
#include "mafVMEExternalData.h"

#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"
#include "mafTransform.h"
#include "vtkPolyData.h"
//#include "vtkCellData.h"
//#include "vtkPolyData.h"

//-----------------------------------------------------------
void mafOpTransformTest::TestDynamicAllocation()
//-----------------------------------------------------------
{
	mafOpTransform *transform = new mafOpTransform();
  mafDEL(transform);
}
//-----------------------------------------------------------
void mafOpTransformTest::TestAccept()
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMEPolylineGraph> polyline;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMERoot> root;
  mafSmartPointer<mafVMEExternalData> external;

	mafOpTransform *transform = new mafOpTransform();
	transform->TestModeOn();

  // Try with accepted data type
  CPPUNIT_ASSERT(transform->Accept(group) );
  CPPUNIT_ASSERT(transform->Accept(volume) );
  CPPUNIT_ASSERT(transform->Accept(polyline) );
  CPPUNIT_ASSERT(transform->Accept(surface) );

  // Try with unwanted data type
  CPPUNIT_ASSERT( !transform->Accept(root) );
  CPPUNIT_ASSERT( !transform->Accept(external) );

  mafDEL(transform);
}
//-----------------------------------------------------------
void mafOpTransformTest::TestResetMove()
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  mafMatrix* old_matrix;
  mafNEW(old_matrix);
  old_matrix->DeepCopy((mafMatrix*)surface->GetOutput()->GetAbsMatrix());
  double value = old_matrix->GetElement(0,0);  

	mafOpTransform *transform = new mafOpTransform();
	transform->TestModeOn();
	transform->SetInput(surface);
	transform->OpRun();

  surface->GetOutput()->GetAbsMatrix()->SetElement(0,0,value*2);
  CPPUNIT_ASSERT( !((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

	transform->Reset();
  CPPUNIT_ASSERT( ((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  mafDEL(old_matrix);
  mafDEL(transform);
}

//-----------------------------------------------------------
void mafOpTransformTest::TestResetScale()
//-----------------------------------------------------------
{
	vtkMAFSmartPointer<vtkCubeSource> cube;
	cube->Update();

	double startPoint[3];
	cube->GetOutput()->GetPoint(0, startPoint);

	mafSmartPointer<mafVMESurface> surfaceInput;

	surfaceInput->SetData(cube->GetOutput(), 0.0);
	surfaceInput->GetOutput()->Update();
	surfaceInput->Update();

	mafOpTransform *transform = new mafOpTransform();
	transform->TestModeOn();
	transform->SetInput(surfaceInput);

	mafMatrix m;
	double scaleFactorX, scaleFactorY, scaleFactorZ;
	scaleFactorX = 2.0;
	scaleFactorY = 3.0;
	scaleFactorZ = 1.0;
	mafTransform::Scale(m, scaleFactorX, scaleFactorY, scaleFactorZ, 0);

	transform->OpRun();
	transform->SetNewAbsMatrix(m);
	transform->Reset();

	mafVMESurface *surfaceOutput = mafVMESurface::SafeDownCast(transform->GetInput());
	surfaceOutput->GetOutput()->Update();
	surfaceOutput->Update();

	vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
	cubeOutput->Update();

	double endPoint[3];
	cubeOutput->GetPoint(0, endPoint);

	CPPUNIT_ASSERT(endPoint[0] == startPoint[0]);
	CPPUNIT_ASSERT(endPoint[1] == startPoint[1]);
	CPPUNIT_ASSERT(endPoint[2] == startPoint[2]);

	mafDEL(transform);
}