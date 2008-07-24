/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLModelView.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-24 08:00:04 $
  Version:   $Revision: 1.2 $
  Authors:   Mel Krokos
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"

#include "medOpMMLModelView.h"
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
medOpMMLModelView::medOpMMLModelView( vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume)
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
	m_ScalingOccured = FALSE;

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
	m_PCenterHorizontalOffsetSpline = vtkKochanekSpline::New();
	m_PCenterHorizontalOffsetSpline->ClosedOff();

	//
	m_PCenterVerticalOffsetSpline = vtkKochanekSpline::New();
	m_PCenterVerticalOffsetSpline->ClosedOff();

	//
	m_PTwistSpline = vtkKochanekSpline::New();
	m_PTwistSpline->ClosedOff();

	m_PHorizontalTranslationSpline = vtkKochanekSpline::New();
	m_PHorizontalTranslationSpline->ClosedOff();

	m_PVerticalTranslationSpline = vtkKochanekSpline::New();
	m_PVerticalTranslationSpline->ClosedOff();

	m_PNorthScalingSpline = vtkKochanekSpline::New();
	m_PNorthScalingSpline->ClosedOff();

	m_PSouthScalingSpline = vtkKochanekSpline::New();
	m_PSouthScalingSpline->ClosedOff();

	m_PEastScalingSpline = vtkKochanekSpline::New();
	m_PEastScalingSpline->ClosedOff();

	m_PWestScalingSpline = vtkKochanekSpline::New();
	m_PWestScalingSpline->ClosedOff();

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
	m_ScalingFlagStack = vtkIntArray::New();
	m_ScalingFlagStack->SetNumberOfComponents(1);

	m_OperationsStack = vtkDoubleArray::New();
	m_OperationsStack->SetNumberOfComponents(5);
	m_OperationsStack->SetNumberOfTuples(2000);

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
	m_PPosXAxisLineSource = vtkLineSource::New();
	m_PPosXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosXAxisAxesTubeFilter->SetInput(m_PPosXAxisLineSource->GetOutput());
	m_PPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosXAxisPolyDataMapper->SetInput(m_PPosXAxisAxesTubeFilter->GetOutput());
	m_PPosXAxisActor = vtkActor::New();
	m_PPosXAxisActor->SetMapper(m_PPosXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PPosXAxisActor);
	m_PPosXAxisActor->VisibilityOff();
	
	// north global axis
	m_PPosYAxisLineSource = vtkLineSource::New();
	m_PPosYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosYAxisAxesTubeFilter->SetInput(m_PPosYAxisLineSource->GetOutput());
	m_PPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosYAxisPolyDataMapper->SetInput(m_PPosYAxisAxesTubeFilter->GetOutput());
	m_PPosYAxisActor = vtkActor::New();
	m_PPosYAxisActor->SetMapper(m_PPosYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PPosYAxisActor);
	m_PPosYAxisActor->VisibilityOff();

	// west global axis
	m_PNegXAxisLineSource = vtkLineSource::New();
	m_PNegXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegXAxisAxesTubeFilter->SetInput(m_PNegXAxisLineSource->GetOutput());
	m_PNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegXAxisPolyDataMapper->SetInput(m_PNegXAxisAxesTubeFilter->GetOutput());
	m_PNegXAxisActor = vtkActor::New();
	m_PNegXAxisActor->SetMapper(m_PNegXAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PNegXAxisActor);
	m_PNegXAxisActor->VisibilityOff();

	// south global axis
	m_PNegYAxisLineSource = vtkLineSource::New();
	m_PNegYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegYAxisAxesTubeFilter->SetInput(m_PNegYAxisLineSource->GetOutput());
	m_PNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegYAxisPolyDataMapper->SetInput(m_PNegYAxisAxesTubeFilter->GetOutput());
	m_PNegYAxisActor = vtkActor::New();
	m_PNegYAxisActor->SetMapper(m_PNegYAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PNegYAxisActor);
	m_PNegYAxisActor->VisibilityOff();

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
	m_PPosZAxisLineSource = vtkLineSource::New();
	m_PPosZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosZAxisAxesTubeFilter->SetInput(m_PPosZAxisLineSource->GetOutput());
	m_PPosZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosZAxisPolyDataMapper->SetInput(m_PPosZAxisAxesTubeFilter->GetOutput());
	m_PPosZAxisActor = vtkActor::New();
	m_PPosZAxisActor->SetMapper(m_PPosZAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PPosZAxisActor);
	m_PPosZAxisActor->VisibilityOff();

	// negative z global axis
	m_PNegZAxisLineSource = vtkLineSource::New();
	m_PNegZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegZAxisAxesTubeFilter->SetInput(m_PNegZAxisLineSource->GetOutput());
	m_PNegZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegZAxisPolyDataMapper->SetInput(m_PNegZAxisAxesTubeFilter->GetOutput());
	m_PNegZAxisActor = vtkActor::New();
	m_PNegZAxisActor->SetMapper(m_PNegZAxisPolyDataMapper);
	m_pRenderer->AddActor(m_PNegZAxisActor);
	m_PNegZAxisActor->VisibilityOff();


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
medOpMMLModelView::~medOpMMLModelView()
//----------------------------------------------------------------------------
{
}





//----------------------------------------------------------------------------
void medOpMMLModelView::FindUnitVectorsAndLengthsOfLandmarkLines()
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
bool medOpMMLModelView::MapAtlasToPatient()
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
bool medOpMMLModelView::MakeActionLineZAxis()
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
void medOpMMLModelView::FindSizeAndResolutionOfSyntheticScans()
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
void medOpMMLModelView::SetUpSyntheticScans()
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
	m_ScalingFlagStack->SetNumberOfTuples(n);
	float a = 0;
	for(int ii = 0; ii < n; ii++)
		m_ScalingFlagStack->SetTuple(ii, &a);

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
void medOpMMLModelView::GetPlaneSourceOriginOfSyntheticScans(int s, double p[])
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
vtkMatrix4x4* medOpMMLModelView::GetPlaneSourceTransformOfSyntheticScans(int s)
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
vtkMatrix4x4* medOpMMLModelView::GetActorTransformOfSyntheticScans(int s)
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
vtkActor* medOpMMLModelView::GetContourActor()
//----------------------------------------------------------------------------
{
	return m_pContourActor;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetPHSpline()
//----------------------------------------------------------------------------
{
	return m_PCenterHorizontalOffsetSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetPVSpline()
//----------------------------------------------------------------------------
{
	return m_PCenterVerticalOffsetSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetTHSpline()
//----------------------------------------------------------------------------
{
	return m_PHorizontalTranslationSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetTVSpline()
//----------------------------------------------------------------------------
{
	return m_PVerticalTranslationSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetRASpline()
//----------------------------------------------------------------------------
{
	return m_PTwistSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetSNSpline()
//----------------------------------------------------------------------------
{
	return m_PNorthScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetSSSpline()
//----------------------------------------------------------------------------
{
	return m_PSouthScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetSESpline()
//----------------------------------------------------------------------------
{
	return m_PEastScalingSpline;
}
//----------------------------------------------------------------------------
vtkKochanekSpline* medOpMMLModelView::GetSWSpline()
//----------------------------------------------------------------------------
{
	return m_PWestScalingSpline;
}
//----------------------------------------------------------------------------
double medOpMMLModelView::GetZOfSyntheticScans(int s)
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
double medOpMMLModelView::GetCurrentZOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return GetZOfSyntheticScans(GetCurrentIdOfSyntheticScans());
}
//----------------------------------------------------------------------------
void medOpMMLModelView::Render()
//----------------------------------------------------------------------------
{
	m_pRenderWindow->Render();
}
//----------------------------------------------------------------------------
vtkTextMapper* medOpMMLModelView::GetTextMapper2()
//----------------------------------------------------------------------------
{
	return m_pTextMapperX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* medOpMMLModelView::GetScaledTextActor2()
//----------------------------------------------------------------------------
{
	return m_pScaledTextActorX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* medOpMMLModelView::GetScaledTextActor1()
//----------------------------------------------------------------------------
{
	return m_pScaledTextActorY;
}
//----------------------------------------------------------------------------
vtkTextMapper* medOpMMLModelView::GetTextMapper1()
//----------------------------------------------------------------------------
{
	return m_pTextMapperY;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNEContourActor()
//----------------------------------------------------------------------------
{
	return m_pNEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNWContourActor()
//----------------------------------------------------------------------------
{
	return m_pNWContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetSEContourActor()
//----------------------------------------------------------------------------
{
	return m_pSEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetSWContourActor()
//----------------------------------------------------------------------------
{
	return m_pSWContourActor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetNEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pNEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetNWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pNWContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetSEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pSEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetSWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pSWContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
/** m = 1, 2 for text actor 1,2 */
void medOpMMLModelView::SetText(int m, double n, int d, int s)
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
vtkCamera* medOpMMLModelView::GetActiveCamera()
//----------------------------------------------------------------------------
{
	return m_pRenderer->GetActiveCamera();
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	m_pCameraFocalPoint[0] = *fp;
	m_pCameraFocalPoint[1] = *(fp+1);
	m_pCameraFocalPoint[2] = *(fp+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	m_pCameraPosition[0] = *cp;
	m_pCameraPosition[1] = *(cp+1);
	m_pCameraPosition[2] = *(cp+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	m_pCameraClippingRange[0] = *cr;
	m_pCameraClippingRange[1] = *(cr+1);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	m_pCameraViewUp[0] = *vu;
	m_pCameraViewUp[1] = *(vu+1);
	m_pCameraViewUp[2] = *(vu+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	*fp = m_pCameraFocalPoint[0];
	*(fp+1) = m_pCameraFocalPoint[1];
	*(fp+2) = m_pCameraFocalPoint[2];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	*cp = m_pCameraPosition[0];
	*(cp+1) = m_pCameraPosition[1];
	*(cp+2) = m_pCameraPosition[2];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	*cr = m_pCameraClippingRange[0];
	*(cr+1) = m_pCameraClippingRange[1];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	*vu = m_pCameraViewUp[0];
	*(vu+1) = m_pCameraViewUp[1];
	*(vu+2) = m_pCameraViewUp[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::AddActor(vtkActor *a)
//----------------------------------------------------------------------------
{
	m_pRenderer->AddActor(a);
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetPositiveXAxisActor()
//----------------------------------------------------------------------------
{
	return m_PPosXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetPositiveYAxisActor()
//----------------------------------------------------------------------------
{
	return m_PPosYAxisActor;
}
vtkActor* medOpMMLModelView::GetPositiveZAxisActor()
//----------------------------------------------------------------------------
{
	return m_PPosZAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNegativeXAxisActor()
//----------------------------------------------------------------------------
{
	return m_PNegXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNegativeYAxisActor()
//----------------------------------------------------------------------------
{
	return m_PNegYAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNegativeZAxisActor()
//----------------------------------------------------------------------------
{
	return m_PNegZAxisActor;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetPositiveLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_PPosXAxisLineSource->SetPoint1(p1);
	m_PPosXAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetNegativeLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_PNegYAxisLineSource->SetPoint1(p1);
	m_PNegYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetPositiveLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_PPosYAxisLineSource->SetPoint1(p1);
	m_PPosYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetNegativeLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
	m_PNegXAxisLineSource->SetPoint1(p1);
	m_PNegXAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::WriteMatrix(char *pch, vtkMatrix4x4 *m)
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
vtkLODActor* medOpMMLModelView::GetMuscleLODActor()
//----------------------------------------------------------------------------
{
	return m_pMuscleLODActor;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetTotalNumberOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansTotalNumber = n;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetResolutionOfSyntheticScans(int x, int y)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansXResolution = x;
	m_nSyntheticScansYResolution = y;
}
//----------------------------------------------------------------------------
int medOpMMLModelView::GetTotalNumberOfSyntheticScans()
//----------------------------------------------------------------------------
{
	assert(m_nSyntheticScansTotalNumber > 3); // at least 3 synthetic scans
	return m_nSyntheticScansTotalNumber;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetResolutionOfSyntheticScans(int *x, int *y)
//----------------------------------------------------------------------------
{
	*x = m_nSyntheticScansXResolution;
	*y = m_nSyntheticScansYResolution;
}

//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetActorOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	return m_pSyntheticScansActor[s];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetCurrentIdOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansCurrentId = n;
}
//----------------------------------------------------------------------------
int medOpMMLModelView::GetCurrentIdOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_nSyntheticScansCurrentId;
}
//----------------------------------------------------------------------------
vtkWindowLevelLookupTable* medOpMMLModelView::GetWindowLevelLookupTableOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_pSyntheticScansWindowLevelLookupTable;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetSizeOfSyntheticScans(float x, float y)
//----------------------------------------------------------------------------
{
	m_nSyntheticScansXSize = x;
	m_nSyntheticScansYSize = y;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::GetSizeOfSyntheticScans(float *x, float *y)
//----------------------------------------------------------------------------
{
	*x = m_nSyntheticScansXSize;
	*y = m_nSyntheticScansYSize;
}
//----------------------------------------------------------------------------
vtkMatrix4x4* medOpMMLModelView::MultiplyMatrix4x4(vtkMatrix4x4 *a, vtkMatrix4x4 *b)
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
void medOpMMLModelView::SetXYScalingFactorsOfMuscle(double x, double y)
//----------------------------------------------------------------------------
{
	m_flMuscleXScalingFactor = x;
	m_flMuscleYScalingFactor = y;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::GetXYScalingFactorsOfMuscle(double *x, double *y)
//----------------------------------------------------------------------------
{
	*x = m_flMuscleXScalingFactor;
	*y = m_flMuscleYScalingFactor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetContourCutterTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pContourCutterTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetContourPlane()
//----------------------------------------------------------------------------
{
	return m_pContourPlane;
}
//----------------------------------------------------------------------------
vtkTransform* medOpMMLModelView::GetContourCutterTransform()
//----------------------------------------------------------------------------
{
	return m_pContourCutterTransform;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetPositiveXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PPosXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetNegativeXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PNegXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetPositiveYAxisLineSource()
//----------------------------------------------------------------------------
{
		return m_PPosYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetNegativeYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PNegYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetPositiveXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PPosXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetNegativeXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PNegXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetPositiveYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PPosYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetNegativeYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PNegYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkRenderWindowInteractor* medOpMMLModelView::GetRenderWindowInteractor()
//----------------------------------------------------------------------------
{
	return m_pRenderWindowInteractor;
}
//----------------------------------------------------------------------------
vtkInteractorStyleImage* medOpMMLModelView::GetInteractorStyleImage()
//----------------------------------------------------------------------------
{
	return m_pInteractorStyleImage;
}
//----------------------------------------------------------------------------
vtkInteractorStyleTrackballCamera* medOpMMLModelView::GetInteractorStyleTrackballCamera()
//----------------------------------------------------------------------------
{
	return m_pInteractorStyleTrackballCamera;
}

//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourPositiveXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourPositiveYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourNegativeXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourNegativeYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourPositiveXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourPositiveYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourNegativeXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourNegativeYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourPositiveXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourPosXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourPositiveYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourPosYAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourNegativeXAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourNegXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourNegativeYAxisActor()
//----------------------------------------------------------------------------
{
	return m_pContourNegYAxisActor;
}
//----------------------------------------------------------------------------
float medOpMMLModelView::GetLowScalar()
//----------------------------------------------------------------------------
{
	return m_SyntheticScansMinScalarValue;
}
//----------------------------------------------------------------------------
float medOpMMLModelView::GetHighScalar()
//----------------------------------------------------------------------------
{
 	return m_SyntheticScansMaxScalarValue;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetTypeOfMuscles(int t)
//----------------------------------------------------------------------------
{
	m_nTypeOfMuscles = t;
}
//----------------------------------------------------------------------------
int medOpMMLModelView::GetTypeOfMuscles()
//----------------------------------------------------------------------------
{
	return m_nTypeOfMuscles;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneNorth()
//----------------------------------------------------------------------------
{
	return m_pX0ZNPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneSouth()
//----------------------------------------------------------------------------
{
	return m_pX0ZSPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneEast()
//----------------------------------------------------------------------------
{
	return m_pY0ZEPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneWest()
//----------------------------------------------------------------------------
{
	return m_pY0ZWPlane;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetContourAxesLengthScale(float l)
{
	m_ContourAxesLengthScale = l;
}
//----------------------------------------------------------------------------
float medOpMMLModelView::GetContourAxesLengthScale()
//----------------------------------------------------------------------------
{
	return m_ContourAxesLengthScale;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::UpdateContourCuttingPlane()
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
		int i;
    for(i = 0; i < 3; i++)
			u[i] = p1[i] - p2[i];
		double n = sqrt(pow(u[0], 2.0) + pow(u[1], 2.0) + pow(u[2], 2.0));
		for(i = 0; i < 3; i++)
			u[i] = u[i] / n;

		// normal
		GetContourPlane()->SetNormal(u[0], u[1], u[2]);
	}
}
//----------------------------------------------------------------------------
void medOpMMLModelView::UpdateSegmentCuttingPlanes()
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
void medOpMMLModelView::UpdateContourAxesTransform()
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
void medOpMMLModelView::SetContourAxesVisibility()
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
void medOpMMLModelView::UpdateGlobalAxesTransform()
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
void medOpMMLModelView::SetGlobalAxesVisibility()
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
void medOpMMLModelView::UpdateSegmentNorthEastTransform()
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
void medOpMMLModelView::UpdateSegmentNorthWestTransform()
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
void medOpMMLModelView::UpdateSegmentSouthEastTransform()
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
void medOpMMLModelView::UpdateSegmentSouthWestTransform()
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
void medOpMMLModelView::SetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleAtlasInsertionPoint1[0] = xyz[0];
	m_DMuscleAtlasInsertionPoint1[1] = xyz[1];
	m_DMuscleAtlasInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleAtlasInsertionPoint2[0] = xyz[0];
	m_DMuscleAtlasInsertionPoint2[1] = xyz[1];
	m_DMuscleAtlasInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleAtlasReferencePoint1[0] = xyz[0];
	m_DMuscleAtlasReferencePoint1[1] = xyz[1];
	m_DMuscleAtlasReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleAtlasReferencePoint2[0] = xyz[0];
	m_DMuscleAtlasReferencePoint2[1] = xyz[1];
	m_DMuscleAtlasReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleAtlasInsertionPoint1[0];
	xyz[1] = m_DMuscleAtlasInsertionPoint1[1];
	xyz[2] = m_DMuscleAtlasInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleAtlasInsertionPoint2[0];
	xyz[1] = m_DMuscleAtlasInsertionPoint2[1];
	xyz[2] = m_DMuscleAtlasInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleAtlasReferencePoint1[0];
	xyz[1] = m_DMuscleAtlasReferencePoint1[1];
	xyz[2] = m_DMuscleAtlasReferencePoint1[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleAtlasReferencePoint2[0];
	xyz[1] = m_DMuscleAtlasReferencePoint2[1];
	xyz[2] = m_DMuscleAtlasReferencePoint2[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleScansInsertionPoint1[0] = xyz[0];
	m_DMuscleScansInsertionPoint1[1] = xyz[1];
	m_DMuscleScansInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleScansInsertionPoint2[0] = xyz[0];
	m_DMuscleScansInsertionPoint2[1] = xyz[1];
	m_DMuscleScansInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleScansReferencePoint1[0] = xyz[0];
	m_DMuscleScansReferencePoint1[1] = xyz[1];
	m_DMuscleScansReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	m_DMuscleScansReferencePoint2[0] = xyz[0];
	m_DMuscleScansReferencePoint2[1] = xyz[1];
	m_DMuscleScansReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleScansInsertionPoint1[0];
	xyz[1] = m_DMuscleScansInsertionPoint1[1];
	xyz[2] = m_DMuscleScansInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleScansInsertionPoint2[0];
	xyz[1] = m_DMuscleScansInsertionPoint2[1];
	xyz[2] = m_DMuscleScansInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleScansReferencePoint1[0];
	xyz[1] = m_DMuscleScansReferencePoint1[1];
	xyz[2] = m_DMuscleScansReferencePoint1[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
	xyz[0] = m_DMuscleScansReferencePoint2[0];
	xyz[1] = m_DMuscleScansReferencePoint2[1];
	xyz[2] = m_DMuscleScansReferencePoint2[2];
}

//----------------------------------------------------------------------------
bool medOpMMLModelView::SetUpContourCoordinateAxes()
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
bool medOpMMLModelView::SetUpGlobalCoordinateAxes()
//----------------------------------------------------------------------------
{
	// east
	m_PPosXAxisAxesTubeFilter->SetRadius(0.5);
    m_PPosXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PPosXAxisLineSource->SetPoint2(m_nSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_PPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_PPosXAxisActor->VisibilityOn();

	// north
	m_PPosYAxisAxesTubeFilter->SetRadius(0.5);
    m_PPosYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PPosYAxisLineSource->SetPoint2(0.0, m_nSyntheticScansYSize / 2.0, 0.0);
	m_PPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_PPosYAxisActor->VisibilityOn();
	
	// west
	m_PNegXAxisAxesTubeFilter->SetRadius(0.5);
    m_PNegXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegXAxisLineSource->SetPoint2(-1.0 * m_nSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_PNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	m_PNegXAxisActor->VisibilityOn();
	
	// south
	m_PNegYAxisAxesTubeFilter->SetRadius(0.5);
    m_PNegYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegYAxisLineSource->SetPoint2(0.0, -1.0 * m_nSyntheticScansYSize / 2.0, 0.0);
	m_PNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_PNegYAxisActor->VisibilityOn();

	return 1;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::Switch3dDisplayOn()
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
	m_PPosXAxisAxesTubeFilter->SetRadius(1.0);
	m_PPosYAxisAxesTubeFilter->SetRadius(1.0);
	m_PNegXAxisAxesTubeFilter->SetRadius(1.0);
	m_PNegYAxisAxesTubeFilter->SetRadius(1.0);

	// positive z axis set up
	m_PPosZAxisAxesTubeFilter->SetRadius(1.0);
    m_PPosZAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PPosZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PPosZAxisLineSource->SetPoint2(0.0, 0.0, (m_nSyntheticScansXSize + m_nSyntheticScansYSize) / 2.0);
	m_PPosZAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

	// negative z axis set up
	m_PNegZAxisAxesTubeFilter->SetRadius(1.0);
    m_PNegZAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegZAxisLineSource->SetPoint2(0.0, 0.0, -1.0 * (m_nSyntheticScansXSize + m_nSyntheticScansYSize) / 2.0);
	m_PNegZAxisActor->GetProperty()->SetColor(1.0, 1.0, 1.0); // white

	// axes off
	m_PPosXAxisActor->VisibilityOff();
	m_PNegXAxisActor->VisibilityOff();
	m_PPosYAxisActor->VisibilityOff();
	m_PNegYAxisActor->VisibilityOff();
	m_PPosZAxisActor->VisibilityOff();
	m_PNegZAxisActor->VisibilityOff();

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
void medOpMMLModelView::Print(vtkObject *obj, wxString msg)
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
int medOpMMLModelView::GetScalingOccured()
//----------------------------------------------------------------------------
{
	return m_ScalingOccured;
}

//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetMuscleTransform2PolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_pMuscleTransform2PolyDataFilter;
}

//----------------------------------------------------------------------------
vtkMatrix4x4* medOpMMLModelView::GetFinalM()
//----------------------------------------------------------------------------
{
	return m_pfinalm;
}

//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourTubeFilter()
//----------------------------------------------------------------------------
{
	return m_pContourTubeFilter;
}

//----------------------------------------------------------------------------
float medOpMMLModelView::GetSyntheticScansWindow()
//----------------------------------------------------------------------------
{
	//assert(m_pWindow > 0);
	return m_pWindow;
}

//----------------------------------------------------------------------------
float medOpMMLModelView::GetSyntheticScansLevel()
//----------------------------------------------------------------------------
{
	return m_pLevel;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::Set4LandmarksFlag(int n)
//----------------------------------------------------------------------------
{
	m_4Landmarks = n;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SetGrainOfScans(int n)
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
