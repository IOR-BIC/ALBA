/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ModelView
 Authors: Mel Krokos, Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __albaOpMML3ModelView_H__
#define __albaOpMML3ModelView_H__

#include "albaDefines.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkDataSet.h"
#include "vtkKochanekSpline.h"
#include "vtkActor.h"
#include "vtkMatrix4x4.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleImage.h"

#include "vtkALBAMatrixVectorMath.h"
#include "albaOpMML3ModelView2DPipe.h"
#include "albaOpMML3ModelView3DPipe.h"

#include <ostream>

#define MAX_CHARS 512



//------------------------------------------------------------------------------
/// albaOpMML3ModelView. \n
/// Helper class for albaOpMML3. \n
/// This class is the visual pipe for the op.
//
//
// Flow of information between widget, model view and visual pipe.
//                                             
//                      MODEL VIEW               
//           slice id                                                                    
//               |     Original contour <---------------------------------------- initialization
//               |        centres                                                     only      
//   WIDGET      |           |                                                          ^
//               V           V                                                          |
// Interaction----------> Splines      slice id                                         |
//     |                     |           / \                                            |
//     |                     |<---------    ------------                                |
//     |                     |                           \   VISUAL PIPE 2D             |
//     V                     V                            \                             |
// rot. handle     <---- Spline(zeta) -------> Transforms ---> contour --------> contour segments --> 
// scaling handles <----                                  ---> cut planes ----->                     |
//                    ^                                   ---> contour axes                          |
//                    |                                                                              V
//                     <-----------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class albaOpMML3ModelView
{
public:
  //----------------------------------------------------------------------------
  // constructor and destructor
  //----------------------------------------------------------------------------
  albaOpMML3ModelView(vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscleIn, vtkPolyData *muscleOut,
    vtkDataSet* volume, int numberOfScans) ; ///< constructor
  virtual ~albaOpMML3ModelView(); ///< destructor

  /// initialize after input parameters have been set
  void Initialize() ;


  //----------------------------------------------------------------------------
  // Display
  //----------------------------------------------------------------------------

  void SetDisplay2D() ; ///< set to 2d visual pipe
  void SetDisplay3D() ; ///< set to 3d visual pipe
  void SetDisplayToPreview() ; ///< set to preview visual pipe
  albaOpMML3ModelView2DPipe* GetVisualPipe2D() {return m_VisualPipe2D ;} ///< return the 2d visual pipe
  albaOpMML3ModelView3DPipe* GetVisualPipe3D() {return m_VisualPipe3D ;} ///< return the 3d visual pipe
  albaOpMML3ModelView3DPipe* GetVisualPipePreview() {return m_VisualPipePreview ;} ///< return the preview visual pipe
  void ResetCameraPosition() ; ///< reset the camera position
  vtkRenderWindowInteractor* GetRenderWindowInteractor() const {return m_Renderer->GetRenderWindow()->GetInteractor() ;}
  vtkRenderer* GetRenderer() const {return m_Renderer;}
  void Render();



  //----------------------------------------------------------------------------
  // Methods which return info required by widget
  //----------------------------------------------------------------------------

  /// Get the bounds of the untransformed contour on the current slice. \n
  /// Returns the value saved by CalculateOriginalContourCenters()
  void GetOriginalContourBounds(double bounds[6]) ; 

  /// Get the center of the untransformed contour on the current slice. \n
  /// Returns the value saved by CalculateOriginalContourCenters()
  void GetOriginalContourCenter(double center[3]) ; 


  //----------------------------------------------------------------------------
  // Landmark methods
  // All landmarks are in patient coords
  //----------------------------------------------------------------------------

  // patient landmarks functions
  void SetLandmark1OfPatient(double *xyz) ;
  void GetLandmark1OfPatient(double *xyz) const ;
  void SetLandmark2OfPatient(double *xyz) ;
  void GetLandmark2OfPatient(double *xyz) const ;
  void SetLandmark3OfPatient(double *xyz) ;
  void GetLandmark3OfPatient(double *xyz) const ;
  void SetLandmark4OfPatient(double *xyz) ;
  void GetLandmark4OfPatient(double *xyz) const ;

  // atlas landmarks functions
  void SetLandmark1OfAtlas(double *xyz) ;
  void GetLandmark1OfAtlas(double *xyz) const ;
  void SetLandmark2OfAtlas(double *xyz) ;
  void GetLandmark2OfAtlas(double *xyz) const ;
  void SetLandmark3OfAtlas(double *xyz) ;
  void GetLandmark3OfAtlas(double *xyz) const ;
  void SetLandmark4OfAtlas(double *xyz) ;
  void GetLandmark4OfAtlas(double *xyz) const ;

  // axis landmarks functions
  void SetLandmark1OfAxis(double *xyz) ;
  void GetLandmark1OfAxis(double *xyz) const ;
  void SetLandmark2OfAxis(double *xyz) ;
  void GetLandmark2OfAxis(double *xyz) const ;
  void SetLandmark3OfAxis(double *xyz) ;
  void GetLandmark3OfAxis(double *xyz) const ;




  //----------------------------------------------------------------------------
  // synthetic scans methods
  //----------------------------------------------------------------------------

  /// Get number of scans. \n
  /// The corresponding set method is protected and can only be used in the constructor
  int GetTotalNumberOfScans() const ;

  /// Set positions (fractional and actual) of scans along axis
  void SetFractionalPosOfScans(double *alpha) ;

  /// Set grain of scans. \n
  /// The grain value 1-5 is used to set the resolution of the slice images.
  void SetGrainOfScans(int n) {m_ScansGrain = n ;}

  /// Set the current scan id. \n
  /// This also sets the corresponding pose matrices in the visual pipes
  void SetCurrentIdOfScans(int i) ;

  /// Get the current scan id
  int GetCurrentIdOfScans() const {return m_ScansCurrentId ;}

  double GetZetaOfSlice(int sliceId) const ;  ///< Calculate distance zeta along slice axis
  double GetZetaOfCurrentSlice() const ;      ///< Calculate distance zeta along slice axis

  void GetSizeOfScans(double *x, double *y) const ; ///< get the size of the polydata slice probe
  void GetResolutionOfScans(int *x, int *y) const ; ///< get the resolution of the slice (no. of quads)





  //----------------------------------------------------------------------------
  // Operations stack methods
  //----------------------------------------------------------------------------
  /// allocate operations stack
  void AllocateOperationsStack(int numberOfComponents, int numberOfTuples) ;

  /// delete operations stack
  void DeleteOperationsStack() {m_OperationsStack->Delete() ;}

  /// get parameter tuple from operations stack
  void GetOperationsStackTuple(int i, double *params) {m_OperationsStack->GetTuple(i, params) ;}

  /// set parameter tuple in operations stack
  void SetOperationsStackTuple(int i, double *params) {m_OperationsStack->SetTuple(i, params) ;}

  bool GetScalingOccured() const {return m_ScalingOccured ;} ///< has scaling occured
  void SetScalingOccured(bool scalingOccured) {m_ScalingOccured = scalingOccured ;} ///< set scaling occured flag

  int GetScalingOccuredOperationId() const {return m_ScalingOccuredOperationId ;} ///< get id
  void SetScalingOccuredOperationId(int id) {m_ScalingOccuredOperationId = id ;} ///< set id



  //----------------------------------------------------------------------------
  // splines functions
  //----------------------------------------------------------------------------
  vtkKochanekSpline* GetPHSpline() const {return m_PlaceHorizontalOffsetSpline ;}
  vtkKochanekSpline* GetPVSpline() const {return m_PlaceVerticalOffsetSpline ;}
  vtkKochanekSpline* GetTHSpline() const {return m_HorizontalTranslationSpline ;}
  vtkKochanekSpline* GetTVSpline() const {return m_VerticalTranslationSpline ;}
  vtkKochanekSpline* GetRASpline() const {return m_TwistSpline ;}
  vtkKochanekSpline* GetSWSpline() const {return m_WestScalingSpline ;}
  vtkKochanekSpline* GetSESpline() const {return m_EastScalingSpline ;}
  vtkKochanekSpline* GetSSSpline() const {return m_SouthScalingSpline ;}
  vtkKochanekSpline* GetSNSpline() const {return m_NorthScalingSpline ;}
  vtkKochanekSpline* GetCHSpline() const {return m_CenterHorizontalOffsetSpline ;}
  vtkKochanekSpline* GetCVSpline() const {return m_CenterVerticalOffsetSpline ;}




  //----------------------------------------------------------------------------
  // Update methods
  // These use the current values of the splines to update the contour and axes.
  //----------------------------------------------------------------------------

  /// Update everything
  void Update() ;

  /// Update the transform which controls the cutting planes which divide the contour into quadrants. \n
  /// Applies the current values of the splines.
  void UpdateCuttingPlanesTransform();

  /// Update the transform which controls the contour axes and the clipping planes. \n
  /// Applies the current values of the splines.
  void UpdateContourAxesTransform();

  /// Update the transform of the global axes. \n
  /// Applies the current values of the splines.
  void UpdateGlobalAxesTransform();

  /// Update the contour quadrant. \n
  /// Applies the current values of the splines.
  void UpdateSegmentNorthEastTransform();

  /// Update the contour quadrant. \n
  /// Applies the current values of the splines.
  void UpdateSegmentNorthWestTransform();

  /// Update the contour quadrant. \n
  /// Applies the current values of the splines.
  void UpdateSegmentSouthEastTransform();

  /// Update the contour quadrant. \n
  /// Applies the current values of the splines.
 void UpdateSegmentSouthWestTransform();




 //----------------------------------------------------------------------------
 // Methods for calculating the deformed output muscle
 // The calculations must of course agree with the corresponding methods for integer slices
 //----------------------------------------------------------------------------

 /// calculate the position of the slice for an arbitrary value of zeta
 void CalculateSlicePosition(double zeta, double *pos) const ;

 /// calculate the normal of the slice for an arbitrary value of zeta
 void CalculateSliceNormal(double zeta, double *normal) const ;

 /// calculate the pose matrix of the slice for an arbitrary value of zeta
 void CalculateSlicePoseMatrix(double zeta, vtkMatrix4x4 *mat) const ;

 /// Calculate height of point x above plane
 /// the value is zero for a point in the plane.
 double HeightAbovePlane(double *normal, double *origin, double *x) const ;

 /// Calculate height of point x above plane defined by zeta
 /// the value is zero for a point in the plane.
 double HeightAbovePlane(double zeta, double *x) const ;

 /// calculate zeta of a point, i.e. which slice plane does point belong to
 double CalculateZetaOfPoint(double *x) const ;

 /// Apply the registration ops to create the output muscle
 void ApplyRegistrationOps() ;




 void SetText(int m, double n, int d, int s);

 int GetTypeOfMuscles() const {return m_MuscleType ;}
 void SetTypeOfMuscles(int t) {m_MuscleType = t ;}

 void Set4LandmarksFlag(int n) {m_Landmarks4 = n ;}

 /// Print self. \n
 /// NB This does not print the visual pipes - \n
 /// their PrintSelf() methods must be called explicitly.
 void PrintSelf(ostream &os, int indent) ;



protected:
  //----------------------------------------------------------------------------
  // synthetic scans methods
  //----------------------------------------------------------------------------
  /// Calculate the origins of the scans along the axis, given the fractional positions. \n
  /// Must call SetFractionalPosOfScans() first.
  void CalculatePositionsOfScans() ;

  /// Calculate the normals of the scans. \n
  /// Must call SetFractionalPosOfScans() first.
  void CalculateNormalsOfScans() ;

  /// Calculate the pose matrices of the scans. \n
  /// Must call CalculatePositionsOfScans() and CalculateNormalsOfScans() first.
  void CalculatePoseMatricesOfScans() ;

  /// Calculate size and resolution of synthetic scans. \n
  /// Must call SetGrainOfScans() and CalculatePoseMatricesOfScans() first. \n
  /// Size depends on bounds of muscle. \n
  /// Resolution is 1/16 to 2 times the image size, depending on the "grain" value.
  void FindSizeAndResolutionOfScans();

  /// Calculate total length of axis or axes
  double LengthOfAxis() const ;

  /// Calculate original centers of contours on each slice. \n
  /// Do this when everything else is set up, \n
  /// and before any transforms have been applied. \n
  /// Don't call it from anywhere except Initialize(). \n
  /// It sets values which affect the transforms, so don't forget to update after.
  void CalculateOriginalContourCenters() ;



  //----------------------------------------------------------------------------
  // set positions and properties of axes
  //----------------------------------------------------------------------------
  bool	SetUpContourCoordinateAxes(); ///< set properties of contour axes
  bool	SetUpGlobalCoordinateAxes();  ///< set properties of global axes



  //----------------------------------------------------------------------------
  // useful maths
  //----------------------------------------------------------------------------

  /// Convenience method for multiplying a non-homo 3-point with a 4x4 matrix
  /// multiply point by matrix c = Ab. \n
  /// c can be the same as b
  void MultiplyMatrixPoint(vtkMatrix4x4* A, double b[3], double c[3]) const ;

  /// Calculate center of any plane created by vtkPlaneSource and scaled by sizx, sizy
  void CalculateCenterOfVtkPlane(double sizx, double sizy, double p[3]) const ;





  //----------------------------------------------------------------------------
  // print debug info
  //----------------------------------------------------------------------------
  void Print(vtkObject *obj, wxString msg = "") const ; //SIL. 24-12-2004: 




  //----------------------------------------------------------------------------
  // member variables
  //----------------------------------------------------------------------------

  /// input and output muscle polydata
  vtkPolyData *m_MuscleInput ; ///< input muscle in patient coords
  vtkPolyData *m_MuscleOutput ; ///< output muscle after registration ops applied.


  // visual pipes and corresponding interactor styles
  int m_Display3D; ///< flag for display mode (0 = 2d, 1 = 3d, 2 = preview)
  albaOpMML3ModelView2DPipe *m_VisualPipe2D ;
  albaOpMML3ModelView3DPipe *m_VisualPipe3D ;
  albaOpMML3ModelView3DPipe *m_VisualPipePreview ;
  vtkInteractorStyleTrackballCamera *m_Style3D ;
  vtkInteractorStyleImage *m_Style2D ;




  // type of muscles
  // 1 for single axis, 2 for two-part piecewise axis
  int m_MuscleType;

  vtkDoubleArray* m_OperationsStack; ///< stack of operations for undo purposes

  vtkIntArray* m_ScalingFlagStack;


  // splines variables
  vtkKochanekSpline *m_PlaceHorizontalOffsetSpline;   ///< place op (translate contour + widget)
  vtkKochanekSpline *m_PlaceVerticalOffsetSpline;
  vtkKochanekSpline *m_TwistSpline;                   ///< rotate op 
  vtkKochanekSpline *m_HorizontalTranslationSpline;   ///< translate op (translate contour only)
  vtkKochanekSpline *m_VerticalTranslationSpline;
  vtkKochanekSpline *m_NorthScalingSpline;            ///< scaling op 
  vtkKochanekSpline *m_SouthScalingSpline;
  vtkKochanekSpline *m_EastScalingSpline;
  vtkKochanekSpline *m_WestScalingSpline;
  vtkKochanekSpline *m_CenterHorizontalOffsetSpline;  ///< center op (translate to contour centre, not set by user or displayed)
  vtkKochanekSpline *m_CenterVerticalOffsetSpline;


  // atlas landmarks variables
  double m_AtlasLandmark1[3];
  double m_AtlasLandmark2[3];
  double m_AtlasLandmark3[3];
  double m_AtlasLandmark4[3];

  // patient landmarks variables
  double m_PatientLandmark1[3];
  double m_PatientLandmark2[3];
  double m_PatientLandmark3[3];
  double m_PatientLandmark4[3];

  // axis landmarks
  double m_AxisLandmark1[3] ;
  double m_AxisLandmark2[3] ;
  double m_AxisLandmark3[3] ;


  // 4 landmarks flag
  int m_Landmarks4;

  // patient scans
  vtkDataSet* m_Scans;

  // synthetic scans
  int m_NumberOfScans;
  int m_ScansGrain;
  int m_ScansCurrentId;
  int m_ScansResolutionX; // resolution of slice image is no. of quads
  int m_ScansResolutionY;
  double m_ScanSizeX;
  double m_ScanSizeY;

  // muscle
  //vtkSTLReader *m_MuscleSTLReader;
  vtkSTLWriter *m_MuscleSTLWriter;
  vtkPolyDataWriter *m_MusclePolyDataWriter;

  // rendering
  vtkRenderWindow *m_RenderWindow;
  vtkRenderer *m_Renderer;

  // scaling occurred flags
  bool m_ScalingOccured ;
  int m_ScalingOccuredOperationId ;


  //----------------------------------------------------------------------------
  // variables for slice positions
  //----------------------------------------------------------------------------

  // fractional distance of slices along axis or axes
  // 0 <= alpha <= 1, where 0 and 1 are the min and max of the slice range
  double *m_Alpha ;

  // actual distance of slices along axis or axes
  // 0 <= zeta <= L, where L is the total length of the axis or axes
  double *m_Zeta ;

  // positions and normals of scans in patient coords
  double (*m_SlicePositions)[3] ; // syntax for declaring 2D array m_SlicePositions[][3]
  double (*m_SliceNormals)[3] ;

  // centers and bounds of contours before transformations have been applied
  double (*m_OriginalContourCenters)[3] ;
  double (*m_OriginalContourBounds)[6] ;

  // matrices containing position and orientation of each slice
  vtkMatrix4x4 **m_SlicePoseMat ;
  vtkMatrix4x4 **m_SlicePoseInvMat ;

  // saved alpha, zeta and normal of the axis mid point, if the muscle axis is in two parts
  double m_AlphaMidPoint ;
  double m_ZetaMidPoint ;
  double m_NormalStart[3] ;     // value at alpha = 0.0, not value of first slice
  double m_NormalMidPoint[3] ;  // value at axis mid point
  double m_NormalEnd[3] ;       // value at alpha = 1.0, not value of last slice

  vtkALBAMatrixVectorMath *m_Math ; 
};

#endif 

