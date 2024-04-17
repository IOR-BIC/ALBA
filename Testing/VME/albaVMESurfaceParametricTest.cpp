/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceParametricTest
 Authors: Daniele Giunchi
 
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
#include "albaVMESurfaceParametricTest.h"
#include <iostream>
#include "albaVMESurfaceParametric.h"
#include "albaVMEOutputSurface.h"
#include "vtkPolyData.h"


void albaVMESurfaceParametricTest::TestSetData()
{
  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurface;
  albaNEW(vmeParametricSurface);
	vmeParametricSurface->Update();

  //test parametric surface properties properties (all the properties are tested with vtk primitives)
  int numberOfPolyDataPoints; //known in base of the geometry and resolution
	double bounds[6];
	double boundsPolyData[6];

	albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(vmeParametricSurface->GetOutput());
	assert(surface_output);
	surface_output->Update();
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
	CPPUNIT_ASSERT(polydata);

	//default is a sphere with radius 2, phi and theta resolution 10
	numberOfPolyDataPoints = 82;
  CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());
      
	bounds[0] = -1.96962;
	bounds[1] = 1.96962;
	bounds[2] = -1.87322;
	bounds[3] = 1.87322;
	bounds[4] = -2;
	bounds[5] = 2;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);


	//cone with resolution 20 radius 2 heigth 5
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CONE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
  numberOfPolyDataPoints = 22;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());
  
	bounds[0] = -2.5;
	bounds[1] = 2.5;
	bounds[2] = -2;
	bounds[3] = 2;
	bounds[4] = -2;
	bounds[5] = 2;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//parallelepiped 2 2 2
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CUBE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 24;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = -1;
	bounds[1] = 1;
	bounds[2] = -1;
	bounds[3] = 1;
	bounds[4] = -1;
	bounds[5] = 1;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//cylinder with resolution 20 radius 2 heigth 5
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CYLINDER);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 80;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = -2;
	bounds[1] = 2;
	bounds[2] = -2.5;
	bounds[3] = 2.5;
	bounds[4] = -2;
	bounds[5] = 2;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//plane point1 2,0,0 point2 0,3,0 Xresulution 2 Yresolution 2
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_PLANE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 9;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = 0;
	bounds[1] = 2;
	bounds[2] = 0;
	bounds[3] = 3;
	bounds[4] = 0;
	bounds[5] = 0;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

  //ellipsoide with resolution 20 radius 2 heigth 5
  vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_ELLIPSOID);
  vmeParametricSurface->Modified();
  vmeParametricSurface->Update();
  numberOfPolyDataPoints = 82;
  CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

  bounds[0] = -0.9848;
  bounds[1] = 0.9848;
  bounds[2] = -1.87321;
  bounds[3] = 1.87321;
  bounds[4] = -3;
  bounds[5] = 3;

  polydata->GetBounds(boundsPolyData);
  CPPUNIT_ASSERT(
    fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
    fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
    fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
    fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
    fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
    fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
    );

	albaVMESurfaceParametric *copySurfaceParametric;
	albaNEW(copySurfaceParametric);
	
	CPPUNIT_ASSERT(copySurfaceParametric->DeepCopy(vmeParametricSurface) == ALBA_OK);

  // destroy vme
	albaDEL(copySurfaceParametric);
  albaDEL(vmeParametricSurface);
}
  
