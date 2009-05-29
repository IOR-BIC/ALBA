/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3ModelView.cpp,v $
Language:  C++
Date:      $Date: 2009-05-29 11:05:29 $
Version:   $Revision: 1.1.2.1 $
Authors:   Mel Krokos
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h"
#include "mafSmartPointer.h"

#include "medOpMML3ModelView.h"

#include "vtkMath.h"
#include "vtkTextSource.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"

//SIL. 24-12-2004: begin
#ifdef VTK_USE_ANSI_STDLIB
#include <sstream>
#endif
//SIL. 24-12-2004: end




//----------------------------------------------------------------------------
// Constructor for model view
medOpMML3ModelView::medOpMML3ModelView( vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume)
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
  m_Finalm = vtkMatrix4x4::New();

  // synthetic slices transform
  m_Slicesm = vtkMatrix4x4::New();

  //
  m_ScalingOccured = FALSE;


  // copies of renderer, window and interactor
  m_Renderer = ren;
  m_RenderWindow = rw;
  m_RenderWindowInteractor = rw->GetInteractor();

  //
  m_Renderer->SetBackground(0.2,0.4,0.6);

  int *size = m_RenderWindow->GetSize();
  assert(size[0] > 0);
  assert(size[1] > 0);

  m_RenderWindow->LineSmoothingOn();


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


  // Set style for 2d display by default
  vtkInteractorStyleImage *style = vtkInteractorStyleImage::New() ;
  m_RenderWindowInteractor->SetInteractorStyle(style);
  style->Delete() ;

  m_CenterHorizontalOffsetSpline = vtkKochanekSpline::New();
  m_CenterHorizontalOffsetSpline->ClosedOff();

  m_CenterVerticalOffsetSpline = vtkKochanekSpline::New();
  m_CenterVerticalOffsetSpline->ClosedOff();

  m_TwistSpline = vtkKochanekSpline::New();
  m_TwistSpline->ClosedOff();

  m_HorizontalTranslationSpline = vtkKochanekSpline::New();
  m_HorizontalTranslationSpline->ClosedOff();

  m_VerticalTranslationSpline = vtkKochanekSpline::New();
  m_VerticalTranslationSpline->ClosedOff();

  m_NorthScalingSpline = vtkKochanekSpline::New();
  m_NorthScalingSpline->ClosedOff();

  m_SouthScalingSpline = vtkKochanekSpline::New();
  m_SouthScalingSpline->ClosedOff();

  m_EastScalingSpline = vtkKochanekSpline::New();
  m_EastScalingSpline->ClosedOff();

  m_WestScalingSpline = vtkKochanekSpline::New();
  m_WestScalingSpline->ClosedOff();


  // synthetic slices
  m_SyntheticScansActor = NULL; // actors, allocated in CreateSyntheticScans()

  // synthetic slices lut
  m_SyntheticScansWindowLevelLookupTable = vtkWindowLevelLookupTable::New();

  //
  m_ScalingFlagStack = vtkIntArray::New();
  m_ScalingFlagStack->SetNumberOfComponents(1);

  // allocate the operations stack, used for undo purposes
  AllocateOperationsStack(5, 2000) ;

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



  //----------------------------------------------------------------------------
  // standard display
  // Contour axes and global axes
  //----------------------------------------------------------------------------

  // east contour axis
  m_ContourPosXAxisLineSource = vtkLineSource::New();
  m_ContourPosXAxisAxesTubeFilter = vtkTubeFilter::New();
  m_ContourPosXAxisAxesTubeFilter->SetInput(m_ContourPosXAxisLineSource->GetOutput());
  m_ContourPosXAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_ContourPosXAxisPolyDataMapper->SetInput(m_ContourPosXAxisAxesTubeFilter->GetOutput());
  m_ContourPosXAxisActor = vtkActor::New();
  m_ContourPosXAxisActor->SetMapper(m_ContourPosXAxisPolyDataMapper);
  m_Renderer->AddActor(m_ContourPosXAxisActor);
  m_ContourPosXAxisActor->VisibilityOff();

  // north contour axis
  m_ContourPosYAxisLineSource = vtkLineSource::New();
  m_ContourPosYAxisAxesTubeFilter = vtkTubeFilter::New();
  m_ContourPosYAxisAxesTubeFilter->SetInput(m_ContourPosYAxisLineSource->GetOutput());
  m_ContourPosYAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_ContourPosYAxisPolyDataMapper->SetInput(m_ContourPosYAxisAxesTubeFilter->GetOutput());
  m_ContourPosYAxisActor = vtkActor::New();
  m_ContourPosYAxisActor->SetMapper(m_ContourPosYAxisPolyDataMapper);
  m_Renderer->AddActor(m_ContourPosYAxisActor);
  m_ContourPosYAxisActor->VisibilityOff();

  // west contour axis
  m_ContourNegXAxisLineSource = vtkLineSource::New();
  m_ContourNegXAxisAxesTubeFilter = vtkTubeFilter::New();
  m_ContourNegXAxisAxesTubeFilter->SetInput(m_ContourNegXAxisLineSource->GetOutput());
  m_ContourNegXAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_ContourNegXAxisPolyDataMapper->SetInput(m_ContourNegXAxisAxesTubeFilter->GetOutput());
  m_ContourNegXAxisActor = vtkActor::New();
  m_ContourNegXAxisActor->SetMapper(m_ContourNegXAxisPolyDataMapper);
  m_Renderer->AddActor(m_ContourNegXAxisActor);
  m_ContourNegXAxisActor->VisibilityOff();

  // south contour axis
  m_ContourNegYAxisLineSource = vtkLineSource::New();
  m_ContourNegYAxisAxesTubeFilter = vtkTubeFilter::New();
  m_ContourNegYAxisAxesTubeFilter->SetInput(m_ContourNegYAxisLineSource->GetOutput());
  m_ContourNegYAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_ContourNegYAxisPolyDataMapper->SetInput(m_ContourNegYAxisAxesTubeFilter->GetOutput());
  m_ContourNegYAxisActor = vtkActor::New();
  m_ContourNegYAxisActor->SetMapper(m_ContourNegYAxisPolyDataMapper);
  m_Renderer->AddActor(m_ContourNegYAxisActor);
  m_ContourNegYAxisActor->VisibilityOff();

  // east global axis
  m_PosXAxisLineSource = vtkLineSource::New();
  m_PosXAxisAxesTubeFilter = vtkTubeFilter::New();
  m_PosXAxisAxesTubeFilter->SetInput(m_PosXAxisLineSource->GetOutput());
  m_PosXAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_PosXAxisPolyDataMapper->SetInput(m_PosXAxisAxesTubeFilter->GetOutput());
  m_PosXAxisActor = vtkActor::New();
  m_PosXAxisActor->SetMapper(m_PosXAxisPolyDataMapper);
  m_Renderer->AddActor(m_PosXAxisActor);
  m_PosXAxisActor->VisibilityOff();

  // north global axis
  m_PosYAxisLineSource = vtkLineSource::New();
  m_PosYAxisAxesTubeFilter = vtkTubeFilter::New();
  m_PosYAxisAxesTubeFilter->SetInput(m_PosYAxisLineSource->GetOutput());
  m_PosYAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_PosYAxisPolyDataMapper->SetInput(m_PosYAxisAxesTubeFilter->GetOutput());
  m_PosYAxisActor = vtkActor::New();
  m_PosYAxisActor->SetMapper(m_PosYAxisPolyDataMapper);
  m_Renderer->AddActor(m_PosYAxisActor);
  m_PosYAxisActor->VisibilityOff();

  // west global axis
  m_NegXAxisLineSource = vtkLineSource::New();
  m_NegXAxisAxesTubeFilter = vtkTubeFilter::New();
  m_NegXAxisAxesTubeFilter->SetInput(m_NegXAxisLineSource->GetOutput());
  m_NegXAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_NegXAxisPolyDataMapper->SetInput(m_NegXAxisAxesTubeFilter->GetOutput());
  m_NegXAxisActor = vtkActor::New();
  m_NegXAxisActor->SetMapper(m_NegXAxisPolyDataMapper);
  m_Renderer->AddActor(m_NegXAxisActor);
  m_NegXAxisActor->VisibilityOff();

  // south global axis
  m_NegYAxisLineSource = vtkLineSource::New();
  m_NegYAxisAxesTubeFilter = vtkTubeFilter::New();
  m_NegYAxisAxesTubeFilter->SetInput(m_NegYAxisLineSource->GetOutput());
  m_NegYAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_NegYAxisPolyDataMapper->SetInput(m_NegYAxisAxesTubeFilter->GetOutput());
  m_NegYAxisActor = vtkActor::New();
  m_NegYAxisActor->SetMapper(m_NegYAxisPolyDataMapper);
  m_Renderer->AddActor(m_NegYAxisActor);
  m_NegYAxisActor->VisibilityOff();



  //----------------------------------------------------------------------------
  // z axis visual pipes
  //----------------------------------------------------------------------------

  // positive z global axis
  m_PosZAxisLineSource = vtkLineSource::New();
  m_PosZAxisAxesTubeFilter = vtkTubeFilter::New();
  m_PosZAxisAxesTubeFilter->SetInput(m_PosZAxisLineSource->GetOutput());
  m_PosZAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_PosZAxisPolyDataMapper->SetInput(m_PosZAxisAxesTubeFilter->GetOutput());
  m_PosZAxisActor = vtkActor::New();
  m_PosZAxisActor->SetMapper(m_PosZAxisPolyDataMapper);
  m_PosZAxisActor->VisibilityOff();
  m_Renderer->AddActor(m_PosZAxisActor);

  // negative z global axis
  m_NegZAxisLineSource = vtkLineSource::New();
  m_NegZAxisAxesTubeFilter = vtkTubeFilter::New();
  m_NegZAxisAxesTubeFilter->SetInput(m_NegZAxisLineSource->GetOutput());
  m_NegZAxisPolyDataMapper = vtkPolyDataMapper::New();
  m_NegZAxisPolyDataMapper->SetInput(m_NegZAxisAxesTubeFilter->GetOutput());
  m_NegZAxisActor = vtkActor::New();
  m_NegZAxisActor->SetMapper(m_NegZAxisPolyDataMapper);
  m_NegZAxisActor->VisibilityOff();
  m_Renderer->AddActor(m_NegZAxisActor);



  //----------------------------------------------------------------------------
  // 4 landmarks and 2 action lines (3d only)
  //----------------------------------------------------------------------------
  // 1st landmark
  m_Landmark1SphereSource = vtkSphereSource::New();
  m_Landmark1PolyDataMapper = vtkPolyDataMapper::New();
  m_Landmark1PolyDataMapper->SetInput(m_Landmark1SphereSource->GetOutput());
  m_Landmark1Actor = vtkActor::New();
  m_Landmark1Actor->SetMapper(m_Landmark1PolyDataMapper);
  m_Landmark1Actor->VisibilityOff();
  m_Renderer->AddActor(m_Landmark1Actor);

  // 2nd landmark
  m_Landmark2SphereSource = vtkSphereSource::New();
  m_Landmark2PolyDataMapper = vtkPolyDataMapper::New();
  m_Landmark2PolyDataMapper->SetInput(m_Landmark2SphereSource->GetOutput());
  m_Landmark2Actor = vtkActor::New();
  m_Landmark2Actor->SetMapper(m_Landmark2PolyDataMapper);
  m_Landmark2Actor->VisibilityOff();
  m_Renderer->AddActor(m_Landmark2Actor);

  // 3rd landmark
  m_Landmark3SphereSource = vtkSphereSource::New();
  m_Landmark3PolyDataMapper = vtkPolyDataMapper::New();
  m_Landmark3PolyDataMapper->SetInput(m_Landmark3SphereSource->GetOutput());
  m_Landmark3Actor = vtkActor::New();
  m_Landmark3Actor->SetMapper(m_Landmark3PolyDataMapper);
  m_Landmark3Actor->VisibilityOff();
  m_Renderer->AddActor(m_Landmark3Actor);

  // 4th landmark
  m_Landmark4SphereSource = vtkSphereSource::New();
  m_Landmark4PolyDataMapper = vtkPolyDataMapper::New();
  m_Landmark4PolyDataMapper->SetInput(m_Landmark4SphereSource->GetOutput());
  m_Landmark4Actor = vtkActor::New();
  m_Landmark4Actor->SetMapper(m_Landmark4PolyDataMapper);
  m_Landmark4Actor->VisibilityOff();
  m_Renderer->AddActor(m_Landmark4Actor);

  // L1 to L2 line (action)
  m_L1L2LineSource = vtkLineSource::New();
  m_L1L2TubeFilter = vtkTubeFilter::New();
  m_L1L2TubeFilter->SetInput(m_L1L2LineSource->GetOutput());
  m_L1L2PolyDataMapper = vtkPolyDataMapper::New();
  m_L1L2PolyDataMapper->SetInput(m_L1L2TubeFilter->GetOutput());
  m_L1L2Actor = vtkActor::New();
  m_L1L2Actor->SetMapper(m_L1L2PolyDataMapper);
  m_L1L2Actor->VisibilityOff();
  m_Renderer->AddActor(m_L1L2Actor);

  // L2 to L3 line
  m_L2L3LineSource = vtkLineSource::New();
  m_L2L3TubeFilter = vtkTubeFilter::New();
  m_L2L3TubeFilter->SetInput(m_L2L3LineSource->GetOutput());
  m_L2L3PolyDataMapper = vtkPolyDataMapper::New();
  m_L2L3PolyDataMapper->SetInput(m_L2L3TubeFilter->GetOutput());
  m_L2L3Actor = vtkActor::New();
  m_L2L3Actor->SetMapper(m_L2L3PolyDataMapper);
  m_L2L3Actor->VisibilityOff();
  m_Renderer->AddActor(m_L2L3Actor);




  //----------------------------------------------------------------------------
  // Muscle transform pipeline
  //
  //              muscle
  //                |           MuscleTransform1 (see MapAtlasToPatient())
  //                |          /
  //     MuscleTransform1PDFilter
  //                |
  //                |           MuscleTransform2 (see MakeActionLineZAxis())
  //                |          /
  //     MuscleTransform2PDFilter ========> Output of Operation
  //                |
  //          MusclePDNormals
  //                |
  //          MusclePDMapper
  //                |
  //                V
  //          MuscleLODActor (3d only)
  //
  //----------------------------------------------------------------------------

  m_MuscleTransform1 = vtkTransform::New();
  m_MuscleTransform1PolyDataFilter = vtkTransformPolyDataFilter::New();
  m_MuscleTransform1PolyDataFilter->SetInput(muscle);
  m_MuscleTransform1PolyDataFilter->SetTransform(m_MuscleTransform1);

  m_MuscleTransform2 = vtkTransform::New();
  m_MuscleTransform2PolyDataFilter = vtkTransformPolyDataFilter::New();
  m_MuscleTransform2PolyDataFilter->SetInput(m_MuscleTransform1PolyDataFilter->GetOutput());
  m_MuscleTransform2PolyDataFilter->SetTransform(m_MuscleTransform2);

  m_MusclePolyDataNormals = vtkPolyDataNormals::New();
  m_MusclePolyDataNormals->SetInput(m_MuscleTransform2PolyDataFilter->GetOutput());
  m_MusclePolyDataNormals->FlipNormalsOn();

  m_MusclePolyDataMapper = vtkPolyDataMapper::New();
  m_MusclePolyDataMapper->SetInput(m_MusclePolyDataNormals->GetOutput());

  m_MuscleLODActor = vtkLODActor::New();
  m_MuscleLODActor->SetMapper(m_MusclePolyDataMapper);
  m_MuscleLODActor->VisibilityOff();   // visibility off, but we can switch it on if we want to (see later)

  m_Renderer->AddActor(m_MuscleLODActor);


  // 2d axes - contour system
  SetContourAxesLengthScale(2.0);



  //----------------------------------------------------------------------------
  // Contour Pipelines: NE, NW, SE, SW and "standard" ContourActor
  //
  //             |
  // MuscleTransform2PolyDataFilter
  //             |
  //             |      ContourPlane (cut function)
  //             V      /
  //       ContourCutter
  //       |  |  |  |  \
  //       |  |  |  |   \
  //       NE NW SE SW   \
  //                      ContourActor (3d only)
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // Standard contour pipeline (3d only)
  //
  //             |
  //        ContourCutter
  //             |          ContourCutterTransform
  //             |         /
  //   ContourCutterTransformPDFilter
  //             |
  //             |
  //      ContourTubeFilter
  //             |
  //             |
  //       ContourPDMapper
  //             |
  //             V
  //        ContourActor (3d only)
  //
  //----------------------------------------------------------------------------
  m_ContourPlane = vtkPlane::New();
  m_ContourCutter = vtkCutter::New();
  m_ContourCutter->SetCutFunction(m_ContourPlane);
  m_ContourCutter->SetInput(m_MuscleTransform2PolyDataFilter->GetOutput());

  m_ContourCutterTransform = vtkTransform::New(); //to put back to z=0 plane
  m_ContourCutterTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_ContourCutterTransformPolyDataFilter->SetInput(m_ContourCutter->GetOutput());
  m_ContourCutterTransformPolyDataFilter->SetTransform(m_ContourCutterTransform);

  m_ContourTubeFilter = vtkTubeFilter::New();
  m_ContourTubeFilter->SetInput(m_ContourCutterTransformPolyDataFilter->GetOutput());
  m_ContourTubeFilter->SetRadius(m_TubeFilterRadius);
  m_ContourTubeFilter->SetNumberOfSides(12);

  m_ContourPolyDataMapper = vtkPolyDataMapper::New();
  m_ContourPolyDataMapper->SetInput(m_ContourTubeFilter->GetOutput());

  m_ContourActor = vtkActor::New();
  m_ContourActor->SetMapper(m_ContourPolyDataMapper);
  m_ContourActor->VisibilityOff();
  m_ContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);

  m_Renderer->AddActor(m_ContourActor);


  
  //----------------------------------------------------------------------------
  // glyphed contour
  //----------------------------------------------------------------------------
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


  //----------------------------------------------------------------------------
  // planes for contour visual pipes
  //----------------------------------------------------------------------------
  m_X0ZNPlane = vtkPlane::New(); // xOz north
  m_X0ZNPlane->SetNormal(0.0, 1.0, 0.0);

  m_X0ZSPlane = vtkPlane::New(); // x0z south
  m_X0ZSPlane->SetNormal(0.0, -1.0, 0.0);

  m_Y0ZEPlane = vtkPlane::New(); // y0z east
  m_Y0ZEPlane->SetNormal(1.0, 0.0, 0.0);

  m_Y0ZWPlane = vtkPlane::New(); // y0z west
  m_Y0ZWPlane->SetNormal(-1.0, 0.0, 0.0);


  //----------------------------------------------------------------------------
  // north-east contour visual pipe
  //
  //             |
  //        ContourCutter
  //             |              X0ZNplane
  //             |             /
  //    NEContourX0ZPlaneClipPD
  //             |              Y0ZEPlane
  //             |             /
  //    NEContourY0ZPlaneClipPD
  //             |                ?
  //             |               /
  //   NEContourTransformPDFilter
  //             |
  //             |
  //      NEContourTubeFilter
  //             |
  //             |
  //       NEContourPDMapper
  //             |
  //             V
  //        NEContourActor
  //
  //----------------------------------------------------------------------------

  m_NEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_NEContourX0ZPlaneClipPolyData->SetInput(m_ContourCutter->GetOutput()); // first cut plane
  m_NEContourX0ZPlaneClipPolyData->SetClipFunction(m_X0ZNPlane);
  m_NEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_NEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_NEContourY0ZPlaneClipPolyData->SetInput(m_NEContourX0ZPlaneClipPolyData->GetOutput()); // second cut plane
  m_NEContourY0ZPlaneClipPolyData->SetClipFunction(m_Y0ZEPlane);
  m_NEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_NEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_NEContourTransformPolyDataFilter->SetInput(m_NEContourY0ZPlaneClipPolyData->GetOutput());

  m_NEContourTubeFilter =  vtkTubeFilter::New();
  m_NEContourTubeFilter->SetInput(m_NEContourTransformPolyDataFilter->GetOutput());
  m_NEContourTubeFilter->SetRadius(m_TubeFilterRadius);
  m_NEContourTubeFilter->SetNumberOfSides(12);

  m_NEContourPolyDataMapper = vtkPolyDataMapper::New();
  m_NEContourPolyDataMapper->SetInput(m_NEContourTubeFilter->GetOutput());

  m_NEContourActor = vtkActor::New();
  m_NEContourActor->SetMapper(m_NEContourPolyDataMapper);
  m_NEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_NEContourActor->VisibilityOff();

  m_Renderer->AddActor(m_NEContourActor);


  //----------------------------------------------------------------------------
  // north-west contour visual pipe
  //
  //             |
  //        ContourCutter
  //             |              X0ZNplane
  //             |             /
  //    NWContourX0ZPlaneClipPD
  //             |              Y0ZWPlane
  //             |             /
  //    NWContourY0ZPlaneClipPD
  //             |                ?
  //             |               /
  //   NWContourTransformPDFilter
  //             |
  //             |
  //      NWContourTubeFilter
  //             |
  //             |
  //       NWContourPDMapper
  //             |
  //             V
  //        NWContourActor
  //
  //----------------------------------------------------------------------------
  m_NWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_NWContourX0ZPlaneClipPolyData->SetInput(m_ContourCutter->GetOutput());
  m_NWContourX0ZPlaneClipPolyData->SetClipFunction(m_X0ZNPlane);
  m_NWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_NWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_NWContourY0ZPlaneClipPolyData->SetInput(m_NWContourX0ZPlaneClipPolyData->GetOutput());
  m_NWContourY0ZPlaneClipPolyData->SetClipFunction(m_Y0ZWPlane);
  m_NWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_NWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_NWContourTransformPolyDataFilter->SetInput(m_NWContourY0ZPlaneClipPolyData->GetOutput());

  m_NWContourTubeFilter =  vtkTubeFilter::New();
  m_NWContourTubeFilter->SetInput(m_NWContourTransformPolyDataFilter->GetOutput());
  m_NWContourTubeFilter->SetRadius(m_TubeFilterRadius);
  m_NWContourTubeFilter->SetNumberOfSides(12);

  m_NWContourPolyDataMapper = vtkPolyDataMapper::New();
  m_NWContourPolyDataMapper->SetInput(m_NWContourTubeFilter->GetOutput());

  m_NWContourActor = vtkActor::New();
  m_NWContourActor->SetMapper(m_NWContourPolyDataMapper);
  m_NWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_NWContourActor->VisibilityOff();

  m_Renderer->AddActor(m_NWContourActor);


  //----------------------------------------------------------------------------
  // south-east contour visual pipe
  //
  //             |
  //        ContourCutter
  //             |              X0ZSplane
  //             |             /
  //    SEContourX0ZPlaneClipPD
  //             |              Y0ZEPlane
  //             |             /
  //    SEContourY0ZPlaneClipPD
  //             |                ?
  //             |               /
  //   SEContourTransformPDFilter
  //             |
  //             |
  //      SEContourTubeFilter
  //             |
  //             |
  //       SEContourPDMapper
  //             |
  //             V
  //        SEContourActor
  //
  //----------------------------------------------------------------------------
  m_SEContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_SEContourX0ZPlaneClipPolyData->SetInput(m_ContourCutter->GetOutput());
  m_SEContourX0ZPlaneClipPolyData->SetClipFunction(m_X0ZSPlane);
  m_SEContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_SEContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_SEContourY0ZPlaneClipPolyData->SetInput(m_SEContourX0ZPlaneClipPolyData->GetOutput());
  m_SEContourY0ZPlaneClipPolyData->SetClipFunction(m_Y0ZEPlane);
  m_SEContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_SEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_SEContourTransformPolyDataFilter->SetInput(m_SEContourY0ZPlaneClipPolyData->GetOutput());

  m_SEContourTubeFilter =  vtkTubeFilter::New();
  m_SEContourTubeFilter->SetInput(m_SEContourTransformPolyDataFilter->GetOutput());
  m_SEContourTubeFilter->SetRadius(m_TubeFilterRadius);
  m_SEContourTubeFilter->SetNumberOfSides(12);

  m_SEContourPolyDataMapper = vtkPolyDataMapper::New();
  m_SEContourPolyDataMapper->SetInput(m_SEContourTubeFilter->GetOutput());

  m_SEContourActor = vtkActor::New();
  m_SEContourActor->SetMapper(m_SEContourPolyDataMapper);
  m_SEContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_SEContourActor->VisibilityOff();

  m_Renderer->AddActor(m_SEContourActor);


  //----------------------------------------------------------------------------
  // south-west contour visual pipe
  //
  //             |
  //        ContourCutter
  //             |              X0ZSplane
  //             |             /
  //    SWContourX0ZPlaneClipPD
  //             |              Y0ZWPlane
  //             |             /
  //    SWContourY0ZPlaneClipPD
  //             |                ?
  //             |               /
  //   SWContourTransformPDFilter
  //             |
  //             |
  //      SWContourTubeFilter
  //             |
  //             |
  //       SWContourPDMapper
  //             |
  //             V
  //        SWContourActor
  //
  //----------------------------------------------------------------------------
  m_SWContourX0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_SWContourX0ZPlaneClipPolyData->SetInput(m_ContourCutter->GetOutput());
  m_SWContourX0ZPlaneClipPolyData->SetClipFunction(m_X0ZSPlane);
  m_SWContourX0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_SWContourY0ZPlaneClipPolyData = vtkClipPolyData::New();
  m_SWContourY0ZPlaneClipPolyData->SetInput(m_SWContourX0ZPlaneClipPolyData->GetOutput());
  m_SWContourY0ZPlaneClipPolyData->SetClipFunction(m_Y0ZWPlane);
  m_SWContourY0ZPlaneClipPolyData->GlobalWarningDisplayOff();

  m_SWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_SWContourTransformPolyDataFilter->SetInput(m_SWContourY0ZPlaneClipPolyData->GetOutput());

  m_SWContourTubeFilter =  vtkTubeFilter::New();
  m_SWContourTubeFilter->SetInput(m_SWContourTransformPolyDataFilter->GetOutput());
  m_SWContourTubeFilter->SetRadius(m_TubeFilterRadius);
  m_SWContourTubeFilter->SetNumberOfSides(12);

  m_SWContourPolyDataMapper = vtkPolyDataMapper::New();
  m_SWContourPolyDataMapper->SetInput(m_SWContourTubeFilter->GetOutput());

  m_SWContourActor = vtkActor::New();
  m_SWContourActor->SetMapper(m_SWContourPolyDataMapper);
  m_SWContourActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_SWContourActor->VisibilityOff();

  m_Renderer->AddActor(m_SWContourActor);


  //----------------------------------------------------------------------------
  // Display text information visual pipes
  // Just set the mapper input to the required text
  //----------------------------------------------------------------------------
  m_TextMapperX = vtkTextMapper::New();
  m_ScaledTextActorX = vtkScaledTextActor::New();
  m_ScaledTextActorX->SetMapper(m_TextMapperX);
  m_ScaledTextActorX->VisibilityOff();
  m_ScaledTextActorX->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_Renderer->AddActor2D(m_ScaledTextActorX);

  m_TextMapperY = vtkTextMapper::New();
  m_ScaledTextActorY = vtkScaledTextActor::New();
  m_ScaledTextActorY->SetMapper(m_TextMapperY);
  m_ScaledTextActorY->VisibilityOff();
  m_ScaledTextActorY->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_Renderer->AddActor2D(m_ScaledTextActorY);
}




//----------------------------------------------------------------------------
// Destructor
medOpMML3ModelView::~medOpMML3ModelView()
//----------------------------------------------------------------------------
{
  m_Finalm->Delete() ;
  m_Slicesm->Delete() ;

  // splines
  m_CenterHorizontalOffsetSpline->Delete() ;
  m_CenterVerticalOffsetSpline->Delete() ;
  m_TwistSpline->Delete() ;
  m_HorizontalTranslationSpline->Delete() ;
  m_VerticalTranslationSpline->Delete() ;
  m_NorthScalingSpline->Delete() ;
  m_SouthScalingSpline->Delete() ;
  m_EastScalingSpline->Delete() ;
  m_WestScalingSpline->Delete() ;

  // lookup table
  m_SyntheticScansWindowLevelLookupTable->Delete() ;

  // scaling stack
  m_ScalingFlagStack->Delete() ;

  // east contour axis
  m_ContourPosXAxisLineSource->Delete() ;
  m_ContourPosXAxisAxesTubeFilter->Delete() ;
  m_ContourPosXAxisPolyDataMapper->Delete() ;
  m_ContourPosXAxisActor->Delete() ;

  // north contour axis
  m_ContourPosYAxisLineSource->Delete() ;
  m_ContourPosYAxisAxesTubeFilter->Delete() ;
  m_ContourPosYAxisPolyDataMapper->Delete() ;
  m_ContourPosYAxisActor->Delete() ;

  // west contour axis
  m_ContourNegXAxisLineSource->Delete() ;
  m_ContourNegXAxisAxesTubeFilter->Delete() ;
  m_ContourNegXAxisPolyDataMapper->Delete() ;
  m_ContourNegXAxisActor->Delete() ;

  // south contour axis
  m_ContourNegYAxisLineSource->Delete() ;
  m_ContourNegYAxisAxesTubeFilter->Delete() ;
  m_ContourNegYAxisPolyDataMapper->Delete() ;
  m_ContourNegYAxisActor->Delete() ;

  // east global axis
  m_PosXAxisLineSource->Delete() ;
  m_PosXAxisAxesTubeFilter->Delete() ;
  m_PosXAxisPolyDataMapper->Delete() ;
  m_PosXAxisActor->Delete() ;

  // north global axis
  m_PosYAxisLineSource->Delete() ;
  m_PosYAxisAxesTubeFilter->Delete() ;
  m_PosYAxisPolyDataMapper->Delete() ; 
  m_PosYAxisActor->Delete() ;

  // west global axis
  m_NegXAxisLineSource->Delete() ;
  m_NegXAxisAxesTubeFilter->Delete() ;
  m_NegXAxisPolyDataMapper->Delete() ; 
  m_NegXAxisActor->Delete() ; 

  // south global axis
  m_NegYAxisLineSource->Delete() ;
  m_NegYAxisAxesTubeFilter->Delete() ;
  m_NegYAxisPolyDataMapper->Delete() ; 
  m_NegYAxisActor->Delete() ; 

  // 1st landmark
  m_Landmark1SphereSource->Delete() ;
  m_Landmark1PolyDataMapper->Delete() ;
  m_Landmark1Actor->Delete() ; 
  // 2nd landmark
  m_Landmark2SphereSource->Delete() ; 
  m_Landmark2PolyDataMapper->Delete() ;
  m_Landmark2Actor->Delete() ;

  // 3rd landmark
  m_Landmark3SphereSource->Delete() ;
  m_Landmark3PolyDataMapper->Delete() ; 
  m_Landmark3Actor->Delete() ;

  // 4th landmark
  m_Landmark4SphereSource->Delete() ;
  m_Landmark4PolyDataMapper->Delete() ; 
  m_Landmark4Actor->Delete() ;

  // L1 to L2 line (action)
  m_L1L2LineSource->Delete() ;
  m_L1L2TubeFilter->Delete() ;
  m_L1L2PolyDataMapper->Delete() ;
  m_L1L2Actor->Delete() ;

  // L2 to L3 line
  m_L2L3LineSource->Delete() ;
  m_L2L3TubeFilter->Delete() ;
  m_L2L3PolyDataMapper->Delete() ;
  m_L2L3Actor->Delete() ;

  // positive z global axis
  m_PosZAxisLineSource->Delete() ;
  m_PosZAxisAxesTubeFilter->Delete() ; 
  m_PosZAxisPolyDataMapper->Delete() ;
  m_PosZAxisActor->Delete() ;

  // negative z global axis
  m_NegZAxisLineSource->Delete() ;
  m_NegZAxisAxesTubeFilter->Delete() ; 
  m_NegZAxisPolyDataMapper->Delete() ;
  m_NegZAxisActor->Delete() ;

  // Transform1 -> Transform2 -> Normals -> Mapper
  m_MuscleTransform1->Delete() ;
  m_MuscleTransform1PolyDataFilter->Delete() ;
  m_MuscleTransform2->Delete() ;
  m_MuscleTransform2PolyDataFilter->Delete() ;
  m_MusclePolyDataNormals->Delete() ; 
  m_MusclePolyDataMapper->Delete() ; 
  m_MuscleLODActor->Delete() ;


  // standard contour pipeline
  m_ContourPlane->Delete() ;
  m_ContourCutter->Delete() ; 
  m_ContourCutterTransform->Delete() ; 
  m_ContourCutterTransformPolyDataFilter->Delete() ; 
  m_ContourTubeFilter->Delete() ; 
  m_ContourPolyDataMapper->Delete() ; 
  m_ContourActor->Delete() ; 

  // glyphed contour
  //m_pContourGlyphSphereSource->Delete() ;
  //m_pContourGlyph3D->Delete() ;
  //m_pContourGlyphPolyDataMapper->Delete() ;
  //m_pContourGlyphActor->Delete() ;

  // scaling contours stuff
  m_X0ZNPlane->Delete() ;
  m_X0ZSPlane->Delete() ;
  m_Y0ZEPlane->Delete() ;
  m_Y0ZWPlane->Delete() ; 

  // north-east
  m_NEContourX0ZPlaneClipPolyData->Delete() ; 
  m_NEContourY0ZPlaneClipPolyData->Delete() ; 
  m_NEContourTubeFilter->Delete() ;  
  m_NEContourTransformPolyDataFilter->Delete() ;
  m_NEContourPolyDataMapper->Delete() ; 
  m_NEContourActor->Delete() ; 

  // north-west
  m_NWContourX0ZPlaneClipPolyData->Delete() ; 
  m_NWContourY0ZPlaneClipPolyData->Delete() ;
  m_NWContourTubeFilter->Delete() ;  
  m_NWContourTransformPolyDataFilter->Delete() ; 
  m_NWContourPolyDataMapper->Delete() ; 
  m_NWContourActor->Delete() ; 

  // south-east
  m_SEContourX0ZPlaneClipPolyData->Delete() ; 
  m_SEContourY0ZPlaneClipPolyData->Delete() ; 
  m_SEContourTubeFilter->Delete() ;  
  m_SEContourTransformPolyDataFilter->Delete() ;
  m_SEContourPolyDataMapper->Delete() ;
  m_SEContourActor->Delete() ;

  // south-west
  m_SWContourX0ZPlaneClipPolyData->Delete() ;
  m_SWContourY0ZPlaneClipPolyData->Delete() ; 
  m_SWContourTubeFilter->Delete() ;  
  m_SWContourTransformPolyDataFilter->Delete() ; 
  m_SWContourPolyDataMapper->Delete() ; 
  m_SWContourActor->Delete() ;

  // display information
  m_TextMapperX->Delete() ; 
  m_ScaledTextActorX->Delete() ; 
  m_TextMapperY->Delete() ; 
  m_ScaledTextActorY->Delete() ;

  DeleteOperationsStack() ;

  DeleteSyntheticScans() ;
}





//----------------------------------------------------------------------------
// Calculate size and resolution
// Must be called prior to CreateSyntheticScans
//
// Size is determined by the bounds of MuscleTransform2PDFilter->GetOutput()
//      or just (200, 200) if muscle type is 2
//
// Resolution is 0.3, 0.5, 1, 2 or 3 * the size, for grain values 1-5.
// Resolution is the no. of cells when the polydata slice is created from vtkPlaneSource().
void medOpMML3ModelView::FindSizeAndResolutionOfSyntheticScans()
//----------------------------------------------------------------------------
{
  float Factor = 1.75; // 75% extra

  switch (m_NTypeOfMuscles)
  {
  case 1: // slicing axis is single line
    // bounds
    double bounds[6];
    m_MuscleTransform2PolyDataFilter->GetOutput()->GetBounds(bounds);

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
  else if (m_SyntheticScansGrain == 2) // x 1/2
  {
    m_NSyntheticScansXResolution = 0.5 * m_NSyntheticScansXSize;
    m_NSyntheticScansYResolution = 0.5 * m_NSyntheticScansYSize;
  }
  else if (m_SyntheticScansGrain == 3) // x1
  {
    m_NSyntheticScansXResolution = 1.0 * m_NSyntheticScansXSize;
    m_NSyntheticScansYResolution = 1.0 * m_NSyntheticScansYSize;
  }
  else if (m_SyntheticScansGrain == 4) // x2
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




//------------------------------------------------------------------------------
// Construct the visual pipes for all the slices
// Allocates memory for m_SyntheticScansActor[] + associated reference counted objects
void medOpMML3ModelView::CreateSyntheticScans()
//------------------------------------------------------------------------------
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


  // allocate arrays
  vtkPlaneSource** PlaneSource = new vtkPlaneSource* [n] ;
  vtkTransform** PlaneSourceTransform = new vtkTransform* [n];
  vtkTransformPolyDataFilter** PlaneSourceTransformPolyDataFilter = new vtkTransformPolyDataFilter* [n];
  vtkProbeFilter** ProbeFilter = new vtkProbeFilter* [n];
  vtkPolyDataMapper** PolyDataMapper = new vtkPolyDataMapper* [n];
  vtkTransform** ActorTransform = new vtkTransform* [n];
  m_SyntheticScansActor = new vtkActor* [n];

  // initial scalar value min/max
  m_SyntheticScansMinScalarValue = 100000;
  m_SyntheticScansMaxScalarValue = -100000;

  // scalar value holder
  double scalars[2];


  //--------------------------------------------------------------------------
  //      Visual pipe for slices
  //
  //        PlaneSource[i]
  //             |             CreatePlaneSourceTransformOfSynthScans(i)
  //             |            / 
  //             |        PlaneSourceTransform[i]
  //             |          /
  //      PlaneSourceTransformPolyDataFilter[i]
  //             |
  //        ProbeFilter[i]
  //             |
  //       PolyDataMapper[i]
  //             |
  //             |            CreateActorTransformOfSynthScans(i) 
  //             |           /
  //             |       ActorTransform
  //             V         /
  //     m_SyntheticScansActor[i]
  //
  //--------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // Loop over all scans
  //----------------------------------------------------------------------------
  for(int i = 0; i < n; i++)
  {
    // set plane source resolution
    PlaneSource[i] = vtkPlaneSource::New();
    PlaneSource[i]->SetResolution(m_NSyntheticScansXResolution, m_NSyntheticScansYResolution);


    // plane source transformation matrix and filter
    vtkMatrix4x4* pstmatrix = CreatePlaneSourceTransformOfSyntheticScans(i) ;
    PlaneSourceTransform[i] = vtkTransform::New();
    PlaneSourceTransform[i]->SetMatrix(pstmatrix);
    pstmatrix->Delete() ;

    PlaneSourceTransformPolyDataFilter[i] = vtkTransformPolyDataFilter::New();
    PlaneSourceTransformPolyDataFilter[i]->SetInput(PlaneSource[i]->GetOutput());
    PlaneSourceTransformPolyDataFilter[i]->SetTransform(PlaneSourceTransform[i]);


    // probe filter
    ProbeFilter[i] = vtkProbeFilter::New();
    ProbeFilter[i]->SetInput(PlaneSourceTransformPolyDataFilter[i]->GetOutput());
    ProbeFilter[i]->SetSource(m_Scans);

    // probe mapper 
    PolyDataMapper[i] = vtkPolyDataMapper::New();
    PolyDataMapper[i]->SetInput(ProbeFilter[i]->GetPolyDataOutput());
    //PolyDataMapper[i]->ImmediateModeRenderingOn(); // for large datasets at the expense of slower rendering


    // actor and actor transform
    ActorTransform[i] = vtkTransform::New();
    vtkMatrix4x4* atmatrix = CreateActorTransformOfSyntheticScans(i) ;
    ActorTransform[i]->SetMatrix(atmatrix);
    atmatrix->Delete() ;

    m_SyntheticScansActor[i] = vtkActor::New();
    m_SyntheticScansActor[i]->SetUserTransform(ActorTransform[i]);
    m_SyntheticScansActor[i]->SetMapper(PolyDataMapper[i]);
    m_SyntheticScansActor[i]->VisibilityOff();
    m_Renderer->AddActor(m_SyntheticScansActor[i]);


    // adjust min/max scalar values
    ProbeFilter[i]->Update();
    ProbeFilter[i]->GetPolyDataOutput()->GetScalarRange(scalars);
    if (scalars[0] < m_SyntheticScansMinScalarValue)
      m_SyntheticScansMinScalarValue = scalars[0];
    if (scalars[1] > m_SyntheticScansMaxScalarValue)
      m_SyntheticScansMaxScalarValue = scalars[1];


    // clean up
    ActorTransform[i]->Delete();
    PolyDataMapper[i]->Delete();
    ProbeFilter[i]->Delete();
    PlaneSourceTransformPolyDataFilter[i]->Delete();
    PlaneSourceTransform[i]->Delete();
    PlaneSource[i]->Delete();
  }

  // window
  m_Window = (m_SyntheticScansMaxScalarValue - m_SyntheticScansMinScalarValue);

  // level
  m_Level = 0.5 * (m_SyntheticScansMinScalarValue + m_SyntheticScansMaxScalarValue);

  // lut
  m_SyntheticScansWindowLevelLookupTable->SetTableRange(m_SyntheticScansMinScalarValue, m_SyntheticScansMaxScalarValue);
  m_SyntheticScansWindowLevelLookupTable->SetHueRange(0.0, 0.0);
  m_SyntheticScansWindowLevelLookupTable->SetSaturationRange(0.0, 0.0);
  m_SyntheticScansWindowLevelLookupTable->SetValueRange(0.0, 1.0);
  m_SyntheticScansWindowLevelLookupTable->SetNumberOfColors(1024);
  m_SyntheticScansWindowLevelLookupTable->SetWindow(m_Window);
  m_SyntheticScansWindowLevelLookupTable->SetLevel(m_Level);
  m_SyntheticScansWindowLevelLookupTable->Build();

  for(int j = 0; j < n; j++)
  {
    PolyDataMapper[j]->SetLookupTable(m_SyntheticScansWindowLevelLookupTable);
    PolyDataMapper[j]->UseLookupTableScalarRangeOn();
    PolyDataMapper[j]->SetColorModeToMapScalars();
  }

  // delete objects
  delete ActorTransform;
  delete ProbeFilter;
  delete PlaneSourceTransformPolyDataFilter;
  delete PlaneSourceTransform;
  delete PlaneSource;
  delete PolyDataMapper;

  // set to scan 0 display
  m_NSyntheticScansCurrentId = 0; // current id
  m_SyntheticScansActor[0]->VisibilityOn(); // actor on
}




//----------------------------------------------------------------------------
// Delete the synthetic scans
// ie delete the actors which were allocated in CreateSyntheticScans()
void medOpMML3ModelView::DeleteSyntheticScans()
//----------------------------------------------------------------------------
{
  if (m_SyntheticScansActor != NULL){
    for(int i = 0; i < this->GetTotalNumberOfSyntheticScans(); i++)
      m_SyntheticScansActor[i]->Delete() ;
    delete [] m_SyntheticScansActor ;
    m_SyntheticScansActor = NULL ;
  }
}




//----------------------------------------------------------------------------
void medOpMML3ModelView::GetPlaneSourceOriginOfSyntheticScans(int scanId, double p[])
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

  switch (m_NTypeOfMuscles)
  {
  case 1: // slicing axis is single line
    // start from landmark 2 (low)
    // ending at landmark 1 (high)

    // current length
    currentlength = m_DLength12 / (n - 1) * scanId ;

    // origin
    for(i = 0; i < 3; i++)
      p[i] = l2[i] + currentlength * m_DUnitVector12[i];
    break;

  case 2: // slicing axis is double line
    // start from landmark 3 (low)
    // ending at landmark 1 (high)

    // current length
    currentlength = m_DOverallLength / (n - 1) * scanId ;

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
// Create and calculate new transform matrix for the
// slices polydata plane source
vtkMatrix4x4* medOpMML3ModelView::CreatePlaneSourceTransformOfSyntheticScans(int scanId)
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

  switch (m_NTypeOfMuscles)
  {
  case 1: // slicing axis is single line

    // synthetic scan: plane source size
    // (as a scaling transformation)
    scalem->Identity();
    scalem->SetElement(0, 0, m_NSyntheticScansXSize);
    scalem->SetElement(1, 1, m_NSyntheticScansYSize);

    // synthetic scan: plane source origin
    // (as a translation transformation)
    this->GetPlaneSourceOriginOfSyntheticScans(scanId, p); 
    transm->Identity();
    transm->SetElement(0, 3, p[0]); // x
    transm->SetElement(1, 3, p[1]); // y
    transm->SetElement(2, 3, p[2]); // z

    //
    inversem_pslicesm->Identity();
    inversem_pslicesm->Invert(m_Slicesm, inversem_pslicesm);

    //  s-th synthetic scan: plane source transformation matrix
    finalm->Identity();
    MultiplyMatrix4x4(transm, finalm, finalm); // 3. translation
    MultiplyMatrix4x4(inversem_pslicesm, finalm, finalm); // 2. alignement: inverse (rotx * roty)
    MultiplyMatrix4x4(scalem, finalm, finalm); // 1. scaling

    break;

  case 2: // slicing axis is double line
    // s-th synthetic scan: plane source size
    // (as a scaling transformation)
    scalem->Identity();
    scalem->SetElement(0, 0, m_NSyntheticScansXSize);
    scalem->SetElement(1, 1, m_NSyntheticScansYSize);

    // s-th synthetic scan: plane source origin
    // (as a translation transformation)
    this->GetPlaneSourceOriginOfSyntheticScans(scanId, p); 
    transm->Identity();
    transm->SetElement(0, 3, p[0]); // x
    transm->SetElement(1, 3, p[1]); // y
    transm->SetElement(2, 3, p[2]); // z

    //
    n = this->GetTotalNumberOfSyntheticScans();

    // s-synthetic scan: plane source normal
    // current length
    currentlength = m_DOverallLength / (n - 1) * scanId ;

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
    MultiplyMatrix4x4(rotaym, inversealignm, inversealignm);
    MultiplyMatrix4x4(rotaxm, inversealignm, inversealignm);

    // align matrix
    alignm->Invert(inversealignm, alignm);

    //  s-th synthetic scan: plane source transformation matrix
    finalm->Identity();
    MultiplyMatrix4x4(transm, finalm, finalm); // 3. translation
    MultiplyMatrix4x4(alignm, finalm, finalm); // 2. align
    MultiplyMatrix4x4(scalem, finalm, finalm); // 1. scaling
    //
    break;
  }

  // clean up
  rotaxm->Delete();
  rotaym->Delete();
  scalem->Delete();
  transm->Delete();
  inversem_pslicesm->Delete();
  alignm->Delete();
  inversealignm->Delete();


  // return the created matrix
  return finalm;
}



//----------------------------------------------------------------------------
// Create and calculate new transform matrix
vtkMatrix4x4* medOpMML3ModelView::CreateActorTransformOfSyntheticScans(int scanId)
//----------------------------------------------------------------------------
{
  //
  double p[3];

  //
  vtkMatrix4x4 *transm = vtkMatrix4x4::New();
  vtkMatrix4x4 *inversem_pslicesm = vtkMatrix4x4::New();
  vtkMatrix4x4 *finalm =  vtkMatrix4x4::New();

  switch (m_NTypeOfMuscles)
  {
  case 1: // slicing axis is single line

    // synthetic scan: plane source origin
    // (as a translation transformation)
    this->GetPlaneSourceOriginOfSyntheticScans(scanId, p); 
    transm->Identity();
    transm->SetElement(0, 3, p[0]); // x
    transm->SetElement(1, 3, p[1]); // y
    transm->SetElement(2, 3, p[2]); // z

    //
    inversem_pslicesm->Identity();
    inversem_pslicesm->Invert(m_Slicesm, inversem_pslicesm);

    finalm->Identity();
    MultiplyMatrix4x4(transm, finalm, finalm); // 2. translation
    MultiplyMatrix4x4(inversem_pslicesm, finalm, finalm); // 1. inverse (rotx * roty)
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


  // return the created matrix
  return finalm;
}



//----------------------------------------------------------------------------
void medOpMML3ModelView::FindUnitVectorsAndLengthsOfLandmarkLines()
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
// Transform 1
// Sets m_MuscleTransform1
bool medOpMML3ModelView::MapAtlasToPatient()
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


  // create transformation matrix 1
  vtkMatrix4x4 *transm1 = vtkMatrix4x4::New();
  transm1->Identity();
  transm1->SetElement(0, 3, p1[0]);
  transm1->SetElement(1, 3, p1[1]);
  transm1->SetElement(2, 3, p1[2]);


  // create transformation matrix 2
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


  // create transformation matrix 3
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


  // create inverse transformation matrix 3
  vtkMatrix4x4 *inversetransm3 = vtkMatrix4x4::New();
  inversetransm3->Identity();
  inversetransm3->Invert(transm3, inversetransm3);


  // create transformation matrix 4
  vtkMatrix4x4 *transm4 = vtkMatrix4x4::New();
  transm4->Identity();
  transm4->SetElement(0, 3, -1.0 * a1[0]);
  transm4->SetElement(1, 3, -1.0 * a1[1]);
  transm4->SetElement(2, 3, -1.0 * a1[2]);


  // create final tranformation matrix
  vtkMatrix4x4 *finalm = vtkMatrix4x4::New();
  finalm->Identity();
  MultiplyMatrix4x4(transm1, finalm, finalm);
  MultiplyMatrix4x4(transm2, finalm, finalm);
  MultiplyMatrix4x4(inversetransm3, finalm, finalm);
  MultiplyMatrix4x4(transm4, finalm, finalm);


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
  // Set transform 1 of the muscle (see constructor)
  m_MuscleTransform1->SetMatrix(finalm);
  finalm->Delete() ;

  // clean up
  transm1->Delete();
  transm2->Delete();
  transm3->Delete();
  transm4->Delete();
  inversetransm3->Delete() ;

  return 1;
}



//----------------------------------------------------------------------------
// Transform 2
// transform coordinates into a coordinate system, in which z-axis
// is aligned with muscle line axis as defined by insertion points
// in patient, and origin being the middle of this muscle line axis.
//
// Calculate matrices m_Slicesm and m_Finalm
// Set m_MuscleTransform2
bool medOpMML3ModelView::MakeActionLineZAxis()
//----------------------------------------------------------------------------
{
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
  m_Slicesm->Identity();
  MultiplyMatrix4x4(rotaym, m_Slicesm, m_Slicesm); // rotation y
  MultiplyMatrix4x4(rotaxm, m_Slicesm, m_Slicesm); // rotation x

  // transformation matrix
  m_Finalm->Identity();
  MultiplyMatrix4x4(scalem, m_Finalm, m_Finalm); // 4. scaling
  MultiplyMatrix4x4(rotaym, m_Finalm, m_Finalm); // 3. rotation y
  MultiplyMatrix4x4(rotaxm, m_Finalm, m_Finalm); // 2. rotation x
  MultiplyMatrix4x4(transm, m_Finalm, m_Finalm); // 1. translation

  // Set transform 2 of muscle (see constructor)
  m_MuscleTransform2->SetMatrix(m_Finalm);


  double newp1[3];
  double newp2[3];

  // transform insertions (tests)
  vtkTransform *transf = vtkTransform::New();
  transf->SetMatrix(m_Finalm);
  transf->TransformPoint(p1, newp1); // landmark 1
  transf->TransformPoint(p2, newp2); // landmark 2
  transf->Delete();

  //
  assert((newp1[0] < 0.0001) && (newp1[1] < 0.0001)); // x, y are zero
  assert((newp2[0] < 0.0001) && (newp2[0] < 0.0001)); // x, y are zero
  assert((newp1[2] + newp2[2]) < 0.0001); // z are opposite, so sum to zero

  // clean up
  transm->Delete();
  rotaym->Delete();
  rotaxm->Delete();
  scalem->Delete();
  //reflzm->Delete() ;

  //
  m_MuscleTransform2PolyDataFilter->Update();

  return 1;
}




//----------------------------------------------------------------------------
double medOpMML3ModelView::GetZOfSyntheticScans(int s)
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
double medOpMML3ModelView::GetCurrentZOfSyntheticScans()
//----------------------------------------------------------------------------
{
  return GetZOfSyntheticScans(GetCurrentIdOfSyntheticScans());
}


//----------------------------------------------------------------------------
void medOpMML3ModelView::Render()
//----------------------------------------------------------------------------
{
  m_RenderWindow->Render();
}


//----------------------------------------------------------------------------
/** m = 1, 2 for text actor 1,2 */
void medOpMML3ModelView::SetText(int m, double n, int d, int s)
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
vtkCamera* medOpMML3ModelView::GetActiveCamera()
//----------------------------------------------------------------------------
{
  return m_Renderer->GetActiveCamera();
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SaveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
  m_CameraFocalPoint[0] = *fp;
  m_CameraFocalPoint[1] = *(fp+1);
  m_CameraFocalPoint[2] = *(fp+2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SaveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
  m_CameraPosition[0] = *cp;
  m_CameraPosition[1] = *(cp+1);
  m_CameraPosition[2] = *(cp+2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SaveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
  m_CameraClippingRange[0] = *cr;
  m_CameraClippingRange[1] = *(cr+1);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SaveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
  m_CameraViewUp[0] = *vu;
  m_CameraViewUp[1] = *(vu+1);
  m_CameraViewUp[2] = *(vu+2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::RetrieveCameraFocalPoint(double *fp)
//----------------------------------------------------------------------------
{
  *fp = m_CameraFocalPoint[0];
  *(fp+1) = m_CameraFocalPoint[1];
  *(fp+2) = m_CameraFocalPoint[2];
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::RetrieveCameraPosition(double *cp)
//----------------------------------------------------------------------------
{
  *cp = m_CameraPosition[0];
  *(cp+1) = m_CameraPosition[1];
  *(cp+2) = m_CameraPosition[2];
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::RetrieveCameraClippingRange(double *cr)
//----------------------------------------------------------------------------
{
  *cr = m_CameraClippingRange[0];
  *(cr+1) = m_CameraClippingRange[1];
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::RetrieveCameraViewUp(double *vu)
//----------------------------------------------------------------------------
{
  *vu = m_CameraViewUp[0];
  *(vu+1) = m_CameraViewUp[1];
  *(vu+2) = m_CameraViewUp[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::AddActor(vtkActor *a)
//----------------------------------------------------------------------------
{
  m_Renderer->AddActor(a);
}


//----------------------------------------------------------------------------
void medOpMML3ModelView::SetPositiveLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
  m_PosXAxisLineSource->SetPoint1(p1);
  m_PosXAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SetNegativeLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
  m_NegYAxisLineSource->SetPoint1(p1);
  m_NegYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SetPositiveLineActorY(double p1[], double p2[])
//----------------------------------------------------------------------------
{
  m_PosYAxisLineSource->SetPoint1(p1);
  m_PosYAxisLineSource->SetPoint2(p2);
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::SetNegativeLineActorX(double p1[], double p2[])
//----------------------------------------------------------------------------
{
  m_NegXAxisLineSource->SetPoint1(p1);
  m_NegXAxisLineSource->SetPoint2(p2);
}


//----------------------------------------------------------------------------
void medOpMML3ModelView::WriteMatrix(char *pch, vtkMatrix4x4 *m) const
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
void medOpMML3ModelView::SetResolutionOfSyntheticScans(int x, int y)
//----------------------------------------------------------------------------
{
  m_NSyntheticScansXResolution = x;
  m_NSyntheticScansYResolution = y;
}

//----------------------------------------------------------------------------
int medOpMML3ModelView::GetTotalNumberOfSyntheticScans() const
//----------------------------------------------------------------------------
{
  assert(m_NSyntheticScansTotalNumber >= 3); // at least 3 synthetic scans
  return m_NSyntheticScansTotalNumber;
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetResolutionOfSyntheticScans(int *x, int *y)
//----------------------------------------------------------------------------
{
  *x = m_NSyntheticScansXResolution;
  *y = m_NSyntheticScansYResolution;
}


//----------------------------------------------------------------------------
void medOpMML3ModelView::SetSizeOfSyntheticScans(float x, float y)
//----------------------------------------------------------------------------
{
  m_NSyntheticScansXSize = x;
  m_NSyntheticScansYSize = y;
}
//----------------------------------------------------------------------------
void medOpMML3ModelView::GetSizeOfSyntheticScans(float *x, float *y)
//----------------------------------------------------------------------------
{
  *x = m_NSyntheticScansXSize;
  *y = m_NSyntheticScansYSize;
}


//----------------------------------------------------------------------------
// Multiply matrices c = ab
// c can be the same as a or b
// Note that the matrix is column major, ie 
void medOpMML3ModelView::MultiplyMatrix4x4(vtkMatrix4x4 *a, vtkMatrix4x4 *b, vtkMatrix4x4 *c) const
//----------------------------------------------------------------------------
{
  //
  // matrix notation (i is row, j is col)
  // matrices are column major, ie mat->GetElement[col][row]
  //
  // C0 C1 C2 C3 |
  // -------------
  // 00 10 20 30 | R0
  // 01 11 21 31 | R1
  // 02 12 22 32 | R2
  // 03 13 23 33 | R3
  //
  // ji element of product is irow
  // of a combined with jcol of b

  vtkMatrix4x4* ctemp = vtkMatrix4x4::New();

  for (int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      ctemp->SetElement(j, i, 
        a->GetElement(0, i) * b->GetElement(j, 0) +
        a->GetElement(1, i) * b->GetElement(j, 1) +
        a->GetElement(2, i) * b->GetElement(j, 2) +
        a->GetElement(3, i) * b->GetElement(j, 3));		
    }
  }

  // copy result to output matrix
  c->DeepCopy(ctemp) ;
  ctemp->Delete() ;
}



//----------------------------------------------------------------------------
void medOpMML3ModelView::SetXYScalingFactorsOfMuscle(double x, double y)
//----------------------------------------------------------------------------
{
  m_FlMuscleXScalingFactor = x;
  m_FlMuscleYScalingFactor = y;
}


//----------------------------------------------------------------------------
void medOpMML3ModelView::GetXYScalingFactorsOfMuscle(double *x, double *y) const
//----------------------------------------------------------------------------
{
  *x = m_FlMuscleXScalingFactor;
  *y = m_FlMuscleYScalingFactor;
}





//----------------------------------------------------------------------------
void medOpMML3ModelView::UpdateContourCuttingPlane()
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
void medOpMML3ModelView::UpdateSegmentCuttingPlanes()
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
void medOpMML3ModelView::UpdateContourAxesTransform()
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
void medOpMML3ModelView::SetContourAxesVisibility()
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
void medOpMML3ModelView::UpdateGlobalAxesTransform()
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
void medOpMML3ModelView::SetGlobalAxesVisibility()
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
void medOpMML3ModelView::UpdateSegmentNorthEastTransform()
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
void medOpMML3ModelView::UpdateSegmentNorthWestTransform()
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
void medOpMML3ModelView::UpdateSegmentSouthEastTransform()
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
void medOpMML3ModelView::UpdateSegmentSouthWestTransform()
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
void medOpMML3ModelView::SetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleAtlasInsertionPoint1[0] = xyz[0];
  m_DMuscleAtlasInsertionPoint1[1] = xyz[1];
  m_DMuscleAtlasInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleAtlasInsertionPoint2[0] = xyz[0];
  m_DMuscleAtlasInsertionPoint2[1] = xyz[1];
  m_DMuscleAtlasInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleAtlasReferencePoint1[0] = xyz[0];
  m_DMuscleAtlasReferencePoint1[1] = xyz[1];
  m_DMuscleAtlasReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleAtlasReferencePoint2[0] = xyz[0];
  m_DMuscleAtlasReferencePoint2[1] = xyz[1];
  m_DMuscleAtlasReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark1OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleAtlasInsertionPoint1[0];
  xyz[1] = m_DMuscleAtlasInsertionPoint1[1];
  xyz[2] = m_DMuscleAtlasInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark2OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleAtlasInsertionPoint2[0];
  xyz[1] = m_DMuscleAtlasInsertionPoint2[1];
  xyz[2] = m_DMuscleAtlasInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark3OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleAtlasReferencePoint1[0];
  xyz[1] = m_DMuscleAtlasReferencePoint1[1];
  xyz[2] = m_DMuscleAtlasReferencePoint1[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark4OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleAtlasReferencePoint2[0];
  xyz[1] = m_DMuscleAtlasReferencePoint2[1];
  xyz[2] = m_DMuscleAtlasReferencePoint2[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleScansInsertionPoint1[0] = xyz[0];
  m_DMuscleScansInsertionPoint1[1] = xyz[1];
  m_DMuscleScansInsertionPoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleScansInsertionPoint2[0] = xyz[0];
  m_DMuscleScansInsertionPoint2[1] = xyz[1];
  m_DMuscleScansInsertionPoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleScansReferencePoint1[0] = xyz[0];
  m_DMuscleScansReferencePoint1[1] = xyz[1];
  m_DMuscleScansReferencePoint1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::SetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_DMuscleScansReferencePoint2[0] = xyz[0];
  m_DMuscleScansReferencePoint2[1] = xyz[1];
  m_DMuscleScansReferencePoint2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark1OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleScansInsertionPoint1[0];
  xyz[1] = m_DMuscleScansInsertionPoint1[1];
  xyz[2] = m_DMuscleScansInsertionPoint1[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark2OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleScansInsertionPoint2[0];
  xyz[1] = m_DMuscleScansInsertionPoint2[1];
  xyz[2] = m_DMuscleScansInsertionPoint2[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark3OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleScansReferencePoint1[0];
  xyz[1] = m_DMuscleScansReferencePoint1[1];
  xyz[2] = m_DMuscleScansReferencePoint1[2];
}

//----------------------------------------------------------------------------
void medOpMML3ModelView::GetLandmark4OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_DMuscleScansReferencePoint2[0];
  xyz[1] = m_DMuscleScansReferencePoint2[1];
  xyz[2] = m_DMuscleScansReferencePoint2[2];
}

//----------------------------------------------------------------------------
bool medOpMML3ModelView::SetUpContourCoordinateAxes()
//----------------------------------------------------------------------------
{
  // east
  m_ContourPosXAxisAxesTubeFilter->SetRadius(0.5);
  m_ContourPosXAxisAxesTubeFilter->SetNumberOfSides(6);
  m_ContourPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
  m_ContourPosXAxisActor->VisibilityOn();

  // north
  m_ContourPosYAxisAxesTubeFilter->SetRadius(0.5);
  m_ContourPosYAxisAxesTubeFilter->SetNumberOfSides(6);
  m_ContourPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
  m_ContourPosYAxisActor->VisibilityOn();

  // west
  m_ContourNegXAxisAxesTubeFilter->SetRadius(0.5);
  m_ContourNegXAxisAxesTubeFilter->SetNumberOfSides(6);
  m_ContourNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
  m_ContourNegXAxisActor->VisibilityOn();

  // south
  m_ContourNegYAxisAxesTubeFilter->SetRadius(0.5);
  m_ContourNegYAxisAxesTubeFilter->SetNumberOfSides(6);
  m_ContourNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
  m_ContourNegYAxisActor->VisibilityOn();

  return 1;
}

//----------------------------------------------------------------------------
bool medOpMML3ModelView::SetUpGlobalCoordinateAxes()
//----------------------------------------------------------------------------
{
  // east
  m_PosXAxisAxesTubeFilter->SetRadius(0.5);
  m_PosXAxisAxesTubeFilter->SetNumberOfSides(6);
  m_PosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_PosXAxisLineSource->SetPoint2(m_NSyntheticScansXSize / 2.0, 0.0, 0.0);
  m_PosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
  m_PosXAxisActor->VisibilityOn();

  // north
  m_PosYAxisAxesTubeFilter->SetRadius(0.5);
  m_PosYAxisAxesTubeFilter->SetNumberOfSides(6);
  m_PosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_PosYAxisLineSource->SetPoint2(0.0, m_NSyntheticScansYSize / 2.0, 0.0);
  m_PosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
  m_PosYAxisActor->VisibilityOn();

  // west
  m_NegXAxisAxesTubeFilter->SetRadius(0.5);
  m_NegXAxisAxesTubeFilter->SetNumberOfSides(6);
  m_NegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_NegXAxisLineSource->SetPoint2(-1.0 * m_NSyntheticScansXSize / 2.0, 0.0, 0.0);
  m_NegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta
  m_NegXAxisActor->VisibilityOn();

  // south
  m_NegYAxisAxesTubeFilter->SetRadius(0.5);
  m_NegYAxisAxesTubeFilter->SetNumberOfSides(6);
  m_NegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_NegYAxisLineSource->SetPoint2(0.0, -1.0 * m_NSyntheticScansYSize / 2.0, 0.0);
  m_NegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
  m_NegYAxisActor->VisibilityOn();

  return 1;
}



//----------------------------------------------------------------------------
/// switch visual pipe to 3d display
void medOpMML3ModelView::Switch3dDisplayOn()
//----------------------------------------------------------------------------
{
  // 3d display flag
  m_3DDisplay = 1;

  // interactor style for 3d display
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New() ;
  m_RenderWindowInteractor->SetInteractorStyle(style);
  style->Delete() ;

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
    m_SyntheticScansActor[i]->SetUserTransform(t);
  }

  // muscle mapped to patient space only, using transform 1
  // thus, transform 2 must simply be an identity transform
  m_MuscleTransform2PolyDataFilter->SetTransform(t);

  // set up muscle and switch on
  m_MuscleLODActor->VisibilityOn();
  m_MuscleLODActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  m_MuscleLODActor->GetProperty()->SetOpacity(0.5);

  // standard contour switch on
  m_ContourActor->VisibilityOn();

  // sub-contours switch off
  m_NEContourActor->VisibilityOff();
  m_NWContourActor->VisibilityOff();
  m_SEContourActor->VisibilityOff();
  m_SWContourActor->VisibilityOff();

  // axes
  // east/north/west/south re-set up and switch on
  // set up performed in SetUpMuscleActionLineAxes
  m_PosXAxisAxesTubeFilter->SetRadius(1.0);
  m_PosYAxisAxesTubeFilter->SetRadius(1.0);
  m_NegXAxisAxesTubeFilter->SetRadius(1.0);
  m_NegYAxisAxesTubeFilter->SetRadius(1.0);

  // positive z axis set up
  m_PosZAxisAxesTubeFilter->SetRadius(1.0);
  m_PosZAxisAxesTubeFilter->SetNumberOfSides(6);
  m_PosZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_PosZAxisLineSource->SetPoint2(0.0, 0.0, (m_NSyntheticScansXSize + m_NSyntheticScansYSize) / 2.0);
  m_PosZAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

  // negative z axis set up
  m_NegZAxisAxesTubeFilter->SetRadius(1.0);
  m_NegZAxisAxesTubeFilter->SetNumberOfSides(6);
  m_NegZAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_NegZAxisLineSource->SetPoint2(0.0, 0.0, -1.0 * (m_NSyntheticScansXSize + m_NSyntheticScansYSize) / 2.0);
  m_NegZAxisActor->GetProperty()->SetColor(1.0, 1.0, 1.0); // white

  // axes off
  m_PosXAxisActor->VisibilityOff();
  m_NegXAxisActor->VisibilityOff();
  m_PosYAxisActor->VisibilityOff();
  m_NegYAxisActor->VisibilityOff();
  m_PosZAxisActor->VisibilityOff();
  m_NegZAxisActor->VisibilityOff();

  // landmarks
  double p1[3]; this->GetLandmark1OfPatient(p1);
  double p2[3]; this->GetLandmark2OfPatient(p2);
  double p3[3]; this->GetLandmark3OfPatient(p3);
  double p4[3]; this->GetLandmark4OfPatient(p4);

  // 1st landmark set up and switch on
  m_Landmark1SphereSource->SetRadius(2.0);
  m_Landmark1SphereSource->SetThetaResolution(10);
  m_Landmark1SphereSource->SetPhiResolution(10);
  m_Landmark1SphereSource->SetCenter(p1[0], p1[1], p1[2]);
  m_Landmark1Actor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red
  m_Landmark1Actor->VisibilityOn();

  // 2nd landmark set up and switch on
  m_Landmark2SphereSource->SetRadius(2.0);
  m_Landmark2SphereSource->SetThetaResolution(10);
  m_Landmark2SphereSource->SetPhiResolution(10);
  m_Landmark2SphereSource->SetCenter(p2[0], p2[1], p2[2]);
  m_Landmark2Actor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
  m_Landmark2Actor->VisibilityOn();

  // 3rd landmark set up and switch on
  m_Landmark3SphereSource->SetRadius(2.0);
  m_Landmark3SphereSource->SetThetaResolution(10);
  m_Landmark3SphereSource->SetPhiResolution(10);
  m_Landmark3SphereSource->SetCenter(p3[0], p3[1], p3[2]);
  m_Landmark3Actor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue
  m_Landmark3Actor->VisibilityOn();

  // 4th landmark set up and switch on
  if (m_4Landmarks == 1)
  {
    m_Landmark4SphereSource->SetRadius(2.0);
    m_Landmark4SphereSource->SetThetaResolution(10);
    m_Landmark4SphereSource->SetPhiResolution(10);
    m_Landmark4SphereSource->SetCenter(p4[0], p4[1], p4[2]);
    m_Landmark4Actor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow
    m_Landmark4Actor->VisibilityOn();	
  }

  // line of action (L1 to L2) set up and switch on
  m_L1L2TubeFilter->SetRadius(1.0);
  m_L1L2TubeFilter->SetNumberOfSides(6);
  m_L1L2LineSource->SetPoint1(p1[0], p1[1], p1[2]);
  m_L1L2LineSource->SetPoint2(p2[0], p2[1], p2[2]);
  m_L1L2Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_L1L2Actor->VisibilityOn();

  // L2 to L3 line set up and switch on
  m_L2L3TubeFilter->SetRadius(1.0);
  m_L2L3TubeFilter->SetNumberOfSides(6);
  m_L2L3LineSource->SetPoint1(p2[0], p2[1], p2[2]);
  m_L2L3LineSource->SetPoint2(p3[0], p3[1], p3[2]);
  m_L2L3Actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_L2L3Actor->VisibilityOn();


  // clean up
  m->Delete() ;
  t->Delete() ;
}



//SIL. 24-12-2004: begin
//----------------------------------------------------------------------------
void medOpMML3ModelView::Print(vtkObject *obj, wxString msg) const
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
// allocate operations stack
void medOpMML3ModelView::AllocateOperationsStack(int numberOfComponents, int numberOfTuples)
//----------------------------------------------------------------------------
{
  m_OperationsStack = vtkDoubleArray::New() ;
  m_OperationsStack->SetNumberOfComponents(numberOfComponents) ;
  m_OperationsStack->SetNumberOfTuples(numberOfTuples) ;
}




//wxLogMessage("============ALIGN MUSCLE======================");
//DEL 	//Print(m_pMuscleTransform1 ,               "m_pMuscleTransform1 ");
//DEL 	//Print(m_pMuscleTransform1PolyDataFilter , "m_pMuscleTransform1PolyDataFilter ");
//DEL 	//Print(m_pMuscleTransform2 ,               "m_pMuscleTransform2 ");
//DEL 	//Print(m_pMuscleTransform2PolyDataFilter , "m_pMuscleTransform2PolyDataFilter ");
//DEL 	//Print(m_pMuscleTransform2PolyDataFilter->GetOutput() , "m_pMuscleTransform2PolyDataFilter->GetOutput()");
//DEL 	//wxLogMessage("==================================");
