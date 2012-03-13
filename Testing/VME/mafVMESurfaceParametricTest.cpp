/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMESurfaceParametricTest.cpp,v $
Language:  C++
Date:      $Date: 2007-04-17 12:44:27 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMESurfaceParametricTest.h"
#include <iostream>
#include "mafVMESurfaceParametric.h"
#include "mafVMEOutputSurface.h"
#include "vtkPolyData.h"


void mafVMESurfaceParametricTest::TestSetData()
{
  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurface;
  mafNEW(vmeParametricSurface);
	vmeParametricSurface->Update();

  //test parametric surface properties properties (all the properties are tested with vtk primitives)
  int numberOfPolyDataPoints; //known in base of the geometry and resolution
	double bounds[6];
	double boundsPolyData[6];

	mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(vmeParametricSurface->GetOutput());
	assert(surface_output);
	surface_output->Update();
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
	CPPUNIT_ASSERT(polydata);
	polydata->Update();

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
	vmeParametricSurface->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_CONE);
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
	vmeParametricSurface->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_CUBE);
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
	vmeParametricSurface->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_CYLINDER);
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
	vmeParametricSurface->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_PLANE);
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

	mafVMESurfaceParametric *copySurfaceParametric;
	mafNEW(copySurfaceParametric);
	
	CPPUNIT_ASSERT(copySurfaceParametric->DeepCopy(vmeParametricSurface) == MAF_OK);

  // destroy vme
	mafDEL(copySurfaceParametric);
  mafDEL(vmeParametricSurface);
}
  
