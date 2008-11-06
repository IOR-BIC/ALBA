/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMMLModelView.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-06 09:06:01 $
  Version:   $Revision: 1.3.2.1 $
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
	m_3DDisplay = 0;

	// 3 landmarks by default for initial mapping
	m_4Landmarks = 0;

	//
	m_Scans = volume;

	// display tubes
	m_TubeFilterRadius = 0.5 ;

	// final transform
	m_PFinalm = vtkMatrix4x4::New();

	// synthetic slices transform
	m_PSlicesm = vtkMatrix4x4::New();

	//
	m_ScalingOccured = FALSE;

	//
	m_PRenderer = ren;
	m_PRenderWindow = rw;
	m_PRenderWindowInteractor = rw->GetInteractor();
	
	//
	m_PRenderer->SetBackground(0.2,0.4,0.6);
	
	int *size = m_PRenderWindow->GetSize();
	assert(size[0] > 0);
	assert(size[1] > 0);
  
	m_PRenderWindow->LineSmoothingOn();
	m_PInteractorStyleImage = vtkInteractorStyleImage::New();
	m_PInteractorStyleTrackballCamera = vtkInteractorStyleTrackballCamera::New();


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
	m_PRenderWindowInteractor->SetInteractorStyle(m_PInteractorStyleImage);

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
	m_PSyntheticScansPlaneSource = NULL ; // planes
	m_PSyntheticScansPlaneSourceTransform = NULL; // transforms
	m_PSyntheticScansPlaneSourceTransformPolyDataFilter = NULL;
	m_PSyntheticScansProbeFilter = NULL; // probes
	m_PSyntheticScansPolyDataMapper = NULL; // mappers
	m_PSyntheticScansActor = NULL; // actors
	m_PSyntheticScansActorTransform = NULL; // transforms

	// synthetic slices lut
	m_PSyntheticScansWindowLevelLookupTable = vtkWindowLevelLookupTable::New();

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
	m_PContourPosXAxisLineSource = vtkLineSource::New();
	m_PContourPosXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PContourPosXAxisAxesTubeFilter->SetInput(m_PContourPosXAxisLineSource->GetOutput());
	m_PContourPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PContourPosXAxisPolyDataMapper->SetInput(m_PContourPosXAxisAxesTubeFilter->GetOutput());
	m_PContourPosXAxisActor = vtkActor::New();
	m_PContourPosXAxisActor->SetMapper(m_PContourPosXAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PContourPosXAxisActor);
	m_PContourPosXAxisActor->VisibilityOff();
	
	// north contour axis
	m_PContourPosYAxisLineSource = vtkLineSource::New();
	m_PContourPosYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PContourPosYAxisAxesTubeFilter->SetInput(m_PContourPosYAxisLineSource->GetOutput());
	m_PContourPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PContourPosYAxisPolyDataMapper->SetInput(m_PContourPosYAxisAxesTubeFilter->GetOutput());
	m_PContourPosYAxisActor = vtkActor::New();
	m_PContourPosYAxisActor->SetMapper(m_PContourPosYAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PContourPosYAxisActor);
	m_PContourPosYAxisActor->VisibilityOff();

	// west contour axis
	m_PContourNegXAxisLineSource = vtkLineSource::New();
	m_PContourNegXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PContourNegXAxisAxesTubeFilter->SetInput(m_PContourNegXAxisLineSource->GetOutput());
	m_PContourNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PContourNegXAxisPolyDataMapper->SetInput(m_PContourNegXAxisAxesTubeFilter->GetOutput());
	m_PContourNegXAxisActor = vtkActor::New();
	m_PContourNegXAxisActor->SetMapper(m_PContourNegXAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PContourNegXAxisActor);
	m_PContourNegXAxisActor->VisibilityOff();
	
	// south contour axis
	m_PContourNegYAxisLineSource = vtkLineSource::New();
	m_PContourNegYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PContourNegYAxisAxesTubeFilter->SetInput(m_PContourNegYAxisLineSource->GetOutput());
	m_PContourNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PContourNegYAxisPolyDataMapper->SetInput(m_PContourNegYAxisAxesTubeFilter->GetOutput());
	m_PContourNegYAxisActor = vtkActor::New();
	m_PContourNegYAxisActor->SetMapper(m_PContourNegYAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PContourNegYAxisActor);
	m_PContourNegYAxisActor->VisibilityOff();

	// east global axis
	m_PPosXAxisLineSource = vtkLineSource::New();
	m_PPosXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosXAxisAxesTubeFilter->SetInput(m_PPosXAxisLineSource->GetOutput());
	m_PPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosXAxisPolyDataMapper->SetInput(m_PPosXAxisAxesTubeFilter->GetOutput());
	m_PPosXAxisActor = vtkActor::New();
	m_PPosXAxisActor->SetMapper(m_PPosXAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PPosXAxisActor);
	m_PPosXAxisActor->VisibilityOff();
	
	// north global axis
	m_PPosYAxisLineSource = vtkLineSource::New();
	m_PPosYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosYAxisAxesTubeFilter->SetInput(m_PPosYAxisLineSource->GetOutput());
	m_PPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosYAxisPolyDataMapper->SetInput(m_PPosYAxisAxesTubeFilter->GetOutput());
	m_PPosYAxisActor = vtkActor::New();
	m_PPosYAxisActor->SetMapper(m_PPosYAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PPosYAxisActor);
	m_PPosYAxisActor->VisibilityOff();

	// west global axis
	m_PNegXAxisLineSource = vtkLineSource::New();
	m_PNegXAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegXAxisAxesTubeFilter->SetInput(m_PNegXAxisLineSource->GetOutput());
	m_PNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegXAxisPolyDataMapper->SetInput(m_PNegXAxisAxesTubeFilter->GetOutput());
	m_PNegXAxisActor = vtkActor::New();
	m_PNegXAxisActor->SetMapper(m_PNegXAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PNegXAxisActor);
	m_PNegXAxisActor->VisibilityOff();

	// south global axis
	m_PNegYAxisLineSource = vtkLineSource::New();
	m_PNegYAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegYAxisAxesTubeFilter->SetInput(m_PNegYAxisLineSource->GetOutput());
	m_PNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegYAxisPolyDataMapper->SetInput(m_PNegYAxisAxesTubeFilter->GetOutput());
	m_PNegYAxisActor = vtkActor::New();
	m_PNegYAxisActor->SetMapper(m_PNegYAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PNegYAxisActor);
	m_PNegYAxisActor->VisibilityOff();

	// 3d display
	// 1st landmark
	m_PLandmark1SphereSource = vtkSphereSource::New();
	m_PLandmark1PolyDataMapper = vtkPolyDataMapper::New();
	m_PLandmark1PolyDataMapper->SetInput(m_PLandmark1SphereSource->GetOutput());
	m_PLandmark1Actor = vtkActor::New();
	m_PLandmark1Actor->SetMapper(m_PLandmark1PolyDataMapper);
	m_PLandmark1Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PLandmark1Actor);

	// 2nd landmark
	m_PLandmark2SphereSource = vtkSphereSource::New();
	m_PLandmark2PolyDataMapper = vtkPolyDataMapper::New();
	m_PLandmark2PolyDataMapper->SetInput(m_PLandmark2SphereSource->GetOutput());
	m_PLandmark2Actor = vtkActor::New();
	m_PLandmark2Actor->SetMapper(m_PLandmark2PolyDataMapper);
	m_PLandmark2Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PLandmark2Actor);

	// 3rd landmark
	m_PLandmark3SphereSource = vtkSphereSource::New();
	m_PLandmark3PolyDataMapper = vtkPolyDataMapper::New();
	m_PLandmark3PolyDataMapper->SetInput(m_PLandmark3SphereSource->GetOutput());
	m_PLandmark3Actor = vtkActor::New();
	m_PLandmark3Actor->SetMapper(m_PLandmark3PolyDataMapper);
	m_PLandmark3Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PLandmark3Actor);

	// 4th landmark
	m_PLandmark4SphereSource = vtkSphereSource::New();
	m_PLandmark4PolyDataMapper = vtkPolyDataMapper::New();
	m_PLandmark4PolyDataMapper->SetInput(m_PLandmark4SphereSource->GetOutput());
	m_PLandmark4Actor = vtkActor::New();
	m_PLandmark4Actor->SetMapper(m_PLandmark4PolyDataMapper);
	m_PLandmark4Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PLandmark4Actor);

	// L1 to L2 line (action)
	m_PL1L2LineSource = vtkLineSource::New();
	m_PL1L2TubeFilter = vtkTubeFilter::New();
	m_PL1L2TubeFilter->SetInput(m_PL1L2LineSource->GetOutput());
	m_PL1L2PolyDataMapper = vtkPolyDataMapper::New();
	m_PL1L2PolyDataMapper->SetInput(m_PL1L2TubeFilter->GetOutput());
	m_PL1L2Actor = vtkActor::New();
	m_PL1L2Actor->SetMapper(m_PL1L2PolyDataMapper);
	m_PL1L2Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PL1L2Actor);

	// L2 to L3 line
	m_PL2L3LineSource = vtkLineSource::New();
	m_PL2L3TubeFilter = vtkTubeFilter::New();
	m_PL2L3TubeFilter->SetInput(m_PL2L3LineSource->GetOutput());
	m_PL2L3PolyDataMapper = vtkPolyDataMapper::New();
	m_PL2L3PolyDataMapper->SetInput(m_PL2L3TubeFilter->GetOutput());
	m_PL2L3Actor = vtkActor::New();
	m_PL2L3Actor->SetMapper(m_PL2L3PolyDataMapper);
	m_PL2L3Actor->VisibilityOff();
	m_PRenderer->AddActor(m_PL2L3Actor);

	// positive z global axis
	m_PPosZAxisLineSource = vtkLineSource::New();
	m_PPosZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PPosZAxisAxesTubeFilter->SetInput(m_PPosZAxisLineSource->GetOutput());
	m_PPosZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PPosZAxisPolyDataMapper->SetInput(m_PPosZAxisAxesTubeFilter->GetOutput());
	m_PPosZAxisActor = vtkActor::New();
	m_PPosZAxisActor->SetMapper(m_PPosZAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PPosZAxisActor);
	m_PPosZAxisActor->VisibilityOff();

	// negative z global axis
	m_PNegZAxisLineSource = vtkLineSource::New();
	m_PNegZAxisAxesTubeFilter = vtkTubeFilter::New();
	m_PNegZAxisAxesTubeFilter->SetInput(m_PNegZAxisLineSource->GetOutput());
	m_PNegZAxisPolyDataMapper = vtkPolyDataMapper::New();
	m_PNegZAxisPolyDataMapper->SetInput(m_PNegZAxisAxesTubeFilter->GetOutput());
	m_PNegZAxisActor = vtkActor::New();
	m_PNegZAxisActor->SetMapper(m_PNegZAxisPolyDataMapper);
	m_PRenderer->AddActor(m_PNegZAxisActor);
	m_PNegZAxisActor->VisibilityOff();


	// muscle transform pipeline
  // Transform1 -> Transform2 -> Normals -> Mapper
	m_PMuscleTransform1 = vtkTransform::New();
	m_PMuscleTransform1PolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PMuscleTransform1PolyDataFilter->SetInput(muscle);
	m_PMuscleTransform1PolyDataFilter->SetTransform(m_PMuscleTransform1);
	m_PMuscleTransform2 = vtkTransform::New();
	m_PMuscleTransform2PolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PMuscleTransform2PolyDataFilter->SetInput(m_PMuscleTransform1PolyDataFilter->GetOutput());
	m_PMuscleTransform2PolyDataFilter->SetTransform(m_PMuscleTransform2);
	m_PMusclePolyDataNormals = vtkPolyDataNormals::New();
	m_PMusclePolyDataNormals->SetInput(m_PMuscleTransform2PolyDataFilter->GetOutput());
	m_PMusclePolyDataNormals->FlipNormalsOn();
	m_PMusclePolyDataMapper = vtkPolyDataMapper::New();
	m_PMusclePolyDataMapper->SetInput(m_PMusclePolyDataNormals->GetOutput());
	m_PMuscleLODActor = vtkLODActor::New();
	m_PMuscleLODActor->SetMapper(m_PMusclePolyDataMapper);
	m_PRenderer->AddActor(m_PMuscleLODActor);
	m_PMuscleLODActor->VisibilityOff();   // visibility off, but we can switch it on if we want to (see later)


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
	m_PContourPlane = vtkPlane::New();
	m_PContourCutter = vtkCutter::New();
	m_PContourCutter->SetCutFunction(m_PContourPlane);
	m_PContourCutter->SetInput(m_PMuscleTransform2PolyDataFilter->GetOutput());

	m_PContourCutterTransform = vtkTransform::New(); //to put back to z=0 plane
	m_PContourCutterTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PContourCutterTransformPolyDataFilter->SetInput(m_PContourCutter->GetOutput());
	m_PContourCutterTransformPolyDataFilter->SetTransform(m_PContourCutterTransform);

	m_PContourTubeFilter = vtkTubeFilter::New();
	m_PContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_PContourTubeFilter->SetNumberOfSides(12);
	m_PContourTubeFilter->SetInput(m_PContourCutterTransformPolyDataFilter->GetOutput());
	
	m_PContourPolyDataMapper = vtkPolyDataMapper::New();
	m_PContourPolyDataMapper->SetInput(m_PContourTubeFilter->GetOutput());
	
	m_PContourActor = vtkActor::New();
	m_PContourActor->SetMapper(m_PContourPolyDataMapper);
	m_PContourActor->VisibilityOff();
	m_PRenderer->AddActor(m_PContourActor);
	m_PContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	
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
	m_PX0ZNPlane = vtkPlane::New(); // xOz north
	m_PX0ZNPlane->SetNormal(0.0, 1.0, 0.0);

	m_PX0ZSPlane = vtkPlane::New(); // x0z south
	m_PX0ZSPlane->SetNormal(0.0, -1.0, 0.0);
	
	m_PY0ZEPlane = vtkPlane::New(); // y0z east
	m_PY0ZEPlane->SetNormal(1.0, 0.0, 0.0);

	m_PY0ZWPlane = vtkPlane::New(); // y0z west
	m_PY0ZWPlane->SetNormal(-1.0, 0.0, 0.0);

	// north-east
	m_PNEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PNEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PNEContourTubeFilter =  vtkTubeFilter::New();
	m_PNEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PNEContourPolyDataMapper = vtkPolyDataMapper::New();
	m_PNEContourActor = vtkActor::New();
	
	m_PNEContourX0ZPlaneClipPolyData->SetInput(m_PContourCutter->GetOutput()); // first cut plane
	m_PNEContourX0ZPlaneClipPolyData->SetClipFunction(m_PX0ZNPlane);
	m_PNEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PNEContourY0ZPlaneClipPolyData->SetInput(m_PNEContourX0ZPlaneClipPolyData->GetOutput()); // second cut plane
	m_PNEContourY0ZPlaneClipPolyData->SetClipFunction(m_PY0ZEPlane);
	m_PNEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PNEContourTransformPolyDataFilter->SetInput(m_PNEContourY0ZPlaneClipPolyData->GetOutput());

	m_PNEContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_PNEContourTubeFilter->SetNumberOfSides(12);
	m_PNEContourTubeFilter->SetInput(m_PNEContourTransformPolyDataFilter->GetOutput());

	m_PNEContourPolyDataMapper->SetInput(m_PNEContourTubeFilter->GetOutput());
	m_PNEContourActor->SetMapper(m_PNEContourPolyDataMapper);
	m_PRenderer->AddActor(m_PNEContourActor);
	m_PNEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_PNEContourActor->VisibilityOff();

	// north-west
	m_PNWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PNWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PNWContourTubeFilter =  vtkTubeFilter::New();
	m_PNWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PNWContourPolyDataMapper = vtkPolyDataMapper::New();
	m_PNWContourActor = vtkActor::New();

	m_PNWContourX0ZPlaneClipPolyData->SetInput(m_PContourCutter->GetOutput());
	m_PNWContourX0ZPlaneClipPolyData->SetClipFunction(m_PX0ZNPlane);
	m_PNWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

	m_PNWContourY0ZPlaneClipPolyData->SetInput(m_PNWContourX0ZPlaneClipPolyData->GetOutput());
	m_PNWContourY0ZPlaneClipPolyData->SetClipFunction(m_PY0ZWPlane);
	m_PNWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

	m_PNWContourTransformPolyDataFilter->SetInput(m_PNWContourY0ZPlaneClipPolyData->GetOutput());

	m_PNWContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_PNWContourTubeFilter->SetNumberOfSides(12);
	m_PNWContourTubeFilter->SetInput(m_PNWContourTransformPolyDataFilter->GetOutput());

	m_PNWContourPolyDataMapper->SetInput(m_PNWContourTubeFilter->GetOutput());
	m_PNWContourActor->SetMapper(m_PNWContourPolyDataMapper);
	m_PRenderer->AddActor(m_PNWContourActor);
	m_PNWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_PNWContourActor->VisibilityOff();

	// south-east
	m_PSEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PSEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PSEContourTubeFilter =  vtkTubeFilter::New();
	m_PSEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PSEContourPolyDataMapper = vtkPolyDataMapper::New();
	m_PSEContourActor = vtkActor::New();
	
	m_PSEContourX0ZPlaneClipPolyData->SetInput(m_PContourCutter->GetOutput());
	m_PSEContourX0ZPlaneClipPolyData->SetClipFunction(m_PX0ZSPlane);
	m_PSEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PSEContourY0ZPlaneClipPolyData->SetInput(m_PSEContourX0ZPlaneClipPolyData->GetOutput());
	m_PSEContourY0ZPlaneClipPolyData->SetClipFunction(m_PY0ZEPlane);
	m_PSEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PSEContourTransformPolyDataFilter->SetInput(m_PSEContourY0ZPlaneClipPolyData->GetOutput());

	m_PSEContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_PSEContourTubeFilter->SetNumberOfSides(12);
	m_PSEContourTubeFilter->SetInput(m_PSEContourTransformPolyDataFilter->GetOutput());

	m_PSEContourPolyDataMapper->SetInput(m_PSEContourTubeFilter->GetOutput());
	m_PSEContourActor->SetMapper(m_PSEContourPolyDataMapper);
	m_PRenderer->AddActor(m_PSEContourActor);
	m_PSEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_PSEContourActor->VisibilityOff();

	// south-west
	m_PSWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PSWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
	m_PSWContourTubeFilter =  vtkTubeFilter::New();
	m_PSWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	m_PSWContourPolyDataMapper = vtkPolyDataMapper::New();
	m_PSWContourActor = vtkActor::New();
	
	m_PSWContourX0ZPlaneClipPolyData->SetInput(m_PContourCutter->GetOutput());
	m_PSWContourX0ZPlaneClipPolyData->SetClipFunction(m_PX0ZSPlane);
	m_PSWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PSWContourY0ZPlaneClipPolyData->SetInput(m_PSWContourX0ZPlaneClipPolyData->GetOutput());
	m_PSWContourY0ZPlaneClipPolyData->SetClipFunction(m_PY0ZWPlane);
	m_PSWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();
	
	m_PSWContourTransformPolyDataFilter->SetInput(m_PSWContourY0ZPlaneClipPolyData->GetOutput());

	m_PSWContourTubeFilter->SetRadius(m_TubeFilterRadius);
    m_PSWContourTubeFilter->SetNumberOfSides(12);
	m_PSWContourTubeFilter->SetInput(m_PSWContourTransformPolyDataFilter->GetOutput());

	m_PSWContourPolyDataMapper->SetInput(m_PSWContourTubeFilter->GetOutput());
	m_PSWContourActor->SetMapper(m_PSWContourPolyDataMapper);
	m_PRenderer->AddActor(m_PSWContourActor);
	m_PSWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	m_PSWContourActor->VisibilityOff();
	
	// display information
	m_PTextMapperX = vtkTextMapper::New();
	m_PScaledTextActorX = vtkScaledTextActor::New();
	m_PScaledTextActorX->SetMapper(m_PTextMapperX);
	m_PScaledTextActorX->VisibilityOff();
	m_PScaledTextActorX->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	m_PRenderer->AddActor2D(m_PScaledTextActorX);
	
	m_PTextMapperY = vtkTextMapper::New();
	m_PScaledTextActorY = vtkScaledTextActor::New();
	m_PScaledTextActorY->SetMapper(m_PTextMapperY);
	m_PScaledTextActorY->VisibilityOff();
	m_PScaledTextActorY->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	m_PRenderer->AddActor2D(m_PScaledTextActorY);
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
		m_DUnitVector12[i] = l1[i] - l2[i];

	// length of vector l1 - l2
	m_DLength12 = sqrt(pow(m_DUnitVector12[0], 2.0) + pow(m_DUnitVector12[1], 2.0) + pow(m_DUnitVector12[2], 2.0));

	// unit vector along l1 - l2
	for(i = 0; i < 3; i++)
		m_DUnitVector12[i] = m_DUnitVector12[i] / m_DLength12;

	// vector l2 - l3
	for(i = 0; i < 3; i++)
		m_DUnitVector23[i] = l2[i] - l3[i];

	// length of vector l2 - l3
	m_DLength23 = sqrt(pow(m_DUnitVector23[0], 2.0) + pow(m_DUnitVector23[1], 2.0) + pow(m_DUnitVector23[2], 2.0));

	// unit vector along l2 - l3
	for(i = 0; i < 3; i++)
		m_DUnitVector23[i] = m_DUnitVector23[i] / m_DLength23;

	// overall length
	m_DOverallLength = m_DLength12 + m_DLength23;
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
	m_PMuscleTransform1->SetMatrix(finalm);

	// execute
	m_PMuscleTransform1PolyDataFilter->SetTransform(this->m_PMuscleTransform1);

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
	m_PSlicesm->Identity();
	m_PSlicesm = MultiplyMatrix4x4(rotaym, m_PSlicesm); // rotation y
	m_PSlicesm = MultiplyMatrix4x4(rotaxm, m_PSlicesm); // rotation x

	// tranformation matrix
	m_PFinalm->Identity();
	m_PFinalm = MultiplyMatrix4x4(scalem, m_PFinalm); // 4. scaling
	m_PFinalm = MultiplyMatrix4x4(rotaym, m_PFinalm); // 3. rotation y
	m_PFinalm = MultiplyMatrix4x4(rotaxm, m_PFinalm); // 2. rotation x
	m_PFinalm = MultiplyMatrix4x4(transm, m_PFinalm); // 1. translation
	
	//vtkMatrix4x4 *finalm = vtkMatrix4x4::New();
	//finalm->Identity();
	//finalm = MultiplyMatrix4x4(scalem, finalm); // 4. scaling
	//finalm = MultiplyMatrix4x4(rotaym, finalm); // 3. rotation y
	//finalm = MultiplyMatrix4x4(rotaxm, finalm); // 2. rotation x
	//finalm = MultiplyMatrix4x4(transm, finalm); // 1. translation


	// transform muscle
	m_PMuscleTransform2->SetMatrix(m_PFinalm);

	// execute
	m_PMuscleTransform2PolyDataFilter->SetTransform(this->m_PMuscleTransform2);

	double newp1[3];
	double newp2[3];

	// transform insertions (tests)
	vtkTransform *transf = vtkTransform::New();
	transf->SetMatrix(m_PFinalm);
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
	m_PMuscleTransform2PolyDataFilter->Update();

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
				m_PMuscleTransform2PolyDataFilter->GetOutput()->GetBounds(bounds);

				// size
				if (fabs(bounds[0]) > fabs(bounds[1]))
					m_NSyntheticScansXSize = Factor * 2.0 * fabs(bounds[0]);
				else
					m_NSyntheticScansXSize = Factor * 2.0 * fabs(bounds[1]);

				if (fabs(bounds[2]) > fabs(bounds[3]))
					m_NSyntheticScansYSize = Factor * 2.0 * fabs(bounds[2]);
				else
					m_NSyntheticScansYSize = Factor * 2.0 * fabs(bounds[3]);
				break;

		case 2: // slicing axis is two lines
				m_NSyntheticScansXSize = 200.0;
				m_NSyntheticScansYSize = 200.0;
				break;
	}

	//
	// resolution
	assert(m_SyntheticScansGrain > 0 && m_SyntheticScansGrain < 6);

	// set
	if (m_SyntheticScansGrain == 1) // x 1/3
	{
		m_NSyntheticScansXResolution = 0.3 * m_NSyntheticScansXSize;
		m_NSyntheticScansYResolution = 0.3 * m_NSyntheticScansYSize;
	}
	else
	if (m_SyntheticScansGrain == 2) // x 1/2
	{
		m_NSyntheticScansXResolution = 0.5 * m_NSyntheticScansXSize;
		m_NSyntheticScansYResolution = 0.5 * m_NSyntheticScansYSize;
	}
	else
 if (m_SyntheticScansGrain == 3) // x1
	{
		m_NSyntheticScansXResolution = 1.0 * m_NSyntheticScansXSize;
		m_NSyntheticScansYResolution = 1.0 * m_NSyntheticScansYSize;
	}
	else
	if (m_SyntheticScansGrain == 4) // x2
	{
		m_NSyntheticScansXResolution = 2.0 * m_NSyntheticScansXSize;
		m_NSyntheticScansYResolution = 2.0 * m_NSyntheticScansYSize;
	}
	else // x3
	{
		m_NSyntheticScansXResolution = 3.0 * m_NSyntheticScansXSize;
		m_NSyntheticScansYResolution = 3.0 * m_NSyntheticScansYSize;
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
	m_PSyntheticScansPlaneSource = new vtkPlaneSource* [n] ;
	assert(!(m_PSyntheticScansPlaneSource == NULL));

	// allocate plane transforms
	m_PSyntheticScansPlaneSourceTransform = new vtkTransform* [n];
	assert(!(m_PSyntheticScansPlaneSourceTransform == NULL));

	//
	m_PSyntheticScansPlaneSourceTransformPolyDataFilter = new vtkTransformPolyDataFilter* [n];
	assert(!(m_PSyntheticScansPlaneSourceTransformPolyDataFilter == NULL));

	// allocate probes
	m_PSyntheticScansProbeFilter = new vtkProbeFilter* [n];
	assert(!(m_PSyntheticScansProbeFilter == NULL));

	// allocate mappers
	m_PSyntheticScansPolyDataMapper = new vtkPolyDataMapper* [n];
	assert(!(m_PSyntheticScansPolyDataMapper == NULL));

	// allocate actors
	m_PSyntheticScansActor = new vtkActor* [n];
	assert(!(m_PSyntheticScansActor == NULL));

	// allocate actor transforms
	m_PSyntheticScansActorTransform = new vtkTransform* [n];
	assert(!(m_PSyntheticScansActorTransform == NULL));

	// initial scalar value min/max
	m_SyntheticScansMinScalarValue = 100000;
	m_SyntheticScansMaxScalarValue = -100000;

	// scalar value holder
	double scalars[2];

	//
	for(int i = 0; i < n; i++)
	{
		// allocate objects
		m_PSyntheticScansPlaneSource[i] = vtkPlaneSource::New();
		assert(!(m_PSyntheticScansPlaneSource[i] == NULL));

		//
		m_PSyntheticScansPlaneSourceTransform[i] = vtkTransform::New();
		assert(!(m_PSyntheticScansPlaneSourceTransform[i] == NULL));

		//
		m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i] = vtkTransformPolyDataFilter::New();
		assert(!(m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i] == NULL));
		
		//
		m_PSyntheticScansProbeFilter[i] = vtkProbeFilter::New();
		assert(!(m_PSyntheticScansProbeFilter[i] == NULL));

		//
		m_PSyntheticScansPolyDataMapper[i] = vtkPolyDataMapper::New();
		assert(!(m_PSyntheticScansPolyDataMapper[i] == NULL));

		//
		m_PSyntheticScansActor[i] = vtkActor::New();
		assert(!(m_PSyntheticScansActor[i] == NULL));

		//
		m_PSyntheticScansActorTransform[i] = vtkTransform::New();
		assert(!(m_PSyntheticScansActorTransform[i] == NULL));

		// plane source resolution
		m_PSyntheticScansPlaneSource[i]->SetResolution(m_NSyntheticScansXResolution, m_NSyntheticScansYResolution);
		
		// plane source transformation matrix
		m_PSyntheticScansPlaneSourceTransform[i]->SetMatrix(this->GetPlaneSourceTransformOfSyntheticScans(i));
		m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i]->SetInput(m_PSyntheticScansPlaneSource[i]->GetOutput());
		m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i]->SetTransform(m_PSyntheticScansPlaneSourceTransform[i]);
		
		// actor transformation matrix
		m_PSyntheticScansActorTransform[i]->SetMatrix(this->GetActorTransformOfSyntheticScans(i));
		m_PSyntheticScansActor[i]->SetUserTransform(m_PSyntheticScansActorTransform[i]);
		m_PSyntheticScansActor[i]->SetMapper(m_PSyntheticScansPolyDataMapper[i]);
		m_PSyntheticScansActor[i]->VisibilityOff();
		m_PRenderer->AddActor(m_PSyntheticScansActor[i]);

		// probe
		m_PSyntheticScansProbeFilter[i]->SetInput(m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i]->GetOutput());
		m_PSyntheticScansProbeFilter[i]->SetSource(m_Scans);
		m_PSyntheticScansProbeFilter[i]->Update();

		// asjust min/max scalar values
		m_PSyntheticScansProbeFilter[i]->GetPolyDataOutput()->GetScalarRange(scalars);
		if (scalars[0] < m_SyntheticScansMinScalarValue)
			m_SyntheticScansMinScalarValue = scalars[0];
		if (scalars[1] > m_SyntheticScansMaxScalarValue)
			m_SyntheticScansMaxScalarValue = scalars[1];

		// probe mapper 
		m_PSyntheticScansPolyDataMapper[i]->SetInput(m_PSyntheticScansProbeFilter[i]->GetPolyDataOutput());
		
		// i-th scan: handles large datasets
		// at the expense of slower rendering
		/*
		m_pSyntheticScansPolyDataMapper[i]->ImmediateModeRenderingOn();
		*/

		// clean up
		m_PSyntheticScansActorTransform[i]->Delete();
		m_PSyntheticScansPolyDataMapper[i]->Delete();
		m_PSyntheticScansProbeFilter[i]->Delete();
		m_PSyntheticScansPlaneSourceTransformPolyDataFilter[i]->Delete();
		m_PSyntheticScansPlaneSourceTransform[i]->Delete();
		m_PSyntheticScansPlaneSource[i]->Delete();
	}

	// window
	m_PWindow = (m_SyntheticScansMaxScalarValue - m_SyntheticScansMinScalarValue);

	// level
	m_PLevel = 0.5 * (m_SyntheticScansMinScalarValue + m_SyntheticScansMaxScalarValue);

	// lut
	m_PSyntheticScansWindowLevelLookupTable->SetTableRange(m_SyntheticScansMinScalarValue, m_SyntheticScansMaxScalarValue);
	m_PSyntheticScansWindowLevelLookupTable->SetHueRange(0.0, 0.0);
	m_PSyntheticScansWindowLevelLookupTable->SetSaturationRange(0.0, 0.0);
	m_PSyntheticScansWindowLevelLookupTable->SetValueRange(0.0, 1.0);
	m_PSyntheticScansWindowLevelLookupTable->SetNumberOfColors(1024);
	m_PSyntheticScansWindowLevelLookupTable->SetWindow(m_PWindow);
	m_PSyntheticScansWindowLevelLookupTable->SetLevel(m_PLevel);
	m_PSyntheticScansWindowLevelLookupTable->Build();
	
	for(int j = 0; j < n; j++)
	{
		m_PSyntheticScansPolyDataMapper[j]->SetLookupTable(m_PSyntheticScansWindowLevelLookupTable);
		m_PSyntheticScansPolyDataMapper[j]->UseLookupTableScalarRangeOn();
		m_PSyntheticScansPolyDataMapper[j]->SetColorModeToMapScalars();
	}

	// delete objects
	delete m_PSyntheticScansActorTransform;
	delete m_PSyntheticScansProbeFilter;
	delete m_PSyntheticScansPlaneSourceTransformPolyDataFilter;
	delete m_PSyntheticScansPlaneSourceTransform;
	delete m_PSyntheticScansPlaneSource;
	delete m_PSyntheticScansPolyDataMapper;

	// set to scan 0 display
	m_NSyntheticScansCurrentId = 0; // current id
	m_PSyntheticScansActor[0]->VisibilityOn(); // actor on
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
				currentlength = m_DLength12 / (n - 1) * s;

				// origin
				for(i = 0; i < 3; i++)
					p[i] = l2[i] + currentlength * m_DUnitVector12[i];
				break;

		case 2: // slicing axis is double line
				// start from landmark 3 (low)
				// ending at landmark 1 (high)
				
				// current length
				currentlength = m_DOverallLength / (n - 1) * s;

				// origin
				for(i = 0; i < 3; i++)
				{
					if (currentlength < m_DLength23)
					{
						p[i] = l3[i] + currentlength * m_DUnitVector23[i];
					}
					else
					{
						p[i] = l2[i] + (currentlength - m_DLength23) * m_DUnitVector12[i];
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
				scalem->SetElement(0, 0, m_NSyntheticScansXSize);
				scalem->SetElement(1, 1, m_NSyntheticScansYSize);

				// s-th synthetic scan: plane source origin
				// (as a translation transformation)
				this->GetPlaneSourceOriginOfSyntheticScans(s, p); 
				transm->Identity();
				transm->SetElement(0, 3, p[0]); // x
				transm->SetElement(1, 3, p[1]); // y
				transm->SetElement(2, 3, p[2]); // z

				//
				inversem_pslicesm->Identity();
				inversem_pslicesm->Invert(m_PSlicesm, inversem_pslicesm);

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
				scalem->SetElement(0, 0, m_NSyntheticScansXSize);
				scalem->SetElement(1, 1, m_NSyntheticScansYSize);

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
				currentlength = m_DOverallLength / (n - 1) * s;

				// current normal
				for(i = 0; i < 3; i++)
				{
					normal[i] = (1.0 / m_DOverallLength) * (currentlength * m_DUnitVector12[i] + 
														   (m_DOverallLength - currentlength)  * m_DUnitVector23[i]);
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
				inversem_pslicesm->Invert(m_PSlicesm, inversem_pslicesm);
				
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
	return m_PContourActor;
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
	m_PRenderWindow->Render();
}
//----------------------------------------------------------------------------
vtkTextMapper* medOpMMLModelView::GetTextMapper2()
//----------------------------------------------------------------------------
{
	return m_PTextMapperX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* medOpMMLModelView::GetScaledTextActor2()
//----------------------------------------------------------------------------
{
	return m_PScaledTextActorX;
}
//----------------------------------------------------------------------------
vtkScaledTextActor* medOpMMLModelView::GetScaledTextActor1()
//----------------------------------------------------------------------------
{
	return m_PScaledTextActorY;
}
//----------------------------------------------------------------------------
vtkTextMapper* medOpMMLModelView::GetTextMapper1()
//----------------------------------------------------------------------------
{
	return m_PTextMapperY;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNEContourActor()
//----------------------------------------------------------------------------
{
	return m_PNEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetNWContourActor()
//----------------------------------------------------------------------------
{
	return m_PNWContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetSEContourActor()
//----------------------------------------------------------------------------
{
	return m_PSEContourActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetSWContourActor()
//----------------------------------------------------------------------------
{
	return m_PSWContourActor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetNEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_PNEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetNWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_PNWContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetSEContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_PSEContourTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetSWContourTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_PSWContourTransformPolyDataFilter;
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
	return m_PRenderer->GetActiveCamera();
}

//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	m_PCameraFocalPoint[0] = *fp;
	m_PCameraFocalPoint[1] = *(fp+1);
	m_PCameraFocalPoint[2] = *(fp+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	m_PCameraPosition[0] = *cp;
	m_PCameraPosition[1] = *(cp+1);
	m_PCameraPosition[2] = *(cp+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	m_PCameraClippingRange[0] = *cr;
	m_PCameraClippingRange[1] = *(cr+1);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SaveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	m_PCameraViewUp[0] = *vu;
	m_PCameraViewUp[1] = *(vu+1);
	m_PCameraViewUp[2] = *(vu+2);
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
	*fp = m_PCameraFocalPoint[0];
	*(fp+1) = m_PCameraFocalPoint[1];
	*(fp+2) = m_PCameraFocalPoint[2];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
	*cp = m_PCameraPosition[0];
	*(cp+1) = m_PCameraPosition[1];
	*(cp+2) = m_PCameraPosition[2];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
	*cr = m_PCameraClippingRange[0];
	*(cr+1) = m_PCameraClippingRange[1];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::RetrieveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
	*vu = m_PCameraViewUp[0];
	*(vu+1) = m_PCameraViewUp[1];
	*(vu+2) = m_PCameraViewUp[2];
}

//----------------------------------------------------------------------------
void medOpMMLModelView::AddActor(vtkActor *a)
//----------------------------------------------------------------------------
{
	m_PRenderer->AddActor(a);
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
	return m_PMuscleLODActor;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetTotalNumberOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_NSyntheticScansTotalNumber = n;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetResolutionOfSyntheticScans(int x, int y)
//----------------------------------------------------------------------------
{
	m_NSyntheticScansXResolution = x;
	m_NSyntheticScansYResolution = y;
}
//----------------------------------------------------------------------------
int medOpMMLModelView::GetTotalNumberOfSyntheticScans()
//----------------------------------------------------------------------------
{
	assert(m_NSyntheticScansTotalNumber > 3); // at least 3 synthetic scans
	return m_NSyntheticScansTotalNumber;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::GetResolutionOfSyntheticScans(int *x, int *y)
//----------------------------------------------------------------------------
{
	*x = m_NSyntheticScansXResolution;
	*y = m_NSyntheticScansYResolution;
}

//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetActorOfSyntheticScans(int s)
//----------------------------------------------------------------------------
{
	return m_PSyntheticScansActor[s];
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetCurrentIdOfSyntheticScans(int n)
//----------------------------------------------------------------------------
{
	m_NSyntheticScansCurrentId = n;
}
//----------------------------------------------------------------------------
int medOpMMLModelView::GetCurrentIdOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_NSyntheticScansCurrentId;
}
//----------------------------------------------------------------------------
vtkWindowLevelLookupTable* medOpMMLModelView::GetWindowLevelLookupTableOfSyntheticScans()
//----------------------------------------------------------------------------
{
	return m_PSyntheticScansWindowLevelLookupTable;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::SetSizeOfSyntheticScans(float x, float y)
//----------------------------------------------------------------------------
{
	m_NSyntheticScansXSize = x;
	m_NSyntheticScansYSize = y;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::GetSizeOfSyntheticScans(float *x, float *y)
//----------------------------------------------------------------------------
{
	*x = m_NSyntheticScansXSize;
	*y = m_NSyntheticScansYSize;
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
	m_FlMuscleXScalingFactor = x;
	m_FlMuscleYScalingFactor = y;
}
//----------------------------------------------------------------------------
void medOpMMLModelView::GetXYScalingFactorsOfMuscle(double *x, double *y)
//----------------------------------------------------------------------------
{
	*x = m_FlMuscleXScalingFactor;
	*y = m_FlMuscleYScalingFactor;
}
//----------------------------------------------------------------------------
vtkTransformPolyDataFilter* medOpMMLModelView::GetContourCutterTransformPolyDataFilter()
//----------------------------------------------------------------------------
{
	return m_PContourCutterTransformPolyDataFilter;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetContourPlane()
//----------------------------------------------------------------------------
{
	return m_PContourPlane;
}
//----------------------------------------------------------------------------
vtkTransform* medOpMMLModelView::GetContourCutterTransform()
//----------------------------------------------------------------------------
{
	return m_PContourCutterTransform;
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
	return m_PRenderWindowInteractor;
}
//----------------------------------------------------------------------------
vtkInteractorStyleImage* medOpMMLModelView::GetInteractorStyleImage()
//----------------------------------------------------------------------------
{
	return m_PInteractorStyleImage;
}
//----------------------------------------------------------------------------
vtkInteractorStyleTrackballCamera* medOpMMLModelView::GetInteractorStyleTrackballCamera()
//----------------------------------------------------------------------------
{
	return m_PInteractorStyleTrackballCamera;
}

//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourPositiveXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PContourPosXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourPositiveYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PContourPosYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourNegativeXAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PContourNegXAxisLineSource;
}
//----------------------------------------------------------------------------
vtkLineSource* medOpMMLModelView::GetContourNegativeYAxisLineSource()
//----------------------------------------------------------------------------
{
	return m_PContourNegYAxisLineSource;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourPositiveXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PContourPosXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourPositiveYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PContourPosYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourNegativeXAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PContourNegXAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourNegativeYAxisTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PContourNegYAxisAxesTubeFilter;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourPositiveXAxisActor()
//----------------------------------------------------------------------------
{
	return m_PContourPosXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourPositiveYAxisActor()
//----------------------------------------------------------------------------
{
	return m_PContourPosYAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourNegativeXAxisActor()
//----------------------------------------------------------------------------
{
	return m_PContourNegXAxisActor;
}
//----------------------------------------------------------------------------
vtkActor* medOpMMLModelView::GetContourNegativeYAxisActor()
//----------------------------------------------------------------------------
{
	return m_PContourNegYAxisActor;
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
	return m_PX0ZNPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneSouth()
//----------------------------------------------------------------------------
{
	return m_PX0ZSPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneEast()
//----------------------------------------------------------------------------
{
	return m_PY0ZEPlane;
}
//----------------------------------------------------------------------------
vtkPlane* medOpMMLModelView::GetCuttingPlaneWest()
//----------------------------------------------------------------------------
{
	return m_PY0ZWPlane;
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
	if (m_3DDisplay == 0) // standard display
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
	if (m_3DDisplay == 1)
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
	if (m_3DDisplay == 1)
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
	m_PContourPosXAxisAxesTubeFilter->SetRadius(0.5);
    m_PContourPosXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PContourPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PContourPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_PContourPosXAxisActor->VisibilityOn();

	// north
	m_PContourPosYAxisAxesTubeFilter->SetRadius(0.5);
    m_PContourPosYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PContourPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PContourPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_PContourPosYAxisActor->VisibilityOn();

	// west
	m_PContourNegXAxisAxesTubeFilter->SetRadius(0.5);
    m_PContourNegXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PContourNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PContourNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	m_PContourNegXAxisActor->VisibilityOn();

	// south
	m_PContourNegYAxisAxesTubeFilter->SetRadius(0.5);
    m_PContourNegYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PContourNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PContourNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_PContourNegYAxisActor->VisibilityOn();
	
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
	m_PPosXAxisLineSource->SetPoint2(m_NSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_PPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_PPosXAxisActor->VisibilityOn();

	// north
	m_PPosYAxisAxesTubeFilter->SetRadius(0.5);
    m_PPosYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PPosYAxisLineSource->SetPoint2(0.0, m_NSyntheticScansYSize / 2.0, 0.0);
	m_PPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_PPosYAxisActor->VisibilityOn();
	
	// west
	m_PNegXAxisAxesTubeFilter->SetRadius(0.5);
    m_PNegXAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegXAxisLineSource->SetPoint2(-1.0 * m_NSyntheticScansXSize / 2.0, 0.0, 0.0);
	m_PNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
	m_PNegXAxisActor->VisibilityOn();
	
	// south
	m_PNegYAxisAxesTubeFilter->SetRadius(0.5);
    m_PNegYAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegYAxisLineSource->SetPoint2(0.0, -1.0 * m_NSyntheticScansYSize / 2.0, 0.0);
	m_PNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_PNegYAxisActor->VisibilityOn();

	return 1;
}

//----------------------------------------------------------------------------
void medOpMMLModelView::Switch3dDisplayOn()
//----------------------------------------------------------------------------
{
 	// 3d display flag
 	m_3DDisplay = 1;
 
 	// interactor style for 3d display
 	m_PRenderWindowInteractor->SetInteractorStyle(GetInteractorStyleTrackballCamera());
 
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
 		m_PSyntheticScansActor[i]->SetUserTransform(t);
 	}
 
 	// muscle mapped to patient space only, using transform 1
	// thus, transform 2 must simply be an identity transform
 	m_PMuscleTransform2PolyDataFilter->SetTransform(t);
 
 	// set up muscle and switch on
 	m_PMuscleLODActor->VisibilityOn();
 	m_PMuscleLODActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
 	m_PMuscleLODActor->GetProperty()->SetOpacity(0.5);
 
 	// standard contour switch on
 	m_PContourActor->VisibilityOn();
 
 	// sub-contours switch off
 	m_PNEContourActor->VisibilityOff();
 	m_PNWContourActor->VisibilityOff();
 	m_PSEContourActor->VisibilityOff();
 	m_PSWContourActor->VisibilityOff();
 
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
	m_PPosZAxisLineSource->SetPoint2(0.0, 0.0, (m_NSyntheticScansXSize + m_NSyntheticScansYSize) / 2.0);
	m_PPosZAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

	// negative z axis set up
	m_PNegZAxisAxesTubeFilter->SetRadius(1.0);
    m_PNegZAxisAxesTubeFilter->SetNumberOfSides(6);
	m_PNegZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
	m_PNegZAxisLineSource->SetPoint2(0.0, 0.0, -1.0 * (m_NSyntheticScansXSize + m_NSyntheticScansYSize) / 2.0);
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
	m_PLandmark1SphereSource->SetRadius(2.0);
	m_PLandmark1SphereSource->SetThetaResolution(10);
	m_PLandmark1SphereSource->SetPhiResolution(10);
 	m_PLandmark1SphereSource->SetCenter(p1[0], p1[1], p1[2]);
 	m_PLandmark1Actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
	m_PLandmark1Actor->VisibilityOn();
 
	// 2nd landmark set up and switch on
	m_PLandmark2SphereSource->SetRadius(2.0);
	m_PLandmark2SphereSource->SetThetaResolution(10);
	m_PLandmark2SphereSource->SetPhiResolution(10);
 	m_PLandmark2SphereSource->SetCenter(p2[0], p2[1], p2[2]);
 	m_PLandmark2Actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
	m_PLandmark2Actor->VisibilityOn();
 
	// 3rd landmark set up and switch on
	m_PLandmark3SphereSource->SetRadius(2.0);
	m_PLandmark3SphereSource->SetThetaResolution(10);
	m_PLandmark3SphereSource->SetPhiResolution(10);
 	m_PLandmark3SphereSource->SetCenter(p3[0], p3[1], p3[2]);
 	m_PLandmark3Actor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
	m_PLandmark3Actor->VisibilityOn();
 	
	// 4th landmark set up and switch on
 	if (m_4Landmarks == 1)
 	{
		m_PLandmark4SphereSource->SetRadius(2.0);
		m_PLandmark4SphereSource->SetThetaResolution(10);
		m_PLandmark4SphereSource->SetPhiResolution(10);
 		m_PLandmark4SphereSource->SetCenter(p4[0], p4[1], p4[2]);
 		m_PLandmark4Actor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow
		m_PLandmark4Actor->VisibilityOn();	
 	}
 
 	// line of action (L1 to L2) set up and switch on
 	m_PL1L2TubeFilter->SetRadius(1.0);
    m_PL1L2TubeFilter->SetNumberOfSides(6);
	m_PL1L2LineSource->SetPoint1(p1[0], p1[1], p1[2]);
 	m_PL1L2LineSource->SetPoint2(p2[0], p2[1], p2[2]);
	m_PL1L2Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
 	m_PL1L2Actor->VisibilityOn();

	// L2 to L3 line set up and switch on
 	m_PL2L3TubeFilter->SetRadius(1.0);
    m_PL2L3TubeFilter->SetNumberOfSides(6);
	m_PL2L3LineSource->SetPoint1(p2[0], p2[1], p2[2]);
 	m_PL2L3LineSource->SetPoint2(p3[0], p3[1], p3[2]);
	m_PL2L3Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
 	m_PL2L3Actor->VisibilityOn();
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
	return m_PMuscleTransform2PolyDataFilter;
}

//----------------------------------------------------------------------------
vtkMatrix4x4* medOpMMLModelView::GetFinalM()
//----------------------------------------------------------------------------
{
	return m_PFinalm;
}

//----------------------------------------------------------------------------
vtkTubeFilter* medOpMMLModelView::GetContourTubeFilter()
//----------------------------------------------------------------------------
{
	return m_PContourTubeFilter;
}

//----------------------------------------------------------------------------
float medOpMMLModelView::GetSyntheticScansWindow()
//----------------------------------------------------------------------------
{
	//assert(m_PWindow > 0);
	return m_PWindow;
}

//----------------------------------------------------------------------------
float medOpMMLModelView::GetSyntheticScansLevel()
//----------------------------------------------------------------------------
{
	return m_PLevel;
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
