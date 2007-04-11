/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMMLModelView.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-11 11:53:06 $
  Version:   $Revision: 1.1 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"

#include "mmoMMLModelView.h"
#include "mafSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"


//SIL. 24-12-2004: begin
#ifdef VTK_USE_ANSI_STDLIB
#include <sstream>
#endif
//SIL. 24-12-2004: end




//----------------------------------------------------------------------------
// Constructor for model view
mmoMMLModelView::mmoMMLModelView( vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume)
//----------------------------------------------------------------------------
{ 
	//mafSmartPointer <vtkTransform> transf;

	//vtkTextActor* TextActor = vtkTextActor::New();
	//TextActor->SetInput("Initializing. Please wait...");
	//vtkCoordinate *c  = TextActor->GetPositionCoordinate();
	//c->SetCoordinateSystemToNormalizedViewport();
	//c->SetValue(0.35, 0.5f);
	//Model->GetRenderer()->AddActor(TextActor);

	// 2d display by default
	m_3dDisplay = 0;

	// 3 landmarks by default for initial mapping
	m_4Landmarks = 0;

	//
	scans = volume;

	// display tubes
	m_TubeFilterRadius = 0.5 ;

	// final transform
	m_pfinalm = vtkMatrix4x4::New();

	// synthetic slices transform
	m_pslicesm = vtkMatrix4x4::New();

	//
	ScalingOccured = FALSE;

	//
	m_pRenderer = ren;
	m_pRenderWindow = rw;
	m_pRenderWindowInteractor = rw->GetInteractor();
	
	//
	m_pRenderer->SetBackground(0.2,0.4,0.6);
	
	int *size = m_pRenderWindow->GetSize();
	assert(size[0] > 0);
	assert(size[1] > 0);
  
	m_pRenderWindow->LineSmoothingOn();
	m_pInteractorStyleImage = vtkInteractorStyleImage::New();
	m_pInteractorStyleTrackballCamera = vtkInteractorStyleTrackballCamera::New();


	//// just to test things
  /*
	vtkTextSource *ts = vtkTextSource::New();
  vtkPolyData *polydata = muscle ;

	ts->SetForegroundColor(1, 1, 1);
	ts->BackingOff();
	ts->SetText("Model View");
	vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
	mapper->SetInput(polydata);
  mapper->Update() ;

  // analyse polydata
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  int ncells = polydata->GetNumberOfCells() ;
  double bnds[6] ;
  polydata->GetBounds(bnds) ;

  // analyse volume data
  int datatype = volume->GetDataObjectType() ;
  volume->GetBounds(bnds) ;
  

	vtkActor* labactor = vtkActor::New();
	labactor->SetMapper(mapper);
  labactor->SetOrigin(0 , 0 , 0);
  labactor->SetPosition(0 , 0 , 0);
  labactor->SetScale(1 , 1 , 1);
  labactor->SetVisibility(1) ;

  m_pRenderer->AddActor(labactor) ;
  */

	
	// 3d display
	//m_pRenderWindowInteractor->SetInteractorStyle(m_pInteractorStyleTrackballCamera);
	
	
	// 2d display
	m_pRenderWindowInteractor->SetInteractorStyle(m_pInteractorStyleImage);

	//
	m_pCenterHorizontalOffsetSpline = vtkKochanekSpline::New();
	m_pCenterHorizontalOffsetSpline->ClosedOff();

	//
	m_pCenterVerticalOffsetSpline = vtkKochanekSpline::New();
	m_pCenterVerticalOffsetSpline->ClosedOff();

	//
	m_pTwistSpline = vtkKochanekSpline::New();
	m_pTwistSpline->ClosedOff();

	m_pHorizontalTranslationSpline = vtkKochanekSpline::New();
	m_pHorizontalTranslationSpline->ClosedOff();

	m_pVerticalTranslationSpline = vtkKochanekSpline::New();
	m_pVerticalTranslationSpline->ClosedOff();

	m_pNorthScalingSpline = vtkKochanekSpline::New();
	m_pNorthScalingSpline->ClosedOff();

	m_pSouthScalingSpline = vtkKochanekSpline::New();
	m_pSouthScalingSpline->ClosedOff();

	m_pEastScalingSpline = vtkKochanekSpline::New();
	m_pEastScalingSpline->ClosedOff();

	m_pWestScalingSpline = vtkKochanekSpline::New();
	m_pWestScalingSpline->ClosedOff();

	// synthetic slices
	m_pSyntheticScansPlaneSource = NULL ; // planes
	m_pSyntheticScansPlaneSourceTransform = NULL; // transforms
	m_pSyntheticScansPlaneSourceTransformPolyDataFilter = NULL;
	m_pSyntheticScansProbeFilter = NULL; // probes
	m_pSyntheticScansPolyDataMapper = NULL; // mappers
	m_pSyntheticScansActor = NULL; // actors
	m_pSyntheticScansActorTransform = NULL; // transforms

	// synthetic slices lut
	m_pSyntheticScansWindowLevelLookupTable = vtkWindowLevelLookupTable::New();

	//
	ScalingFlagStack = vtkIntArray::New();
	ScalingFlagStack->SetNumberOfComponents(1);

	OperationsStack = vtkDoubleArray::New();
	OperationsStack->SetNumberOfComponents(5);
	OperationsStack->SetNumberOfTuples(2000);

	/*SStack = vtkDoubleArray::New();
	SStack->SetNumberOfComponents(1);

	ZStack = vtkDoubleArray::New();
	ZStack->SetNumberOfComponents(1);

	PopStack = vtkDoubleArray::New();
	PopStack->SetNumberOfComponents(2);

	TopStack = vtkDoubleArray::New();
	TopStack->SetNumberOfComponents(2);

	RopStack = vtkDoubleArray::New();
	RopStack->SetNumberOfComponents(1);

	SopStack = vtkDoubleArray::New();
	SopStack->SetNumberOfComponents(4);*/

	// standard display
	// east contour axis
	m_pContourPosXAxisLineSource = vtkLineSource::New();
	m_pContourPosXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pContourPosXAxisAxesTubeFilter->SetInput(m_pContourPosXAxisLineSource->GetOutput());
	m_pContourPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pContourPosXAxisPolyDataMapper->SetInput(m_pContourPosXAxisAxesTubeFilter->GetOutput());
	m_pContourPosXAxisActor = vtkActor::New();
	m_pContourPosXAxisActor->SetMapper(m_pContourPosXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pContourPosXAxisActor);
	m_pContourPosXAxisActor->VisibilityOff();
	
	// north contour axis
	m_pContourPosYAxisLineSource = vtkLineSource::New();
	m_pContourPosYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pContourPosYAxisAxesTubeFilter->SetInput(m_pContourPosYAxisLineSource->GetOutput());
	m_pContourPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pContourPosYAxisPolyDataMapper->SetInput(m_pContourPosYAxisAxesTubeFilter->GetOutput());
	m_pContourPosYAxisActor = vtkActor::New();
	m_pContourPosYAxisActor->SetMapper(m_pContourPosYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pContourPosYAxisActor);
	m_pContourPosYAxisActor->VisibilityOff();

	// west contour axis
	m_pContourNegXAxisLineSource = vtkLineSource::New();
	m_pContourNegXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pContourNegXAxisAxesTubeFilter->SetInput(m_pContourNegXAxisLineSource->GetOutput());
	m_pContourNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pContourNegXAxisPolyDataMapper->SetInput(m_pContourNegXAxisAxesTubeFilter->GetOutput());
	m_pContourNegXAxisActor = vtkActor::New();
	m_pContourNegXAxisActor->SetMapper(m_pContourNegXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pContourNegXAxisActor);
	m_pContourNegXAxisActor->VisibilityOff();
	
	// south contour axis
	m_pContourNegYAxisLineSource = vtkLineSource::New();
	m_pContourNegYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pContourNegYAxisAxesTubeFilter->SetInput(m_pContourNegYAxisLineSource->GetOutput());
	m_pContourNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pContourNegYAxisPolyDataMapper->SetInput(m_pContourNegYAxisAxesTubeFilter->GetOutput());
	m_pContourNegYAxisActor = vtkActor::New();
	m_pContourNegYAxisActor->SetMapper(m_pContourNegYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pContourNegYAxisActor);
	m_pContourNegYAxisActor->VisibilityOff();

	// east global axis
	m_pPosXAxisLineSource = vtkLineSource::New();
	m_pPosXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pPosXAxisAxesTubeFilter->SetInput(m_pPosXAxisLineSource->GetOutput());
	m_pPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pPosXAxisPolyDataMapper->SetInput(m_pPosXAxisAxesTubeFilter->GetOutput());
	m_pPosXAxisActor = vtkActor::New();
	m_pPosXAxisActor->SetMapper(m_pPosXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pPosXAxisActor);
	m_pPosXAxisActor->VisibilityOff();
	
	// north global axis
	m_pPosYAxisLineSource = vtkLineSource::New();
	m_pPosYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pPosYAxisAxesTubeFilter->SetInput(m_pPosYAxisLineSource->GetOutput());
	m_pPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pPosYAxisPolyDataMapper->SetInput(m_pPosYAxisAxesTubeFilter->GetOutput());
	m_pPosYAxisActor = vtkActor::New();
	m_pPosYAxisActor->SetMapper(m_pPosYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pPosYAxisActor);
	m_pPosYAxisActor->VisibilityOff();

	// west global axis
	m_pNegXAxisLineSource = vtkLineSource::New();
	m_pNegXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pNegXAxisAxesTubeFilter->SetInput(m_pNegXAxisLineSource->GetOutput());
	m_pNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pNegXAxisPolyDataMapper->SetInput(m_pNegXAxisAxesTubeFilter->GetOutput());
	m_pNegXAxisActor = vtkActor::New();
	m_pNegXAxisActor->SetMapper(m_pNegXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pNegXAxisActor);
	m_pNegXAxisActor->VisibilityOff();

	// south global axis
	m_pNegYAxisLineSource = vtkLineSource::New();
	m_pNegYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pNegYAxisAxesTubeFilter->SetInput(m_pNegYAxisLineSource->GetOutput());
	m_pNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pNegYAxisPolyDataMapper->SetInput(m_pNegYAxisAxesTubeFilter->GetOutput());
	m_pNegYAxisActor = vtkActor::New();
	m_pNegYAxisActor->SetMapper(m_pNegYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pNegYAxisActor);
	m_pNegYAxisActor->VisibilityOff();

	// 3d display
	// 1st landmark
	m_pLandmark1SphereSource = vtkSphereSource::New();
	m_pLandmark1PolyDataMapper = vtkPolyDataMapper::New();
	m_pLandmark1PolyDataMapper->SetInput(m_pLandmark1SphereSource->GetOutput());
	m_pLandmark1Actor = vtkActor::New();
	m_pLandmark1Actor->SetMapper(m_pLandmark1PolyDataMapper);
	m_pLandmark1Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pLandmark1Actor);

	// 2nd landmark
	m_pLandmark2SphereSource = vtkSphereSource::New();
	m_pLandmark2PolyDataMapper = vtkPolyDataMapper::New();
	m_pLandmark2PolyDataMapper->SetInput(m_pLandmark2SphereSource->GetOutput());
	m_pLandmark2Actor = vtkActor::New();
	m_pLandmark2Actor->SetMapper(m_pLandmark2PolyDataMapper);
	m_pLandmark2Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pLandmark2Actor);

	// 3rd landmark
	m_pLandmark3SphereSource = vtkSphereSource::New();
	m_pLandmark3PolyDataMapper = vtkPolyDataMapper::New();
	m_pLandmark3PolyDataMapper->SetInput(m_pLandmark3SphereSource->GetOutput());
	m_pLandmark3Actor = vtkActor::New();
	m_pLandmark3Actor->SetMapper(m_pLandmark3PolyDataMapper);
	m_pLandmark3Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pLandmark3Actor);

	// 4th landmark
	m_pLandmark4SphereSource = vtkSphereSource::New();
	m_pLandmark4PolyDataMapper = vtkPolyDataMapper::New();
	m_pLandmark4PolyDataMapper->SetInput(m_pLandmark4SphereSource->GetOutput());
	m_pLandmark4Actor = vtkActor::New();
	m_pLandmark4Actor->SetMapper(m_pLandmark4PolyDataMapper);
	m_pLandmark4Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pLandmark4Actor);

	// L1 to L2 line (action)
	m_pL1L2LineSource = vtkLineSource::New();
	m_pL1L2TubeFilter = vtkTubeFilter::New();
	m_pL1L2TubeFilter->SetInput(m_pL1L2LineSource->GetOutput());
	m_pL1L2PolyDataMapper = vtkPolyDataMapper::New();
	m_pL1L2PolyDataMapper->SetInput(m_pL1L2TubeFilter->GetOutput());
	m_pL1L2Actor = vtkActor::New();
	m_pL1L2Actor->SetMapper(m_pL1L2PolyDataMapper);
	m_pL1L2Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pL1L2Actor);

	// L2 to L3 line
	m_pL2L3LineSource = vtkLineSource::New();
	m_pL2L3TubeFilter = vtkTubeFilter::New();
	m_pL2L3TubeFilter->SetInput(m_pL2L3LineSource->GetOutput());
	m_pL2L3PolyDataMapper = vtkPolyDataMapper::New();
	m_pL2L3PolyDataMapper->SetInput(m_pL2L3TubeFilter->GetOutput());
	m_pL2L3Actor = vtkActor::New();
	m_pL2L3Actor->SetMapper(m_pL2L3PolyDataMapper);
	m_pL2L3Actor->VisibilityOff();
	m_pRenderer->AddActor(m_pL2L3Actor);

	// positive z global axis
	m_pPosZAxisLineSource = vtkLineSource::New();
	m_pPosZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pPosZAxisAxesTubeFilter->SetInput(m_pPosZAxisLineSource->GetOutput());
	m_pPosZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pPosZAxisPolyDataMapper->SetInput(m_pPosZAxisAxesTubeFilter->GetOutput());
	m_pPosZAxisActor = vtkActor::New();
	m_pPosZAxisActor->SetMapper(m_pPosZAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pPosZAxisActor);
	m_pPosZAxisActor->VisibilityOff();

	// negative z global axis
	m_pNegZAxisLineSource = vtkLineSource::New();
	m_pNegZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_pNegZAxisAxesTubeFilter->SetInput(m_pNegZAxisLineSource->GetOutput());
	m_pNegZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_pNegZAxisPolyDataMapper->SetInput(m_pNegZAxisAxesTubeFilter->GetOutput());
	m_pNegZAxisActor = vtkActor::New();
	m_pNegZAxisActor->SetMapper(m_pNegZAxisPolyDataMapper);
	m_pRenderer->AddActor(m_pNegZAxisActor);
	m_pNegZAxisActor->VisibilityOff();


	// muscle transform pipeline
  // Transform1 -> Transform2 -> Normals -> Mapper
	m_pMuscleTransform1 = vtkTransform::New();
	m_pMuscleTransform1PolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pMuscleTransform1PolyDataFilter->SetInput(muscle);
	m_pMuscleTransform1PolyDataFilter->SetTransform(m_pMuscleTransform1);
	m_pMuscleTransform2 = vtkTransform::New();
	m_pMuscleTransform2PolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pMuscleTransform2PolyDataFilter->SetInput(m_pMuscleTransform1PolyDataFilter->GetOutput());
	m_pMuscleTransform2PolyDataFilter->SetTransform(m_pMuscleTransform2);
	m_pMusclePolyDataNormals = vtkPolyDataNormals::New();
	m_pMusclePolyDataNormals->SetInput(m_pMuscleTransform2PolyDataFilter->GetOutput());
	m_pMusclePolyDataNormals->FlipNormalsOn();
	m_pMusclePolyDataMapper = vtkPolyDataMapper::New();
	m_pMusclePolyDataMapper->SetInput(m_pMusclePolyDataNormals->GetOutput());
	m_pMuscleLODActor = vtkLODActor::New();
	m_pMuscleLODActor->SetMapper(m_pMusclePolyDataMapper);
	m_pRenderer->AddActor(m_pMuscleLODActor);
	m_pMuscleLODActor->VisibilityOff();   // visibility off, but we can switch it on if we want to (see later)


	// 2d axes - contour system
	SetContourAxesLengthScale(2.0);
	
	////SIL. 24-12-2004: begin
	//wxLogMessage("==================================");
	//Print(m_pMuscleTransform1 ,               "m_pMuscleTransform1 ");
	//Print(m_pMuscleTransform1PolyDataFilter , "m_pMuscleTransform1PolyDataFilter ");
	//Print(m_pMuscleTransform2 ,               "m_pMuscleTransform2 ");
	//Print(m_pMuscleTransform2PolyDataFilter , "m_pMuscleTransform2PolyDataFilter ");
	//Print(m_pMuscleTransform2PolyDataFilter->GetOutput() , "m_pMuscleTransform2PolyDataFilter->GetOutput()");
	//wxLogMessage("==================================");
	//SIL. 24-12-2004: end


	// standard contour pipeline
	m_pContourPlane = vtkPlane::New();
	m_pContourCutter = vtkCutter::New();
	m_pContourCutter->SetCutFunction(m_pContourPlane);
	m_pContourCutter->SetInput(m_pMuscleTransform2PolyDataFilter->GetOutput());

	m_pContourCutterTransform = vtkTransform::New(); //to put back to z=0 plane
	m_pContourCutterTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pContourCutterTransformPolyDataFilter->SetInput(m_pContourCutter->GetOutput());
	m_pContourCutterTransformPolyDataFilter->SetTransform(m_pContourCutterTransform);

	m_pContourTubeFilter = vtkTubeFilter::New();
	m_pContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_pContourTubeFilter->SetNumberOfSides(12);
	m_pContourTubeFilter->SetInput(m_pContourCutterTransformPolyDataFilter->GetOutput());
	
	m_pContourPolyDataMapper = vtkPolyDataMapper::New();
	m_pContourPolyDataMapper->SetInput(m_pContourTubeFilter->GetOutput());
	
	m_pContourActor = vtkActor::New();
	m_pContourActor->SetMapper(m_pContourPolyDataMapper);
	m_pContourActor->VisibilityOff();
	m_pRenderer->AddActor(m_pContourActor);
	m_pContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	
	//// glyphed contour
	//
	//m_pContourGlyphSphereSource = vtkSphereSource::New();
	//m_pContourGlyph3D = vtkGlyph3D::New();
	//m_pContourGlyph3D->SetInput(m_pContourCutter->GetOutput());
	//m_pContourGlyph3D->SetSource(m_pContourGlyphSphereSource->GetOutput());
	//m_pContourGlyphPolyDataMapper = vtkPolyDataMapper::New();
	//m_pContourGlyphPolyDataMapper->SetInput(m_pContourGlyph3D->GetOutput());
	//m_pContourGlyphActor = vtkActor::New();
	//m_pContourGlyphActor->SetMapper(m_pContourGlyphPolyDataMapper);
	//m_pContourGlyphActor->VisibilityOff();
	//

	// scaling contours stuff
	m_pX0ZNPlane = vtkPlane::New(); // xOz north
	m_pX0ZNPlane->SetNormal(0.0, 1.0, 0.0);

	m_pX0ZSPlane = vtkPlane::New(); // x0z south
	m_pX0ZSPlane->SetNormal(0.0, -1.0, 0.0);
	
	m_pY0ZEPlane = vtkPlane::New(); // y0z east
	m_pY0ZEPlane->SetNormal(1.0, 0.0, 0.0);

	m_pY0ZWPlane = vtkPlane::New(); // y0z west
	m_pY0ZWPlane->SetNormal(-1.0, 0.0, 0.0);

	// north-east
	m_pNEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pNEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pNEContourTubeFilter =  vtkTubeFilter::New();
	m_pNEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pNEContourPolyDataMapper = vtkPolyDataMapper::New();
	m_pNEContourActor = vtkActor::New();
	
	m_pNEContourX0ZPlaneClipPolyData->SetInput(m_pContourCutter->GetOutput()); // first cut plane
	m_pNEContourX0ZPlaneClipPolyData->SetClipFunction(m_pX0ZNPlane);
	m_pNEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pNEContourY0ZPlaneClipPolyData->SetInput(m_pNEContourX0ZPlaneClipPolyData->GetOutput()); // second cut plane
	m_pNEContourY0ZPlaneClipPolyData->SetClipFunction(m_pY0ZEPlane);
	m_pNEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pNEContourTransformPolyDataFilter->SetInput(m_pNEContourY0ZPlaneClipPolyData->GetOutput());

	m_pNEContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_pNEContourTubeFilter->SetNumberOfSides(12);
	m_pNEContourTubeFilter->SetInput(m_pNEContourTransformPolyDataFilter->GetOutput());

	m_pNEContourPolyDataMapper->SetInput(m_pNEContourTubeFilter->GetOutput());
	m_pNEContourActor->SetMapper(m_pNEContourPolyDataMapper);
	m_pRenderer->AddActor(m_pNEContourActor);
	m_pNEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_pNEContourActor->VisibilityOff();

	// north-west
	m_pNWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pNWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pNWContourTubeFilter =  vtkTubeFilter::New();
	m_pNWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pNWContourPolyDataMapper = vtkPolyDataMapper::New();
	m_pNWContourActor = vtkActor::New();

	m_pNWContourX0ZPlaneClipPolyData->SetInput(m_pContourCutter->GetOutput());
	m_pNWContourX0ZPlaneClipPolyData->SetClipFunction(m_pX0ZNPlane);
	m_pNWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

	m_pNWContourY0ZPlaneClipPolyData->SetInput(m_pNWContourX0ZPlaneClipPolyData->GetOutput());
	m_pNWContourY0ZPlaneClipPolyData->SetClipFunction(m_pY0ZWPlane);
	m_pNWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

	m_pNWContourTransformPolyDataFilter->SetInput(m_pNWContourY0ZPlaneClipPolyData->GetOutput());

	m_pNWContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_pNWContourTubeFilter->SetNumberOfSides(12);
	m_pNWContourTubeFilter->SetInput(m_pNWContourTransformPolyDataFilter->GetOutput());

	m_pNWContourPolyDataMapper->SetInput(m_pNWContourTubeFilter->GetOutput());
	m_pNWContourActor->SetMapper(m_pNWContourPolyDataMapper);
	m_pRenderer->AddActor(m_pNWContourActor);
	m_pNWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_pNWContourActor->VisibilityOff();

	// south-east
	m_pSEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pSEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pSEContourTubeFilter =  vtkTubeFilter::New();
	m_pSEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pSEContourPolyDataMapper = vtkPolyDataMapper::New();
	m_pSEContourActor = vtkActor::New();
	
	m_pSEContourX0ZPlaneClipPolyData->SetInput(m_pContourCutter->GetOutput());
	m_pSEContourX0ZPlaneClipPolyData->SetClipFunction(m_pX0ZSPlane);
	m_pSEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pSEContourY0ZPlaneClipPolyData->SetInput(m_pSEContourX0ZPlaneClipPolyData->GetOutput());
	m_pSEContourY0ZPlaneClipPolyData->SetClipFunction(m_pY0ZEPlane);
	m_pSEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pSEContourTransformPolyDataFilter->SetInput(m_pSEContourY0ZPlaneClipPolyData->GetOutput());

	m_pSEContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_pSEContourTubeFilter->SetNumberOfSides(12);
	m_pSEContourTubeFilter->SetInput(m_pSEContourTransformPolyDataFilter->GetOutput());

	m_pSEContourPolyDataMapper->SetInput(m_pSEContourTubeFilter->GetOutput());
	m_pSEContourActor->SetMapper(m_pSEContourPolyDataMapper);
	m_pRenderer->AddActor(m_pSEContourActor);
	m_pSEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_pSEContourActor->VisibilityOff();

	// south-west
	m_pSWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pSWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_pSWContourTubeFilter =  vtkTubeFilter::New();
	m_pSWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_pSWContourPolyDataMapper = vtkPolyDataMapper::New();
	m_pSWContourActor = vtkActor::New();
	
	m_pSWContourX0ZPlaneClipPolyData->SetInput(m_pContourCutter->GetOutput());
	m_pSWContourX0ZPlaneClipPolyData->SetClipFunction(m_pX0ZSPlane);
	m_pSWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pSWContourY0ZPlaneClipPolyData->SetInput(m_pSWContourX0ZPlaneClipPolyData->GetOutput());
	m_pSWContourY0ZPlaneClipPolyData->SetClipFunction(m_pY0ZWPlane);
	m_pSWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_pSWContourTransformPolyDataFilter->SetInput(m_pSWContourY0ZPlaneClipPolyData->GetOutput());

	m_pSWContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_pSWContourTubeFilter->SetNumberOfSides(12);
	m_pSWContourTubeFilter->SetInput(m_pSWContourTransformPolyDataFilter->GetOutput());

	m_pSWContourPolyDataMapper->SetInput(m_pSWContourTubeFilter->GetOutput());
	m_pSWContourActor->SetMapper(m_pSWContourPolyDataMapper);
	m_pRenderer->AddActor(m_pSWContourActor);
	m_pSWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_pSWContourActor->VisibilityOff();
	
	// display information
	m_pTextMapperX = vtkTextMapper::New();
	m_pScaledTextActorX = vtkScaledTextActor::New();
	m_pScaledTextActorX->SetMapper(m_pTextMapperX);
	m_pScaledTextActorX->VisibilityOff();
	m_pScaledTextActorX->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	m_pRenderer->AddActor2D(m_pScaledTextActorX);
	
	m_pTextMapperY = vtkTextMapper::New();
	m_pScaledTextActorY = vtkScaledTextActor::New();
	m_pScaledTextActorY->SetMapper(m_pTextMapperY);
	m_pScaledTextActorY->VisibilityOff();
	m_pScaledTextActorY->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	m_pRenderer->AddActor2D(m_pScaledTextActorY);
}




//----------------------------------------------------------------------------
mmoMMLModelView::~mmoMMLModelView()
//----------------------------------------------------------------------------
{
}





//----------------------------------------------------------------------------
void mmoMMLModelView::FindUnitVectorsAndLengthsOfLandmarkLines()
//----------------------------------------------------------------------------
{
	int i;
	double l1[3];
	double l2[3];
	double l3[3];
	this->GetLandmark1OfPatient(l1);
	this->GetLandmark2OfPatient(l2);
	this->GetLandmark3OfPatient(l3);

	// vector l1 - l2
	for(i = 0; i < 3; i++)
		m_dunitvector12[i] = l1[i] - l2[i];

	// length of vector l1 - l2
	m_dlength12 = sqrt(pow(m_dunitvector12[0], 2.0) + pow(m_dunitvector12[1], 2.0) + pow(m_dunitvector12[2], 2.0));

	// unit vector along l1 - l2
	for(i = 0; i < 3; i++)
		m_dunitvector12[i] = m_dunitvector12[i] / m_dlength12;

	// vector l2 - l3
	for(i = 0; i < 3; i++)
		m_dunitvector23[i] = l2[i] - l3[i];

	// length of vector l2 - l3
	m_dlength23 = sqrt(pow(m_dunitvector23[0], 2.0) + pow(m_dunitvector23[1], 2.0) + pow(m_dunitvector23[2], 2.0));

	// unit vector along l2 - l3
	for(i = 0; i < 3; i++)
		m_dunitvector23[i] = m_dunitvector23[i] / m_dlength23;

	// overall length
	m_doveralllength = m_dlength12 + m_dlength23;
}

//----------------------------------------------------------------------------
bool mmoMMLModelView::MapAtlasToPatient()
//----------------------------------------------------------------------------
{
	// insertions
	double a1[3];
	double a2[3];
	double a3[3];
	double a4[3];
	double p1[3];
	double p2[3];
	double p3[3];
	double p4[3];
	GetLandmark1OfAtlas(a1);
	GetLandmark2OfAtlas(a2);
	GetLandmark3OfAtlas(a3);
	GetLandmark4OfAtlas(a4);
	GetLandmark1OfPatient(p1);
	GetLandmark2OfPatient(p2);
	GetLandmark3OfPatient(p3);
	GetLandmark4OfPatient(p4);

	// transformation matrix 1
	vtkMatrix4x4 *transm1 = vtkMatrix4x4::New();
	transm1->Identity();
	transm1->SetElement(0, 3, p1[0]);
	transm1->SetElement(1, 3, p1[1]);
	transm1->SetElement(2, 3, p1[2]);

	// transformation matrix 2
	vtkMatrix4x4 *transm2 = vtkMatrix4x4::New();
	transm2->Identity();
	transm2->SetElement(0, 0, p2[0] - p1[0]);
	transm2->SetElement(0, 1, p3[0] - p1[0]);
	if (m_4Landmarks == 1)
		transm2->SetElement(0, 2, p4[0] - p1[0]);
	else
		transm2->SetElement(0, 2, (p2[1]-p1[1])*(p3[2]-p1[2])-(p2[2]-p1[2])*(p3[1]-p1[1]));
	transm2->SetElement(1, 0, p2[1] - p1[1]);
	transm2->SetElement(1, 1, p3[1] - p1[1]);
	if (m_4Landmarks == 1)
		transm2->SetElement(1, 2, p4[1] - p1[1]);
	else
		transm2->SetElement(1, 2, (p2[2]-p1[2])*(p3[0]-p1[0])-(p2[0]-p1[0])*(p3[2]-p1[2]));
	transm2->SetElement(2, 0, p2[2] - p1[2]);
	transm2->SetElement(2, 1, p3[2] - p1[2]);
	if (m_4Landmarks == 1)
		transm2->SetElement(2, 2, p4[2] - p1[2]);
	else
		transm2->SetElement(2, 2, (p2[0]-p1[0])*(p3[1]-p1[1])-(p2[1]-p1[1])*(p3[0]-p1[0]));

	// transformation matrix 3
	vtkMatrix4x4 *transm3 = vtkMatrix4x4::New();
	transm3->Identity();
	transm3->SetElement(0, 0, a2[0] - a1[0]);
	transm3->SetElement(0, 1, a3[0] - a1[0]);
	if (m_4Landmarks == 1)
		transm3->SetElement(0, 2, a4[0] - a1[0]);
	else
		transm3->SetElement(0, 2, (a2[1]-a1[1])*(a3[2]-a1[2])-(a2[2]-a1[2])*(a3[1]-a1[1]));
	transm3->SetElement(1, 0, a2[1] - a1[1]);
	transm3->SetElement(1, 1, a3[1] - a1[1]);
	if (m_4Landmarks == 1)
		transm3->SetElement(1, 2, a4[1] - a1[1]);
	else
		transm3->SetElement(1, 2, (a2[2]-a1[2])*(a3[0]-a1[0])-(a2[0]-a1[0])*(a3[2]-a1[2]));
	transm3->SetElement(2, 0, a2[2] - a1[2]);
	transm3->SetElement(2, 1, a3[2] - a1[2]);
	if (m_4Landmarks == 1)
		transm3->SetElement(2, 2, a4[2] - a1[2]);
	else
		transm3->SetElement(2, 2, (a2[0]-a1[0])*(a3[1]-a1[1])-(a2[1]-a1[1])*(a3[0]-a1[0]));

	// inverse transformation matrix 3
	vtkMatrix4x4 *inversetransm3 = vtkMatrix4x4::New();
	inversetransm3->Identity();
	inversetransm3->Invert(transm3, inversetransm3);

	// transformation matrix 4
	vtkMatrix4x4 *transm4 = vtkMatrix4x4::New();
	transm4->Identity();
	transm4->SetElement(0, 3, -1.0 * a1[0]);
	transm4->SetElement(1, 3, -1.0 * a1[1]);
	transm4->SetElement(2, 3, -1.0 * a1[2]);

	// final tranformation matrix
	vtkMatrix4x4 *finalm = vtkMatrix4x4::New();
	finalm->Identity();
	finalm = MultiplyMatrix4x4(transm1, finalm); //
	finalm = MultiplyMatrix4x4(transm2, finalm); //
	finalm = MultiplyMatrix4x4(inversetransm3, finalm); //
	finalm = MultiplyMatrix4x4(transm4, finalm); //

	double newa1[3];
	double newa2[3];
	double newa3[3];
	double newa4[3];

	// transform insertions (tests)
	vtkTransform *transf = vtkTransform::New();
	transf->SetMatrix(finalm);
	transf->TransformPoint(a1, newa1); // landmark 1
	transf->TransformPoint(a2, newa2); // landmark 2
	transf->TransformPoint(a3, newa3); // landmark 3
	if (m_4Landmarks == 1)
		transf->TransformPoint(a4, newa4); // landmark 4
	transf->Delete();

	double diff1[3];
	double diff2[3];
	double diff3[3];
	double diff4[3];
	for(int i = 0; i < 3; i++)
	{
		diff1[i] = p1[i] - newa1[i];
		diff2[i] = p2[i] - newa2[i];
		diff3[i] = p3[i] - newa3[i];
		if (m_4Landmarks == 1)
			diff4[i] = p4[i] - newa4[i];
	}
	assert(sqrt(pow(diff1[0], 2.0) + pow(diff1[1], 2.0) + pow(diff1[2], 2.0)) < 0.001);
	assert(sqrt(pow(diff2[0], 2.0) + pow(diff2[1], 2.0) + pow(diff2[2], 2.0)) < 0.001);
	assert(sqrt(pow(diff3[0], 2.0) + pow(diff3[1], 2.0) + pow(diff3[2], 2.0)) < 0.001);
	if (m_4Landmarks == 1)
		assert(sqrt(pow(diff4[0], 2.0) + pow(diff4[1], 2.0) + pow(diff4[2], 2.0)) < 0.001);

	// transform muscle
	m_pMuscleTransform1->SetMatrix(finalm);

	// execute
	m_pMuscleTransform1PolyDataFilter->SetTransform(this->m_pMuscleTransform1);

	// clean up
	finalm->Delete();
	
	return 1;
}

//----------------------------------------------------------------------------
bool mmoMMLModelView::MakeActionLineZAxis()
//----------------------------------------------------------------------------
{
	// transform 2
	// transform coordinates into a coordinate system, in which z-axis
	// is aligned with muscle line axis as defined by insertion points
	// in patient, and origin being the middle of this muscle line axis.
	
	int i;

	// insertions
	double p1[3];
	double p2[3];
	GetLandmark1OfPatient(p1); // high
	GetLandmark2OfPatient(p2); // low

	// middle
	double m[3];
	for(i = 0; i < 3; i++)
		m[i] = p1[i] + (p2[i] - p1[i]) / 2.0;

	// unit vector along muscle axis
	double u[3];
	for(i = 0; i < 3; i++)
		u[i] = p1[i] - p2[i];
	double n = sqrt(pow(u[0], 2.0) + pow(u[1], 2.0) + pow(u[2], 2.0));
	for(i = 0; i < 3; i++)
		u[i] = u[i] / n;
	
	// projection of unit vector along muscle axis on y0z plane
	double d;
	d = sqrt(pow(u[1], 2.0) + pow(u[2], 2.0));

	// translation
	vtkMatrix4x4 *transm = vtkMatrix4x4::New();
	transm->Identity();
	for(i = 0; i < 3; i++)
		transm->SetElement(i, 3, -1.0 * m[i]);

	// rotation around x
	// rotation around y
	vtkMatrix4x4 *rotaxm = vtkMatrix4x4::New();
	vtkMatrix4x4 *rotaym = vtkMatrix4x4::New();
	if (d  > 0.0)
	{
		rotaxm->Identity();
		rotaxm->SetElement(1, 1, u[2] / d);
		rotaxm->SetElement(2, 1, u[1] / d);
		rotaxm->SetElement(1, 2, -1.0 * u[1] / d);
		rotaxm->SetElement(2, 2, u[2] / d);

		rotaym->Identity();
		rotaym->SetElement(0, 0, d);
		rotaym->SetElement(2, 0, u[0]);
		rotaym->SetElement(0, 2, -1.0 * u[0]);
		rotaym->SetElement(2, 2, d);
	}
	else
	{
		rotaym->Identity();
		rotaym->SetElement(0, 0, u[2]);
		rotaym->SetElement(2, 0, -1.0 * u[0]);
		rotaym->SetElement(0, 2, u[0]);
		rotaym->SetElement(2, 2, u[2]);
	}

	//// z = 0 plane reflection
	//vtkMatrix4x4 *reflzm = vtkMatrix4x4::New();
	//reflzm->Identity();
	//reflzm->SetElement(2, 2, -1.0);

	double factorX, factorY;
	GetXYScalingFactorsOfMuscle(&factorX, &factorY);
	
	// scaling
	vtkMatrix4x4 *scalem = vtkMatrix4x4::New();
	scalem->Identity();
	scalem->SetElement(0, 0, factorX);
	scalem->SetElement(1, 1, factorY);
	scalem->SetElement(2, 2, 1.0);

	// synthetic slices transform
	m_pslicesm->Identity();
	m_pslicesm = MultiplyMatrix4x4(rotaym, m_pslicesm); // rotation y
	m_pslicesm = MultiplyMatrix4x4(rotaxm, m_pslicesm); // rotation x

	// tranformation matrix
	m_pfinalm->Identity();
	m_pfinalm = MultiplyMatrix4x4(scalem, m_pfinalm); // 4. scaling
	m_pfinalm = MultiplyMatrix4x4(rotaym, m_pfinalm); // 3. rotation y
	m_pfinalm = MultiplyMatrix4x4(rotaxm, m_pfinalm); // 2. rotation x
	m_pfinalm = MultiplyMatrix4x4(transm, m_pfinalm); // 1. translation
	
	//vtkMatrix4x4 *finalm = vtkMatrix4x4::New();
	//finalm->Identity();
	//finalm = MultiplyMatrix4x4(scalem, finalm); // 4. scaling
	//finalm = MultiplyMatrix4x4(rotaym, finalm); // 3. rotation y
	//finalm = MultiplyMatrix4x4(rotaxm, finalm); // 2. rotation x
	//finalm = MultiplyMatrix4x4(transm, finalm); // 1. translation


	// transform muscle
	m_pMuscleTransform2->SetMatrix(m_pfinalm);

	// execute
	m_pMuscleTransform2PolyDataFilter->SetTransform(this->m_pMuscleTransform2);

	double newp1[3];
	double newp2[3];

	// transform insertions (tests)
	vtkTransform *transf = vtkTransform::New();
	transf->SetMatrix(m_pfinalm);
	transf->TransformPoint(p1, newp1); // landmark 1
	transf->TransformPoint(p2, newp2); // landmark 2
	transf->Delete();

	//
	assert((newp1[0] < 0.0001) && (newp1[1] < 0.0001)); // x, y are zero
	assert((newp2[0] < 0.0001) && (newp2[0] < 0.0001)); // x, y are zero
	assert((newp1[2] + newp2[2]) < 0.0001); // z are opposite, so sum to zero
	
	// clean up
	rotaym->Delete();
	rotaxm->Delete();
	transm->Delete();

	//
	m_pMuscleTransform2PolyDataFilter->Update();

	return 1;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::FindSizeAndResolutionOfSyntheticScans()
//----------------------------------------------------------------------------
{
	// must be called prior to SetUpSyntheticScans
	// to define compute scans size and resolution
	float Factor = 1.75; // 75% extra

	switch (m_nTypeOfMuscles)
	{
		case 1: // slicing axis is single line
				// bounds
				double bounds[6];
				m_pMuscleTransform2PolyDataFilter->GetOutput()->GetBounds(bounds);

				// size
				if (fabs(bounds[0]) > fabs(bounds[1]))
					m_nSyntheticScansXSize = Factor * 2.0 * fabs(bounds[0]);
				else
					m_nSyntheticScansXSize = Factor * 2.0 * fabs(bounds[1]);

				if (fabs(bounds[2]) > fabs(bounds[3]))
					m_nSyntheticScansYSize = Factor * 2.0 * fabs(bounds[2]);
				else
					m_nSyntheticScansYSize = Factor * 2.0 * fabs(bounds[3]);
				break;

		case 2: // slicing axis is two lines
				m_nSyntheticScansXSize = 200.0;
				m_nSyntheticScansYSize = 200.0;
				break;
	}

	//
	// resolution
	assert(m_SyntheticScansGrain > 0 && m_SyntheticScansGrain < 6);

	// set
	if (m_SyntheticScansGrain == 1) // x 1/3
	{
		m_nSyntheticScansXResolution = 0.3 * m_nSyntheticScansXSize;
		m_nSyntheticScansYResolution = 0.3 * m_nSyntheticScansYSize;
	}
	else
	if (m_SyntheticScansGrain == 2) // x 1/2
	{
		m_nSyntheticScansXResolution = 0.5 * m_nSyntheticScansXSize;
		m_nSyntheticScansYResolution = 0.5 * m_nSyntheticScansYSize;
	}
	else
 if (m_SyntheticScansGrain == 3) // x1
	{
		m_nSyntheticScansXResolution = 1.0 * m_nSyntheticScansXSize;
		m_nSyntheticScansYResolution = 1.0 * m_nSyntheticScansYSize;
	}
	else
	if (m_SyntheticScansGrain == 4) // x2
	{
		m_nSyntheticScansXResolution = 2.0 * m_nSyntheticScansXSize;
		m_nSyntheticScansYResolution = 2.0 * m_nSyntheticScansYSize;
	}
	else // x3
	{
		m_nSyntheticScansXResolution = 3.0 * m_nSyntheticScansXSize;
		m_nSyntheticScansYResolution = 3.0 * m_nSyntheticScansYSize;
	}
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetUpSyntheticScans()
//----------------------------------------------------------------------------
{
	/*
	// grid parameters
	int dims[3];
	scans->GetDimensions(dims); // (249, 279, 176)
	double bounds[6];
	scans->GetBounds(bounds); // (0, 184.063), (100.290, 306.618), (-512.50, -34.50) 
	float center[3];
	scans->GetCenter(center); // (92.0313, 203.454, -273.500)
	int extent[6];
	scans->GetExtent(extent); // (0, 248), (0, 278), (0, 175)
	float length;
	length = scans->GetLength(); // 552.209
	float scalarrange[2];
	scans->GetScalarRange(scalarrange); // (-3024, 1892)
	*/

	//
	int n;
	n = this->GetTotalNumberOfSyntheticScans();

	// initialise scaling flag stack
	ScalingFlagStack->SetNumberOfTuples(n);
	float a = 0;
	for(int ii = 0; ii < n; ii++)
		ScalingFlagStack->SetTuple(ii, &a);

	// allocate planes
	m_pSyntheticScansPlaneSource = new vtkPlaneSource* [n] ;
	assert(!(m_pSyntheticScansPlaneSource == NULL));

	// allocate plane transforms
	m_pSyntheticScansPlaneSourceTransform = new vtkTransform* [n];
	assert(!(m_pSyntheticScansPlaneSourceTransform == NULL));

	//
	m_pSyntheticScansPlaneSourceTransformPolyDataFilter = new vtkTransformPolyDataFilter* [n];
	assert(!(m_pSyntheticScansPlaneSourceTransformPolyDataFilter == NULL));

	// allocate probes
	m_pSyntheticScansProbeFilter = new vtkProbeFilter* [n];
	assert(!(m_pSyntheticScansProbeFilter == NULL));

	// allocate mappers
	m_pSyntheticScansPolyDataMapper = new vtkPolyDataMapper* [n];
	assert(!(m_pSyntheticScansPolyDataMapper == NULL));

	// allocate actors
	m_pSyntheticScansActor = new vtkActor* [n];
	assert(!(m_pSyntheticScansActor == NULL));

	// allocate actor transforms
	m_pSyntheticScansActorTransform = new vtkTransform* [n];
	assert(!(m_pSyntheticScansActorTransform == NULL));

	// initial scalar value min/max
	m_SyntheticScansMinScalarValue = 100000;
	m_SyntheticScansMaxScalarValue = -100000;

	// scalar value holder
	double scalars[2];

	//
	for(int i = 0; i < n; i++)
	{
		// allocate objects
		m_pSyntheticScansPlaneSource[i] = vtkPlaneSource::New();
		assert(!(m_pSyntheticScansPlaneSource[i] == NULL));

		//
		m_pSyntheticScansPlaneSourceTransform[i] = vtkTransform::New();
		assert(!(m_pSyntheticScansPlaneSourceTransform[i] == NULL));

		//
		m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i] = vtkTransformPolyDataFilter::New();
		assert(!(m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i] == NULL));
		
		//
		m_pSyntheticScansProbeFilter[i] = vtkProbeFilter::New();
		assert(!(m_pSyntheticScansProbeFilter[i] == NULL));

		//
		m_pSyntheticScansPolyDataMapper[i] = vtkPolyDataMapper::New();
		assert(!(m_pSyntheticScansPolyDataMapper[i] == NULL));

		//
		m_pSyntheticScansActor[i] = vtkActor::New();
		assert(!(m_pSyntheticScansActor[i] == NULL));

		//
		m_pSyntheticScansActorTransform[i] = vtkTransform::New();
		assert(!(m_pSyntheticScansActorTransform[i] == NULL));

		// plane source resolution
		m_pSyntheticScansPlaneSource[i]->SetResolution(m_nSyntheticScansXResolution, m_nSyntheticScansYResolution);
		
		// plane source transformation matrix
		m_pSyntheticScansPlaneSourceTransform[i]->SetMatrix(this->GetPlaneSourceTransformOfSyntheticScans(i));
		m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i]->SetInput(m_pSyntheticScansPlaneSource[i]->GetOutput());
		m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i]->SetTransform(m_pSyntheticScansPlaneSourceTransform[i]);
		
		// actor transformation matrix
		m_pSyntheticScansActorTransform[i]->SetMatrix(this->GetActorTransformOfSyntheticScans(i));
		m_pSyntheticScansActor[i]->SetUserTransform(m_pSyntheticScansActorTransform[i]);
		m_pSyntheticScansActor[i]->SetMapper(m_pSyntheticScansPolyDataMapper[i]);
		m_pSyntheticScansActor[i]->VisibilityOff();
		m_pRenderer->AddActor(m_pSyntheticScansActor[i]);

		// probe
		m_pSyntheticScansProbeFilter[i]->SetInput(m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i]->GetOutput());
		m_pSyntheticScansProbeFilter[i]->SetSource(scans);
		m_pSyntheticScansProbeFilter[i]->Update();

		// asjust min/max scalar values
		m_pSyntheticScansProbeFilter[i]->GetPolyDataOutput()->GetScalarRange(scalars);
		if (scalars[0] < m_SyntheticScansMinScalarValue)
			m_SyntheticScansMinScalarValue = scalars[0];
		if (scalars[1] > m_SyntheticScansMaxScalarValue)
			m_SyntheticScansMaxScalarValue = scalars[1];

		// probe mapper 
		m_pSyntheticScansPolyDataMapper[i]->SetInput(m_pSyntheticScansProbeFilter[i]->GetPolyDataOutput());
		
		// i-th scan: handles large datasets
		// at the expense of slower rendering
		/*
		m_pSyntheticScansPolyDataMapper[i]->ImmediateModeRenderingOn();
		*/

		// clean up
		m_pSyntheticScansActorTransform[i]->Delete();
		m_pSyntheticScansPolyDataMapper[i]->Delete();
		m_pSyntheticScansProbeFilter[i]->Delete();
		m_pSyntheticScansPlaneSourceTransformPolyDataFilter[i]->Delete();
		m_pSyntheticScansPlaneSourceTransform[i]->Delete();
		m_pSyntheticScansPlaneSource[i]->Delete();
	}

	// window
	m_pWindow = (m_SyntheticScansMaxScalarValue - m_SyntheticScansMinScalarValue);

	// level
	m_pLevel = 0.5 * (m_SyntheticScansMinScalarValue + m_SyntheticScansMaxScalarValue);

	// lut
	m_pSyntheticScansWindowLevelLookupTable->SetTableRange(m_SyntheticScansMinScalarValue, m_SyntheticScansMaxScalarValue);
	m_pSyntheticScansWindowLevelLookupTable->SetHueRange(0.0, 0.0);
	m_pSyntheticScansWindowLevelLookupTable->SetSaturationRange(0.0, 0.0);
	m_pSyntheticScansWindowLevelLookupTable->SetValueRange(0.0, 1.0);
	m_pSyntheticScansWindowLevelLookupTable->SetNumberOfColors(1024);
	m_pSyntheticScansWindowLevelLookupTable->SetWindow(m_pWindow);
	m_pSyntheticScansWindowLevelLookupTable->SetLevel(m_pLevel);
	m_pSyntheticScansWindowLevelLookupTable->Build();
	
	for(int j = 0; j < n; j++)
	{
		m_pSyntheticScansPolyDataMapper[j]->SetLookupTable(m_pSyntheticScansWindowLevelLookupTable);
		m_pSyntheticScansPolyDataMapper[j]->UseLookupTableScalarRangeOn();
		m_pSyntheticScansPolyDataMapper[j]->SetColorModeToMapScalars();
	}

	// delete objects
	delete m_pSyntheticScansActorTransform;
	delete m_pSyntheticScansProbeFilter;
	delete m_pSyntheticScansPlaneSourceTransformPolyDataFilter;
	delete m_pSyntheticScansPlaneSourceTransform;
	delete m_pSyntheticScansPlaneSource;
	delete m_pSyntheticScansPolyDataMapper;

	// set to scan 0 display
	m_nSyntheticScansCurrentId = 0; // current id
	m_pSyntheticScansActor[0]->VisibilityOn(); // actor on
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetPlaneSourceOriginOfSyntheticScans(int s, double p[])
//----------------------------------------------------------------------------
{
	int i;
	int n;
	double currentlength;
	double l1[3];
	double l2[3];
	double l3[3];

	n = this->GetTotalNumberOfSyntheticScans();
	this->GetLandmark1OfPatient(l1);
	this->GetLandmark2OfPatient(l2);
	this->GetLandmark3OfPatient(l3);

	switch (m_nTypeOfMuscles)
	{
		case 1: // slicing axis is single line
				// start from landmark 2 (low)
				// ending at landmark 1 (high)

				// current length
				currentlength = m_dlength12 / (n - 1) * s;

				// origin
				for(i = 0; i < 3; i++)
					p[i] = l2[i] + currentlength * m_dunitvector12[i];
				break;

		case 2: // slicing axis is double line
				// start from landmark 3 (low)
				// ending at landmark 1 (high)
				
				// current length
				currentlength = m_doveralllength / (n - 1) * s;

				// origin
				for(i = 0; i < 3; i++)
				{
					if (currentlength < m_dlength23)
					{
						p[i] = l3[i] + currentlength * m_dunitvector23[i];
					}
					else
					{
						p[i] = l2[i] + (currentlength - m_dlength23) * m_dunitvector12[i];
					}
				}
				
				break;
	}
}

//----------------------------------------------------------------------------
vtkMatrix4x4* mmoMMLModelView::GetPlaneSourceTransformOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	//
	int i;
	int n;

	//
	double p[3];

	//
	double currentlength;
	double normal[3];
	double d;
	
	//
	vtkMatrix4x4 *rotaxm = vtkMatrix4x4::New();
	vtkMatrix4x4 *rotaym = vtkMatrix4x4::New();
	vtkMatrix4x4 *scalem = vtkMatrix4x4::New();
	vtkMatrix4x4 *transm = vtkMatrix4x4::New();
	vtkMatrix4x4 *inversem_pslicesm = vtkMatrix4x4::New();
	vtkMatrix4x4 *alignm =  vtkMatrix4x4::New();
	vtkMatrix4x4 *inversealignm =  vtkMatrix4x4::New();
	vtkMatrix4x4 *finalm =  vtkMatrix4x4::New();

	switch (m_nTypeOfMuscles)
	{
		case 1: // slicing axis is single line
				
				// s-th synthetic scan: plane source size
				// (as a scaling transformation)
				scalem->Identity();
				scalem->SetElement(0, 0, m_nSyntheticScansXSize);
				scalem->SetElement(1, 1, m_nSyntheticScansYSize);

				// s-th synthetic scan: plane source origin
				// (as a translation transformation)
				this->GetPlaneSourceOriginOfSyntheticScans(s, p); 
				transm->Identity();
				transm->SetElement(0, 3, p[0]); // x
				transm->SetElement(1, 3, p[1]); // y
				transm->SetElement(2, 3, p[2]); // z

				//
				inversem_pslicesm->Identity();
				inversem_pslicesm->Invert(m_pslicesm, inversem_pslicesm);

				//  s-th synthetic scan: plane source transformation matrix
				finalm->Identity();
				finalm = MultiplyMatrix4x4(transm, finalm); // 3. translation
				finalm = MultiplyMatrix4x4(inversem_pslicesm, finalm); // 2. alignement: inverse (rotx * roty)
				finalm = MultiplyMatrix4x4(scalem, finalm); // 1. scaling
				break;

		case 2: // slicing axis is double line
				// s-th synthetic scan: plane source size
				// (as a scaling transformation)
				scalem->Identity();
				scalem->SetElement(0, 0, m_nSyntheticScansXSize);
				scalem->SetElement(1, 1, m_nSyntheticScansYSize);

				// s-th synthetic scan: plane source origin
				// (as a translation transformation)
				this->GetPlaneSourceOriginOfSyntheticScans(s, p); 
				transm->Identity();
				transm->SetElement(0, 3, p[0]); // x
				transm->SetElement(1, 3, p[1]); // y
				transm->SetElement(2, 3, p[2]); // z

				//
				n = this->GetTotalNumberOfSyntheticScans();

				// s-synthetic scan: plane source normal
				// current length
				currentlength = m_doveralllength / (n - 1) * s;

				// current normal
				for(i = 0; i < 3; i++)
				{
					normal[i] = (1.0 / m_doveralllength) * (currentlength * m_dunitvector12[i] + 
														   (m_doveralllength - currentlength)  * m_dunitvector23[i]);
				}

				// align transformation
				// project on y0z plane
				d = sqrt(pow(normal[1], 2.0) + pow(normal[2], 2.0));
				
				// rotate around x, then rotate around y
				if (d  > 0.0)
				{
					rotaxm->Identity();
					rotaxm->SetElement(1, 1, normal[2] / d);
					rotaxm->SetElement(2, 1, normal[1] / d);
					rotaxm->SetElement(1, 2, -1.0 * normal[1] / d);
					rotaxm->SetElement(2, 2, normal[2] / d);

					rotaym->Identity();
					rotaym->SetElement(0, 0, d);
					rotaym->SetElement(2, 0, normal[0]);
					rotaym->SetElement(0, 2, -1.0 * normal[0]);
					rotaym->SetElement(2, 2, d);
				}
				else
				{
					rotaym->Identity();
					rotaym->SetElement(0, 0, normal[2]);
					rotaym->SetElement(2, 0, -1.0 * normal[0]);
					rotaym->SetElement(0, 2, normal[0]);
					rotaym->SetElement(2, 2, normal[2]);
				}

				// inverse align matrix
				inversealignm->Identity();
				inversealignm = MultiplyMatrix4x4(rotaym, inversealignm);
				inversealignm = MultiplyMatrix4x4(rotaxm, inversealignm);

				// align matrix
				alignm->Invert(inversealignm, alignm);
	
				//  s-th synthetic scan: plane source transformation matrix
				finalm->Identity();
				finalm = MultiplyMatrix4x4(transm, finalm); // 3. translation
				finalm = MultiplyMatrix4x4(alignm, finalm); // 2. align
				finalm = MultiplyMatrix4x4(scalem, finalm); // 1. scaling
				//
				break;
	}

	// clean up
	scalem->Delete();
	transm->Delete();
	inversem_pslicesm->Delete();
	alignm->Delete();
	inversealignm->Delete();
	rotaxm->Delete();
	rotaym->Delete();

	return finalm;
}

//----------------------------------------------------------------------------
vtkMatrix4x4* mmoMMLModelView::GetActorTransformOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	//
	double p[3];
	
	//
	vtkMatrix4x4 *transm = vtkMatrix4x4::New();
	vtkMatrix4x4 *inversem_pslicesm = vtkMatrix4x4::New();
	vtkMatrix4x4 *finalm =  vtkMatrix4x4::New();

	switch (m_nTypeOfMuscles)
	{
		case 1: // slicing axis is single line
				
				// s-th synthetic scan: plane source origin
				// (as a translation transformation)
				this->GetPlaneSourceOriginOfSyntheticScans(s, p); 
				transm->Identity();
				transm->SetElement(0, 3, p[0]); // x
				transm->SetElement(1, 3, p[1]); // y
				transm->SetElement(2, 3, p[2]); // z

				//
				inversem_pslicesm->Identity();
				inversem_pslicesm->Invert(m_pslicesm, inversem_pslicesm);
				
				finalm->Identity();
				finalm = MultiplyMatrix4x4(transm, finalm); // 2. translation
				finalm = MultiplyMatrix4x4(inversem_pslicesm, finalm); // 1. inverse (rotx * roty)
				finalm->Invert(finalm, finalm); // invert
				break;

		case 2: // slicing axis is double line
				// the opposite of plane transform to put it perpendicular
				// to the z axis, just for viewing.
				//
				break;
	}

	// clean up
	transm->Delete();
	inversem_pslicesm->Delete();

	return finalm;
}

//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetContourActor()
//----------------------------------------------------------------------------
{
	return m_pContourActor;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetPHSpline()
//----------------------------------------------------------------------------
{
	return m_pCenterHorizontalOffsetSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetPVSpline()
//----------------------------------------------------------------------------
{
	return m_pCenterVerticalOffsetSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetTHSpline()
//----------------------------------------------------------------------------
{
	return m_pHorizontalTranslationSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetTVSpline()
//----------------------------------------------------------------------------
{
	return m_pVerticalTranslationSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetRASpline()
//----------------------------------------------------------------------------
{
	return m_pTwistSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetSNSpline()
//----------------------------------------------------------------------------
{
	return m_pNorthScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetSSSpline()
//----------------------------------------------------------------------------
{
	return m_pSouthScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetSESpline()
//----------------------------------------------------------------------------
{
	return m_pEastScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* mmoMMLModelView::GetSWSpline()
//----------------------------------------------------------------------------
{
	return m_pWestScalingSpline;
}
//----------------------------------------------------------------------------
double mmoMMLModelView::GetZOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	int n; // number of scans
	double p1[3]; // low insertion
	double p2[3]; // high insertion

	n = GetTotalNumberOfSyntheticScans();
	GetLandmark1OfPatient(p1);
	GetLandmark2OfPatient(p2);

	double length;
	vtkMath *pMath = vtkMath::New();
	length = sqrt(pMath->Distance2BetweenPoints(p1, p2));
	pMath->Delete();

	double start; // first z (slice id = 0)
	start = -1.0 * length / 2.0;
	
	return start + length / (n - 1) * s;
}
//----------------------------------------------------------------------------
double mmoMMLModelView::GetCurrentZOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return GetZOfSyntheticScans(GetCurrentIdOfSyntheticScans());
}
//----------------------------------------------------------------------------
void mmoMMLModelView::Render()
//----------------------------------------------------------------------------
{
	m_pRenderWindow->Render();
}
//----------------------------------------------------------------------------
vtkTextMapper* mmoMMLModelView::GetTextMapper2()
//----------------------------------------------------------------------------
{
	return m_pTextMapperX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* mmoMMLModelView::GetScaledTextActor2()
//----------------------------------------------------------------------------
{
	return m_pScaledTextActorX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* mmoMMLModelView::GetScaledTextActor1()
//----------------------------------------------------------------------------
{
	return m_pScaledTextActorY;
}
//----------------------------------------------------------------------------
vtkTextMapper* mmoMMLModelView::GetTextMapper1()
//----------------------------------------------------------------------------
{
	return m_pTextMapperY;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetNEContourActor()
//----------------------------------------------------------------------------
{
	return m_pNEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetNWContourActor()
//----------------------------------------------------------------------------
{
	return m_pNWContourActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetSEContourActor()
//----------------------------------------------------------------------------
{
	return m_pSEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetSWContourActor()
//----------------------------------------------------------------------------
{
	return m_pSWContourActor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetNEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pNEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetNWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pNWContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetSEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pSEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetSWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pSWContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
/** m = 1, 2 for text actor 1,2 */
void mmoMMLModelView::SetText(int m, double n, int d, int s)
//----------------------------------------------------------------------------
{
	char text[10];
	
	char *buffer;
	int decimal, sign;
  
	double nfloor;

	// scaling display
	if (s == 1) // scaling flag
	{
		if (n >= 1.0)
		{
			if (n > 9.0) n = 9.0;
				strcpy(text, "X");
				buffer = _ecvt(n, 1, &decimal, &sign);
			strcat(text, buffer);
			strcat(text, ".0");
		}
		else
		{	
			n = 10*n;
			if (n > 9.0) n = 9.0;
				strcpy(text, "X0.");
				buffer = _ecvt(n, 1, &decimal, &sign);
			strcat(text, buffer);
			//strcat(text, ".0");
		}
		
	}
	// non-scaling display
	else
	{
		//remove decimals
		nfloor = floor(n);
		if (n < nfloor + 0.5)
			n = nfloor;
		else
			n = nfloor + 1.0;

		// 2 digit display
		if (d == 2) 
		{
			if (fabs(n) < 10.0)
			{
				buffer = _ecvt(n, 1, &decimal, &sign);

				// sign
				if (sign == 0)
					strcpy(text, "+");
				else
					strcpy(text, "-");
			
				// pad zero
				strcat(text, "0");
			}
			else
			if (fabs(n) < 100.0)
			{
				buffer = _ecvt(n, 2, &decimal, &sign);

				// sign
				if (sign == 0)
					strcpy(text, "+");
				else
					strcpy(text, "-");
			}
			else
				return;
			strcat(text, buffer);
		}
	}
	
	// 3 digit display
	if (d == 3) 
	{
		if (fabs(n) < 10)
		{
			buffer = _ecvt(n, 1, &decimal, &sign);

			// sign
			if (sign == 0)
				strcpy(text, "+");
			else
				strcpy(text, "-");
			
			// pad zero
			strcat(text, "00");
		}
		else
		if (fabs(n) < 100)
		{
			buffer = _ecvt(n, 2, &decimal, &sign);

			// sign
			if (sign == 0)
				strcpy(text, "+");
			else
				strcpy(text, "-");
			
			// pad zero
			strcat(text, "0");
		}
		else
		{
			buffer = _ecvt(n, 3, &decimal, &sign);

			// sign
			if (sign == 0)
				strcpy(text, "+");
			else
				strcpy(text, "-");
		}

		strcat(text, buffer);
	}

	// assign to text mapper
	if (m == 1)
		GetTextMapper1()->SetInput(text);
	else
	if (m == 2)
		GetTextMapper2()->SetInput(text);
}
//----------------------------------------------------------------------------
vtkCamera* mmoMMLModelView::GetActiveCamera()
//----------------------------------------------------------------------------
{
	return m_pRenderer->GetActiveCamera();
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SaveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	m_pCameraFocalPoint[0] = *fp;
	m_pCameraFocalPoint[1] = *(fp+1);
	m_pCameraFocalPoint[2] = *(fp+2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SaveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	m_pCameraPosition[0] = *cp;
	m_pCameraPosition[1] = *(cp+1);
	m_pCameraPosition[2] = *(cp+2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SaveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	m_pCameraClippingRange[0] = *cr;
	m_pCameraClippingRange[1] = *(cr+1);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SaveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	m_pCameraViewUp[0] = *vu;
	m_pCameraViewUp[1] = *(vu+1);
	m_pCameraViewUp[2] = *(vu+2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::RetrieveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	*fp = m_pCameraFocalPoint[0];
	*(fp+1) = m_pCameraFocalPoint[1];
	*(fp+2) = m_pCameraFocalPoint[2];
}
//----------------------------------------------------------------------------
void mmoMMLModelView::RetrieveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	*cp = m_pCameraPosition[0];
	*(cp+1) = m_pCameraPosition[1];
	*(cp+2) = m_pCameraPosition[2];
}
//----------------------------------------------------------------------------
void mmoMMLModelView::RetrieveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	*cr = m_pCameraClippingRange[0];
	*(cr+1) = m_pCameraClippingRange[1];
}
//----------------------------------------------------------------------------
void mmoMMLModelView::RetrieveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	*vu = m_pCameraViewUp[0];
	*(vu+1) = m_pCameraViewUp[1];
	*(vu+2) = m_pCameraViewUp[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::AddActor(vtkActor *a)
//----------------------------------------------------------------------------
{
	m_pRenderer->AddActor(a);
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetPositiveXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pPosXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetPositiveYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pPosYAxisActor;
}
vtkActor* mmoMMLModelView::GetPositiveZAxisActor()
//----------------------------------------------------------------------------
{
	return m_pPosZAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetNegativeXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pNegXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetNegativeYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pNegYAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetNegativeZAxisActor()
//----------------------------------------------------------------------------
{
	return m_pNegZAxisActor;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetPositiveLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_pPosXAxisLineSource->SetPoint1(p1);
	m_pPosXAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetNegativeLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_pNegYAxisLineSource->SetPoint1(p1);
	m_pNegYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetPositiveLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_pPosYAxisLineSource->SetPoint1(p1);
	m_pPosYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetNegativeLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_pNegXAxisLineSource->SetPoint1(p1);
	m_pNegXAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void mmoMMLModelView::WriteMatrix(char *pch, vtkMatrix4x4 *m)
//----------------------------------------------------------------------------
{
	int i, j;
	FILE *stream;
	char FileName[MAX_CHARS];
	
	// copy
	strcpy(FileName, pch);

	stream = fopen( FileName, "w" );

    if( stream == NULL )
		exit(0);
    else
    {
		for (i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				fprintf(stream, " [%d] [%d] %f\n", i, j, m->GetElement(i, j));
			}
			fprintf(stream, "\n");
		}	
	}
	fclose( stream );
}
//----------------------------------------------------------------------------
vtkLODActor* mmoMMLModelView::GetMuscleLODActor()
//----------------------------------------------------------------------------
{
	return m_pMuscleLODActor;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetTotalNumberOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansTotalNumber = n;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetResolutionOfSyntheticScans(int x, int y)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansXResolution = x;
	m_nSyntheticScansYResolution = y;
}
//----------------------------------------------------------------------------
int mmoMMLModelView::GetTotalNumberOfSyntheticScans()
//----------------------------------------------------------------------------
{
	assert(m_nSyntheticScansTotalNumber > 3); // at least 3 synthetic scans
	return m_nSyntheticScansTotalNumber;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetResolutionOfSyntheticScans(int *x, int *y)
//----------------------------------------------------------------------------
{
	*x = m_nSyntheticScansXResolution;
	*y = m_nSyntheticScansYResolution;
}

//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetActorOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	return m_pSyntheticScansActor[s];
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetCurrentIdOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansCurrentId = n;
}
//----------------------------------------------------------------------------
int mmoMMLModelView::GetCurrentIdOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_nSyntheticScansCurrentId;
}
//----------------------------------------------------------------------------
vtkWindowLevelLookupTable* mmoMMLModelView::GetWindowLevelLookupTableOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_pSyntheticScansWindowLevelLookupTable;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetSizeOfSyntheticScans(float x, float y)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansXSize = x;
	m_nSyntheticScansYSize = y;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::GetSizeOfSyntheticScans(float *x, float *y)
//----------------------------------------------------------------------------
{
	*x = m_nSyntheticScansXSize;
	*y = m_nSyntheticScansYSize;
}
//----------------------------------------------------------------------------
vtkMatrix4x4* mmoMMLModelView::MultiplyMatrix4x4(vtkMatrix4x4 *a, vtkMatrix4x4 *b)
//----------------------------------------------------------------------------
{
	//
	// matrix notation
	//
	// C0 C1 C2 C3 |
	// -------------
	// 00 10 20 30 | R0
	// 01 11 21 31 | R1
	// 02 12 22 32 | R2
	// 03 13 23 33 | R3
	//
	// ji element of product is irow
	// of a combined with j col of b

	vtkMatrix4x4* c = vtkMatrix4x4::New();

	for (int i = 0; i < 4; i++) // row i
	{
		for(int j = 0; j < 4; j++) // col j
		{
			c->SetElement(j, i, a->GetElement(0, i) * b->GetElement(j, 0) +
							    a->GetElement(1, i) * b->GetElement(j, 1) +
								a->GetElement(2, i) * b->GetElement(j, 2) +
								a->GetElement(3, i) * b->GetElement(j, 3));		
		}
	}
	

	return c;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetXYScalingFactorsOfMuscle(double x, double y)
//----------------------------------------------------------------------------
{
	m_flMuscleXScalingFactor = x;
	m_flMuscleYScalingFactor = y;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::GetXYScalingFactorsOfMuscle(double *x, double *y)
//----------------------------------------------------------------------------
{
	*x = m_flMuscleXScalingFactor;
	*y = m_flMuscleYScalingFactor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetContourCutterTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pContourCutterTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkPlane* mmoMMLModelView::GetContourPlane()
//----------------------------------------------------------------------------
{
	return m_pContourPlane;
}
//----------------------------------------------------------------------------
vtkTransform* mmoMMLModelView::GetContourCutterTransform()
//----------------------------------------------------------------------------
{
	return m_pContourCutterTransform;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetPositiveXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pPosXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetNegativeXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pNegXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetPositiveYAxisLineSource()
//----------------------------------------------------------------------------
{
		return m_pPosYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetNegativeYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pNegYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetPositiveXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pPosXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetNegativeXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pNegXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetPositiveYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pPosYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetNegativeYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pNegYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkRenderWindowInteractor* mmoMMLModelView::GetRenderWindowInteractor()
//----------------------------------------------------------------------------
{
	return m_pRenderWindowInteractor;
}
//----------------------------------------------------------------------------
vtkInteractorStyleImage* mmoMMLModelView::GetInteractorStyleImage()
//----------------------------------------------------------------------------
{
	return m_pInteractorStyleImage;
}
//----------------------------------------------------------------------------
vtkInteractorStyleTrackballCamera* mmoMMLModelView::GetInteractorStyleTrackballCamera()
//----------------------------------------------------------------------------
{
	return m_pInteractorStyleTrackballCamera;
}

//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetContourPositiveXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetContourPositiveYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetContourNegativeXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* mmoMMLModelView::GetContourNegativeYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetContourPositiveXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetContourPositiveYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetContourNegativeXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetContourNegativeYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetContourPositiveXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetContourPositiveYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetContourNegativeXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* mmoMMLModelView::GetContourNegativeYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisActor;
}
//----------------------------------------------------------------------------
float mmoMMLModelView::GetLowScalar()
//----------------------------------------------------------------------------
{
	return m_SyntheticScansMinScalarValue;
}
//----------------------------------------------------------------------------
float mmoMMLModelView::GetHighScalar()
//----------------------------------------------------------------------------
{
 	return m_SyntheticScansMaxScalarValue;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetTypeOfMuscles(int t)
//----------------------------------------------------------------------------
{
	m_nTypeOfMuscles = t;
}
//----------------------------------------------------------------------------
int mmoMMLModelView::GetTypeOfMuscles()
//----------------------------------------------------------------------------
{
	return m_nTypeOfMuscles;
}
//----------------------------------------------------------------------------
vtkPlane* mmoMMLModelView::GetCuttingPlaneNorth()
//----------------------------------------------------------------------------
{
	return m_pX0ZNPlane;
}
//----------------------------------------------------------------------------
vtkPlane* mmoMMLModelView::GetCuttingPlaneSouth()
//----------------------------------------------------------------------------
{
	return m_pX0ZSPlane;
}
//----------------------------------------------------------------------------
vtkPlane* mmoMMLModelView::GetCuttingPlaneEast()
//----------------------------------------------------------------------------
{
	return m_pY0ZEPlane;
}
//----------------------------------------------------------------------------
vtkPlane* mmoMMLModelView::GetCuttingPlaneWest()
//----------------------------------------------------------------------------
{
	return m_pY0ZWPlane;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::SetContourAxesLengthScale(float l)
{
	ContourAxesLengthScale = l;
}
//----------------------------------------------------------------------------
float mmoMMLModelView::GetContourAxesLengthScale()
//----------------------------------------------------------------------------
{
	return ContourAxesLengthScale;
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateContourCuttingPlane()
//----------------------------------------------------------------------------
{
	if (m_3dDisplay == 0) // standard display
	{
		// get z level
		double z = GetCurrentZOfSyntheticScans();

		GetContourPlane()->SetOrigin(0.0, 0.0, z);
		GetContourPlane()->SetNormal(0.0, 0.0, 1.0); // along z axis
		GetContourCutterTransform()->Identity();
		GetContourCutterTransform()->Translate(0.0, 0.0,-1.0 * z); // to put down to z = 0 plane
	}
	else // 3d display
	{
		// slice no
		int s = GetCurrentIdOfSyntheticScans();

		// position
		double p[3];
		GetPlaneSourceOriginOfSyntheticScans(s, p); // starting from L2 (low) going up to L1 (high)
		GetContourPlane()->SetOrigin(p);

		// insertions
		double p1[3];
		double p2[3];
		GetLandmark1OfPatient(p1); // high
		GetLandmark2OfPatient(p2); // low

		// unit vector along muscle axis
		double u[3];
		for(int i = 0; i < 3; i++)
			u[i] = p1[i] - p2[i];
		double n = sqrt(pow(u[0], 2.0) + pow(u[1], 2.0) + pow(u[2], 2.0));
		for(i = 0; i < 3; i++)
			u[i] = u[i] / n;

		// normal
		GetContourPlane()->SetNormal(u[0], u[1], u[2]);
	}
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateSegmentCuttingPlanes()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z

	// get h/v translation
	double trans[2];
	trans[0] = GetTHSpline()->Evaluate(z);
	trans[1] = GetTVSpline()->Evaluate(z);

	GetCuttingPlaneNorth()->SetOrigin(0.0, center[1] - trans[1], 0.0); // north/south moved by center[1] - vtrans
	GetCuttingPlaneSouth()->SetOrigin(0.0, center[1] - trans[1], 0.0); // y translation only

	GetCuttingPlaneEast()->SetOrigin(center[0] - trans[0], 0.0, 0.0); // east/west moved by center[0] - htrans
	GetCuttingPlaneWest()->SetOrigin(center[0] - trans[0], 0.0, 0.0); // x translation only
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateContourAxesTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	Transform->Translate(center[0], center[1], 0.0); // axes origin

	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);

	// r operation
	Transform->RotateZ(twist);

	// set up 2d contour axes first
	double width = GetContourAxesLengthScale() * (bounds[1] - bounds[0]) / 2.0;
	double height = GetContourAxesLengthScale() * (bounds[3] - bounds[2]) / 2.0;

	if (width > 0.0) // positive/negative x axis
	{
		GetContourPositiveXAxisLineSource()->SetPoint2(width, 0.0, 0.0);
		GetContourNegativeXAxisLineSource()->SetPoint2(-1.0 * width, 0.0, 0.0);
	}

	if (height > 0.0) // positive/negative y axis
	{
		GetContourPositiveYAxisLineSource()->SetPoint2(0.0, height, 0.0);
		GetContourNegativeYAxisLineSource()->SetPoint2(0.0, -1.0 * height, 0.0);
	}

	//
	GetContourPositiveXAxisActor()->SetUserTransform(Transform);
	GetContourPositiveYAxisActor()->SetUserTransform(Transform);
	GetContourNegativeXAxisActor()->SetUserTransform(Transform);
	GetContourNegativeYAxisActor()->SetUserTransform(Transform);

	// clean up
	Transform->Delete();
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetContourAxesVisibility()
//----------------------------------------------------------------------------
{
	if (m_3dDisplay == 1)
	{
		GetContourPositiveXAxisActor()->VisibilityOff();
		GetContourNegativeXAxisActor()->VisibilityOff();
		GetContourPositiveYAxisActor()->VisibilityOff();
		GetContourNegativeYAxisActor()->VisibilityOff();
		return;
	}

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// set up 2d contour axes first
	double width = GetContourAxesLengthScale() * (bounds[1] - bounds[0]) / 2.0;
	double height = GetContourAxesLengthScale() * (bounds[3] - bounds[2]) / 2.0;

	if (width > 0.0) // positive/negative x axis
	{
		GetContourPositiveXAxisActor()->VisibilityOn();
		GetContourNegativeXAxisActor()->VisibilityOn();
	}
	else
	{
		GetContourPositiveXAxisActor()->VisibilityOff();
		GetContourNegativeXAxisActor()->VisibilityOff();
	}

	if (height > 0.0) // positive/negative y axis
	{
		GetContourPositiveYAxisActor()->VisibilityOn();
		GetContourNegativeYAxisActor()->VisibilityOn();
	}
	else
	{
		GetContourPositiveYAxisActor()->VisibilityOff();
		GetContourNegativeYAxisActor()->VisibilityOff();
	}
}

//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateGlobalAxesTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	// r operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	
	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);
		
	//
	GetPositiveXAxisActor()->SetUserTransform(Transform);
	GetPositiveYAxisActor()->SetUserTransform(Transform);
	GetNegativeXAxisActor()->SetUserTransform(Transform);
	GetNegativeYAxisActor()->SetUserTransform(Transform);
	
	// clean up
	Transform->Delete();
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetGlobalAxesVisibility()
//----------------------------------------------------------------------------
{
	if (m_3dDisplay == 1)
	{
		GetPositiveXAxisActor()->VisibilityOff();
		GetNegativeXAxisActor()->VisibilityOff();
		GetPositiveYAxisActor()->VisibilityOff();
		GetNegativeYAxisActor()->VisibilityOff();
		return;
	}
}

//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateSegmentNorthEastTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = GetTHSpline()->Evaluate(z);
	trans[1] = GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = GetSESpline()->Evaluate(z); // east
	scale[1] = GetSWSpline()->Evaluate(z); // west
	scale[2] = GetSNSpline()->Evaluate(z); // north
	scale[3] = GetSSSpline()->Evaluate(z); // south;

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	// s operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Scale(scale[0], scale[2], 1.0); // s operation
	Transform->RotateZ(-1.0 * twist); // inverse r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	
	// r operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	Transform->Translate(trans[0], trans[1], 0.0);
	
	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);
	
	//
	Transform->Translate(0.0, 0.0, -1.0 * z); // place on Oxy plane for correct display
	
	//
	GetNEContourTransformPolyDataFilter()->SetTransform(Transform);
	GetNEContourTransformPolyDataFilter()->Update();
	
	// clean up
	Transform->Delete();
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateSegmentNorthWestTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = GetTHSpline()->Evaluate(z);
	trans[1] = GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = GetSESpline()->Evaluate(z); // east
	scale[1] = GetSWSpline()->Evaluate(z); // west
	scale[2] = GetSNSpline()->Evaluate(z); // north
	scale[3] = GetSSSpline()->Evaluate(z); // south;

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	// s operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Scale(scale[1], scale[2], 1.0); // s operation
	Transform->RotateZ(-1.0 * twist); // inverse r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	
	// r operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// t operation
	Transform->Translate(trans[0], trans[1], 0.0);
	
	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);
	
	//
	Transform->Translate(0.0, 0.0, -1.0 * z); // place on Oxy plane for correct display
	
	//
	GetNWContourTransformPolyDataFilter()->SetTransform(Transform);
	GetNWContourTransformPolyDataFilter()->Update();
	
	// clean up
	Transform->Delete();
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateSegmentSouthEastTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = GetTHSpline()->Evaluate(z);
	trans[1] = GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = GetSESpline()->Evaluate(z); // east
	scale[1] = GetSWSpline()->Evaluate(z); // west
	scale[2] = GetSNSpline()->Evaluate(z); // north
	scale[3] = GetSSSpline()->Evaluate(z); // south;

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	// s operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Scale(scale[0], scale[3], 1.0); // s operation
	Transform->RotateZ(-1.0 * twist); // inverse r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// r operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	
	// t operation
	Transform->Translate(trans[0], trans[1], 0.0);
	
	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);
	
	//
	Transform->Translate(0.0, 0.0, -1.0 * z); // place on Oxy plane for correct display
	
	//
	GetSEContourTransformPolyDataFilter()->SetTransform(Transform);
	GetSEContourTransformPolyDataFilter()->Update();
	
	// clean up
	Transform->Delete();
}
//----------------------------------------------------------------------------
void mmoMMLModelView::UpdateSegmentSouthWestTransform()
//----------------------------------------------------------------------------
{
	// get z level
	double z = GetCurrentZOfSyntheticScans();

	// original bounds
	double bounds[6];
	GetContourActor()->GetBounds(bounds);

	// original center
	double center[3];
	center[0] = (bounds[0] + bounds[1]) / 2.0; // x
	center[1] = (bounds[2] + bounds[3]) / 2.0; // y
	center[2] = (bounds[4] + bounds[5]) / 2.0; // z 

	// get center h/v translation
	double ctrans[2];
	ctrans[0] = GetPHSpline()->Evaluate(z);
	ctrans[1] = GetPVSpline()->Evaluate(z);

	// get twist
	double twist = GetRASpline()->Evaluate(z);

	// get h/v translation
	double trans[2];
	trans[0] = GetTHSpline()->Evaluate(z);
	trans[1] = GetTVSpline()->Evaluate(z);

	// get scaling
	double scale[4];
	scale[0] = GetSESpline()->Evaluate(z); // east
	scale[1] = GetSWSpline()->Evaluate(z); // west
	scale[2] = GetSNSpline()->Evaluate(z); // north
	scale[3] = GetSSSpline()->Evaluate(z); // south;

	// initialise
	vtkTransform* Transform = vtkTransform::New();
	Transform->Identity();

	// s operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Scale(scale[1], scale[3], 1.0); // s operation
	Transform->RotateZ(-1.0 * twist); // inverse r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin

	// r operation
	Transform->Translate(center[0], center[1], 0.0); // axes origin
	Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
	Transform->RotateZ(twist); // r operation
	Transform->Translate(-1.0 * ctrans[0], -1.0 * ctrans[1], 0.0); // inverse p operation
	Transform->Translate(-1.0 * center[0], -1.0 * center[1], 0.0); // inverse axes origin
	
	// t operation
	Transform->Translate(trans[0], trans[1], 0.0);
	
	// p operation
	Transform->Translate(ctrans[0], ctrans[1], 0.0);
	
	//
	Transform->Translate(0.0, 0.0, -1.0 * z); // place on Oxy plane for correct display
	
	//
	GetSWContourTransformPolyDataFilter()->SetTransform(Transform);
	GetSWContourTransformPolyDataFilter()->Update();
	
	// clean up
	Transform->Delete();
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleAtlasInsertionPoint1[0] = xyz[0];
	m_dMuscleAtlasInsertionPoint1[1] = xyz[1];
	m_dMuscleAtlasInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleAtlasInsertionPoint2[0] = xyz[0];
	m_dMuscleAtlasInsertionPoint2[1] = xyz[1];
	m_dMuscleAtlasInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleAtlasReferencePoint1[0] = xyz[0];
	m_dMuscleAtlasReferencePoint1[1] = xyz[1];
	m_dMuscleAtlasReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleAtlasReferencePoint2[0] = xyz[0];
	m_dMuscleAtlasReferencePoint2[1] = xyz[1];
	m_dMuscleAtlasReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleAtlasInsertionPoint1[0];
	xyz[1] = m_dMuscleAtlasInsertionPoint1[1];
	xyz[2] = m_dMuscleAtlasInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleAtlasInsertionPoint2[0];
	xyz[1] = m_dMuscleAtlasInsertionPoint2[1];
	xyz[2] = m_dMuscleAtlasInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleAtlasReferencePoint1[0];
	xyz[1] = m_dMuscleAtlasReferencePoint1[1];
	xyz[2] = m_dMuscleAtlasReferencePoint1[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleAtlasReferencePoint2[0];
	xyz[1] = m_dMuscleAtlasReferencePoint2[1];
	xyz[2] = m_dMuscleAtlasReferencePoint2[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleScansInsertionPoint1[0] = xyz[0];
	m_dMuscleScansInsertionPoint1[1] = xyz[1];
	m_dMuscleScansInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleScansInsertionPoint2[0] = xyz[0];
	m_dMuscleScansInsertionPoint2[1] = xyz[1];
	m_dMuscleScansInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleScansReferencePoint1[0] = xyz[0];
	m_dMuscleScansReferencePoint1[1] = xyz[1];
	m_dMuscleScansReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_dMuscleScansReferencePoint2[0] = xyz[0];
	m_dMuscleScansReferencePoint2[1] = xyz[1];
	m_dMuscleScansReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleScansInsertionPoint1[0];
	xyz[1] = m_dMuscleScansInsertionPoint1[1];
	xyz[2] = m_dMuscleScansInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleScansInsertionPoint2[0];
	xyz[1] = m_dMuscleScansInsertionPoint2[1];
	xyz[2] = m_dMuscleScansInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleScansReferencePoint1[0];
	xyz[1] = m_dMuscleScansReferencePoint1[1];
	xyz[2] = m_dMuscleScansReferencePoint1[2];
}

//----------------------------------------------------------------------------
void mmoMMLModelView::GetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_dMuscleScansReferencePoint2[0];
	xyz[1] = m_dMuscleScansReferencePoint2[1];
	xyz[2] = m_dMuscleScansReferencePoint2[2];
}

//----------------------------------------------------------------------------
bool mmoMMLModelView::SetUpContourCoordinateAxes()
//----------------------------------------------------------------------------
{
	// east
	m_pContourPosXAxisAxesTubeFilter->SetRadius(0.5);
    m_pContourPosXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pContourPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pContourPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_pContourPosXAxisActor->VisibilityOn();

	// north
	m_pContourPosYAxisAxesTubeFilter->SetRadius(0.5);
    m_pContourPosYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pContourPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pContourPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_pContourPosYAxisActor->VisibilityOn();

	// west
	m_pContourNegXAxisAxesTubeFilter->SetRadius(0.5);
    m_pContourNegXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pContourNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pContourNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	m_pContourNegXAxisActor->VisibilityOn();

	// south
	m_pContourNegYAxisAxesTubeFilter->SetRadius(0.5);
    m_pContourNegYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pContourNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pContourNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_pContourNegYAxisActor->VisibilityOn();
	
	return 1;
}

//----------------------------------------------------------------------------
bool mmoMMLModelView::SetUpGlobalCoordinateAxes()
//----------------------------------------------------------------------------
{
	// east
	m_pPosXAxisAxesTubeFilter->SetRadius(0.5);
    m_pPosXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pPosXAxisLineSource->SetPoint2(m_nSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_pPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_pPosXAxisActor->VisibilityOn();

	// north
	m_pPosYAxisAxesTubeFilter->SetRadius(0.5);
    m_pPosYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pPosYAxisLineSource->SetPoint2(0.0, m_nSyntheticScansYSize / 2.0, 0.0);
	m_pPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_pPosYAxisActor->VisibilityOn();
	
	// west
	m_pNegXAxisAxesTubeFilter->SetRadius(0.5);
    m_pNegXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pNegXAxisLineSource->SetPoint2(-1.0 * m_nSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_pNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	m_pNegXAxisActor->VisibilityOn();
	
	// south
	m_pNegYAxisAxesTubeFilter->SetRadius(0.5);
    m_pNegYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pNegYAxisLineSource->SetPoint2(0.0, -1.0 * m_nSyntheticScansYSize / 2.0, 0.0);
	m_pNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_pNegYAxisActor->VisibilityOn();

	return 1;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::Switch3dDisplayOn()
//----------------------------------------------------------------------------
{
 	// 3d display flag
 	m_3dDisplay = 1;
 
 	// interactor style for 3d display
 	m_pRenderWindowInteractor->SetInteractorStyle(GetInteractorStyleTrackballCamera());
 
 	// identity transform
 	vtkTransform *t = vtkTransform::New();
 	vtkMatrix4x4 *m = vtkMatrix4x4::New();
 	m->Identity();
 	t->SetMatrix(m);
 
 	// synthetic scans stay in patient space only
	// thus scan actor transform must be identity
 	int n = GetTotalNumberOfSyntheticScans();
 	for(int i = 0; i < n; i++)
 	{
 		m_pSyntheticScansActor[i]->SetUserTransform(t);
 	}
 
 	// muscle mapped to patient space only, using transform 1
	// thus, transform 2 must simply be an identity transform
 	m_pMuscleTransform2PolyDataFilter->SetTransform(t);
 
 	// set up muscle and switch on
 	m_pMuscleLODActor->VisibilityOn();
 	m_pMuscleLODActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
 	m_pMuscleLODActor->GetProperty()->SetOpacity(0.5);
 
 	// standard contour switch on
 	m_pContourActor->VisibilityOn();
 
 	// sub-contours switch off
 	m_pNEContourActor->VisibilityOff();
 	m_pNWContourActor->VisibilityOff();
 	m_pSEContourActor->VisibilityOff();
 	m_pSWContourActor->VisibilityOff();
 
	// axes
	// east/north/west/south re-set up and switch on
	// set up performed in SetUpMuscleActionLineAxes
	m_pPosXAxisAxesTubeFilter->SetRadius(1.0);
	m_pPosYAxisAxesTubeFilter->SetRadius(1.0);
	m_pNegXAxisAxesTubeFilter->SetRadius(1.0);
	m_pNegYAxisAxesTubeFilter->SetRadius(1.0);

	// positive z axis set up
	m_pPosZAxisAxesTubeFilter->SetRadius(1.0);
    m_pPosZAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pPosZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pPosZAxisLineSource->SetPoint2(0.0, 0.0, (m_nSyntheticScansXSize + m_nSyntheticScansYSize) / 2.0);
	m_pPosZAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

	// negative z axis set up
	m_pNegZAxisAxesTubeFilter->SetRadius(1.0);
    m_pNegZAxisAxesTubeFilter->SetNumberOfSides(6);
	m_pNegZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_pNegZAxisLineSource->SetPoint2(0.0, 0.0, -1.0 * (m_nSyntheticScansXSize + m_nSyntheticScansYSize) / 2.0);
	m_pNegZAxisActor->GetProperty()->SetColor(1.0, 1.0, 1.0); // white

	// axes off
	m_pPosXAxisActor->VisibilityOff();
	m_pNegXAxisActor->VisibilityOff();
	m_pPosYAxisActor->VisibilityOff();
	m_pNegYAxisActor->VisibilityOff();
	m_pPosZAxisActor->VisibilityOff();
	m_pNegZAxisActor->VisibilityOff();

 	// landmarks
 	double p1[3]; this->GetLandmark1OfPatient(p1);
 	double p2[3]; this->GetLandmark2OfPatient(p2);
 	double p3[3]; this->GetLandmark3OfPatient(p3);
 	double p4[3]; this->GetLandmark4OfPatient(p4);
 	
 	// 1st landmark set up and switch on
	m_pLandmark1SphereSource->SetRadius(2.0);
	m_pLandmark1SphereSource->SetThetaResolution(10);
	m_pLandmark1SphereSource->SetPhiResolution(10);
 	m_pLandmark1SphereSource->SetCenter(p1[0], p1[1], p1[2]);
 	m_pLandmark1Actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_pLandmark1Actor->VisibilityOn();
 
	// 2nd landmark set up and switch on
	m_pLandmark2SphereSource->SetRadius(2.0);
	m_pLandmark2SphereSource->SetThetaResolution(10);
	m_pLandmark2SphereSource->SetPhiResolution(10);
 	m_pLandmark2SphereSource->SetCenter(p2[0], p2[1], p2[2]);
 	m_pLandmark2Actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_pLandmark2Actor->VisibilityOn();
 
	// 3rd landmark set up and switch on
	m_pLandmark3SphereSource->SetRadius(2.0);
	m_pLandmark3SphereSource->SetThetaResolution(10);
	m_pLandmark3SphereSource->SetPhiResolution(10);
 	m_pLandmark3SphereSource->SetCenter(p3[0], p3[1], p3[2]);
 	m_pLandmark3Actor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_pLandmark3Actor->VisibilityOn();
 	
	// 4th landmark set up and switch on
 	if (m_4Landmarks == 1)
 	{
		m_pLandmark4SphereSource->SetRadius(2.0);
		m_pLandmark4SphereSource->SetThetaResolution(10);
		m_pLandmark4SphereSource->SetPhiResolution(10);
 		m_pLandmark4SphereSource->SetCenter(p4[0], p4[1], p4[2]);
 		m_pLandmark4Actor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow
		m_pLandmark4Actor->VisibilityOn();	
 	}
 
 	// line of action (L1 to L2) set up and switch on
 	m_pL1L2TubeFilter->SetRadius(1.0);
    m_pL1L2TubeFilter->SetNumberOfSides(6);
	m_pL1L2LineSource->SetPoint1(p1[0], p1[1], p1[2]);
 	m_pL1L2LineSource->SetPoint2(p2[0], p2[1], p2[2]);
	m_pL1L2Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
 	m_pL1L2Actor->VisibilityOn();

	// L2 to L3 line set up and switch on
 	m_pL2L3TubeFilter->SetRadius(1.0);
    m_pL2L3TubeFilter->SetNumberOfSides(6);
	m_pL2L3LineSource->SetPoint1(p2[0], p2[1], p2[2]);
 	m_pL2L3LineSource->SetPoint2(p3[0], p3[1], p3[2]);
	m_pL2L3Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
 	m_pL2L3Actor->VisibilityOn();
}

//SIL. 24-12-2004: begin
//----------------------------------------------------------------------------
void mmoMMLModelView::Print(vtkObject *obj, wxString msg)
//----------------------------------------------------------------------------
{
		wxLogMessage("%s",msg);
    #ifdef VTK_USE_ANSI_STDLIB
			std::stringstream ss1;

			obj->Print(ss1);
      wxString message=ss1.str().c_str();
      wxLogMessage("[%s PRINTOUT:]\n", obj->GetClassName());
 
      for (int pos=message.Find('\n');pos>=0;pos=message.Find('\n'))
      {
        wxString tmp=message.Mid(0,pos);
        wxLogMessage(tmp);
        message=message.Mid(pos+1);
      }
		#else
			strstream ss1,ss2;
			obj->Print(ss1);
      wxLogMessage("[%s PRINTOUT:]\n", obj->GetClassName());
			wxLogMessage("%s\n", ss1.str()); 
		#endif

}
//SIL. 24-12-2004: end

//----------------------------------------------------------------------------
int mmoMMLModelView::GetScalingOccured()
//----------------------------------------------------------------------------
{
	return ScalingOccured;
}

//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* mmoMMLModelView::GetMuscleTransform2PolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pMuscleTransform2PolyDataFilter;
}

//----------------------------------------------------------------------------
vtkMatrix4x4* mmoMMLModelView::GetFinalM()
//----------------------------------------------------------------------------
{
	return m_pfinalm;
}

//----------------------------------------------------------------------------
vtkTubeFilter* mmoMMLModelView::GetContourTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourTubeFilter;
}

//----------------------------------------------------------------------------
float mmoMMLModelView::GetSyntheticScansWindow()
//----------------------------------------------------------------------------
{
	//assert(m_pWindow > 0);
	return m_pWindow;
}

//----------------------------------------------------------------------------
float mmoMMLModelView::GetSyntheticScansLevel()
//----------------------------------------------------------------------------
{
	return m_pLevel;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::Set4LandmarksFlag(int n)
//----------------------------------------------------------------------------
{
	m_4Landmarks = n;
}

//----------------------------------------------------------------------------
void mmoMMLModelView::SetGrainOfScans(int n)
//----------------------------------------------------------------------------
{
	m_SyntheticScansGrain = n;
}


//wxLogMessage("============ALIGN MUSCLE======================");
//DEL 	//Print(m_pMuscleTransform1 ,               "m_pMuscleTransform1 ");
//DEL 	//Print(m_pMuscleTransform1PolyDataFilter , "m_pMuscleTransform1PolyDataFilter ");
//DEL 	//Print(m_pMuscleTransform2 ,               "m_pMuscleTransform2 ");
//DEL 	//Print(m_pMuscleTransform2PolyDataFilter , "m_pMuscleTransform2PolyDataFilter ");
//DEL 	//Print(m_pMuscleTransform2PolyDataFilter->GetOutput() , "m_pMuscleTransform2PolyDataFilter->GetOutput()");
//DEL 	//wxLogMessage("==================================");
