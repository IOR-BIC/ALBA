/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImplicitPolyDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAImplicitPolyDataTest.h"
#include "vtkALBASmartPointer.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkSphereSource.h"
#include "vtkCamera.h"
#include "vtkTriangleFilter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkSampleFunction.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkImplicitBoolean.h"


void vtkALBAImplicitPolyDataTest::TestFixture()
{

}

void vtkALBAImplicitPolyDataTest::RenderData( vtkPolyData *data )
{
  vtkALBASmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);
  
  vtkALBASmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);
  
  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInput(data);
  mapper->ScalarVisibilityOn();
  
  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);
  
  renderer->AddActor(actor);
  renderWindow->Render();

  renderWindowInteractor->Start();

}

void vtkALBAImplicitPolyDataTest::TestClipSphere1PolydataWithSphere2PolydataUsedAsImplicitFunction()
{
  // Clip one polygonal sphere with another.

  // polygonal sphere 1, this will be clipped by sphere 2
  vtkSphereSource *s1 = vtkSphereSource::New();
  s1->SetThetaResolution( 6 );
  s1->SetPhiResolution (  6 );
  s1->SetCenter( 0, 0, 0 );
  s1->SetRadius( 1.0 );
  

  // triangulate sphere 1 for input to subdivision filter
  vtkTriangleFilter *tf1 = vtkTriangleFilter::New();
  tf1->SetInput( s1->GetOutput() );

  // subdivide triangles in sphere 1 to get better clipping
  vtkLinearSubdivisionFilter *lsf1 = vtkLinearSubdivisionFilter::New();
  lsf1->SetInput( tf1->GetOutput() );
  lsf1->SetNumberOfSubdivisions( 1 );	// use this (0-3+) to see improvement in clipping

  // polygonal sphere 2, this will clip sphere 1
  vtkSphereSource *s2 = vtkSphereSource::New();
  s2->SetThetaResolution( 6 );	// vary resolution (3+) to get different clipper shapes
  s2->SetPhiResolution (  6 );
  s2->SetCenter( 0, 1, 0 );	// offset position from sphere 1
  s2->SetRadius( 1.0 );

  // use sphere 2 polydata as an implicit function
  vtkALBAImplicitPolyData *ip2 = vtkALBAImplicitPolyData::New();
  ip2->SetInput( s2->GetOutput() );

  // clip sphere 1 with sphere 2 polydata
  vtkClipPolyData *cpd1 = vtkClipPolyData::New();
  cpd1->SetInput( lsf1->GetOutput() );
  cpd1->SetClipFunction( ip2 );
  cpd1->SetGenerateClipScalars ( 0 );	// 0 outputs input data scalars, 1 outputs implicit function values
  cpd1->SetInsideOut( 0 );		// use 0/1 to reverse sense of clipping
  cpd1->SetValue( 0 );			// use this to control distance from implicit surface

  cpd1->Update();
  int cellsNumber = cpd1->GetOutput()->GetNumberOfCells();
  CPPUNIT_ASSERT(cellsNumber == 186);
  
  // Rendering for debug purposes...
  // RenderData(cpd1->GetOutput())  ;

  s1->Delete();
  tf1->Delete();
  lsf1->Delete();
  s2->Delete();
  ip2->Delete();
  cpd1->Delete();
}

void vtkALBAImplicitPolyDataTest::TestEvaluateGradient()
{
  vtkSphereSource *s = vtkSphereSource::New();
  s->SetCenter( 0, 0, 0 );
  s->SetRadius( 1.0 );
  s->Update();

  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  ip->SetInput( s->GetOutput() );
  
  double gradient[3] = {-9999,-9999,-9999};
  double point[3] = {0,0,1};
  ip->EvaluateGradient(point,gradient);

  CPPUNIT_ASSERT(gradient[0] == 0);
  CPPUNIT_ASSERT(gradient[1] == 0);
  CPPUNIT_ASSERT(gradient[2] == 1);
  
  ip->Delete();
  s->Delete();
}


void vtkALBAImplicitPolyDataTest::TestEvaluateFunction()
{

  vtkSphereSource *s = vtkSphereSource::New();
  s->SetThetaResolution( 6 );
  s->SetPhiResolution (  6 );
  s->SetCenter( 0, 0, 0 );	
  s->SetRadius( 1.0 );
  s->Update();

  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  ip->SetInput( s->GetOutput() );

  double pointOnSphere[3] = {0,0,1};
  double ret = ip->EvaluateFunction(pointOnSphere);
  CPPUNIT_ASSERT(ret == 0);

  double pointOutOfSphere[3] = {0,0,2};
  ret = ip->EvaluateFunction(pointOutOfSphere);
  CPPUNIT_ASSERT(ret > 0);

  double pointInsideSphere[3] = {0,0,0};
  ret = ip->EvaluateFunction(pointInsideSphere);
  CPPUNIT_ASSERT(ret <0);

  ip->Delete(); 
  s->Delete();
}

void vtkALBAImplicitPolyDataTest::TestSetInput()
{
  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  CPPUNIT_ASSERT(ip->Input == NULL);
  
  vtkSphereSource *s = vtkSphereSource::New();
  s->Update();
  ip->SetInput(s->GetOutput());
  
  CPPUNIT_ASSERT(ip->Input !=  NULL);

  s->Delete();
  ip->Delete();
}

void vtkALBAImplicitPolyDataTest::TestConstructorDestructor()
{
  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  CPPUNIT_ASSERT(ip != NULL);
  ip->Delete();
}

void vtkALBAImplicitPolyDataTest::TestSetGetNoValue()
{
  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  ip->SetNoValue(10);
  CPPUNIT_ASSERT(ip->GetNoValue() == 10);
  ip->Delete();
}

void vtkALBAImplicitPolyDataTest::TestSetGetNoGradient()
{
  vtkALBAImplicitPolyData *ip = vtkALBAImplicitPolyData::New();
  double inGradient[3] = {1,2,3};
  ip->SetNoGradient(inGradient);
  
  double outGradient[3] = {0,0,0};
  ip->GetNoGradient(outGradient);

  for (int i = 0; i < 3; i++) 
  {
    CPPUNIT_ASSERT(inGradient[i] == outGradient[i]);
  }
  
  ip->Delete();
}

void vtkALBAImplicitPolyDataTest::TestGenerateOffsetSurfaceFromPolydata()
{
  vtkSphereSource *s = vtkSphereSource::New();
  s->SetThetaResolution( 6 );
  s->SetPhiResolution (  6 );
  s->SetCenter( 0, 0, 0 );	
  s->SetRadius( 1.0 );
  s->Update();

  float offset = 5.0;     // desired surface offset distance
  int resolution = 10;    // xyz resolution for sampling surface

  // use users polydata as an implicit function
  vtkALBAImplicitPolyData *ip1 = vtkALBAImplicitPolyData::New();
  ip1->SetInput( s->GetOutput() ); 

  // determine enlarged volume to accommodate offset surface
  double bounds[6], buffer = offset*1.5;
  s->GetOutput()->GetBounds( bounds );
  bounds[0] -= buffer;
  bounds[1] += buffer;
  bounds[2] -= buffer;
  bounds[3] += buffer;
  bounds[4] -= buffer;
  bounds[5] += buffer;

  // sample a volume around the surface
  vtkSampleFunction *sf1 = vtkSampleFunction::New();
  sf1->SetImplicitFunction( ip1 );
  sf1->SetModelBounds( bounds );
  sf1->SetSampleDimensions( resolution, resolution, resolution );    // define xyz resolution
  sf1->ComputeNormalsOff();

  vtkContourFilter *cf1 = vtkContourFilter::New();
  cf1->SetInput( sf1->GetOutput() );
  cf1->SetValue( 0, offset );
  
  int numCells = cf1->GetOutput()->GetNumberOfCells();
  CPPUNIT_ASSERT(numCells == 0);

  cf1->Update();
  
  numCells = cf1->GetOutput()->GetNumberOfCells();
  CPPUNIT_ASSERT(numCells == 380);

  // rendering for debug purposes...
  // RenderData(cf1->GetOutput());

  s->Delete();
  ip1->Delete();
  sf1->Delete();
  cf1->Delete();
}

void vtkALBAImplicitPolyDataTest::TestUnionBetweenTwoPolydata()
{
  // Determine difference between two sets of PolyData using vtkImplicitBoolean.

  int resolution = 10;    // xyz resolution for sampling surface
  
  vtkSphereSource *s1 = vtkSphereSource::New();
  s1->SetThetaResolution( 6 );
  s1->SetPhiResolution (  6 );
  s1->SetCenter( 0, 0, 0 );
  s1->SetRadius( 1.0 );

  vtkSphereSource *s2 = vtkSphereSource::New();
  s2->SetThetaResolution( 6 );
  s2->SetPhiResolution (  6 );
  s2->SetCenter( 0.7, 0, 0 );
  s2->SetRadius( 1.0 );

  // treat users polydata as an implicit function
  vtkALBAImplicitPolyData *ip1 = vtkALBAImplicitPolyData::New();
  ip1->SetInput(s1->GetOutput()); // user might have to use ->GetOutput() from an upstream filter in vtk

  // treat users polydata as an implicit function
  vtkALBAImplicitPolyData *ip2 = vtkALBAImplicitPolyData::New();
  ip2->SetInput(s2->GetOutput()); // user might have to use ->GetOutput() from an upstream filter in vtk

  // make a boolean combination of the two surfaces
  vtkImplicitBoolean * ib1 = vtkImplicitBoolean::New();
  ib1->SetOperationTypeToUnion(); // see notes below
  ib1->AddFunction( ip1 );
  ib1->AddFunction( ip2 );

  // determine enlarged volume to accommodate both poydata sets
  double bounds1[6], bounds2[6];
  s1->GetOutput()->GetBounds( bounds1 );
  s2->GetOutput()->GetBounds( bounds2 );
  if( bounds2[0] < bounds1[0] ) bounds1[0] = bounds2[0];
  if( bounds2[1] > bounds1[1] ) bounds1[1] = bounds2[1];
  if( bounds2[2] < bounds1[2] ) bounds1[2] = bounds2[2];
  if( bounds2[3] > bounds1[3] ) bounds1[3] = bounds2[3];
  if( bounds2[4] < bounds1[4] ) bounds1[4] = bounds2[4];
  if( bounds2[5] > bounds1[5] ) bounds1[5] = bounds2[5];

  // sample a volume around the implicit surface
  vtkSampleFunction *sf1 = vtkSampleFunction::New();
  sf1->SetImplicitFunction( ib1 );
  sf1->SetModelBounds( bounds1 );
  sf1->SetSampleDimensions( resolution, resolution, resolution );    // define xyz resolution
  sf1->ComputeNormalsOff();

  double offset = 0;

  vtkContourFilter *cf1 = vtkContourFilter::New();
  cf1->SetInput( sf1->GetOutput() );
  cf1->SetValue( 0, offset );
  
  int numCells = cf1->GetOutput()->GetNumberOfCells();
  CPPUNIT_ASSERT(numCells == 0);

  cf1->Update();

  numCells = cf1->GetOutput()->GetNumberOfCells();
  CPPUNIT_ASSERT(numCells == 716);

  // rendering for debug purposes...
  // RenderData(cf1->GetOutput());

  s1->Delete();
  s2->Delete();
  ip1->Delete();
  ip2->Delete();
  ib1->Delete();
  sf1->Delete();
  cf1->Delete();
}