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

#ifndef __albaOpMML3ModelView3DPipe_H__
#define __albaOpMML3ModelView3DPipe_H__

#include "albaDefines.h"

#include "vtkDataSet.h"
#include "vtkMatrix4x4.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkProbeFilter.h"
#include "vtkCutter.h"
#include "vtkPlane.h"
#include "vtkTubeFilter.h"
#include "vtkWindowLevelLookupTable.h"

#include <ostream>




//------------------------------------------------------------------------------
/// albaOpMML3ModelView3DPipe. \n
/// Helper class for albaOpMML3. \n
/// Visual pipe for 3D view of muscle and slice. \n
/// The coordinate system of this visual pipe is patient space, ie the input muscle.
//------------------------------------------------------------------------------
class albaOpMML3ModelView3DPipe
{
public:
  /// Constructor
  albaOpMML3ModelView3DPipe(vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume, int numberOfSlices) ;

  /// Destructor
  ~albaOpMML3ModelView3DPipe() ;

  /// set visibility of whole pipe
  void SetVisibility(int visibility) ;

  /// Update current visible parts of the pipeline.
  void Update() ;

  /// Force update of all slices and contours. \n
  /// Call this once to pre-process the slices, to avoid sticky processing when moving slider.
  void UpdateAllSlices() ;

  /// Set current slice id
  void SetCurrentSliceId(int i) ;

  /// set transformation matrix which positions and rotates slice i
  void SetSliceTransform(int i, vtkMatrix4x4 *mat) ;

  /// add a landmark
  void AddLandmark(double *pos) ;

  /// add an axis landmark
  void AddAxisLandmark(double *pos) ;

  /// Set resolution of slices
  void SetSliceResolution(int resX, int resY);

  /// Set size of slices
  void SetSliceSize(double sizeX, double sizeY);


  /// set opacity of muscle
  void SetOpacity(double opacity) {m_MuscleActor->GetProperty()->SetOpacity(opacity) ;}

  /// set color of muscle
  void SetColor(double r, double g, double b) {m_MuscleActor->GetProperty()->SetColor(r,g,b) ;}

  /// set landmark size
  void SetLandmarkRadius(double r) ;

  /// set tube radius for contour and axis line
  void SetTubeRadius(double r) ;

  /// Get bounds of current contour
  void GetContourBounds(double *bnds) ;

  /// Get centre of current contour
  void GetContourCenter(double *center) ;

  /// set range of lut
  void SetLutRange(double scalarMin, double scalarMax) ;

  /// Print self
  void PrintSelf(ostream& os, int indent) ;


  //----------------------------------------------------------------------------
  // Methods which take advantage of the pipeline for post-viewing registration calculations
  // Don't like these methods - find some way to avoid them being in this class
  //----------------------------------------------------------------------------

  /// Set origin of contour plane.\n
  /// Use only for registration calculations, but not when visualizing !
  void SetContourPlaneOrigin(double x, double y, double z) {m_ContourPlane[0]->SetOrigin(x,y,z) ;}

  /// Set normal of contour plane.\n
  /// Use only for registration calculations, but not when visualizing !
  void SetContourPlaneNormal(double n0, double n1, double n2) {m_ContourPlane[0]->SetNormal(n0,n1,n2) ;}

  /// Set contour cutter transform to perform simple translation to (0,0,z). \n
  /// Use only for registration calculations, but not when visualizing !
  void SetContourCutterTransform(double z) {m_SliceTransform[0]->Identity(); m_SliceTransform[0]->Translate(0,0,z) ;}


private:
  // number of slices
  int m_NumberOfSlices ;
  int m_CurrentSliceId ;

  // renderer
  vtkRenderer *m_Renderer ;

  // visibility of pipeline
  int m_CurrentVisibility ;
  int m_AxisLineOn ;


  // look up table
  vtkWindowLevelLookupTable *m_Lut ;
  double m_LutWindow;
  double m_LutLevel;


  // bounding box of volume
  vtkOutlineCornerFilter *m_BoxFilter ;
  vtkPolyDataMapper *m_BoxMapper ;
  vtkActor *m_BoxActor ;

  // muscle polydata
  vtkPolyDataMapper *m_MuscleMapper ;
  vtkActor *m_MuscleActor ;

  // landmarks
  double m_LandmarkSize ;
  static const int m_MaxNumberOfLandmarks = 100 ;
  int m_NumberOfLandmarks ;
  vtkSphereSource *m_LmarkSource[m_MaxNumberOfLandmarks] ;
  vtkPolyDataMapper *m_LmarkMapper[m_MaxNumberOfLandmarks] ;
  vtkActor *m_LmarkActor[m_MaxNumberOfLandmarks] ;

  // axis landmarks
  static const int m_MaxNumberOfAxisLandmarks = 3 ;
  int m_NumberOfAxisLandmarks ;
  vtkSphereSource *m_AxisMarkSource[m_MaxNumberOfAxisLandmarks] ;
  vtkPolyDataMapper *m_AxisMarkMapper[m_MaxNumberOfAxisLandmarks] ;
  vtkActor *m_AxisMarkActor[m_MaxNumberOfAxisLandmarks] ;

  // axis polyline
  double m_TubeSize ;
  vtkPolyData *m_AxisLinePolydata ;
  vtkTubeFilter *m_AxisLineTubeFilter ;
  vtkPolyDataMapper *m_AxisLineMapper ;
  vtkActor *m_AxisLineActor ;

  // slice
  vtkPlaneSource *m_SliceSource ;
  vtkTransform *m_SliceTransformScale ; // sets size of slice
  vtkTransformPolyDataFilter *m_SliceTransformScaleFilter ;
  vtkTransform **m_SliceTransform ;  // position and rotation of slice, set by user
  vtkTransformPolyDataFilter **m_SliceTransformFilter ;
  vtkProbeFilter **m_SliceProbeFilter ;
  vtkPolyDataMapper **m_SliceMapper ;
  vtkActor **m_SliceActor ;

  // contour
  vtkPlane **m_ContourPlane ;
  vtkTransform **m_ContourCutterTransform ;  // position and rotation of cutting plane (use inverse !!)
  vtkCutter **m_ContourCutter ;
  vtkTubeFilter **m_ContourTubeFilter ;
  vtkPolyDataMapper **m_ContourMapper ;
  vtkActor **m_ContourActor ;
} ;



#endif