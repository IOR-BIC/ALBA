/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3ModelView.h,v $
Language:  C++
Date:      $Date: 2009-06-11 17:20:08 $
Version:   $Revision: 1.1.2.3 $
Authors:   Mel Krokos
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __medOpMML3ModelView_H__
#define __medOpMML3ModelView_H__

#include "mafDefines.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataNormals.h"
#include "vtkScaledTextActor.h"
#include "vtkDataSet.h"
#include "vtkKochanekSpline.h"
#include "vtkTubeFilter.h"
#include "vtkActor.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkMatrix4x4.h"
#include "vtkPlane.h"
#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLODActor.h"
#include "vtkCamera.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkProbeFilter.h"
#include "vtkPlaneSource.h"
#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkCutter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkClipPolyData.h"
#include "vtkTextMapper.h"

#define MAX_CHARS 512
#define MAX_SLICES 250



//------------------------------------------------------------------------------
/// medOpMML3ModelView. \n
/// Helper class for medOpMML3. \n
/// This class is the visual pipe for the op.
//------------------------------------------------------------------------------
class medOpMML3ModelView
{
public:
  //----------------------------------------------------------------------------
  // constructor and destructor
  //----------------------------------------------------------------------------
  medOpMML3ModelView(vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume); ///< constructor
  virtual ~medOpMML3ModelView(); ///< destructor



  //----------------------------------------------------------------------------
  // Outputs of model view
  //----------------------------------------------------------------------------

  /// Get the total transform of the muscle from input to output
  vtkMatrix4x4* GetFinalM() const {return m_FinalMat ;}

  /// Get the output muscle polydata
  vtkPolyData* GetOutputPolydata() const {return m_MuscleTransform2PolyDataFilter->GetOutput() ;}



  //----------------------------------------------------------------------------
  // synthetic scans methods
  //----------------------------------------------------------------------------

  /// Set grain of scans. \n
  /// The grain value 1-5 is used to set the resolution of the slice images.
  void SetGrainOfScans(int n) {m_SyntheticScansGrain = n ;}

  /// Calculate size and resolution of synthetic scans. \n
  /// Must be called prior to CreateSyntheticScans. \n
  /// Size depends on bounds of muscle. \n
  /// Resolution is 1/16 to 2 times the image size, depending on the "grain" value.
  void FindSizeAndResolutionOfSyntheticScans();

  /// Construct the visual pipes for all the slices
  /// NB must call FindSizeAndResolutionOfSyntheticScans() first
  void CreateSyntheticScans();

  /// Delete the synthetic scans
  void DeleteSyntheticScans();

  /// Get slice actor, for setting visibility
  vtkActor* GetActorOfSyntheticScans(int s) const {return m_SyntheticScansActor[s] ;}

  /// Get look up table for scans
  vtkWindowLevelLookupTable* GetWindowLevelLookupTableOfSyntheticScans() const {return m_SyntheticScansWindowLevelLookupTable ;}

  int GetTotalNumberOfSyntheticScans() const ;
  void SetTotalNumberOfSyntheticScans(int n) {m_NSyntheticScansTotalNumber = n ;}

  double GetCurrentZOfSyntheticScans();

  int GetCurrentIdOfSyntheticScans() const {return m_NSyntheticScansCurrentId ;}
  void SetCurrentIdOfSyntheticScans(int n) {m_NSyntheticScansCurrentId = n ;}

  double GetZOfSyntheticScans(int s);

  double GetSyntheticScansLevel() const {return m_Window ;}
  double GetSyntheticScansWindow() const {return m_Level ;}

  void SetSizeOfSyntheticScans(float x, float y);
  void GetSizeOfSyntheticScans(float *x, float *y);
  void GetResolutionOfSyntheticScans(int *x, int *y);
  void SetResolutionOfSyntheticScans(int x, int y);

  float GetLowScalar() const {return m_SyntheticScansMinScalarValue ;}
  float GetHighScalar() const {return m_SyntheticScansMaxScalarValue ;}





  //----------------------------------------------------------------------------
  // landmark methods
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

  void FindUnitVectorsAndLengthsOfLandmarkLines();

  /// Muscle transform 1 - global registration using landmarks. \n
  /// This is the transform which moves the muscle landmarks to the corresponding 
  /// points on the volume.
  bool MapAtlasToPatient();



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

  /// switch visual pipe to 3d display
  void Switch3dDisplayOn();



  bool GetScalingOccured() const {return m_ScalingOccured ;} ///< has scaling occured
  void SetScalingOccured(bool scalingOccured) {m_ScalingOccured = scalingOccured ;} ///< set scaling occured flag

  int GetScalingOccuredOperationId() const {return m_ScalingOccuredOperationId ;} ///< get id
  void SetScalingOccuredOperationId(int id) {m_ScalingOccuredOperationId = id ;} ///< set id

  void UpdateSegmentNorthEastTransform();
  void UpdateSegmentNorthWestTransform();
  void UpdateSegmentSouthEastTransform();
  void UpdateSegmentSouthWestTransform();

  vtkScaledTextActor* GetScaledTextActor1() const {return m_ScaledTextActorY ;}
  vtkScaledTextActor* GetScaledTextActor2() const {return m_ScaledTextActorX ;}

  // splines functions
  vtkKochanekSpline* GetPHSpline() const {return m_CenterHorizontalOffsetSpline ;}
  vtkKochanekSpline* GetPVSpline() const {return m_CenterVerticalOffsetSpline ;}
  vtkKochanekSpline* GetTHSpline() const {return m_HorizontalTranslationSpline ;}
  vtkKochanekSpline* GetTVSpline() const {return m_VerticalTranslationSpline ;}
  vtkKochanekSpline* GetRASpline() const {return m_TwistSpline ;}
  vtkKochanekSpline* GetSWSpline() const {return m_WestScalingSpline ;}
  vtkKochanekSpline* GetSESpline() const {return m_EastScalingSpline ;}
  vtkKochanekSpline* GetSSSpline() const {return m_SouthScalingSpline ;}
  vtkKochanekSpline* GetSNSpline() const {return m_NorthScalingSpline ;}

  vtkActor* GetPositiveXAxisActor() const {return m_PosXAxisActor ;}
  vtkActor* GetNegativeXAxisActor() const {return m_NegXAxisActor ;}
  vtkActor* GetPositiveYAxisActor() const {return m_PosYAxisActor ;}
  vtkActor* GetNegativeYAxisActor() const {return m_NegYAxisActor ;}
  vtkActor* GetPositiveZAxisActor() const {return m_PosZAxisActor ;}
  vtkActor* GetNegativeZAxisActor() const {return m_NegZAxisActor ;}

  vtkActor* GetContourActor() const {return m_ContourActor ;}

  void SetText(int m, double n, int d, int s);
  vtkTransformPolyDataFilter* GetNEContourTransformPolyDataFilter() const {return m_NEContourTransformPolyDataFilter ;}
  vtkTransformPolyDataFilter* GetNWContourTransformPolyDataFilter() const {return m_NWContourTransformPolyDataFilter ;}
  vtkTransformPolyDataFilter* GetSEContourTransformPolyDataFilter() const {return m_SEContourTransformPolyDataFilter ;}
  vtkTransformPolyDataFilter* GetSWContourTransformPolyDataFilter() const {return m_SWContourTransformPolyDataFilter ;}

  void GetXYScalingFactorsOfMuscle(double* x, double *y) const ;
  void SetXYScalingFactorsOfMuscle(double x, double y);

  void UpdateContourAxesTransform();
  void UpdateGlobalAxesTransform();
  void UpdateContourCuttingPlane();
  void UpdateSegmentCuttingPlanes();

  vtkRenderWindowInteractor* GetRenderWindowInteractor() const {return m_RenderWindowInteractor ;}
  vtkRenderer* GetRenderer() const {return m_Renderer;}
  void Render();

  int GetTypeOfMuscles() const {return m_NTypeOfMuscles ;}
  void SetTypeOfMuscles(int t) {m_NTypeOfMuscles = t ;}

  void SetGlobalAxesVisibility();
  void SetContourAxesVisibility();

  vtkActor* GetContourPositiveXAxisActor() const {return m_ContourPosXAxisActor ;}
  vtkActor* GetContourNegativeXAxisActor() const {return m_ContourNegXAxisActor ;}
  vtkActor* GetContourPositiveYAxisActor() const {return m_ContourPosYAxisActor ;}
  vtkActor* GetContourNegativeYAxisActor() const {return m_ContourNegYAxisActor ;}

  vtkActor* GetNEContourActor() const {return m_NEContourActor ;}
  vtkActor* GetNWContourActor() const {return m_NWContourActor ;}
  vtkActor* GetSEContourActor() const {return m_SEContourActor ;}
  vtkActor* GetSWContourActor() const {return m_SWContourActor ;}

  vtkTextMapper* GetTextMapper1() const {return m_TextMapperY ;}
  vtkTextMapper* GetTextMapper2() const {return m_TextMapperX ;}

  void Set4LandmarksFlag(int n) {m_4Landmarks = n ;}


  /// Muscle transform 2. \n
  /// Transform coordinates into a coordinate system, in which z-axis \n
  /// is aligned with muscle line axis as defined by insertion points \n
  /// in patient, and origin being the middle of this muscle line axis. \n
  bool MakeActionLineZAxis();


  vtkTubeFilter* GetContourTubeFilter() const {return m_ContourTubeFilter ;}
  bool	SetUpContourCoordinateAxes();
  bool	SetUpGlobalCoordinateAxes();

  vtkPlane* GetContourPlane() const {return m_ContourPlane ;}

protected:
  //----------------------------------------------------------------------------
  // synthetic scans methods
  //----------------------------------------------------------------------------
  vtkMatrix4x4* CreateActorTransformOfSyntheticScans(int scanId);  ///< allocate and calculate new transform matrix
  vtkMatrix4x4* CreatePlaneSourceTransformOfSyntheticScans(int scanId);  ///< allocate and calculate new transform matrix

  /// Calculate the origin of the slice, \n
  /// ie the intersection of the axis with the slice. \n
  /// The axis goes from (patient) landmark 2 to landmark 1.
  void GetPlaneSourceOriginOfSyntheticScans(int scanId, double p[3]);

  double GetContourAxesLengthScale() const {return m_ContourAxesLengthScale ;} ///< get length of contour axes
  void SetContourAxesLengthScale(double length) {m_ContourAxesLengthScale = length ;} ///< set length of contour axes

  vtkPlane* GetCuttingPlaneWest() const {return m_Y0ZWPlane ;}
  vtkPlane* GetCuttingPlaneEast() const {return m_Y0ZEPlane ;}
  vtkPlane* GetCuttingPlaneNorth() const {return m_X0ZNPlane ;}
  vtkPlane* GetCuttingPlaneSouth() const {return m_X0ZSPlane ;}

  vtkTubeFilter* GetPositiveXAxisTubeFilter() const {return m_PosXAxisAxesTubeFilter ;}
  vtkTubeFilter* GetNegativeXAxisTubeFilter() const {return m_NegXAxisAxesTubeFilter ;}
  vtkTubeFilter* GetPositiveYAxisTubeFilter() const {return m_PosYAxisAxesTubeFilter ;}
  vtkTubeFilter* GetNegativeYAxisTubeFilter() const {return m_NegYAxisAxesTubeFilter ;}

  vtkTubeFilter* GetContourPositiveXAxisTubeFilter() const {return m_ContourPosXAxisAxesTubeFilter ;}
  vtkTubeFilter* GetContourNegativeXAxisTubeFilter() const {return m_ContourNegXAxisAxesTubeFilter ;}
  vtkTubeFilter* GetContourPositiveYAxisTubeFilter() const {return m_ContourPosYAxisAxesTubeFilter ;}
  vtkTubeFilter* GetContourNegativeYAxisTubeFilter() const {return m_ContourNegYAxisAxesTubeFilter ;}

  vtkLineSource* GetPositiveXAxisLineSource() const {return m_PosXAxisLineSource ;}
  vtkLineSource* GetNegativeXAxisLineSource() const {return m_NegXAxisLineSource ;}
  vtkLineSource* GetPositiveYAxisLineSource() const {return m_PosYAxisLineSource ;}
  vtkLineSource* GetNegativeYAxisLineSource() const {return m_NegYAxisLineSource ;}

  vtkLineSource* GetContourPositiveXAxisLineSource() const {return m_ContourPosXAxisLineSource ;}
  vtkLineSource* GetContourNegativeXAxisLineSource() const {return m_ContourNegXAxisLineSource ;}
  vtkLineSource* GetContourPositiveYAxisLineSource() const {return m_ContourPosYAxisLineSource ;}
  vtkLineSource* GetContourNegativeYAxisLineSource() const {return m_ContourNegYAxisLineSource ;}

  vtkTransform* GetContourCutterTransform() const {return m_ContourCutterTransform ;}
  vtkTransformPolyDataFilter* GetContourCutterTransformPolyDataFilter() const {return m_ContourCutterTransformPolyDataFilter ;}

  vtkLODActor* GetMuscleLODActor() const {return m_MuscleLODActor ;}
  void WriteMatrix(char *pch, vtkMatrix4x4 *m) const ;
  void SetNegativeLineActorX(double p1[3], double p2[3]);
  void SetPositiveLineActorY(double p1[3], double p2[3]);
  void SetNegativeLineActorY(double p1[3], double p2[3]);
  void SetPositiveLineActorX(double p1[3], double p2[3]);
  void AddActor(vtkActor* a);
  void RetrieveCameraViewUp(double* vu);
  void RetrieveCameraClippingRange(double *cr);
  void RetrieveCameraPosition(double *cp);
  void RetrieveCameraFocalPoint(double *fp);
  void SaveCameraViewUp(double *vu);
  void SaveCameraClippingRange(double *cr);
  void SaveCameraPosition(double *cp);
  void SaveCameraFocalPoint(double *fp);
  vtkCamera* GetActiveCamera();

  /// multiply matrices c = ab
  /// c can be the same as a or b
  void MultiplyMatrix4x4(vtkMatrix4x4* a, vtkMatrix4x4* b, vtkMatrix4x4* c) const ;

  /// Calculate centre of any plane created by vtkPlaneSource and scaled by sizx, sizy
  void CalculateCentreOfVtkPlane(double sizx, double sizy, double p[3]) const ;

  void Print(vtkObject *obj, wxString msg = "") const ; //SIL. 24-12-2004: 



  //----------------------------------------------------------------------------
  // member variables
  //----------------------------------------------------------------------------

  // Final transform, set in MakeActionLineZAxis(), and equal to transform 2.
  // This transforms the muscle to the coord system of the stack of slices.
  vtkMatrix4x4 *m_FinalMat;

  // synthetic slices transform, set in MakeActionLineZAxis().
  // This is the rotation component of m_FinalMat.
  // This rotates the muscle to the slice plane.
  // Its inverse rotates the slice plane to the required slicing direction.
  vtkMatrix4x4 *m_SliceRotationMat;

  // type of muscles
  int m_NTypeOfMuscles;

  double m_ContourAxesLengthScale; ///< length of contour axes

  vtkDoubleArray* m_OperationsStack; ///< stack of operations for undo purposes

  vtkIntArray* m_ScalingFlagStack;
  /*vtkDoubleArray* SStack;
  vtkDoubleArray* ZStack;

  vtkDoubleArray* PopStack;
  vtkDoubleArray* TopStack;
  vtkDoubleArray* RopStack;
  vtkDoubleArray* SopStack;*/


  // splines variables
  vtkKochanekSpline *m_CenterHorizontalOffsetSpline;
  vtkKochanekSpline *m_CenterVerticalOffsetSpline;
  vtkKochanekSpline *m_TwistSpline;
  vtkKochanekSpline *m_HorizontalTranslationSpline;
  vtkKochanekSpline *m_VerticalTranslationSpline;
  vtkKochanekSpline *m_NorthScalingSpline;
  vtkKochanekSpline *m_SouthScalingSpline;
  vtkKochanekSpline *m_EastScalingSpline;
  vtkKochanekSpline *m_WestScalingSpline;

  // atlas landmarks variables
  double m_DMuscleAtlasInsertionPoint1[3];
  double m_DMuscleAtlasInsertionPoint2[3];
  double m_DMuscleAtlasReferencePoint1[3];
  double m_DMuscleAtlasReferencePoint2[3];

  // patient landmarks variables
  double m_DMuscleScansInsertionPoint1[3];
  double m_DMuscleScansInsertionPoint2[3];
  double m_DMuscleScansReferencePoint1[3];
  double m_DMuscleScansReferencePoint2[3];



  // 2d axes - action line system
  vtkLineSource *m_PosXAxisLineSource; // positive x
  vtkActor *m_PosXAxisActor;
  vtkTubeFilter *m_PosXAxisAxesTubeFilter;
  vtkPolyDataMapper *m_PosXAxisPolyDataMapper;
  vtkLineSource *m_PosYAxisLineSource; // positive y
  vtkActor *m_PosYAxisActor;
  vtkTubeFilter *m_PosYAxisAxesTubeFilter;
  vtkPolyDataMapper *m_PosYAxisPolyDataMapper;
  vtkLineSource *m_NegXAxisLineSource; // negative x
  vtkActor *m_NegXAxisActor;
  vtkTubeFilter *m_NegXAxisAxesTubeFilter;
  vtkPolyDataMapper *m_NegXAxisPolyDataMapper;
  vtkLineSource *m_NegYAxisLineSource; // negative y
  vtkActor *m_NegYAxisActor;
  vtkTubeFilter *m_NegYAxisAxesTubeFilter;
  vtkPolyDataMapper *m_NegYAxisPolyDataMapper;
  vtkLineSource *m_PosZAxisLineSource; // positive z (3d display only)
  vtkActor *m_PosZAxisActor;
  vtkTubeFilter *m_PosZAxisAxesTubeFilter;
  vtkPolyDataMapper *m_PosZAxisPolyDataMapper;
  vtkLineSource *m_NegZAxisLineSource; // negative z (3d display only)
  vtkActor *m_NegZAxisActor;
  vtkTubeFilter *m_NegZAxisAxesTubeFilter;
  vtkPolyDataMapper *m_NegZAxisPolyDataMapper;

  // 2d axes - contour system
  vtkLineSource *m_ContourPosXAxisLineSource; // positive x
  vtkActor *m_ContourPosXAxisActor;
  vtkTubeFilter *m_ContourPosXAxisAxesTubeFilter;
  vtkPolyDataMapper *m_ContourPosXAxisPolyDataMapper;
  vtkLineSource *m_ContourPosYAxisLineSource; // positive y
  vtkActor *m_ContourPosYAxisActor;
  vtkTubeFilter *m_ContourPosYAxisAxesTubeFilter;
  vtkPolyDataMapper *m_ContourPosYAxisPolyDataMapper;
  vtkLineSource *m_ContourNegXAxisLineSource; // negative x
  vtkActor *m_ContourNegXAxisActor;
  vtkTubeFilter *m_ContourNegXAxisAxesTubeFilter;
  vtkPolyDataMapper *m_ContourNegXAxisPolyDataMapper;
  vtkLineSource *m_ContourNegYAxisLineSource; // negative y
  vtkActor *m_ContourNegYAxisActor;
  vtkTubeFilter *m_ContourNegYAxisAxesTubeFilter;
  vtkPolyDataMapper *m_ContourNegYAxisPolyDataMapper;

  // tube filters radius
  float m_TubeFilterRadius;

  // 3d display flag
  int m_3DDisplay;

  // 4 landmarks flag
  int m_4Landmarks;

  // patient scans
  vtkDataSet* m_Scans;

  // synthetic scans
  int m_SyntheticScansGrain;
  int m_NSyntheticScansTotalNumber;
  int m_NSyntheticScansCurrentId;
  int m_NSyntheticScansXResolution; // resolution of slice image is no. of quads
  int m_NSyntheticScansYResolution;
  float m_NSyntheticScansXSize;
  float m_NSyntheticScansYSize;
  float m_SyntheticScansMinScalarValue;
  float m_SyntheticScansMaxScalarValue;

  vtkWindowLevelLookupTable *m_SyntheticScansWindowLevelLookupTable; // lut
  vtkActor **m_SyntheticScansActor; ///< actors for synthetic scans


  // muscle
  //vtkSTLReader *m_MuscleSTLReader;
  vtkSTLWriter *m_MuscleSTLWriter;
  vtkPolyDataWriter *m_MusclePolyDataWriter;
  vtkPolyData* m_MusclePolyData;
  vtkPolyDataMapper *m_MusclePolyDataMapper;
  vtkLODActor *m_MuscleLODActor;

  // three transforms which are pipelined together
  vtkTransform *m_MuscleTransform1;
  vtkTransform *m_MuscleTransform2;
  vtkTransformPolyDataFilter *m_MuscleTransform1PolyDataFilter;  // input is vtkPolydata* muscle
  vtkTransformPolyDataFilter *m_MuscleTransform2PolyDataFilter;
  vtkPolyDataNormals *m_MusclePolyDataNormals;                   // adds normals to polydata

  double m_FlMuscleXScalingFactor;
  double m_FlMuscleYScalingFactor;
  float m_FlMusclePolyDataBounds[6];
  float m_FlMusclePolyDataCenter[3];
  float m_FlMusclePolyDataExtent[3];


  // window/level
  double m_Window;
  double m_Level;

  // contour
  vtkPlane *m_ContourPlane;
  vtkCutter *m_ContourCutter;
  vtkTransform* m_ContourCutterTransform;
  vtkTransformPolyDataFilter *m_ContourCutterTransformPolyDataFilter;
  vtkTubeFilter *m_ContourTubeFilter;
  vtkPolyDataMapper *m_ContourPolyDataMapper;
  vtkActor *m_ContourActor;
  vtkSphereSource *m_ContourGlyphSphereSource;
  vtkGlyph3D *m_ContourGlyph3D;
  vtkPolyDataMapper *m_ContourGlyphPolyDataMapper;
  vtkActor *m_ContourGlyphActor;

  // landmarks
  vtkSphereSource *m_Landmark1SphereSource;
  vtkPolyDataMapper *m_Landmark1PolyDataMapper;
  vtkActor *m_Landmark1Actor;
  vtkSphereSource *m_Landmark2SphereSource;
  vtkPolyDataMapper *m_Landmark2PolyDataMapper;
  vtkActor *m_Landmark2Actor;
  vtkSphereSource *m_Landmark3SphereSource;
  vtkPolyDataMapper *m_Landmark3PolyDataMapper;
  vtkActor *m_Landmark3Actor;
  vtkSphereSource *m_Landmark4SphereSource;
  vtkPolyDataMapper *m_Landmark4PolyDataMapper;
  vtkActor *m_Landmark4Actor;

  // L1 to L2 line (action)
  vtkLineSource *m_L1L2LineSource;
  vtkTubeFilter *m_L1L2TubeFilter;
  vtkPolyDataMapper *m_L1L2PolyDataMapper;
  vtkActor *m_L1L2Actor;

  // L2 to L3 line
  vtkLineSource *m_L2L3LineSource;
  vtkTubeFilter *m_L2L3TubeFilter;
  vtkPolyDataMapper *m_L2L3PolyDataMapper;
  vtkActor *m_L2L3Actor;

  // scaling contours stuff
  vtkPlane *m_X0ZNPlane; // pointing N
  vtkPlane *m_X0ZSPlane; // pointing S
  vtkPlane *m_Y0ZEPlane; // pointing E
  vtkPlane *m_Y0ZWPlane; // pointing W

  // north-east
  vtkTubeFilter *m_NEContourTubeFilter;
  vtkTransformPolyDataFilter *m_NEContourTransformPolyDataFilter;
  vtkClipPolyData *m_NEContourX0ZPlaneClipPolyData;
  vtkClipPolyData *m_NEContourY0ZPlaneClipPolyData;
  vtkPolyDataMapper *m_NEContourPolyDataMapper;
  vtkActor *m_NEContourActor;

  // south-east
  vtkTubeFilter *m_SEContourTubeFilter;
  vtkTransformPolyDataFilter *m_SEContourTransformPolyDataFilter;
  vtkClipPolyData *m_SEContourX0ZPlaneClipPolyData;
  vtkClipPolyData *m_SEContourY0ZPlaneClipPolyData;
  vtkPolyDataMapper *m_SEContourPolyDataMapper;
  vtkActor *m_SEContourActor;

  // north-west
  vtkTubeFilter *m_NWContourTubeFilter;
  vtkTransformPolyDataFilter *m_NWContourTransformPolyDataFilter;
  vtkClipPolyData *m_NWContourX0ZPlaneClipPolyData;
  vtkClipPolyData *m_NWContourY0ZPlaneClipPolyData;
  vtkPolyDataMapper *m_NWContourPolyDataMapper;
  vtkActor *m_NWContourActor;

  // south-west
  vtkTubeFilter *m_SWContourTubeFilter;
  vtkTransformPolyDataFilter *m_SWContourTransformPolyDataFilter;
  vtkClipPolyData *m_SWContourX0ZPlaneClipPolyData;
  vtkClipPolyData *m_SWContourY0ZPlaneClipPolyData;
  vtkPolyDataMapper *m_SWContourPolyDataMapper;
  vtkActor *m_SWContourActor;

  // information display
  vtkTextMapper *m_TextMapperX;
  vtkScaledTextActor *m_ScaledTextActorX;

  vtkTextMapper *m_TextMapperY;
  vtkScaledTextActor *m_ScaledTextActorY;

  // rendering
  vtkRenderWindow *m_RenderWindow;
  vtkRenderer *m_Renderer;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  // camera
  double m_CameraFocalPoint[3];
  double m_CameraPosition[3];
  double m_CameraClippingRange[2];
  double m_CameraViewUp[3];

  // unit vectors and lengths of landmark lines
  double m_DUnitVector12[3];
  double m_DUnitVector23[3];
  double m_DLength12;
  double m_DLength23;
  double m_DOverallLength;

  // scaling occured flags
  bool m_ScalingOccured ;
  int m_ScalingOccuredOperationId ;


};

#endif 

