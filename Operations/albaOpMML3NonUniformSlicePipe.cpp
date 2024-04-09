/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3NonUniformSlicePipe
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"

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
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkSphereSource.h"
#include "vtkMatrix4x4.h"
#include "vtkCoordinate.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

#include "vtkALBAMatrixVectorMath.h"
#include "albaOpMML3NonUniformSlicePipe.h"

#include <vector>

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif



//------------------------------------------------------------------------------
// constructor
albaOpMML3NonUniformSlicePipe::albaOpMML3NonUniformSlicePipe(vtkPolyData *surface, vtkRenderer *renderer, int numberOfSections)
: m_Renderer(renderer), m_NumberOfSections(numberOfSections), m_NumberOfSlices(0)
//------------------------------------------------------------------------------
{
  m_Math = vtkALBAMatrixVectorMath::New() ;

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
  transformFilter->SetInputData(surface) ;
  transformFilter->SetTransform(m_Transform) ;

  vtkPlane *plane = vtkPlane::New() ;
  plane->SetNormal(0,0,1) ;
  plane->SetOrigin(0,0,0) ;

  vtkCutter *cutter = vtkCutter::New();
  cutter->SetCutFunction(plane);
  cutter->SetInputConnection(transformFilter->GetOutputPort());
  cutter->SetNumberOfContours(1) ;
  cutter->SetValue(0, 0.0);          // set the function value which defines each contour
  cutter->SetSortBy(0);
  cutter->SetGenerateCutScalars(0);

  vtkPolyDataMapper2D *contourMapper = vtkPolyDataMapper2D::New() ;
  contourMapper->SetInputConnection(cutter->GetOutputPort()) ;
  contourMapper->SetTransformCoordinate(coordSystem) ;

  m_ContourActor = vtkActor2D::New() ;
  m_ContourActor->SetMapper(contourMapper) ;
  m_ContourActor->GetProperty()->SetColor(1,0,0) ;

  m_Renderer->AddActor2D(m_ContourActor) ;





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
    sphereTransform[i]->SetInputConnection(m_Sphere[i]->GetOutputPort()) ;
    sphereTransform[i]->SetTransform(m_Transform) ;

    sphereMapper[i] = vtkPolyDataMapper2D::New() ;
    sphereMapper[i]->SetInputConnection(sphereTransform[i]->GetOutputPort()) ;
    sphereMapper[i]->SetTransformCoordinate(coordSystem) ;

    m_SphereActor[i] = vtkActor2D::New() ;
    m_SphereActor[i]->SetMapper(sphereMapper[i]) ;
    m_SphereActor[i]->GetProperty()->SetColor(1,1,0) ;
    renderer->AddActor2D(m_SphereActor[i]) ;
  }




  //----------------------------------------------------------------------------
  // set the pipeline for the section lines
  //----------------------------------------------------------------------------
  int numberOfSectionLines = m_NumberOfSections+1 ;

  vtkPoints *sectionLinesEndPts = vtkPoints::New() ;
  sectionLinesEndPts->Initialize() ;
  for (int i = 0 ;  i < numberOfSectionLines ;  i++){
    double y = (double)i / (double)(numberOfSectionLines-1) ;
    sectionLinesEndPts->InsertNextPoint(0.0, y, 0) ;
    sectionLinesEndPts->InsertNextPoint(0.05, y, 0) ;
    sectionLinesEndPts->InsertNextPoint(0.95, y, 0) ;
    sectionLinesEndPts->InsertNextPoint(1.0, y, 0) ;
  }

  vtkCellArray *sectionLinesCells = vtkCellArray::New() ;
  for (int i = 0 ;  i < numberOfSectionLines ;  i++){
		vtkIdType endPtIndices[2] ;

    // left tick mark
    endPtIndices[0] = 4*i ;
    endPtIndices[1] = 4*i + 1 ;
    sectionLinesCells->InsertNextCell(2, endPtIndices) ;

    // right tick mark
    endPtIndices[0] = 4*i + 2 ;
    endPtIndices[1] = 4*i + 3 ;
    sectionLinesCells->InsertNextCell(2, endPtIndices) ;
  }

  vtkPolyData *sectionLinesPolydata = vtkPolyData::New() ;
  sectionLinesPolydata->SetPoints(sectionLinesEndPts) ;
  sectionLinesPolydata->SetLines(sectionLinesCells) ;
  sectionLinesEndPts->Delete() ;
  sectionLinesCells->Delete() ;

  vtkPolyDataMapper2D *sectionLinesMapper = vtkPolyDataMapper2D::New() ;
  sectionLinesMapper->SetInputData(sectionLinesPolydata) ;
  sectionLinesMapper->SetTransformCoordinate(coordSystem) ;

  m_SectionLinesActor = vtkActor2D::New() ;
  m_SectionLinesActor->SetMapper(sectionLinesMapper) ;
  m_SectionLinesActor->GetProperty()->SetColor(1,1,0) ;
  m_SectionLinesActor->GetProperty()->SetOpacity(1.0) ;
  renderer->AddActor2D(m_SectionLinesActor) ;



  //----------------------------------------------------------------------------
  // set the pipeline for the slice lines, initially with no lines
  //----------------------------------------------------------------------------
  m_NumberOfSlices = 0 ;

  vtkPoints *sliceLinesEndPts = vtkPoints::New() ;
  sliceLinesEndPts->Initialize() ;

  vtkCellArray *sliceLinesCells = vtkCellArray::New() ;
  sliceLinesCells->Initialize() ;

  m_SliceLinesPolydata = vtkPolyData::New() ;
  m_SliceLinesPolydata->SetPoints(sliceLinesEndPts) ;
  m_SliceLinesPolydata->SetLines(sliceLinesCells) ;
  sliceLinesEndPts->Delete() ;
  sliceLinesCells->Delete() ;

  vtkPolyDataMapper2D *sliceLinesMapper = vtkPolyDataMapper2D::New() ;
  sliceLinesMapper->SetInputData(m_SliceLinesPolydata) ;
  sliceLinesMapper->SetTransformCoordinate(coordSystem) ;

  m_SliceLinesActor = vtkActor2D::New() ;
  m_SliceLinesActor->SetMapper(sliceLinesMapper) ;
  m_SliceLinesActor->GetProperty()->SetColor(1,1,1) ;
  m_SliceLinesActor->GetProperty()->SetOpacity(0.3) ;
  renderer->AddActor2D(m_SliceLinesActor) ;






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

  sectionLinesPolydata->Delete() ;
  sectionLinesMapper->Delete() ;

  sliceLinesMapper->Delete() ; // don't delete slice lines polydata here because we want to use it elsewhere

  coordSystem->Delete() ;

  for (int i = 0 ;  i < 2 ;  i++){
    sphereMapper[i]->Delete() ;
    sphereTransform[i]->Delete() ;
  }
}



//------------------------------------------------------------------------------
// deconstructor
albaOpMML3NonUniformSlicePipe::~albaOpMML3NonUniformSlicePipe()
//------------------------------------------------------------------------------
{
  m_Math->Delete() ;

  m_Transform->Delete() ;
  m_ContourActor->Delete() ;
  m_SectionLinesActor->Delete() ;
  m_SliceLinesActor->Delete() ;

  m_SliceLinesPolydata->Delete() ;

  for (int i = 0 ;  i < 2 ;  i++){
    m_Sphere[i]->Delete() ;
    m_SphereActor[i]->Delete() ;
  }
}




//------------------------------------------------------------------------------
// set coords of ends of axis
void albaOpMML3NonUniformSlicePipe::SetEndsOfAxis(double *x0, double *x1)
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
void albaOpMML3NonUniformSlicePipe::CalculateAxisVector(double *u)
//------------------------------------------------------------------------------
{
  m_Math->SubtractVectors(m_AxisEnd, m_AxisStart, u) ;
  m_Math->NormalizeVector(u) ;
}



//------------------------------------------------------------------------------
// calculate axis length
double albaOpMML3NonUniformSlicePipe::CalculateAxisLength()
//------------------------------------------------------------------------------
{
  double u[3], length ;

  m_Math->SubtractVectors(m_AxisEnd, m_AxisStart, u) ;
  length = m_Math->MagnitudeOfVector(u) ;

  return length ;
}



//------------------------------------------------------------------------------
// calculate axis mid-point
void albaOpMML3NonUniformSlicePipe::CalculateAxisMidPoint(double *midPoint)
//------------------------------------------------------------------------------
{
  m_Math->AddVectors(m_AxisEnd, m_AxisStart, midPoint) ;
  m_Math->DivideVectorByScalar(2.0, midPoint, midPoint) ;
}



//------------------------------------------------------------------------------
// calculate normals to axis
void albaOpMML3NonUniformSlicePipe::CalculateAxisNormals(double *v,  double *w)
//------------------------------------------------------------------------------
{
  // get vector direction of axis
  double u[3] ;
  CalculateAxisVector(u) ;

  m_Math->CalculateNormalsToU(u,v,w) ;
  m_Math->NormalizeVector(v) ;
  m_Math->NormalizeVector(w) ;
}





//------------------------------------------------------------------------------
// set positions of slices
void albaOpMML3NonUniformSlicePipe::SetSlicePositions(int numberOfSlices, double *alpha)
//------------------------------------------------------------------------------
{
  m_Alpha.clear() ;

  m_NumberOfSlices = numberOfSlices ;
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++)
    m_Alpha.push_back(alpha[i]) ;
}



//------------------------------------------------------------------------------
// update the transform
void albaOpMML3NonUniformSlicePipe::UpdateTransform()
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
void albaOpMML3NonUniformSlicePipe::UpdateSpheres()
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
// update slice lines
void albaOpMML3NonUniformSlicePipe::UpdateSliceLines()
//------------------------------------------------------------------------------
{
  vtkPoints *sliceLinesEndPts = m_SliceLinesPolydata->GetPoints() ;
  sliceLinesEndPts->Initialize() ;
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    sliceLinesEndPts->InsertNextPoint(0, m_Alpha.at(i), 0) ;
    sliceLinesEndPts->InsertNextPoint(1, m_Alpha.at(i), 0) ;
  }

  vtkCellArray *sliceLinesCells = m_SliceLinesPolydata->GetLines() ;
  sliceLinesCells->Initialize() ;
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
		vtkIdType endPtIndices[2] ;
    endPtIndices[0] = 2*i ;
    endPtIndices[1] = 2*i+1 ;
    sliceLinesCells->InsertNextCell(2, endPtIndices) ;
  }

  m_SliceLinesPolydata->Modified() ;
}




//------------------------------------------------------------------------------
/// update the pipeline objects
void albaOpMML3NonUniformSlicePipe::Update()
//------------------------------------------------------------------------------
{
  UpdateTransform() ;
  UpdateSpheres() ;
  UpdateSliceLines() ;
  m_Renderer->GetRenderWindow()->Render() ;
}
