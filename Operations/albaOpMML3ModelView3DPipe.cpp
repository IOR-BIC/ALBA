/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ModelView3DPipe
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"

#include "vtkDataSet.h"
#include "vtkMatrix4x4.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkProbeFilter.h"
#include "vtkCutter.h"
#include "vtkPlane.h"
#include "vtkTubeFilter.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkImplicitFunction.h"
#include "vtkWindowLevelLookupTable.h"

#include <ostream>
#include <algorithm>



#include "albaOpMML3ModelView3DPipe.h"

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
albaOpMML3ModelView3DPipe::albaOpMML3ModelView3DPipe
(vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume, int numberOfSlices)
 : m_Renderer(ren), m_NumberOfSlices(numberOfSlices), m_CurrentSliceId(0),
 m_NumberOfLandmarks(0), m_NumberOfAxisLandmarks(0),
 m_CurrentVisibility(0), m_AxisLineOn(0)
{
  //----------------------------------------------------------------------------
  // Calculate scalar lut
  //----------------------------------------------------------------------------
  double r[2] ;
  volume->GetScalarRange(r) ;
  m_LutWindow = (r[1]-r[0]);
  m_LutLevel = (r[1]+r[0]) / 2.0 ;

  m_Lut = vtkWindowLevelLookupTable::New() ;
  m_Lut->SetTableRange(r[0], r[1]);
  m_Lut->SetHueRange(0.0, 0.0);
  m_Lut->SetSaturationRange(0.0, 0.0);
  m_Lut->SetValueRange(0.0, 1.0);
  m_Lut->SetNumberOfColors(512);
  m_Lut->SetWindow(m_LutWindow);
  m_Lut->SetLevel(m_LutLevel);
  m_Lut->Build();



  //----------------------------------------------------------------------------
  // Calculate size of landmarks and tube.
  // NB If this is preview, make sure the muscle has data in it.
  //----------------------------------------------------------------------------
  double bnds[6], siz[3], minSize ;
  muscle->GetBounds(bnds) ;
  siz[0] = bnds[1] - bnds[0] ;
  siz[1] = bnds[3] - bnds[2] ;
  siz[2] = bnds[5] - bnds[4] ;
  minSize = std::min(siz[0], siz[1]) ;
  minSize = std::min(siz[2], minSize) ;
  m_LandmarkSize = minSize / 20.0 ;
  m_TubeSize = minSize / 100.0 ;


  //----------------------------------------------------------------------------
  // volume bounding box
  //----------------------------------------------------------------------------
  m_BoxFilter = vtkOutlineCornerFilter::New() ;
  m_BoxFilter->SetInputData(volume) ;
  m_BoxFilter->SetCornerFactor(0.05) ;

  m_BoxMapper = vtkPolyDataMapper::New() ;
  m_BoxMapper->SetInputConnection(m_BoxFilter->GetOutputPort()) ;

  m_BoxActor = vtkActor::New() ;
  m_BoxActor->SetMapper(m_BoxMapper) ;
  m_Renderer->AddActor(m_BoxActor) ;


  //----------------------------------------------------------------------------
  // muscle polydata
  //----------------------------------------------------------------------------
  m_MuscleMapper = vtkPolyDataMapper::New() ;
  m_MuscleMapper->SetInputData(muscle) ;

  m_MuscleActor = vtkActor::New() ;
  m_MuscleActor->SetMapper(m_MuscleMapper) ;
  m_MuscleActor->GetProperty()->SetOpacity(0.3) ;
  m_MuscleActor->GetProperty()->SetColor(1,0,0) ; // red
  m_Renderer->AddActor(m_MuscleActor) ;


  //----------------------------------------------------------------------------
  // landmarks
  //----------------------------------------------------------------------------
  for (int i = 0 ;  i < m_MaxNumberOfLandmarks ;  i++){
    m_LmarkSource[i] = vtkSphereSource::New() ;
    m_LmarkSource[i]->SetThetaResolution(12) ;
    m_LmarkSource[i]->SetPhiResolution(12) ;
    m_LmarkSource[i]->SetRadius(m_LandmarkSize) ;

    m_LmarkMapper[i] = vtkPolyDataMapper::New() ;
    m_LmarkMapper[i]->SetInputConnection(m_LmarkSource[i]->GetOutputPort()) ;

    m_LmarkActor[i] = vtkActor::New() ;
    m_LmarkActor[i]->SetMapper(m_LmarkMapper[i]) ;
    m_LmarkActor[i]->GetProperty()->SetColor(0,1,0) ; // green
    m_LmarkActor[i]->SetVisibility(0) ;
    m_Renderer->AddActor(m_LmarkActor[i]) ;
  }


  //----------------------------------------------------------------------------
  // axis landmarks
  //----------------------------------------------------------------------------
  for (int i = 0 ;  i < m_MaxNumberOfAxisLandmarks ;  i++){
    m_AxisMarkSource[i] = vtkSphereSource::New() ;
    m_AxisMarkSource[i]->SetThetaResolution(12) ;
    m_AxisMarkSource[i]->SetPhiResolution(12) ;
    m_AxisMarkSource[i]->SetRadius(m_LandmarkSize) ;

    m_AxisMarkMapper[i] = vtkPolyDataMapper::New() ;
    m_AxisMarkMapper[i]->SetInputConnection(m_AxisMarkSource[i]->GetOutputPort()) ;

    m_AxisMarkActor[i] = vtkActor::New() ;
    m_AxisMarkActor[i]->SetMapper(m_AxisMarkMapper[i]) ;
    m_AxisMarkActor[i]->GetProperty()->SetColor(0,0,1) ; // blue
    m_AxisMarkActor[i]->SetVisibility(0) ;
    m_Renderer->AddActor(m_AxisMarkActor[i]) ;
  }


  //----------------------------------------------------------------------------
  // axis polyline
  //----------------------------------------------------------------------------
  vtkPoints *axisLinePts = vtkPoints::New() ;
  axisLinePts->Initialize() ;

  vtkCellArray *axisLineCells = vtkCellArray::New() ;
  axisLineCells->Initialize() ;

  m_AxisLinePolydata = vtkPolyData::New() ;
  m_AxisLinePolydata->SetPoints(axisLinePts) ;
  m_AxisLinePolydata->SetLines(axisLineCells) ;
  axisLinePts->Delete() ;
  axisLineCells->Delete() ;

  m_AxisLineTubeFilter = vtkTubeFilter::New() ;
  m_AxisLineTubeFilter->SetInputData(m_AxisLinePolydata) ;
  m_AxisLineTubeFilter->SetRadius(m_TubeSize) ;


  m_AxisLineMapper = vtkPolyDataMapper::New() ;
  m_AxisLineMapper->SetInputConnection(m_AxisLineTubeFilter->GetOutputPort()) ;

  m_AxisLineActor = vtkActor::New() ;
  m_AxisLineActor->SetMapper(m_AxisLineMapper) ;
  m_AxisLineActor->GetProperty()->SetColor(1,1,0) ; // yellow
  m_AxisLineActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_AxisLineActor) ;




  //----------------------------------------------------------------------------
  // Slice pipeline.
  //
  //            plane source
  //                |
  //                |           scale transform (sets size)
  //                |          /
  //        TransformScaleFilter 
  //                |
  //                |           slice transform[i] (sets position and rotation)
  //                |          /
  //        SliceTransformFilter[i]
  //                |
  //                |       volume
  //                |      /
  //            ProbeFilter[i]
  //                |
  //                |
  //             Mapper[i]
  //                |
  //              Actor[i]
  //                
  //----------------------------------------------------------------------------

  m_SliceSource = vtkPlaneSource::New() ;
  m_SliceSource->SetXResolution(64) ;
  m_SliceSource->SetYResolution(64) ;

  m_SliceTransformScale = vtkTransform::New() ; // transform which sets size
  m_SliceTransformScale->Identity() ;

  m_SliceTransformScaleFilter = vtkTransformPolyDataFilter::New() ;
  m_SliceTransformScaleFilter->SetInputConnection(m_SliceSource->GetOutputPort()) ;
  m_SliceTransformScaleFilter->SetTransform(m_SliceTransformScale) ;

  m_SliceTransform = new (vtkTransform*[m_NumberOfSlices]) ;
  m_SliceTransformFilter = new (vtkTransformPolyDataFilter*[m_NumberOfSlices]) ;
  m_SliceProbeFilter = new (vtkProbeFilter*[m_NumberOfSlices]) ;
  m_SliceMapper = new (vtkPolyDataMapper*[m_NumberOfSlices]) ;
  m_SliceActor = new (vtkActor*[m_NumberOfSlices]) ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceTransform[i] = vtkTransform::New() ; // transform which sets position and rotation
    m_SliceTransform[i]->Identity() ;

    m_SliceTransformFilter[i] = vtkTransformPolyDataFilter::New() ;
    m_SliceTransformFilter[i]->SetInputConnection(m_SliceTransformScaleFilter->GetOutputPort()) ;
    m_SliceTransformFilter[i]->SetTransform(m_SliceTransform[i]) ;

    m_SliceProbeFilter[i] = vtkProbeFilter::New() ;
		m_SliceProbeFilter[i]->SetSourceData(volume) ;
    m_SliceProbeFilter[i]->SetInputConnection(m_SliceTransformFilter[i]->GetOutputPort()) ;

    m_SliceMapper[i] = vtkPolyDataMapper::New() ;
    m_SliceMapper[i]->SetInputConnection(m_SliceProbeFilter[i]->GetOutputPort()) ;
    m_SliceMapper[i]->SetLookupTable(m_Lut);
    m_SliceMapper[i]->UseLookupTableScalarRangeOn();
    m_SliceMapper[i]->SetColorModeToMapScalars();

    m_SliceActor[i] = vtkActor::New() ;
    m_SliceActor[i]->SetMapper(m_SliceMapper[i]) ;
    m_Renderer->AddActor(m_SliceActor[i]) ;
  }


  //----------------------------------------------------------------------------
  // Contour pipeline
  //
  //              muscle
  //                |          contourCutterTransform[i] (inverse of slice transform to get same plane as slice)
  //                |           /
  //                |        contourPlane[i]
  //                |         /
  //          ContourCutter[i]
  //                |
  //                |       volume
  //                |      /
  //           TubeFilter[i]
  //                |
  //                |
  //             Mapper[i]
  //                |
  //             Actor[i]
  //         
  //----------------------------------------------------------------------------

  m_ContourCutterTransform = new (vtkTransform*[m_NumberOfSlices]) ;
  m_ContourPlane = new (vtkPlane*[m_NumberOfSlices]) ;
  m_ContourCutter = new (vtkCutter*[m_NumberOfSlices]) ;
  m_ContourTubeFilter = new (vtkTubeFilter*[m_NumberOfSlices]) ;
  m_ContourMapper = new (vtkPolyDataMapper*[m_NumberOfSlices]) ;
  m_ContourActor = new (vtkActor*[m_NumberOfSlices]) ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_ContourCutterTransform[i] = vtkTransform::New() ;
    m_ContourCutterTransform[i]->Identity() ;

    m_ContourPlane[i] = vtkPlane::New() ;
    m_ContourPlane[i]->SetOrigin(0,0,0) ;
    m_ContourPlane[i]->SetNormal(0,0,1) ;
    m_ContourPlane[i]->SetTransform(m_ContourCutterTransform[i]) ;

    m_ContourCutter[i] = vtkCutter::New() ;
    m_ContourCutter[i]->SetInputData(muscle) ;
    m_ContourCutter[i]->SetCutFunction(m_ContourPlane[i]) ;
    m_ContourCutter[i]->SetNumberOfContours(1) ;
    m_ContourCutter[i]->SetValue(0, 0.0) ;

    m_ContourTubeFilter[i] = vtkTubeFilter::New() ;
    m_ContourTubeFilter[i]->SetInputConnection(m_ContourCutter[i]->GetOutputPort()) ;
    m_ContourTubeFilter[i]->SetRadius(m_TubeSize) ;

    m_ContourMapper[i] = vtkPolyDataMapper::New() ;
    m_ContourMapper[i]->SetInputConnection(m_ContourTubeFilter[i]->GetOutputPort()) ;

    m_ContourActor[i] = vtkActor::New() ;
    m_ContourActor[i]->SetMapper(m_ContourMapper[i]) ;
    m_ContourActor[i]->GetProperty()->SetColor(1,1,0) ; // yellow
    m_Renderer->AddActor(m_ContourActor[i]) ;
  }
}




//------------------------------------------------------------------------------
// Destructor
albaOpMML3ModelView3DPipe::~albaOpMML3ModelView3DPipe()
//------------------------------------------------------------------------------
{
  // lut
  m_Lut->Delete() ;

  // volume bounding box
  m_BoxFilter->Delete() ;
  m_BoxMapper->Delete() ;
  m_BoxActor->Delete() ;

  // muscle polydata
  m_MuscleMapper->Delete() ;
  m_MuscleActor->Delete() ;

  // landmarks
  for (int i = 0 ;  i < m_MaxNumberOfLandmarks ;  i++){
    m_LmarkSource[i]->Delete() ;
    m_LmarkMapper[i]->Delete() ; 
    m_LmarkActor[i]->Delete() ; 
  }

  // axis landmarks
  for (int i = 0 ;  i < m_MaxNumberOfAxisLandmarks ;  i++){
    m_AxisMarkSource[i]->Delete() ;
    m_AxisMarkMapper[i]->Delete() ; 
    m_AxisMarkActor[i]->Delete() ; 
  }

  // axis polyline
  m_AxisLinePolydata->Delete() ;
  m_AxisLineTubeFilter->Delete() ;
  m_AxisLineMapper->Delete() ;
  m_AxisLineActor->Delete() ;


  // slice
  m_SliceSource->Delete() ;
  m_SliceTransformScale->Delete() ;
  m_SliceTransformScaleFilter->Delete() ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceTransform[i]->Delete() ;
    m_SliceTransformFilter[i]->Delete() ;
    m_SliceProbeFilter[i]->Delete() ;
    m_SliceMapper[i]->Delete() ;
    m_SliceActor[i]->Delete() ;
  }
  delete [] m_SliceTransform ;
  delete [] m_SliceTransformFilter ;
  delete [] m_SliceProbeFilter ;
  delete [] m_SliceMapper ;
  delete [] m_SliceActor ;


  // contour
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_ContourCutterTransform[i]->Delete() ;
    m_ContourPlane[i]->Delete() ;
    m_ContourCutter[i]->Delete() ;
    m_ContourTubeFilter[i]->Delete() ;
    m_ContourMapper[i]->Delete() ;
    m_ContourActor[i]->Delete() ;
  }
  delete [] m_ContourCutterTransform ;
  delete [] m_ContourPlane ;
  delete [] m_ContourCutter ;
  delete [] m_ContourTubeFilter ;
  delete [] m_ContourMapper ;
  delete [] m_ContourActor ;
}




//------------------------------------------------------------------------------
// Set visibility
void albaOpMML3ModelView3DPipe::SetVisibility(int visibility)
//------------------------------------------------------------------------------
{
  // set overall visibility state
  m_CurrentVisibility = visibility ;


  m_BoxActor->SetVisibility(visibility) ;
  m_MuscleActor->SetVisibility(visibility) ;

  for (int i = 0 ;  i < m_NumberOfLandmarks ;  i++){
    m_LmarkActor[i]->SetVisibility(visibility) ;
  }

  for (int i = 0 ;  i < m_NumberOfAxisLandmarks ;  i++){
    m_AxisMarkActor[i]->SetVisibility(visibility) ;
  }

  if (m_AxisLineOn)
    m_AxisLineActor->SetVisibility(visibility) ;
  
  // turn off all slices and contours except the current id
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceActor[i]->SetVisibility(0) ;
    m_ContourActor[i]->SetVisibility(0) ;
  }
  m_SliceActor[m_CurrentSliceId]->SetVisibility(visibility) ;
  m_ContourActor[m_CurrentSliceId]->SetVisibility(visibility) ;
}




//------------------------------------------------------------------------------
// Update current visible parts of the pipeline.
void albaOpMML3ModelView3DPipe::Update()
//------------------------------------------------------------------------------
{
  m_Renderer->GetRenderWindow()->Render() ;
}



//------------------------------------------------------------------------------
// Force update of all slices and contours.
// Call this once to pre-process the slices, to avoid sticky processing when moving slider.
void albaOpMML3ModelView3DPipe::UpdateAllSlices()
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceActor[i]->SetVisibility(1) ;    // temporarily switch on actors so that they update
    m_ContourActor[i]->SetVisibility(1) ;
  }
  m_Renderer->GetRenderWindow()->Render() ;

  // set visibility back to normal
  SetVisibility(m_CurrentVisibility) ;
  m_Renderer->GetRenderWindow()->Render() ;

}




//------------------------------------------------------------------------------
// Set current slice id
void albaOpMML3ModelView3DPipe::SetCurrentSliceId(int i)
//------------------------------------------------------------------------------
{
  m_CurrentSliceId = i ;
  SetVisibility(m_CurrentVisibility) ;
  Update() ;
}



//------------------------------------------------------------------------------
// set transformation matrix which positions and rotates slice and contour i
void albaOpMML3ModelView3DPipe::SetSliceTransform(int i, vtkMatrix4x4 *mat)
//------------------------------------------------------------------------------
{
  // set the transform of the slice
  m_SliceTransform[i]->SetMatrix(mat) ;

  // NB plane->SetTransform() is actually the inverse of TransformPolydataFilter->SetTransform()
  // It does not transform the plane - it transforms points to the plane.
  // Therefore we must set the contour transform to the inverse of the slice transform.
  vtkMatrix4x4 *invMat = vtkMatrix4x4::New() ;
  mat->Invert(mat, invMat) ;
  m_ContourCutterTransform[i]->SetMatrix(invMat) ;
  invMat->Delete() ;
}



//------------------------------------------------------------------------------
// add a landmark
void albaOpMML3ModelView3DPipe::AddLandmark(double *pos)
//------------------------------------------------------------------------------
{
  assert(m_NumberOfLandmarks < m_MaxNumberOfLandmarks) ;

  // set position and make visibile
  int i = m_NumberOfLandmarks ;
  m_LmarkActor[i]->SetPosition(pos) ;
  m_LmarkActor[i]->SetVisibility(m_CurrentVisibility) ;

  m_NumberOfLandmarks++ ;
}




//------------------------------------------------------------------------------
// add an axis landmark
void albaOpMML3ModelView3DPipe::AddAxisLandmark(double *pos)
//------------------------------------------------------------------------------
{
  assert(m_NumberOfAxisLandmarks < m_MaxNumberOfAxisLandmarks) ;

  // set position and make visibile
  int i = m_NumberOfAxisLandmarks ;
  m_AxisMarkActor[i]->SetPosition(pos) ;
  m_AxisMarkActor[i]->SetVisibility(m_CurrentVisibility) ;

  // add point and cell to polyline and make visible
  m_AxisLinePolydata->GetPoints()->InsertNextPoint(pos) ;
  if (i >= 1){
		vtkIdType endPointIds[2] ;
    endPointIds[0] = i-1 ;
    endPointIds[1] = i ;
    m_AxisLinePolydata->GetLines()->InsertNextCell(2, endPointIds) ;
    m_AxisLineActor->SetVisibility(m_CurrentVisibility) ;
  }

  m_NumberOfAxisLandmarks++ ;

  // make axis line visible when there are enough landmarks
  if (m_NumberOfAxisLandmarks >= 2){
    m_AxisLineOn = 1 ;
    m_AxisLineActor->SetVisibility(1) ;
  }
}




//------------------------------------------------------------------------------
// Set resolution of slices
void albaOpMML3ModelView3DPipe::SetSliceResolution(int resX, int resY)
//------------------------------------------------------------------------------
{
  m_SliceSource->SetXResolution(resX) ;
  m_SliceSource->SetYResolution(resY) ;
}


//------------------------------------------------------------------------------
// Set size of slices
void albaOpMML3ModelView3DPipe::SetSliceSize(double sizeX, double sizeY)
//------------------------------------------------------------------------------
{
  m_SliceTransformScale->Scale(sizeX, sizeY, 1.0) ;
}





//------------------------------------------------------------------------------
// set landmark size
void albaOpMML3ModelView3DPipe::SetLandmarkRadius(double r)
//------------------------------------------------------------------------------
{
  m_LandmarkSize = r ;

  for (int i = 0 ;  i < m_MaxNumberOfLandmarks ;  i++){
    m_LmarkSource[i]->SetRadius(r) ;
  }

  for (int i = 0 ;  i < m_MaxNumberOfAxisLandmarks ;  i++){
    m_AxisMarkSource[i]->SetRadius(r) ;
  }
}


//------------------------------------------------------------------------------
// set radius of axis line
void albaOpMML3ModelView3DPipe::SetTubeRadius(double r)
//------------------------------------------------------------------------------
{
  m_TubeSize = r ;

  m_AxisLineTubeFilter->SetRadius(r) ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++)
    m_ContourTubeFilter[i]->SetRadius(r) ;
}






//------------------------------------------------------------------------------
// Get bounds of current contour
void albaOpMML3ModelView3DPipe::GetContourBounds(double *bounds)
//------------------------------------------------------------------------------
{
  m_ContourCutter[m_CurrentSliceId]->GetOutput()->GetBounds(bounds) ;
}


//------------------------------------------------------------------------------
// Get center of current contour
void albaOpMML3ModelView3DPipe::GetContourCenter(double *center)
//------------------------------------------------------------------------------
{
  m_ContourCutter[m_CurrentSliceId]->GetOutput()->GetCenter(center) ;
}




//------------------------------------------------------------------------------
// set lut range
void albaOpMML3ModelView3DPipe::SetLutRange(double scalarMin, double scalarMax)
//------------------------------------------------------------------------------
{
  m_LutWindow = (scalarMax -  scalarMin);
  m_LutLevel = (scalarMax + scalarMin) / 2.0 ;

  m_Lut->SetWindow(m_LutWindow);
  m_Lut->SetLevel(m_LutLevel);
}





//------------------------------------------------------------------------------
// Print self
void albaOpMML3ModelView3DPipe::PrintSelf(ostream& os, int indent)
//------------------------------------------------------------------------------
{
  Update() ;

  os << "MML Visual pipe 3d" << std::endl ;
  os << std::endl ;

  os << "visibility = " << m_CurrentVisibility << std::endl ;

  os << "no. of landmarks = " << m_NumberOfLandmarks << std::endl ;
  os << "no. of axis landmarks = " << m_NumberOfAxisLandmarks << std::endl ;
  os << std::endl ;

  os << "current slice = " << m_CurrentSliceId << std::endl ;
  os << std::endl ;

  double *b ;
  double *c ;

  b = m_BoxActor->GetBounds() ;
  c = m_BoxActor->GetCenter() ;
  os << "box " << "visibility = " << m_BoxActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  b = m_MuscleActor->GetBounds() ;
  c = m_MuscleActor->GetCenter() ;
  os << "muscle " << "visibility = " << m_MuscleActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  GetContourBounds(b) ;
  GetContourCenter(c) ;
  os << "contour " << "visibility = " << m_ContourActor[m_CurrentSliceId]->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  for (int i = 0 ;  i < m_NumberOfLandmarks ;  i++){
    b = m_LmarkActor[i]->GetBounds() ;
    c = m_LmarkActor[i]->GetCenter() ;
    os << "landmark " << i << " " << "visibility = " << m_LmarkActor[i]->GetVisibility() << std::endl ;
    os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
    os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
    os << std::endl ;
  }

  for (int i = 0 ;  i < m_NumberOfAxisLandmarks ;  i++){
    b = m_AxisMarkActor[i]->GetBounds() ;
    c = m_AxisMarkActor[i]->GetCenter() ;
    os << "axis landmark " << i << " " << "visibility = " << m_AxisMarkActor[i]->GetVisibility() << std::endl ;
    os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
    os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
    os << std::endl ;
  }

  b = m_AxisLineActor->GetBounds() ;
  c = m_AxisLineActor->GetCenter() ;
  os << "axis line " << "visibility = " << m_AxisLineActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;


  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    b = m_SliceActor[i]->GetBounds() ;
    c = m_SliceActor[i]->GetCenter() ;
    os << "slice " << i << " " << "visibility = " << m_SliceActor[i]->GetVisibility() << std::endl ;
    os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
    os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
    os << std::endl ;
  }

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    b = m_ContourActor[i]->GetBounds() ;
    c = m_ContourActor[i]->GetCenter() ;
    os << "contour " << i << " " << "visibility = " << m_ContourActor[i]->GetVisibility() << std::endl ;
    os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
    os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
    os << std::endl ;
  }
  os << std::endl ;
  os << std::endl ;

}