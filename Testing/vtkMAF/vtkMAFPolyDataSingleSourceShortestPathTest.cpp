/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataSingleSourceShortestPathTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFPolyDataSingleSourceShortestPathTest.h"

#include "vtkMAFSmartPointer.h"

#include "vtkActor.h"
#include "vtkPolyDatamapper.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTubeFilter.h"
#include "vtkMAFPolyDataSingleSourceShortestPath.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkMath.h"


//-------------------------------------------------------
void vtkMAFPolyDataSingleSourceShortestPathTest::setUp()
//-------------------------------------------------------
{

}
//-------------------------------------------------------
void vtkMAFPolyDataSingleSourceShortestPathTest::tearDown()
//-------------------------------------------------------
{

}
//-------------------------------------------------------
void vtkMAFPolyDataSingleSourceShortestPathTest::TestFixture()
//-------------------------------------------------------
{

}
//-------------------------------------------------------
void vtkMAFPolyDataSingleSourceShortestPathTest::TestDynamicAllocation()
//-------------------------------------------------------
{
  vtkMAFPolyDataSingleSourceShortestPath *sp = NULL;
  sp = vtkMAFPolyDataSingleSourceShortestPath::New();

  sp->Delete();
  sp = NULL;
}
/*void vtkMAFPolyDataSingleSourceShortestPathTest::TestStaticAllocation()
{
  vtkMAFPolyDataSingleSourceShortestPath coord;
}*/
//-------------------------------------------------------
void vtkMAFPolyDataSingleSourceShortestPathTest::TestFilter()
//-------------------------------------------------------
{
  // create a rendering window and renderer
	vtkRenderer *ren = vtkRenderer::New();
	vtkRenderWindow *renWindow = vtkRenderWindow::New();
	renWindow->AddRenderer(ren);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWindow);
	renWindow->SetSize( 300, 300 );
	
	vtkSphereSource *sphere = vtkSphereSource::New();
	sphere->SetPhiResolution(30); sphere->SetThetaResolution(30);
  sphere->SetRadius(10.);
	
	// Distort the shape of the sphere
	vtkTransform *trans = vtkTransform::New();
	//trans->Scale(1.3, 2.5, 0.8);
  trans->Scale(1., 1., 1.);
	
	vtkTransformPolyDataFilter *transformer = vtkTransformPolyDataFilter::New();
	transformer->SetInput(sphere->GetOutput());
	transformer->SetTransform(trans);
	
	// Generate a shortest path between two vertices
	// and show the geodesic distance to the start vertex with color coding
	vtkMAFPolyDataSingleSourceShortestPath *path = vtkMAFPolyDataSingleSourceShortestPath::New();
	path->SetInput(transformer->GetOutput());
	path->SetStartVertex(7);
	path->SetEndVertex(297);
	path->Update();
	
	transformer->GetOutput()->GetPointData()->SetScalars((vtkDataArray*)path->Getd());
	
	// Use a tube filter to enhance the appearance of the found path
	vtkTubeFilter *tube = vtkTubeFilter::New();
	tube->SetInput(path->GetOutput());
	tube->SetNumberOfSides(16);
	tube->SetRadius(0.2);
		
	vtkPolyDataMapper *sphereMapper = vtkPolyDataMapper::New();
	sphereMapper->SetInput(transformer->GetOutput());
	sphereMapper->SetScalarRange(transformer->GetOutput()->GetPointData()->GetScalars()->GetRange());
	vtkActor *sphereActor = vtkActor::New();
	sphereActor->SetMapper(sphereMapper);
  sphereActor->GetProperty()->SetRepresentationToWireframe();
  sphereActor->GetProperty()->SetOpacity(0.5);
	
	vtkPolyDataMapper *pathMapper = vtkPolyDataMapper::New();
	pathMapper->SetInput(tube->GetOutput());
	vtkActor *pathActor = vtkActor::New();
	pathActor->SetMapper(pathMapper);
	
	ren->AddActor(sphereActor);
	ren->AddActor(pathActor);
	
	renWindow->Render();
	
  mafSleep(1000);
	//iren->Start();
	
  CPPUNIT_ASSERT(CheckCoerenceValue(sphere->GetOutput(),path->GetPathLength()));

	ren->Delete();
	renWindow->Delete();
	iren->Delete();
	sphere->Delete();
	sphereMapper->Delete();
	sphereActor->Delete();
	pathMapper->Delete();
	pathActor->Delete();
	tube->Delete();
	transformer->Delete();
	trans->Delete();
	path->Delete();

}
//-------------------------------------------------------
bool vtkMAFPolyDataSingleSourceShortestPathTest::CheckCoerenceValue(vtkPolyData *polySphere, double value)
//-------------------------------------------------------
{
  bool result = false;
  double point1[3], point2[3];
  polySphere->GetPoints()->GetPoint(7, point1);
  polySphere->GetPoints()->GetPoint(197, point2);
  double distance = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
  double lengthPath = value;
  return lengthPath >= distance;
}
