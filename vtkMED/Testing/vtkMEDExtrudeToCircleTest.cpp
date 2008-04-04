/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircleTest.cpp,v $
Language:  C++
Date:      $Date: 2008-04-04 15:17:50 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDefines.h" 

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkCylinderSource.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkMEDExtrudeToCircle.h"
#include "vtkMEDExtrudeToCircleTest.h"

#include "mafConfigure.h"


static const bool renderingOn = true ;  // switch interactive rendering on

void vtkMEDExtrudeToCircleTest::setUp()
{
  m_testData = NULL ;
  m_extrusion = NULL ;
}

void vtkMEDExtrudeToCircleTest::tearDown()
{
  if (m_testData != NULL){
    m_testData->Delete() ;
    m_testData = NULL ;
  }

  if (m_extrusion != NULL){
    m_extrusion->Delete() ;
    m_extrusion = NULL ;
  }
}


void vtkMEDExtrudeToCircleTest::TestFixture()
{
}


//------------------------------------------------------------------------------
// render the input and output data
void vtkMEDExtrudeToCircleTest::RenderExtrusion()
//------------------------------------------------------------------------------
{
  // Create a Renderer, a RenderWindow and a RenderWindowInteractor

  vtkRenderer *R = vtkRenderer::New();
  R->SetBackground(0.5, 0.5, 0.5);
  R->SetLightFollowCamera(1);

  vtkRenderWindow *RW = vtkRenderWindow::New();
  RW->AddRenderer(R);
  RW->AddRenderer(R);
  RW->SetSize(400, 400);

  vtkRenderWindowInteractor *RWI = vtkRenderWindowInteractor::New();
  RWI->SetRenderWindow(RW);

  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New() ;
  RWI->SetInteractorStyle(style) ;


  // set up pipeline to visualize original data
  vtkPolyDataMapper *mapper1 = vtkPolyDataMapper::New();
  mapper1->SetInput(m_testData) ;

  vtkActor *A1 = vtkActor::New();
  A1->SetMapper(mapper1);


  // set up pipeline to visualize the extrusion
  vtkPolyDataMapper *mapper2 = vtkPolyDataMapper::New();
  mapper2->SetInput(m_extrusion);

  vtkActor *A2 = vtkActor::New();
  A2->SetMapper(mapper2);
  A2->GetProperty()->SetColor(1,1,0) ;


  // -------------------------------
  // Reset the camera and initialize
  // -------------------------------

  R->AddActor( A1 );
  R->AddActor( A2 );

  R->ResetCamera();
  R->ResetCameraClippingRange();
  RW->Render();
  RWI->Initialize();
  RWI->Start();


  R->Delete() ;
  RW->Delete() ;
  RWI->Delete() ;
  style->Delete() ;
  mapper1->Delete();
  A1->Delete();
  mapper2->Delete();
  A2->Delete();
}




//------------------------------------------------------------------------------
// Create test data
void vtkMEDExtrudeToCircleTest::CreateTestData() 
//------------------------------------------------------------------------------
{
  // create a cylinder with open ends
  vtkCylinderSource *cyl = vtkCylinderSource::New() ;
  cyl->SetResolution(10) ;
  cyl->SetHeight(10) ;
  cyl->SetRadius(1.0) ;
  cyl->SetCapping(0) ;

  cyl->GetOutput()->Update() ;
  m_testData = vtkPolyData::New() ;
  m_testData->DeepCopy(cyl->GetOutput()) ;


  // mess up the shape of the cylinder.
  int i ;
  double xa[3], xb[3] ;

  double xinc[10] = {0.303844709,0.342944797,0.325211305,-0.081062732,-0.37609408,-0.513841563,-0.190581172,-0.394675426,0.127765989,0.456488174} ;
  double zinc[10] = {-0.119779481,0.12174925,0.355289816,0.463100679,0.458244457,-0.240880002,-0.119889833,-0.347051271,-0.23729248,-0.333491135} ;

  // change the shape of the ends, keeping points in same plane
  for (i = 0 ;  i < 10 ;  i++){
    m_testData->GetPoint(2*i, xa) ;
    m_testData->GetPoints()->SetPoint(2*i, xa[0]+xinc[i], xa[1], xa[2]+zinc[i]) ;
    m_testData->GetPoint(2*i+1, xb) ;
    m_testData->GetPoints()->SetPoint(2*i+1, xb[0]+xinc[i], xb[1], xb[2]+zinc[i]) ;
  }

  // shear the cylinder
  for (i = 0 ;  i < 10 ;  i++){
    m_testData->GetPoint(2*i, xa) ;
    m_testData->GetPoints()->SetPoint(2*i, xa[0], xa[1]+0.2*xa[0], xa[2]) ;
    m_testData->GetPoint(2*i+1, xb) ;
    m_testData->GetPoints()->SetPoint(2*i+1, xb[0], xb[1]+0.2*xb[0], xb[2]) ;
  }

  cyl->Delete() ;
}



//------------------------------------------------------------------------------
// Test extrusion filter
void vtkMEDExtrudeToCircleTest::TestExtrusion() 
//------------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // Create the test polydata
  //----------------------------------------------------------------------------
  CreateTestData() ;


  //----------------------------------------------------------------------------
  // Find and select holes in input polydata
  //----------------------------------------------------------------------------

  // This filter extracts the edge features of the mesh
  vtkFeatureEdges *fEdges1 = vtkFeatureEdges::New() ;
  fEdges1->BoundaryEdgesOn() ;
  fEdges1->FeatureEdgesOff() ;
  fEdges1->NonManifoldEdgesOff() ;
  fEdges1->ManifoldEdgesOff() ;
  fEdges1->SetInput(m_testData) ;

  // This filter extracts connected regions of the polydata.
  // Note: it does this by changing the cells, but it does not delete the points !!
  vtkPolyDataConnectivityFilter *PDCF = vtkPolyDataConnectivityFilter::New() ;
  PDCF->SetInput(fEdges1->GetOutput()) ;
  PDCF->SetExtractionModeToSpecifiedRegions() ; // sets mode to extract only requested regions
  PDCF->InitializeSpecifiedRegionList() ;
  PDCF->AddSpecifiedRegion(0) ;                 // request this region to be extracted

  // Clean the unused points from the polydata
  vtkCleanPolyData *CPD = vtkCleanPolyData::New() ;
  CPD->SetInput(PDCF->GetOutput()) ;



  //----------------------------------------------------------------------------
  // Extrusion Pipeline
  //----------------------------------------------------------------------------

  vtkMEDExtrudeToCircle *ETC = vtkMEDExtrudeToCircle::New() ;
  ETC->SetInput(CPD->GetOutput()) ;
  double direc[3] = {0.0, -1.0, 0.0} ;
  ETC->SetDirection(direc) ;
  ETC->GetOutput()->Update() ;
  m_extrusion = vtkPolyData::New() ;
  m_extrusion->DeepCopy(ETC->GetOutput()) ;



  // render the data
  if (renderingOn)
    RenderExtrusion() ;


  // delete vtk objects
  fEdges1->Delete() ;
  PDCF->Delete() ;
  CPD->Delete() ;
  ETC->Delete() ;
}
