/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3ModelView2DPipe.h,v $
Language:  C++
Date:      $Date: 2009-09-18 08:10:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMML3ModelView2DPipe_H__
#define __medOpMML3ModelView2DPipe_H__

#include "mafDefines.h"

#include "vtkDataSet.h"
#include "vtkMatrix4x4.h"
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
#include "vtkLineSource.h"
#include "vtkTextMapper.h"
#include "vtkScaledTextActor.h"
#include "vtkClipPolyData.h"
#include "vtkWindowLevelLookupTable.h"



//------------------------------------------------------------------------------
/// medOpMML3ModelView2DPipe. \n
/// Helper class for medOpMML3. \n
/// Visual pipe for 2D view of muscle and slice. \n
/// The coordinate system in this view is that of the current slice. \n
//------------------------------------------------------------------------------
class medOpMML3ModelView2DPipe
{
public:
  /// Constructor
  medOpMML3ModelView2DPipe(vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume, int numberOfSlices) ;

  /// Destructor
  ~medOpMML3ModelView2DPipe() ;

  /// set visibility of whole pipe
  void SetVisibility(int visibility) ;

  /// Update current visible parts of the pipeline.
  void Update() ;

  /// Update all slices. \n
  /// Call this once to pre-process the slices, to avoid sticky processing later when moving slider.
  void UpdateAllSlices() ;

  /// Set current slice id
  void SetCurrentSliceId(int i) ;

  /// set transformation matrix which positions and rotates slice i
  void SetSliceTransform(int i, vtkMatrix4x4 *mat) ;

  /// Set resolution of slice
  void SetSliceResolution(int resX, int resY);

  /// Set size of slice. \n
  /// This also sets the length of the axes and the tube size.
  void SetSliceSize(double sizeX, double sizeY);

  /// set radius of tube around contour and axis lines
  void SetTubeRadius(double r) ;

  /// set range of lut
  void SetLutRange(double scalarMin, double scalarMax) ;

  /// Transform a point in coords of current slice to patient coords. \n
  /// Useful for positioning the camera
  void TransformPointToPatient(double *p) ;



  //----------------------------------------------------------------------------
  // text display methods
  //----------------------------------------------------------------------------
  void SetTextX(const char *text) {m_TextMapperX->SetInput(text) ;} ///< set text info
  void SetTextY(const char *text) {m_TextMapperY->SetInput(text) ;} ///< set text info
  void SetTextXPosition(double x, double y) {m_ScaledTextActorX->GetPositionCoordinate()->SetValue(x,y) ;} ///< set text position
  void SetTextYPosition(double x, double y) {m_ScaledTextActorY->GetPositionCoordinate()->SetValue(x,y) ;} ///< set text position
  void SetTextXVisibility(int visibility) ;   ///< set visibility of text info
  void SetTextYVisibility(int visibility) ;   ///< set visibility of text info



  //----------------------------------------------------------------------------
  // methods for contour and global axes
  //----------------------------------------------------------------------------
  void SetContourAxesVisibility(int visibility) ; ///< set visibility of contour axes
  void SetGlobalAxesVisibility(int visibility) ;  ///< set visibility of global axes

  void SetContourAxesTransform(vtkTransform *transform) ; ///< set transform of contour axes
  void SetGlobalAxesTransform(vtkTransform *transform) ; ///< set transform of global axes

  void GetOriginalSegmentBoundsNE(double *bounds) ; ///< Get bounds of contour quadrant after clipping, before transform
  void GetOriginalSegmentBoundsSE(double *bounds) ; ///< Get bounds of contour quadrant after clipping, before transform
  void GetOriginalSegmentBoundsNW(double *bounds) ; ///< Get bounds of contour quadrant after clipping, before transform
  void GetOriginalSegmentBoundsSW(double *bounds) ; ///< Get bounds of contour quadrant after clipping, before transform
  void GetOriginalSegmentCenterNE(double *center) ; ///< Get center of contour quadrant after clipping, before transform
  void GetOriginalSegmentCenterSE(double *center) ; ///< Get center of contour quadrant after clipping, before transform
  void GetOriginalSegmentCenterNW(double *center) ; ///< Get center of contour quadrant after clipping, before transform
  void GetOriginalSegmentCenterSW(double *center) ; ///< Get center of contour quadrant after clipping, before transform
  void GetCurrentContourBounds(double *bounds) ;   ///< Get the bounds of the contour
  void GetCurrentContourCenter(double *center) ;   ///< Get the center of the contour

  /// Calculate robust center of the current (untransformed) contour. \n
  /// Gives much better result than GetCurrentContourCenter().
  void CalculateRobustCenterOfContour(double *rcenter) const ; 



  //----------------------------------------------------------------------------
  // methods which apply the effect of the widget to the contour quadrants
  //----------------------------------------------------------------------------
  void SetCuttingPlanesTransform(vtkTransform *transform) ; ///< Set the transform of the quadrant cutting planes
  void SetNEContourTransform(vtkTransform *transform) ;  ///< Set the transform of the contour quadrant
  void SetSEContourTransform(vtkTransform *transform) ;  ///< Set the transform of the contour quadrant
  void SetNWContourTransform(vtkTransform *transform) ;  ///< Set the transform of the contour quadrant
  void SetSWContourTransform(vtkTransform *transform) ;  ///< Set the transform of the contour quadrant

  /// Print self
  void PrintSelf(ostream& os, int indent) ;

private:
  void SetUpContourCoordinateAxes() ; ///< set parameters of contour axes
  void SetUpGlobalCoordinateAxes() ;  ///< set parameters of global axes


  // number of slices
  int m_NumberOfSlices ;
  int m_CurrentSliceId ;

  // renderer
  vtkRenderer *m_Renderer ;

  // visibility of pipeline
  int m_CurrentVisibility ; ///< visibility of visual pipe
  int m_MusclePolyDataOn ;  ///< visibility of muscle
  int m_GlobalAxesOn ; ///< visibility of global axes
  int m_ContourAxesOn ; ///< visibility of contour axes
  int m_TextXOn ; ///< visibility of x text
  int m_TextYOn ; ///< visibility of y text

  // size of tube
  double m_TubeSize ;

  // size and resolution
  double m_SizeX, m_SizeY ;
  int m_ResX, m_ResY ;

  // length of axes
  double m_AxisLengthX, m_AxisLengthY ;


  // look up table
  vtkWindowLevelLookupTable *m_Lut ;
  double m_LutWindow;
  double m_LutLevel;


  // muscle polydata
  vtkMatrix4x4 **m_Mat ;  // array of transform matrices for each slice
  vtkMatrix4x4 **m_Invmat ;
  vtkTransform *m_MuscleTransform ;
  vtkTransformPolyDataFilter *m_MuscleTransformFilter ;
  vtkPolyDataMapper *m_MuscleMapper ;
  vtkActor *m_MuscleActor ;


  // slice
  vtkPlaneSource *m_SliceSource ;
  vtkTransform *m_SliceTransformScale ; // sets size of slice
  vtkTransformPolyDataFilter *m_SliceTransformScaleFilter ;
  vtkTransform **m_SliceTransform ;  // position and rotation of slice, set by user
  vtkTransformPolyDataFilter **m_SliceTransformFilter ;
  vtkProbeFilter **m_SliceProbeFilter ;
  vtkTransform **m_SliceInvTransform ;  // inverse transform which returns to slice coords
  vtkTransformPolyDataFilter **m_SliceInvTransformFilter ;
  vtkPolyDataMapper **m_SliceMapper ;
  vtkActor **m_SliceActor ;




  // contour axes
  vtkTransform *m_ContourAxesTransform ;  // transform which controls contour axes

  vtkLineSource *m_ContourPosXAxisLineSource; // positive x
  vtkTransformPolyDataFilter *m_ContourPosXAxisTransformPDFilter ;
  vtkTubeFilter *m_ContourPosXAxisTubeFilter;
  vtkPolyDataMapper *m_ContourPosXAxisMapper;
  vtkActor *m_ContourPosXAxisActor;

  vtkLineSource *m_ContourPosYAxisLineSource; // positive y
  vtkTransformPolyDataFilter *m_ContourPosYAxisTransformPDFilter ;
  vtkTubeFilter *m_ContourPosYAxisTubeFilter;
  vtkPolyDataMapper *m_ContourPosYAxisMapper;
  vtkActor *m_ContourPosYAxisActor;

  vtkLineSource *m_ContourNegXAxisLineSource; // negative x
  vtkTransformPolyDataFilter *m_ContourNegXAxisTransformPDFilter ;
  vtkTubeFilter *m_ContourNegXAxisTubeFilter;
  vtkPolyDataMapper *m_ContourNegXAxisMapper;
  vtkActor *m_ContourNegXAxisActor;

  vtkLineSource *m_ContourNegYAxisLineSource; // negative y
  vtkTransformPolyDataFilter *m_ContourNegYAxisTransformPDFilter ;
  vtkTubeFilter *m_ContourNegYAxisTubeFilter;
  vtkPolyDataMapper *m_ContourNegYAxisMapper;
  vtkActor *m_ContourNegYAxisActor;



  // global axes
  vtkTransform *m_GlobalAxesTransform ;  // transform which controls global axes

  vtkLineSource *m_GlobalPosXAxisLineSource; // positive x
  vtkTransformPolyDataFilter *m_GlobalPosXAxisTransformPDFilter ;
  vtkTubeFilter *m_GlobalPosXAxisTubeFilter;
  vtkPolyDataMapper *m_GlobalPosXAxisMapper;
  vtkActor *m_GlobalPosXAxisActor;

  vtkLineSource *m_GlobalPosYAxisLineSource; // positive y
  vtkTransformPolyDataFilter *m_GlobalPosYAxisTransformPDFilter ;
  vtkTubeFilter *m_GlobalPosYAxisTubeFilter;
  vtkPolyDataMapper *m_GlobalPosYAxisMapper;
  vtkActor *m_GlobalPosYAxisActor;

  vtkLineSource *m_GlobalNegXAxisLineSource; // negative x
  vtkTransformPolyDataFilter *m_GlobalNegXAxisTransformPDFilter ;
  vtkTubeFilter *m_GlobalNegXAxisTubeFilter;
  vtkPolyDataMapper *m_GlobalNegXAxisMapper;
  vtkActor *m_GlobalNegXAxisActor;

  vtkLineSource *m_GlobalNegYAxisLineSource; // negative y
  vtkTransformPolyDataFilter *m_GlobalNegYAxisTransformPDFilter ;
  vtkTubeFilter *m_GlobalNegYAxisTubeFilter;
  vtkPolyDataMapper *m_GlobalNegYAxisMapper;
  vtkActor *m_GlobalNegYAxisActor;



  // contour cutter
  vtkPlane *m_ContourPlane;
  vtkCutter *m_ContourCutter;

  // cutting planes for contour
  // these cut the contour into 4 quadrants
  vtkTransform *m_CuttingPlanesTransform ;  // transforms clipping planes (use inverse !!)
  vtkPlane *m_CuttingPlaneN; // pointing N
  vtkPlane *m_CuttingPlaneS; // pointing S
  vtkPlane *m_CuttingPlaneE; // pointing E
  vtkPlane *m_CuttingPlaneW; // pointing W

  // north-east
  vtkTubeFilter *m_NEContourTubeFilter;
  vtkTransform *m_NEContourTransform ;
  vtkTransformPolyDataFilter *m_NEContourTransformPolyDataFilter;
  vtkClipPolyData *m_NEContourClipFilterN;
  vtkClipPolyData *m_NEContourClipFilterE;
  vtkPolyDataMapper *m_NEContourMapper;
  vtkActor *m_NEContourActor;

  // south-east
  vtkTubeFilter *m_SEContourTubeFilter;
  vtkTransform *m_SEContourTransform ;
  vtkTransformPolyDataFilter *m_SEContourTransformPolyDataFilter;
  vtkClipPolyData *m_SEContourClipFilterS;
  vtkClipPolyData *m_SEContourClipFilterE;
  vtkPolyDataMapper *m_SEContourMapper;
  vtkActor *m_SEContourActor;

  // north-west
  vtkTubeFilter *m_NWContourTubeFilter;
  vtkTransform *m_NWContourTransform ;
  vtkTransformPolyDataFilter *m_NWContourTransformPolyDataFilter;
  vtkClipPolyData *m_NWContourClipFilterN;
  vtkClipPolyData *m_NWContourClipFilterW;
  vtkPolyDataMapper *m_NWContourMapper;
  vtkActor *m_NWContourActor;

  // south-west
  vtkTubeFilter *m_SWContourTubeFilter;
  vtkTransform *m_SWContourTransform ;
  vtkTransformPolyDataFilter *m_SWContourTransformPolyDataFilter;
  vtkClipPolyData *m_SWContourClipFilterS;
  vtkClipPolyData *m_SWContourClipFilterW;
  vtkPolyDataMapper *m_SWContourMapper;
  vtkActor *m_SWContourActor;


  // information display
  vtkTextMapper *m_TextMapperX;
  vtkScaledTextActor *m_ScaledTextActorX;
  vtkTextMapper *m_TextMapperY;
  vtkScaledTextActor *m_ScaledTextActorY;



} ;



#endif