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

	//default is a sphere with radius 20, phi and theta resolution 25
	numberOfPolyDataPoints = 577;
  CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());
      
	bounds[0] = -19.84229;
	bounds[1] = 20.0;
	bounds[2] = -19.96053;
	bounds[3] = 19.96053;
	bounds[4] = -20;
	bounds[5] = 20;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);


	//cone with resolution 25 radius 20 heigth 50
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CONE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
  numberOfPolyDataPoints = 27;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());
  
	bounds[0] = -25.0;
	bounds[1] = 25.0;
	bounds[2] = -19.84229;
	bounds[3] = 20;
	bounds[4] = -19.96053;
	bounds[5] = 19.96053;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//parallelepiped 2 2 2, side 10
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CUBE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 24;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = -10;
	bounds[1] = 10;
	bounds[2] = -10;
	bounds[3] = 10;
	bounds[4] = -10;
	bounds[5] = 10;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//cylinder with resolution 25 radius 20 heigth 50
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CYLINDER);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 100;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = -19.84229;
	bounds[1] = 20.0;
	bounds[2] = -25.0;
	bounds[3] = 25.0;
	bounds[4] = -19.96053;
	bounds[5] = 19.96053;

	polydata->GetBounds(boundsPolyData);
	CPPUNIT_ASSERT(
		fabs(boundsPolyData[0] - bounds[0]) < 0.0001 &&
		fabs(boundsPolyData[1] - bounds[1]) < 0.0001 &&
		fabs(boundsPolyData[2] - bounds[2]) < 0.0001 &&
		fabs(boundsPolyData[3] - bounds[3]) < 0.0001 &&
		fabs(boundsPolyData[4] - bounds[4]) < 0.0001 &&
		fabs(boundsPolyData[5] - bounds[5]) < 0.0001 
		);

	//plane point1 20,0,0 point2 0,30,0 Xresulution 2 Yresolution 2
	vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_PLANE);
	vmeParametricSurface->Modified();
	vmeParametricSurface->Update();
	numberOfPolyDataPoints = 9;
	CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

	bounds[0] = 0;
	bounds[1] = 20.0;
	bounds[2] = 0;
	bounds[3] = 30.0;
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

  //ellipsoide with resolution 25 lengths 15, 20, 25
  vmeParametricSurface->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_ELLIPSOID);
  vmeParametricSurface->Modified();
  vmeParametricSurface->Update();
  numberOfPolyDataPoints = 577;
  CPPUNIT_ASSERT(numberOfPolyDataPoints == polydata->GetNumberOfPoints());

  bounds[0] = -14.88172;
  bounds[1] = 15.0;
  bounds[2] = -19.96053;
  bounds[3] = 19.96053;
  bounds[4] = -25;
  bounds[5] = 25;

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
  
