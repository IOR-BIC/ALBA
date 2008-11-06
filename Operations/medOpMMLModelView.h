/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLModelView.h,v $
  Language:  C++
  Date:      $Date: 2008-11-06 09:13:10 $
  Version:   $Revision: 1.3.2.2 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#ifndef __medOpMMLModelView_H__
#define __medOpMMLModelView_H__

#include "mafDefines.h"

#define MAX_CHARS 512
#define MAX_SLICES 250

#include "assert.h"  					//?????????????????
#include "direct.h"						//?????????????????
#include "vtkRenderWindow.h"
#include "vtkKochanekSpline.h"
#include "vtkPoints.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkSTLReader.h"
#include "vtkLODActor.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkBMPReader.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageShiftScale.h"
#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkMath.h"
#include "vtkUnsignedShortArray.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkTextMapper.h"
#include "vtkScaledTextActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkExtractPolyDataGeometry.h"
#include "vtkProperty.h"
#include "vtkClipPolyData.h"
#include "vtkSTLWriter.h"
#include "vtkRectilinearGridReader.h"
#include "vtkPlaneSource.h"
#include "vtkProbeFilter.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkRectilinearGrid.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkTextSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataNormals.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// medOpMMLModelView     :
//----------------------------------------------------------------------------
class medOpMMLModelView  
{
public:
  medOpMMLModelView(vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume);
  virtual ~medOpMMLModelView();

  void FindUnitVectorsAndLengthsOfLandmarkLines();
	vtkMatrix4x4* GetActorTransformOfSyntheticScans(int s);
	vtkMatrix4x4* GetPlaneSourceTransformOfSyntheticScans(int s);
	void GetPlaneSourceTransformOfSyntheticScans(int s, vtkMatrix4x4* m);
	void FindSizeAndResolutionOfSyntheticScans();
	void Switch3dDisplayOn();
	void SetGrainOfScans(int n);
	void Set4LandmarksFlag(int n);
	float GetSyntheticScansLevel();
	float GetSyntheticScansWindow();
	float GetLowScalar();
	float GetHighScalar();
	vtkTubeFilter* GetContourTubeFilter();
	vtkMatrix4x4* GetFinalM();
	vtkTransformPolyDataFilter* GetMuscleTransform2PolyDataFilter();
	int GetScalingOccured();
	bool	SetUpContourCoordinateAxes();
	bool	SetUpGlobalCoordinateAxes();
 
  void Print(vtkObject *obj, wxString msg = ""); //SIL. 24-12-2004: 
  
  
  BOOL m_ScalingOccured;
	int m_ScalingOccuredOperationId;
	vtkIntArray* m_ScalingFlagStack;
	vtkDoubleArray* m_OperationsStack;
	/*vtkDoubleArray* SStack;
	vtkDoubleArray* ZStack;

	vtkDoubleArray* PopStack;
	vtkDoubleArray* TopStack;
	vtkDoubleArray* RopStack;
	vtkDoubleArray* SopStack;*/

	void SetGlobalAxesVisibility();
	void SetContourAxesVisibility();
	// length of contour axes
	float m_ContourAxesLengthScale;
	float GetContourAxesLengthScale();
	void SetContourAxesLengthScale(float l);
	void UpdateContourAxesTransform();
	void UpdateGlobalAxesTransform();
	void UpdateContourCuttingPlane();
	void UpdateSegmentCuttingPlanes();
	void UpdateSegmentNorthEastTransform();
	void UpdateSegmentNorthWestTransform();
	void UpdateSegmentSouthEastTransform();
	void UpdateSegmentSouthWestTransform();
	vtkPlane* GetCuttingPlaneWest();
	vtkPlane* GetCuttingPlaneEast();
	vtkPlane* GetCuttingPlaneSouth();
	vtkPlane* GetCuttingPlaneNorth();
	int GetTypeOfMuscles();
	void SetTypeOfMuscles(int t);
	
	vtkActor* GetContourNegativeYAxisActor();
	vtkActor* GetContourNegativeXAxisActor();
	vtkActor* GetContourPositiveYAxisActor();
	vtkActor* GetContourPositiveXAxisActor();
	vtkTubeFilter* GetContourNegativeYAxisTubeFilter();
	vtkTubeFilter* GetContourNegativeXAxisTubeFilter();
	vtkLineSource* GetContourNegativeXAxisLineSource();
	vtkTubeFilter* GetContourPositiveYAxisTubeFilter();
	vtkTubeFilter* GetContourPositiveXAxisTubeFilter();
	vtkLineSource* GetContourNegativeYAxisLineSource();
	vtkLineSource* GetContourPositiveYAxisLineSource();
	vtkLineSource* GetContourPositiveXAxisLineSource();
	vtkInteractorStyleTrackballCamera* GetInteractorStyleTrackballCamera();
	vtkInteractorStyleImage* GetInteractorStyleImage();
	vtkRenderWindowInteractor* GetRenderWindowInteractor();
  vtkRenderer* GetRenderer() {return m_PRenderer;}; //SIL. 23-12-2004: 
	vtkTubeFilter* GetNegativeYAxisTubeFilter();
	vtkTubeFilter* GetPositiveYAxisTubeFilter();
	vtkTubeFilter* GetNegativeXAxisTubeFilter();
	vtkTubeFilter* GetPositiveXAxisTubeFilter();
	vtkLineSource* GetNegativeYAxisLineSource();
	vtkLineSource* GetPositiveYAxisLineSource();
	vtkLineSource* GetNegativeXAxisLineSource();
	vtkLineSource* GetPositiveXAxisLineSource();
	vtkTransform* GetContourCutterTransform();
	vtkPlane* GetContourPlane();
	vtkTransformPolyDataFilter* GetContourCutterTransformPolyDataFilter();
	double GetZOfSyntheticScans(int s);
	double GetCurrentZOfSyntheticScans();
	void GetXYScalingFactorsOfMuscle(double* x, double *y);
	void SetXYScalingFactorsOfMuscle(double x, double y);
	vtkMatrix4x4* MultiplyMatrix4x4(vtkMatrix4x4* a, vtkMatrix4x4* b);
	void SetSizeOfSyntheticScans(float x, float y);
	void GetSizeOfSyntheticScans(float *x, float *y);
	vtkWindowLevelLookupTable* GetWindowLevelLookupTableOfSyntheticScans();
	int GetCurrentIdOfSyntheticScans();
	void SetCurrentIdOfSyntheticScans(int n);
	vtkActor* GetActorOfSyntheticScans(int s);
	void GetPlaneSourceOriginOfSyntheticScans(int s, double p[3]);
	void GetResolutionOfSyntheticScans(int *x, int *y);
	
	int GetTotalNumberOfSyntheticScans();
	void SetResolutionOfSyntheticScans(int x, int y);
	void SetTotalNumberOfSyntheticScans(int n);
	vtkLODActor* GetMuscleLODActor();
	void WriteMatrix(char *pch, vtkMatrix4x4 *m);
	void SetNegativeLineActorX(double p1[3], double p2[3]);
	void SetPositiveLineActorY(double p1[3], double p2[3]);
	void SetNegativeLineActorY(double p1[3], double p2[3]);
	void SetPositiveLineActorX(double p1[3], double p2[3]);
	vtkActor* GetNegativeYAxisActor();
	vtkActor* GetNegativeXAxisActor();
	vtkActor* GetPositiveYAxisActor();
	vtkActor* GetPositiveXAxisActor();
	vtkActor* GetPositiveZAxisActor();
	vtkActor* GetNegativeZAxisActor();
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
	void SetText(int m, double n, int d, int s);
	vtkTransformPolyDataFilter* GetNEContourTransformPolyDataFilter();
	vtkTransformPolyDataFilter* GetNWContourTransformPolyDataFilter();
	vtkTransformPolyDataFilter* GetSEContourTransformPolyDataFilter();
	vtkTransformPolyDataFilter* GetSWContourTransformPolyDataFilter();
	vtkActor* GetNEContourActor();
	vtkActor* GetNWContourActor();
	vtkActor* GetSEContourActor();
	vtkActor* GetSWContourActor();
	vtkTextMapper* GetTextMapper1();
	vtkScaledTextActor* GetScaledTextActor1();
	vtkScaledTextActor* GetScaledTextActor2();
	vtkTextMapper* GetTextMapper2();
	void Render();

	// splines functions
	vtkKochanekSpline* GetPHSpline();
	vtkKochanekSpline* GetPVSpline();
	vtkKochanekSpline* GetTHSpline();
	vtkKochanekSpline* GetTVSpline();
	vtkKochanekSpline* GetRASpline();
	vtkKochanekSpline* GetSWSpline();
	vtkKochanekSpline* GetSESpline();
	vtkKochanekSpline* GetSSSpline();
	vtkKochanekSpline* GetSNSpline();
	
	// splines variables
	vtkKochanekSpline *m_PCenterHorizontalOffsetSpline;
	vtkKochanekSpline *m_PCenterVerticalOffsetSpline;
	vtkKochanekSpline *m_PTwistSpline;
	vtkKochanekSpline *m_PHorizontalTranslationSpline;
	vtkKochanekSpline *m_PVerticalTranslationSpline;
	vtkKochanekSpline *m_PNorthScalingSpline;
	vtkKochanekSpline *m_PSouthScalingSpline;
	vtkKochanekSpline *m_PEastScalingSpline;
	vtkKochanekSpline *m_PWestScalingSpline;

	// atlas landmarks functions
	void SetLandmark1OfAtlas(double *xyz);
	void GetLandmark1OfAtlas(double *xyz);
	void SetLandmark2OfAtlas(double *xyz);
	void GetLandmark2OfAtlas(double *xyz);
	void SetLandmark3OfAtlas(double *xyz);
	void GetLandmark3OfAtlas(double *xyz);
	void SetLandmark4OfAtlas(double *xyz);
	void GetLandmark4OfAtlas(double *xyz);

	// atlas landmarks variables
	double m_DMuscleAtlasInsertionPoint1[3];
	double m_DMuscleAtlasInsertionPoint2[3];
	double m_DMuscleAtlasReferencePoint1[3];
	double m_DMuscleAtlasReferencePoint2[3];

	// patient landmarks functions
	void SetLandmark1OfPatient(double *xyz);
	void GetLandmark1OfPatient(double *xyz);
	void SetLandmark2OfPatient(double *xyz);
	void GetLandmark2OfPatient(double *xyz);
	void SetLandmark3OfPatient(double *xyz);
	void GetLandmark3OfPatient(double *xyz);
	void SetLandmark4OfPatient(double *xyz);
	void GetLandmark4OfPatient(double *xyz);

	// patient landmarks variables
	double m_DMuscleScansInsertionPoint1[3];
	double m_DMuscleScansInsertionPoint2[3];
	double m_DMuscleScansReferencePoint1[3];
	double m_DMuscleScansReferencePoint2[3];
	
	// other functions
	void SetUpSyntheticScans();

	vtkActor* GetContourActor();
	bool MapAtlasToPatient();
	bool MakeActionLineZAxis();


	// 2d axes - action line system
	vtkLineSource *m_PPosXAxisLineSource; // positive x
	vtkActor *m_PPosXAxisActor;
	vtkTubeFilter *m_PPosXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PPosXAxisPolyDataMapper;
	vtkLineSource *m_PPosYAxisLineSource; // positive y
	vtkActor *m_PPosYAxisActor;
	vtkTubeFilter *m_PPosYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PPosYAxisPolyDataMapper;
	vtkLineSource *m_PNegXAxisLineSource; // negative x
	vtkActor *m_PNegXAxisActor;
	vtkTubeFilter *m_PNegXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PNegXAxisPolyDataMapper;
	vtkLineSource *m_PNegYAxisLineSource; // negative y
	vtkActor *m_PNegYAxisActor;
	vtkTubeFilter *m_PNegYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PNegYAxisPolyDataMapper;
	vtkLineSource *m_PPosZAxisLineSource; // positive z (3d display only)
	vtkActor *m_PPosZAxisActor;
	vtkTubeFilter *m_PPosZAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PPosZAxisPolyDataMapper;
	vtkLineSource *m_PNegZAxisLineSource; // negative z (3d display only)
	vtkActor *m_PNegZAxisActor;
	vtkTubeFilter *m_PNegZAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PNegZAxisPolyDataMapper;

	// 2d axes - contour system
	vtkLineSource *m_PContourPosXAxisLineSource; // positive x
	vtkActor *m_PContourPosXAxisActor;
	vtkTubeFilter *m_PContourPosXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PContourPosXAxisPolyDataMapper;
	vtkLineSource *m_PContourPosYAxisLineSource; // positive y
	vtkActor *m_PContourPosYAxisActor;
	vtkTubeFilter *m_PContourPosYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PContourPosYAxisPolyDataMapper;
	vtkLineSource *m_PContourNegXAxisLineSource; // negative x
	vtkActor *m_PContourNegXAxisActor;
	vtkTubeFilter *m_PContourNegXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PContourNegXAxisPolyDataMapper;
	vtkLineSource *m_PContourNegYAxisLineSource; // negative y
	vtkActor *m_PContourNegYAxisActor;
	vtkTubeFilter *m_PContourNegYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_PContourNegYAxisPolyDataMapper;

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
	int m_NSyntheticScansXResolution;
	int m_NSyntheticScansYResolution;
	float m_NSyntheticScansXSize;
	float m_NSyntheticScansYSize;
	float m_SyntheticScansMinScalarValue;
	float m_SyntheticScansMaxScalarValue;
	vtkPlaneSource **m_PSyntheticScansPlaneSource; // planes
	vtkTransform **m_PSyntheticScansPlaneSourceTransform; // transforms
	vtkTransformPolyDataFilter **m_PSyntheticScansPlaneSourceTransformPolyDataFilter;
	vtkProbeFilter **m_PSyntheticScansProbeFilter; // probes
	vtkWindowLevelLookupTable *m_PSyntheticScansWindowLevelLookupTable; // lut
	vtkPolyDataMapper **m_PSyntheticScansPolyDataMapper; // mappers
	vtkActor **m_PSyntheticScansActor; // actors
	vtkTransform **m_PSyntheticScansActorTransform;


	// muscle
	//vtkSTLReader *m_PMuscleSTLReader;
	vtkSTLWriter *m_PMuscleSTLWriter;
	vtkPolyDataWriter *m_PMusclePolyDataWriter;
	vtkPolyData* m_PMusclePolyData;
	vtkPolyDataMapper *m_PMusclePolyDataMapper;
	vtkLODActor *m_PMuscleLODActor;

  // three transforms which are pipelined together
	vtkTransform *m_PMuscleTransform1;
	vtkTransform *m_PMuscleTransform2;
	vtkTransformPolyDataFilter *m_PMuscleTransform1PolyDataFilter;  // input is vtkPolydata* muscle
	vtkTransformPolyDataFilter *m_PMuscleTransform2PolyDataFilter;
	vtkPolyDataNormals *m_PMusclePolyDataNormals;                   // adds normals to polydata

	double m_FlMuscleXScalingFactor;
	double m_FlMuscleYScalingFactor;
	float m_FlMusclePolyDataBounds[6];
	float m_FlMusclePolyDataCenter[3];
	float m_FlMusclePolyDataExtent[3];

	// final transform
	vtkMatrix4x4 *m_PFinalm;

	// synthetic slices transform
	vtkMatrix4x4 *m_PSlicesm;

	// type of muscles
	int m_NTypeOfMuscles;

	// window/level
	float m_PWindow;
	float m_PLevel;

	// contour
	vtkPlane *m_PContourPlane;
	vtkCutter *m_PContourCutter;
	vtkTransform* m_PContourCutterTransform;
	vtkTransformPolyDataFilter *m_PContourCutterTransformPolyDataFilter;
	vtkTubeFilter *m_PContourTubeFilter;
	vtkPolyDataMapper *m_PContourPolyDataMapper;
	vtkActor *m_PContourActor;
	vtkSphereSource *m_PContourGlyphSphereSource;
	vtkGlyph3D *m_PContourGlyph3D;
	vtkPolyDataMapper *m_PContourGlyphPolyDataMapper;
	vtkActor *m_PContourGlyphActor;

	// landmarks
	vtkSphereSource *m_PLandmark1SphereSource;
	vtkPolyDataMapper *m_PLandmark1PolyDataMapper;
	vtkActor *m_PLandmark1Actor;
	vtkSphereSource *m_PLandmark2SphereSource;
	vtkPolyDataMapper *m_PLandmark2PolyDataMapper;
	vtkActor *m_PLandmark2Actor;
	vtkSphereSource *m_PLandmark3SphereSource;
	vtkPolyDataMapper *m_PLandmark3PolyDataMapper;
	vtkActor *m_PLandmark3Actor;
	vtkSphereSource *m_PLandmark4SphereSource;
	vtkPolyDataMapper *m_PLandmark4PolyDataMapper;
	vtkActor *m_PLandmark4Actor;

	// L1 to L2 line (action)
	vtkLineSource *m_PL1L2LineSource;
	vtkTubeFilter *m_PL1L2TubeFilter;
	vtkPolyDataMapper *m_PL1L2PolyDataMapper;
	vtkActor *m_PL1L2Actor;

	// L2 to L3 line
	vtkLineSource *m_PL2L3LineSource;
	vtkTubeFilter *m_PL2L3TubeFilter;
	vtkPolyDataMapper *m_PL2L3PolyDataMapper;
	vtkActor *m_PL2L3Actor;

	// scaling contours stuff
	vtkPlane *m_PX0ZNPlane; // pointing N
	vtkPlane *m_PX0ZSPlane; // pointing S
	vtkPlane *m_PY0ZEPlane; // pointing E
	vtkPlane *m_PY0ZWPlane; // pointing W

	// north-east
	vtkTubeFilter *m_PNEContourTubeFilter;
	vtkTransformPolyDataFilter *m_PNEContourTransformPolyDataFilter;
	vtkClipPolyData *m_PNEContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_PNEContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_PNEContourPolyDataMapper;
	vtkActor *m_PNEContourActor;

	// south-east
	vtkTubeFilter *m_PSEContourTubeFilter;
	vtkTransformPolyDataFilter *m_PSEContourTransformPolyDataFilter;
	vtkClipPolyData *m_PSEContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_PSEContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_PSEContourPolyDataMapper;
	vtkActor *m_PSEContourActor;

	// north-west
	vtkTubeFilter *m_PNWContourTubeFilter;
	vtkTransformPolyDataFilter *m_PNWContourTransformPolyDataFilter;
	vtkClipPolyData *m_PNWContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_PNWContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_PNWContourPolyDataMapper;
	vtkActor *m_PNWContourActor;

	// south-west
	vtkTubeFilter *m_PSWContourTubeFilter;
	vtkTransformPolyDataFilter *m_PSWContourTransformPolyDataFilter;
	vtkClipPolyData *m_PSWContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_PSWContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_PSWContourPolyDataMapper;
	vtkActor *m_PSWContourActor;

	// information display
	vtkTextMapper *m_PTextMapperX;
	vtkScaledTextActor *m_PScaledTextActorX;

	vtkTextMapper *m_PTextMapperY;
	vtkScaledTextActor *m_PScaledTextActorY;

	// rendering
	vtkRenderWindow *m_PRenderWindow;
	vtkRenderer *m_PRenderer;
	vtkRenderWindowInteractor *m_PRenderWindowInteractor;
	vtkInteractorStyleImage *m_PInteractorStyleImage;
	vtkInteractorStyleTrackballCamera *m_PInteractorStyleTrackballCamera;

	// camera
	double m_PCameraFocalPoint[3];
	double m_PCameraPosition[3];
	double m_PCameraClippingRange[2];
	double m_PCameraViewUp[3];

	// unit vectors and lengths of landmark lines
	double m_DUnitVector12[3];
	double m_DUnitVector23[3];
	double m_DLength12;
	double m_DLength23;
	double m_DOverallLength;
};

#endif 

