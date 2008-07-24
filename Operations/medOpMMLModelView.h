/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLModelView.h,v $
  Language:  C++
  Date:      $Date: 2008-07-24 08:00:04 $
  Version:   $Revision: 1.2 $
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
  vtkRenderer* GetRenderer() {return m_pRenderer;}; //SIL. 23-12-2004: 
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
	vtkLineSource *m_pContourPosXAxisLineSource; // positive x
	vtkActor *m_pContourPosXAxisActor;
	vtkTubeFilter *m_pContourPosXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_pContourPosXAxisPolyDataMapper;
	vtkLineSource *m_pContourPosYAxisLineSource; // positive y
	vtkActor *m_pContourPosYAxisActor;
	vtkTubeFilter *m_pContourPosYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_pContourPosYAxisPolyDataMapper;
	vtkLineSource *m_pContourNegXAxisLineSource; // negative x
	vtkActor *m_pContourNegXAxisActor;
	vtkTubeFilter *m_pContourNegXAxisAxesTubeFilter;
	vtkPolyDataMapper *m_pContourNegXAxisPolyDataMapper;
	vtkLineSource *m_pContourNegYAxisLineSource; // negative y
	vtkActor *m_pContourNegYAxisActor;
	vtkTubeFilter *m_pContourNegYAxisAxesTubeFilter;
	vtkPolyDataMapper *m_pContourNegYAxisPolyDataMapper;

	// tube filters radius
	float m_TubeFilterRadius;

	// 3d display flag
	int m_3dDisplay;

	// 4 landmarks flag
	int m_4Landmarks;

	// patient scans
	vtkDataSet* scans;

	// synthetic scans
	int m_SyntheticScansGrain;
	int m_nSyntheticScansTotalNumber;
	int m_nSyntheticScansCurrentId;
	int m_nSyntheticScansXResolution;
	int m_nSyntheticScansYResolution;
	float m_nSyntheticScansXSize;
	float m_nSyntheticScansYSize;
	float m_SyntheticScansMinScalarValue;
	float m_SyntheticScansMaxScalarValue;
	vtkPlaneSource **m_pSyntheticScansPlaneSource; // planes
	vtkTransform **m_pSyntheticScansPlaneSourceTransform; // transforms
	vtkTransformPolyDataFilter **m_pSyntheticScansPlaneSourceTransformPolyDataFilter;
	vtkProbeFilter **m_pSyntheticScansProbeFilter; // probes
	vtkWindowLevelLookupTable *m_pSyntheticScansWindowLevelLookupTable; // lut
	vtkPolyDataMapper **m_pSyntheticScansPolyDataMapper; // mappers
	vtkActor **m_pSyntheticScansActor; // actors
	vtkTransform **m_pSyntheticScansActorTransform;


	// muscle
	//vtkSTLReader *m_pMuscleSTLReader;
	vtkSTLWriter *m_pMuscleSTLWriter;
	vtkPolyDataWriter *m_pMusclePolyDataWriter;
	vtkPolyData* m_pMusclePolyData;
	vtkPolyDataMapper *m_pMusclePolyDataMapper;
	vtkLODActor *m_pMuscleLODActor;

  // three transforms which are pipelined together
	vtkTransform *m_pMuscleTransform1;
	vtkTransform *m_pMuscleTransform2;
	vtkTransformPolyDataFilter *m_pMuscleTransform1PolyDataFilter;  // input is vtkPolydata* muscle
	vtkTransformPolyDataFilter *m_pMuscleTransform2PolyDataFilter;
	vtkPolyDataNormals *m_pMusclePolyDataNormals;                   // adds normals to polydata

	double m_flMuscleXScalingFactor;
	double m_flMuscleYScalingFactor;
	float m_flMusclePolyDataBounds[6];
	float m_flMusclePolyDataCenter[3];
	float m_flMusclePolyDataExtent[3];

	// final transform
	vtkMatrix4x4 *m_pfinalm;

	// synthetic slices transform
	vtkMatrix4x4 *m_pslicesm;

	// type of muscles
	int m_nTypeOfMuscles;

	// window/level
	float m_pWindow;
	float m_pLevel;

	// contour
	vtkPlane *m_pContourPlane;
	vtkCutter *m_pContourCutter;
	vtkTransform* m_pContourCutterTransform;
	vtkTransformPolyDataFilter *m_pContourCutterTransformPolyDataFilter;
	vtkTubeFilter *m_pContourTubeFilter;
	vtkPolyDataMapper *m_pContourPolyDataMapper;
	vtkActor *m_pContourActor;
	vtkSphereSource *m_pContourGlyphSphereSource;
	vtkGlyph3D *m_pContourGlyph3D;
	vtkPolyDataMapper *m_pContourGlyphPolyDataMapper;
	vtkActor *m_pContourGlyphActor;

	// landmarks
	vtkSphereSource *m_pLandmark1SphereSource;
	vtkPolyDataMapper *m_pLandmark1PolyDataMapper;
	vtkActor *m_pLandmark1Actor;
	vtkSphereSource *m_pLandmark2SphereSource;
	vtkPolyDataMapper *m_pLandmark2PolyDataMapper;
	vtkActor *m_pLandmark2Actor;
	vtkSphereSource *m_pLandmark3SphereSource;
	vtkPolyDataMapper *m_pLandmark3PolyDataMapper;
	vtkActor *m_pLandmark3Actor;
	vtkSphereSource *m_pLandmark4SphereSource;
	vtkPolyDataMapper *m_pLandmark4PolyDataMapper;
	vtkActor *m_pLandmark4Actor;

	// L1 to L2 line (action)
	vtkLineSource *m_pL1L2LineSource;
	vtkTubeFilter *m_pL1L2TubeFilter;
	vtkPolyDataMapper *m_pL1L2PolyDataMapper;
	vtkActor *m_pL1L2Actor;

	// L2 to L3 line
	vtkLineSource *m_pL2L3LineSource;
	vtkTubeFilter *m_pL2L3TubeFilter;
	vtkPolyDataMapper *m_pL2L3PolyDataMapper;
	vtkActor *m_pL2L3Actor;

	// scaling contours stuff
	vtkPlane *m_pX0ZNPlane; // pointing N
	vtkPlane *m_pX0ZSPlane; // pointing S
	vtkPlane *m_pY0ZEPlane; // pointing E
	vtkPlane *m_pY0ZWPlane; // pointing W

	// north-east
	vtkTubeFilter *m_pNEContourTubeFilter;
	vtkTransformPolyDataFilter *m_pNEContourTransformPolyDataFilter;
	vtkClipPolyData *m_pNEContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_pNEContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_pNEContourPolyDataMapper;
	vtkActor *m_pNEContourActor;

	// south-east
	vtkTubeFilter *m_pSEContourTubeFilter;
	vtkTransformPolyDataFilter *m_pSEContourTransformPolyDataFilter;
	vtkClipPolyData *m_pSEContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_pSEContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_pSEContourPolyDataMapper;
	vtkActor *m_pSEContourActor;

	// north-west
	vtkTubeFilter *m_pNWContourTubeFilter;
	vtkTransformPolyDataFilter *m_pNWContourTransformPolyDataFilter;
	vtkClipPolyData *m_pNWContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_pNWContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_pNWContourPolyDataMapper;
	vtkActor *m_pNWContourActor;

	// south-west
	vtkTubeFilter *m_pSWContourTubeFilter;
	vtkTransformPolyDataFilter *m_pSWContourTransformPolyDataFilter;
	vtkClipPolyData *m_pSWContourX0ZPlaneClipPolyData;
	vtkClipPolyData *m_pSWContourY0ZPlaneClipPolyData;
	vtkPolyDataMapper *m_pSWContourPolyDataMapper;
	vtkActor *m_pSWContourActor;

	// information display
	vtkTextMapper *m_pTextMapperX;
	vtkScaledTextActor *m_pScaledTextActorX;

	vtkTextMapper *m_pTextMapperY;
	vtkScaledTextActor *m_pScaledTextActorY;

	// rendering
	vtkRenderWindow *m_pRenderWindow;
	vtkRenderer *m_pRenderer;
	vtkRenderWindowInteractor *m_pRenderWindowInteractor;
	vtkInteractorStyleImage *m_pInteractorStyleImage;
	vtkInteractorStyleTrackballCamera *m_pInteractorStyleTrackballCamera;

	// camera
	double m_pCameraFocalPoint[3];
	double m_pCameraPosition[3];
	double m_pCameraClippingRange[2];
	double m_pCameraViewUp[3];

	// unit vectors and lengths of landmark lines
	double m_dunitvector12[3];
	double m_dunitvector23[3];
	double m_dlength12;
	double m_dlength23;
	double m_doveralllength;
};

#endif 

