/*=========================================================================
Program:   Multimod Application Framework
#include "  Module:    $RCSfile: medOpMML3NonUniformSlicePipe.cpp,v $
Language:  C++
Date:      $Date: 2009-09-18 08:10:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h"

#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkSphereSource.h"
#include "vtkMatrix4x4.h"
#include "vtkCoordinate.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include "medOpMatrixVectorMath.h"
#include "medOpMML3NonUniformSlicePipe.h"

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif



//------------------------------------------------------------------------------
// constructor
medOpMML3NonUniformSlicePipe::medOpMML3NonUniformSlicePipe(vtkPolyData *surface, vtkRenderer *renderer, int numberOfSections)
: m_Renderer(renderer), m_NumberOfSections(numberOfSections)
//------------------------------------------------------------------------------
{
  // default values for end-of-axis landmarks
  m_AxisStart[0] = 100.0 ;
  m_AxisStart[1] = 100.0 ;
  m_AxisStart[2] = 100.0 ;
  m_AxisEnd[0] = 200.0 ;
  m_AxisEnd[1] = 300.0 ;
  m_AxisEnd[2] = 300.0 ;


  //----------------------------------------------------------------------------
  // Create a vtkCoord object and set to normalized viewport
  // This controls the mapping from 3D to 2D
  //----------------------------------------------------------------------------
  vtkCoordinate *coordSystem = vtkCoordinate::New() ;
  coordSystem->SetCoordinateSystemToNormalizedViewport() ;



  //----------------------------------------------------------------------------
  // set the contour cutting pipeline
  //----------------------------------------------------------------------------
  m_Transform = vtkTransform::New() ;

  vtkTransformPolyDataFilter *transformFilter = vtkTransformPolyDataFilter::New() ;
  transformFilter->SetInput(surface) ;
  transformFilter->SetTransform(m_Transform) ;

  vtkPlane *plane = vtkPlane::New() ;
  plane->SetNormal(0,0,1) ;
  plane->SetOrigin(0,0,0) ;

  vtkCutter *cutter = vtkCutter::New();
  cutter->SetCutFunction(plane);
  cutter->SetInput((vtkDataSet *) transformFilter->GetOutput());
  cutter->SetNumberOfContours(1) ;
  cutter->SetValue(0, 0.0);          // set the function value which defines each contour
  cutter->SetSortBy(0);
  cutter->SetGenerateCutScalars(0);

  vtkPolyDataMapper2D *contourMapper = vtkPolyDataMapper2D::New() ;
  contourMapper->SetInput(cutter->GetOutput()) ;
  contourMapper->SetTransformCoordinate(coordSystem) ;

  m_ContourActor = vtkActor2D::New() ;
  m_ContourActor->SetMapper(contourMapper) ;
  m_ContourActor->GetProperty()->SetColor(1,0,0) ;

  m_Renderer->AddActor2D(m_ContourActor) ;



  //----------------------------------------------------------------------------
  // set the surface display pipeline
  //----------------------------------------------------------------------------
  vtkTransformPolyDataFilter *surfaceTransform = vtkTransformPolyDataFilter::New() ;
  surfaceTransform->SetInput(surface) ;
  surfaceTransform->SetTransform(m_Transform) ;

  vtkPolyDataMapper2D *surfaceMapper = vtkPolyDataMapper2D::New() ;
  surfaceMapper->SetInput(surfaceTransform->GetOutput()) ;
  surfaceMapper->SetTransformCoordinate(coordSystem) ;

  m_SurfaceActor = vtkActor2D::New() ;
  m_SurfaceActor->SetMapper(surfaceMapper) ;

  //m_Renderer->AddActor2D(m_SurfaceActor) ;





  //----------------------------------------------------------------------------
  // set the landmarks pipeline
  //----------------------------------------------------------------------------
  vtkPolyDataMapper2D *sphereMapper[2] ;
  vtkTransformPolyDataFilter *sphereTransform[2] ;

  for (int i = 0 ;  i < 2 ;  i++){
    m_Sphere[i] = vtkSphereSource::New() ;
    m_Sphere[i]->SetPhiResolution(5) ;
    m_Sphere[i]->SetThetaResolution(5) ;
    m_Sphere[i]->SetRadius(5) ;

    sphereTransform[i] = vtkTransformPolyDataFilter::New() ;
    sphereTransform[i]->SetInput(m_Sphere[i]->GetOutput()) ;
    sphereTransform[i]->SetTransform(m_Transform) ;

    sphereMapper[i] = vtkPolyDataMapper2D::New() ;
    sphereMapper[i]->SetInput(sphereTransform[i]->GetOutput()) ;
    sphereMapper[i]->SetTransformCoordinate(coordSystem) ;

    m_SphereActor[i] = vtkActor2D::New() ;
    m_SphereActor[i]->SetMapper(sphereMapper[i]) ;
    m_SphereActor[i]->GetProperty()->SetColor(1,1,0) ;
    renderer->AddActor2D(m_SphereActor[i]) ;
  }




  //----------------------------------------------------------------------------
  // set the pipeline for the section lines
  //----------------------------------------------------------------------------
  int nlines = m_NumberOfSections+1 ;

  vtkPoints *sectionLineEndPts = vtkPoints::New() ;
  sectionLineEndPts->Initialize() ;
  for (int i = 0 ;  i < nlines ;  i++){
    double y = (double)i / (double)(nlines-1) ;
    sectionLineEndPts->InsertNextPoint(0, y, 0) ;
    sectionLineEndPts->InsertNextPoint(1, y, 0) ;
  }

  int endPtIndices[2] ;
  vtkCellArray *lineCells = vtkCellArray::New() ;
  for (int i = 0 ;  i < nlines ;  i++){
    endPtIndices[0] = 2*i ;
    endPtIndices[1] = 2*i+1 ;
    lineCells->InsertNextCell(2, endPtIndices) ;
  }

  vtkPolyData *sectionLinesPolydata = vtkPolyData::New() ;
  sectionLinesPolydata->SetPoints(sectionLineEndPts) ;
  sectionLinesPolydata->SetLines(lineCells) ;
  sectionLineEndPts->Delete() ;
  lineCells->Delete() ;

  vtkPolyDataMapper2D *sectionLinesMapper = vtkPolyDataMapper2D::New() ;
  sectionLinesMapper->SetInput(sectionLinesPolydata) ;
  sectionLinesMapper->SetTransformCoordinate(coordSystem) ;

  m_SectionLinesActor = vtkActor2D::New() ;
  m_SectionLinesActor->SetMapper(sectionLinesMapper) ;
  m_SectionLinesActor->GetProperty()->SetColor(1,1,0) ;
  renderer->AddActor2D(m_SectionLinesActor) ;





  //----------------------------------------------------------------------------
  // update the parameters of the pipeline
  //----------------------------------------------------------------------------
  Update() ;



  //----------------------------------------------------------------------------
  // delete reference counted parts of pipeline
  //----------------------------------------------------------------------------
  plane->Delete() ;
  transformFilter->Delete() ;
  cutter->Delete() ;
  contourMapper->Delete() ;

  surfaceTransform->Delete() ;
  surfaceMapper->Delete() ;

  sectionLinesPolydata->Delete() ;
  sectionLinesMapper->Delete() ;

  coordSystem->Delete() ;

  for (int i = 0 ;  i < 2 ;  i++){
    sphereMapper[i]->Delete() ;
    sphereTransform[i]->Delete() ;
  }
}



//------------------------------------------------------------------------------
// deconstructor
medOpMML3NonUniformSlicePipe::~medOpMML3NonUniformSlicePipe()
//------------------------------------------------------------------------------
{
  m_Transform->Delete() ;
  m_ContourActor->Delete() ;
  m_SurfaceActor->Delete() ;
  m_SectionLinesActor->Delete() ;

  for (int i = 0 ;  i < 2 ;  i++){
    m_Sphere[i]->Delete() ;
    m_SphereActor[i]->Delete() ;
  }
}




//------------------------------------------------------------------------------
// set coords of ends of axis
void medOpMML3NonUniformSlicePipe::SetEndsOfAxis(double *x0, double *x1)
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++){
    m_AxisStart[i] = x0[i] ;
    m_AxisEnd[i] = x1[i] ;
  }

  // update the necessary parts of the pipeline
  Update() ;
}





//------------------------------------------------------------------------------
// calculate axis unit vector
void medOpMML3NonUniformSlicePipe::CalculateAxisVector(double *u)
//------------------------------------------------------------------------------
{
  medOpMatrixVectorMath *vecMath = new medOpMatrixVectorMath ;

  vecMath->SubtractVectors(m_AxisEnd, m_AxisStart, u) ;
  vecMath->NormalizeVector(u) ;

  delete vecMath ;
}



//------------------------------------------------------------------------------
// calculate axis length
double medOpMML3NonUniformSlicePipe::CalculateAxisLength()
//------------------------------------------------------------------------------
{
  double u[3], length ;

  medOpMatrixVectorMath *vecMath = new medOpMatrixVectorMath ;

  vecMath->SubtractVectors(m_AxisEnd, m_AxisStart, u) ;
  length = vecMath->MagnitudeOfVector(u) ;

  delete vecMath ;
  return length ;
}



//------------------------------------------------------------------------------
// calculate axis mid-point
void medOpMML3NonUniformSlicePipe::CalculateAxisMidPoint(double *midPoint)
//------------------------------------------------------------------------------
{
  medOpMatrixVectorMath *vecMath = new medOpMatrixVectorMath ;

  vecMath->AddVectors(m_AxisEnd, m_AxisStart, midPoint) ;
  vecMath->DivideVectorByScalar(2.0, midPoint, midPoint) ;

  delete vecMath ;
}



//------------------------------------------------------------------------------
// calculate normals to axis
void medOpMML3NonUniformSlicePipe::CalculateAxisNormals(double *v,  double *w)
//------------------------------------------------------------------------------
{
  // get vector direction of axis
  double u[3] ;
  CalculateAxisVector(u) ;

  medOpMatrixVectorMath *vecMath = new medOpMatrixVectorMath ;

  vecMath->CalculateNormalsToU(u,v,w) ;
  vecMath->NormalizeVector(v) ;
  vecMath->NormalizeVector(w) ;

  delete vecMath ;
}




//------------------------------------------------------------------------------
// update the transform
void medOpMML3NonUniformSlicePipe::UpdateTransform()
//------------------------------------------------------------------------------
{
  double u[3], v[3], w[3], midPoint[3], len ;

  // get axis and its normal vectors
  CalculateAxisVector(u) ;
  CalculateAxisNormals(v, w) ;

  // get axis midpoint
  CalculateAxisMidPoint(midPoint) ;

  // get axis length
  len = CalculateAxisLength() ;

  // translate so that midpoint moves to (0,0,0)
  vtkMatrix4x4 *tmat = vtkMatrix4x4::New() ;
  tmat->Identity() ;
  for (int i = 0 ;  i < 3 ;  i++)
    tmat->SetElement(i,3,-midPoint[i]) ;

  // rotate so that axis becomes x and normal becomes y
  vtkMatrix4x4 *rmat = vtkMatrix4x4::New() ;
  rmat->Identity() ;
  for (int i = 0 ;  i < 3 ;  i++){
    rmat->SetElement(0,i,w[i]) ;  
    rmat->SetElement(1,i,u[i]) ;
    rmat->SetElement(2,i,v[i]) ;
  }

  // scale so that axis length fits into range of normalized viewport [0,1]
  vtkMatrix4x4 *smat = vtkMatrix4x4::New() ;
  smat->Identity() ;
  for (int i = 0 ;  i < 3 ;  i++)
    smat->SetElement(i, i, 1.0/len) ;

  // translate so that midpoint moves to centre of viewport (0.5, 0.5, 0)
  vtkMatrix4x4 *posmat = vtkMatrix4x4::New() ;
  posmat->Identity() ;
  posmat->SetElement(0, 3, 0.5) ;
  posmat->SetElement(1, 3, 0.5) ;
  posmat->SetElement(2, 3, 0) ;

  vtkMatrix4x4 *mat = m_Transform->GetMatrix() ;
  mat->Identity() ;
  mat->Multiply4x4(tmat, mat, mat) ;
  mat->Multiply4x4(rmat, mat, mat) ;
  mat->Multiply4x4(smat, mat, mat) ;
  mat->Multiply4x4(posmat, mat, mat) ;

  tmat->Delete() ;
  rmat->Delete() ;
}




//------------------------------------------------------------------------------
// update the position and orientation of the slicing plane
void medOpMML3NonUniformSlicePipe::UpdateSpheres()
//------------------------------------------------------------------------------
{
  // set sphere positions
  m_Sphere[0]->SetCenter(m_AxisStart) ;
  m_Sphere[1]->SetCenter(m_AxisEnd) ;

  // set sphere sizes
  double len = CalculateAxisLength() ;
  m_Sphere[0]->SetRadius(len/100.0) ;
  m_Sphere[1]->SetRadius(len/100.0) ;
}






//------------------------------------------------------------------------------
/// update the pipeline objects
void medOpMML3NonUniformSlicePipe::Update()
//------------------------------------------------------------------------------
{
  UpdateTransform() ;
  UpdateSpheres() ;
}
