/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ModelView2DPipe
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"

#include "vtkDataSet.h"
#include "vtkMatrix4x4.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"
#include "vtkTransform.h"
#include "vtkLinearTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkProbeFilter.h"
#include "vtkCutter.h"
#include "vtkPlane.h"
#include "vtkTubeFilter.h"
#include "vtkLineSource.h"
#include "vtkClipPolyData.h"
#include "vtkTextMapper.h"
#include "vtkScaledTextActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkCamera.h"

#include "albaOpMML3ModelView2DPipe.h"

#include <vector>
#include <algorithm>


#ifndef M_PI
#define _USE_MATH_DEFINES
#endif



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
albaOpMML3ModelView2DPipe::albaOpMML3ModelView2DPipe(vtkRenderer *ren, vtkPolyData *muscle, vtkDataSet* volume, int numberOfSlices)
: m_Renderer(ren), m_NumberOfSlices(numberOfSlices), m_CurrentVisibility(0),
m_CurrentSliceId(0),
m_GlobalAxesOn(0), m_ContourAxesOn(1), m_TextXOn(0), m_TextYOn(0), m_ContourOn(1),
m_MusclePolyDataOn(0), m_TubeSize(0.01), m_AxisLengthX(1.0), m_AxisLengthY(1.0)
{
  //----------------------------------------------------------------------------
  // Calculate scalar lut
  //----------------------------------------------------------------------------
  double r[2] ;
  volume->GetScalarRange(r) ;
  m_LutWindow = (r[1]-r[0]);
  m_LutLevel = (r[1]+r[0]) / 2.0 ;

  m_Lut = vtkWindowLevelLookupTable::New() ;
  m_Lut->SetTableRange(r[0], r[1]);
  m_Lut->SetHueRange(0.0, 0.0);
  m_Lut->SetSaturationRange(0.0, 0.0);
  m_Lut->SetValueRange(0.0, 1.0);
  m_Lut->SetNumberOfColors(512);
  m_Lut->SetWindow(m_LutWindow);
  m_Lut->SetLevel(m_LutLevel);
  m_Lut->Build();



  //----------------------------------------------------------------------------
  // list of transform matrices for all slices
  // don't forget to set the matrix when the slice id changes
  //----------------------------------------------------------------------------
  m_Mat = new (vtkMatrix4x4*[m_NumberOfSlices]) ;
  m_Invmat = new (vtkMatrix4x4*[m_NumberOfSlices]) ;
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_Mat[i] = vtkMatrix4x4::New() ;
    m_Invmat[i] = vtkMatrix4x4::New() ;
  }



  //----------------------------------------------------------------------------
  // muscle polydata.
  // The muscle is transformed into slice coords.
  // The mapper and actor are only provided here in case you want to make it
  // visible for debugging purposes
  //
  //              muscle (patient space)
  //                |
  //                |           MuscleTransform to coords of current slice
  //                |          /
  //       MuscleTransformFilter
  //                |
  //              Mapper
  //                |
  //              Actor (not normally visible unless required for debugging)
  //
  //----------------------------------------------------------------------------


  m_MuscleTransform = vtkTransform::New() ; // transforms the muscle into slice coords
  m_MuscleTransform->Identity() ;

  m_MuscleTransformFilter = vtkTransformPolyDataFilter::New() ;
  m_MuscleTransformFilter->SetInputData(muscle) ;
  m_MuscleTransformFilter->SetTransform(m_MuscleTransform) ;

  m_MuscleMapper = vtkPolyDataMapper::New() ;
  m_MuscleMapper->SetInputConnection(m_MuscleTransformFilter->GetOutputPort()) ;

  m_MuscleActor = vtkActor::New() ;
  m_MuscleActor->SetMapper(m_MuscleMapper) ;
  m_MuscleActor->GetProperty()->SetOpacity(0.1) ;
  m_MuscleActor->GetProperty()->SetColor(1,0,0) ; // red
  m_MuscleActor->SetVisibility(m_MusclePolyDataOn) ;
  m_Renderer->AddActor(m_MuscleActor) ;




  //----------------------------------------------------------------------------
  // Slice pipeline.
  // Note that the slicing takes place in patient space,
  // but the slice is transformed back to slice coords for viewing.
  //
  //            plane source
  //                |
  //                |           scale transform (sets size)
  //                |          /
  //        TransformScaleFilter 
  //                |
  //                |           slice transform[i] (sets position and rotation)
  //                |          /
  //        SliceTransformFilter[i]
  //                |
  //                |       muscle (patient space)
  //                |      /
  //            ProbeFilter[i]
  //                |
  //                |           inv transform[i] back to slice coords
  //                |          /
  //       SliceInvTransformFilter[i]
  //                |
  //              Mapper[i]
  //                |
  //              Actor[i]
  //                
  //----------------------------------------------------------------------------
  m_SliceSource = vtkPlaneSource::New() ;
  m_SliceSource->SetXResolution(64) ;
  m_SliceSource->SetYResolution(64) ;

  m_SliceTransformScale = vtkTransform::New() ; // transform which sets size
  m_SliceTransformScale->Identity() ;

  m_SliceTransformScaleFilter = vtkTransformPolyDataFilter::New() ;
  m_SliceTransformScaleFilter->SetInputConnection(m_SliceSource->GetOutputPort()) ;
  m_SliceTransformScaleFilter->SetTransform(m_SliceTransformScale) ;

  m_SliceTransform = new (vtkTransform*[m_NumberOfSlices]) ;
  m_SliceTransformFilter = new (vtkTransformPolyDataFilter*[m_NumberOfSlices]) ;
  m_SliceProbeFilter = new (vtkProbeFilter*[m_NumberOfSlices]) ;
  m_SliceInvTransform = new (vtkTransform*[m_NumberOfSlices]) ;
  m_SliceInvTransformFilter = new (vtkTransformPolyDataFilter*[m_NumberOfSlices]) ;
  m_SliceMapper = new (vtkPolyDataMapper*[m_NumberOfSlices]) ;
  m_SliceActor = new (vtkActor*[m_NumberOfSlices]) ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceTransform[i] = vtkTransform::New() ; // transform which sets position and rotation
    m_SliceTransform[i]->Identity() ;

    m_SliceTransformFilter[i] = vtkTransformPolyDataFilter::New() ;
    m_SliceTransformFilter[i]->SetInputConnection(m_SliceTransformScaleFilter->GetOutputPort()) ;
    m_SliceTransformFilter[i]->SetTransform(m_SliceTransform[i]) ;

    m_SliceProbeFilter[i] = vtkProbeFilter::New() ;
    m_SliceProbeFilter[i]->SetSourceData(volume) ; // input volume
    m_SliceProbeFilter[i]->SetInputConnection(m_SliceTransformFilter[i]->GetOutputPort()) ;

    m_SliceInvTransform[i] = vtkTransform::New() ; // inverse transform back to slice coords
    m_SliceInvTransform[i]->Identity() ;

    m_SliceInvTransformFilter[i] = vtkTransformPolyDataFilter::New() ;
    m_SliceInvTransformFilter[i]->SetInputConnection(m_SliceProbeFilter[i]->GetOutputPort()) ;
    m_SliceInvTransformFilter[i]->SetTransform(m_SliceInvTransform[i]) ;

    m_SliceMapper[i] = vtkPolyDataMapper::New() ;
    m_SliceMapper[i]->SetInputConnection(m_SliceInvTransformFilter[i]->GetOutputPort()) ;
    m_SliceMapper[i]->SetLookupTable(m_Lut);
    m_SliceMapper[i]->UseLookupTableScalarRangeOn();
    m_SliceMapper[i]->SetColorModeToMapScalars();

    m_SliceActor[i] = vtkActor::New() ;
    m_SliceActor[i]->SetMapper(m_SliceMapper[i]) ;
    m_Renderer->AddActor(m_SliceActor[i]) ;
  }




  //----------------------------------------------------------------------------
  // Contour axes
  //
  //              LineSource
  //                  |             ContourAxesTransform
  //                  |            /
  //              TransfromPDFilter
  //                  |
  //              TubeFilter
  //                  |
  //               Mapper
  //                  |
  //                Actor
  //
  //----------------------------------------------------------------------------

  // Transform 
  m_ContourAxesTransform = vtkTransform::New() ;
  m_ContourAxesTransform->Identity() ;

  // east contour axis
  m_ContourPosXAxisLineSource = vtkLineSource::New();
  m_ContourPosXAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_ContourPosXAxisTransformPDFilter->SetInputConnection(m_ContourPosXAxisLineSource->GetOutputPort()) ;
  m_ContourPosXAxisTransformPDFilter->SetTransform(m_ContourAxesTransform) ;
  m_ContourPosXAxisTubeFilter = vtkTubeFilter::New();
  m_ContourPosXAxisTubeFilter->SetInputConnection(m_ContourPosXAxisTransformPDFilter->GetOutputPort());
  m_ContourPosXAxisMapper = vtkPolyDataMapper::New();
  m_ContourPosXAxisMapper->SetInputConnection(m_ContourPosXAxisTubeFilter->GetOutputPort());
  m_ContourPosXAxisActor = vtkActor::New();
  m_ContourPosXAxisActor->SetMapper(m_ContourPosXAxisMapper);
  m_ContourPosXAxisActor->SetVisibility(m_ContourAxesOn) ;
  m_Renderer->AddActor(m_ContourPosXAxisActor);

  // north contour axis
  m_ContourPosYAxisLineSource = vtkLineSource::New();
  m_ContourPosYAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_ContourPosYAxisTransformPDFilter->SetInputConnection(m_ContourPosYAxisLineSource->GetOutputPort()) ;
  m_ContourPosYAxisTransformPDFilter->SetTransform(m_ContourAxesTransform) ;
  m_ContourPosYAxisTubeFilter = vtkTubeFilter::New();
  m_ContourPosYAxisTubeFilter->SetInputConnection(m_ContourPosYAxisTransformPDFilter->GetOutputPort());
  m_ContourPosYAxisMapper = vtkPolyDataMapper::New();
  m_ContourPosYAxisMapper->SetInputConnection(m_ContourPosYAxisTubeFilter->GetOutputPort());
  m_ContourPosYAxisActor = vtkActor::New();
  m_ContourPosYAxisActor->SetMapper(m_ContourPosYAxisMapper);
  m_ContourPosYAxisActor->SetVisibility(m_ContourAxesOn) ;
  m_Renderer->AddActor(m_ContourPosYAxisActor);

  // west contour axis
  m_ContourNegXAxisLineSource = vtkLineSource::New();
  m_ContourNegXAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_ContourNegXAxisTransformPDFilter->SetInputConnection(m_ContourNegXAxisLineSource->GetOutputPort()) ;
  m_ContourNegXAxisTransformPDFilter->SetTransform(m_ContourAxesTransform) ;
  m_ContourNegXAxisTubeFilter = vtkTubeFilter::New();
  m_ContourNegXAxisTubeFilter->SetInputConnection(m_ContourNegXAxisTransformPDFilter->GetOutputPort());
  m_ContourNegXAxisMapper = vtkPolyDataMapper::New();
  m_ContourNegXAxisMapper->SetInputConnection(m_ContourNegXAxisTubeFilter->GetOutputPort());
  m_ContourNegXAxisActor = vtkActor::New();
  m_ContourNegXAxisActor->SetMapper(m_ContourNegXAxisMapper);
  m_ContourNegXAxisActor->SetVisibility(m_ContourAxesOn) ;
  m_Renderer->AddActor(m_ContourNegXAxisActor);

  // south contour axis
  m_ContourNegYAxisLineSource = vtkLineSource::New();
  m_ContourNegYAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_ContourNegYAxisTransformPDFilter->SetInputConnection(m_ContourNegYAxisLineSource->GetOutputPort()) ;
  m_ContourNegYAxisTransformPDFilter->SetTransform(m_ContourAxesTransform) ;
  m_ContourNegYAxisTubeFilter = vtkTubeFilter::New();
  m_ContourNegYAxisTubeFilter->SetInputConnection(m_ContourNegYAxisTransformPDFilter->GetOutputPort());
  m_ContourNegYAxisMapper = vtkPolyDataMapper::New();
  m_ContourNegYAxisMapper->SetInputConnection(m_ContourNegYAxisTubeFilter->GetOutputPort());
  m_ContourNegYAxisActor = vtkActor::New();
  m_ContourNegYAxisActor->SetMapper(m_ContourNegYAxisMapper);
  m_ContourNegYAxisActor->SetVisibility(m_ContourAxesOn) ;
  m_Renderer->AddActor(m_ContourNegYAxisActor);

  // set parameters of axes
  SetUpContourCoordinateAxes() ;




  //----------------------------------------------------------------------------
  // global axes
  //              LineSource
  //                  |             GlobalAxesTransform
  //                  |            /
  //              TransfromPDFilter
  //                  |
  //              TubeFilter
  //                  |
  //               Mapper
  //                  |
  //                Actor
  //
  //----------------------------------------------------------------------------

  // Transform 
  m_GlobalAxesTransform = vtkTransform::New() ;
  m_GlobalAxesTransform->Identity() ;

  // east global axis
  m_GlobalPosXAxisLineSource = vtkLineSource::New();
  m_GlobalPosXAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_GlobalPosXAxisTransformPDFilter->SetInputConnection(m_GlobalPosXAxisLineSource->GetOutputPort()) ;
  m_GlobalPosXAxisTransformPDFilter->SetTransform(m_GlobalAxesTransform) ;
  m_GlobalPosXAxisTubeFilter = vtkTubeFilter::New();
  m_GlobalPosXAxisTubeFilter->SetInputConnection(m_GlobalPosXAxisTransformPDFilter->GetOutputPort());
  m_GlobalPosXAxisMapper = vtkPolyDataMapper::New();
  m_GlobalPosXAxisMapper->SetInputConnection(m_GlobalPosXAxisTubeFilter->GetOutputPort());
  m_GlobalPosXAxisActor = vtkActor::New();
  m_GlobalPosXAxisActor->SetMapper(m_GlobalPosXAxisMapper);
  m_GlobalPosXAxisActor->SetVisibility(m_GlobalAxesOn) ;
  m_Renderer->AddActor(m_GlobalPosXAxisActor);

  // north global axis
  m_GlobalPosYAxisLineSource = vtkLineSource::New();
  m_GlobalPosYAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_GlobalPosYAxisTransformPDFilter->SetInputConnection(m_GlobalPosYAxisLineSource->GetOutputPort()) ;
  m_GlobalPosYAxisTransformPDFilter->SetTransform(m_GlobalAxesTransform) ;
  m_GlobalPosYAxisTubeFilter = vtkTubeFilter::New();
  m_GlobalPosYAxisTubeFilter->SetInputConnection(m_GlobalPosYAxisTransformPDFilter->GetOutputPort());
  m_GlobalPosYAxisMapper = vtkPolyDataMapper::New();
  m_GlobalPosYAxisMapper->SetInputConnection(m_GlobalPosYAxisTubeFilter->GetOutputPort());
  m_GlobalPosYAxisActor = vtkActor::New();
  m_GlobalPosYAxisActor->SetMapper(m_GlobalPosYAxisMapper);
  m_GlobalPosYAxisActor->SetVisibility(m_GlobalAxesOn) ;
  m_Renderer->AddActor(m_GlobalPosYAxisActor);

  // west global axis
  m_GlobalNegXAxisLineSource = vtkLineSource::New();
  m_GlobalNegXAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_GlobalNegXAxisTransformPDFilter->SetInputConnection(m_GlobalNegXAxisLineSource->GetOutputPort()) ;
  m_GlobalNegXAxisTransformPDFilter->SetTransform(m_GlobalAxesTransform) ;
  m_GlobalNegXAxisTubeFilter = vtkTubeFilter::New();
  m_GlobalNegXAxisTubeFilter->SetInputConnection(m_GlobalNegXAxisTransformPDFilter->GetOutputPort());
  m_GlobalNegXAxisMapper = vtkPolyDataMapper::New();
  m_GlobalNegXAxisMapper->SetInputConnection(m_GlobalNegXAxisTubeFilter->GetOutputPort());
  m_GlobalNegXAxisActor = vtkActor::New();
  m_GlobalNegXAxisActor->SetMapper(m_GlobalNegXAxisMapper);
  m_GlobalNegXAxisActor->SetVisibility(m_GlobalAxesOn) ;
  m_Renderer->AddActor(m_GlobalNegXAxisActor);

  // south global axis
  m_GlobalNegYAxisLineSource = vtkLineSource::New();
  m_GlobalNegYAxisTransformPDFilter = vtkTransformPolyDataFilter::New() ;
  m_GlobalNegYAxisTransformPDFilter->SetInputConnection(m_GlobalNegYAxisLineSource->GetOutputPort()) ;
  m_GlobalNegYAxisTransformPDFilter->SetTransform(m_GlobalAxesTransform) ;
  m_GlobalNegYAxisTubeFilter = vtkTubeFilter::New();
  m_GlobalNegYAxisTubeFilter->SetInputConnection(m_GlobalNegYAxisTransformPDFilter->GetOutputPort());
  m_GlobalNegYAxisMapper = vtkPolyDataMapper::New();
  m_GlobalNegYAxisMapper->SetInputConnection(m_GlobalNegYAxisTubeFilter->GetOutputPort());
  m_GlobalNegYAxisActor = vtkActor::New();
  m_GlobalNegYAxisActor->SetMapper(m_GlobalNegYAxisMapper);
  m_GlobalNegYAxisActor->SetVisibility(m_GlobalAxesOn) ;
  m_Renderer->AddActor(m_GlobalNegYAxisActor);


  // set parameters of axes
  SetUpGlobalCoordinateAxes() ;






  //----------------------------------------------------------------------------
  // Contour Pipelines: NE, NW, SE, SW
  //
  //              muscle (patient space)
  //                |
  //                |        ContourCutterTransform (inverse of slice transform to get same plane as slice)
  //                |        /
  //                |      ContourPlane (cut function)
  //                V      /
  //          ContourCutter
  //                |
  //                |          ContourTransform (transform into coords of current slice)
  //                V          /
  //      ContourTransformFilter =====> robust calc. of contour center
  //           /   / \   \
  //          /   /   \   \
  //        NE  NW     SE  SW 
  //         |   |     |   |  
  //         V   V     V   V
  //
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // cutter for contour
  //----------------------------------------------------------------------------

  m_ContourCutterTransform = vtkTransform::New() ;
  m_ContourCutterTransform->Identity() ;

  m_ContourPlane = vtkPlane::New();
  m_ContourPlane->SetOrigin(0,0,0) ;
  m_ContourPlane->SetNormal(0,0,1) ;
  m_ContourPlane->SetTransform(m_ContourCutterTransform) ;

  m_ContourCutter = vtkCutter::New();
  m_ContourCutter->SetCutFunction(m_ContourPlane);
  m_ContourCutter->SetInputData(muscle);

  m_ContourTransform = vtkTransform::New() ;
  m_ContourTransform->Identity() ;

  m_ContourTransformFilter = vtkTransformPolyDataFilter::New() ;
  m_ContourTransformFilter->SetInputConnection(m_ContourCutter->GetOutputPort()) ;
  m_ContourTransformFilter->SetTransform(m_ContourTransform) ;



  //----------------------------------------------------------------------------
  // Cutting planes for contour visual pipes
  // These cut the contour into four quarters.
  //----------------------------------------------------------------------------
  m_CuttingPlanesTransform = vtkTransform::New() ;
  m_CuttingPlanesTransform->Identity() ;

  m_CuttingPlaneN = vtkPlane::New();
  m_CuttingPlaneN->SetNormal(0.0, 1.0, 0.0);
  m_CuttingPlaneN->SetTransform(m_CuttingPlanesTransform) ;

  m_CuttingPlaneS = vtkPlane::New();
  m_CuttingPlaneS->SetNormal(0.0, -1.0, 0.0);
  m_CuttingPlaneS->SetTransform(m_CuttingPlanesTransform) ;

  m_CuttingPlaneE = vtkPlane::New();
  m_CuttingPlaneE->SetNormal(1.0, 0.0, 0.0);
  m_CuttingPlaneE->SetTransform(m_CuttingPlanesTransform) ;

  m_CuttingPlaneW = vtkPlane::New();
  m_CuttingPlaneW->SetNormal(-1.0, 0.0, 0.0);
  m_CuttingPlaneW->SetTransform(m_CuttingPlanesTransform) ;




  //----------------------------------------------------------------------------
  // north-east contour visual pipe
  //
  //        ContourCutter
  //             |
  //             |          ContourTransform (transform into coords of current slice)
  //             V          /
  //      ContourTransformFilter
  //             |
  //             |      cuttingPlanesTransform
  //             |              /
  //             |             /
  //             |           cutting plane N
  //             |           /
  //     NEContourClipFilterN
  //             |             cuttingPlanesTransform
  //             |              /
  //             |             /
  //             |           cutting plane E
  //             |           /
  //     NEContourClipFilterE ==================> original bounds and center
  //             |
  //             |                Transform set by widget
  //             |               /
  //   NEContourTransformPDFilter ==============> current bounds and center
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

  m_NEContourClipFilterN = vtkClipPolyData::New();
  m_NEContourClipFilterN->SetInputConnection(m_ContourTransformFilter->GetOutputPort()); // first cut plane
  m_NEContourClipFilterN->SetClipFunction(m_CuttingPlaneN);
  m_NEContourClipFilterN->GlobalWarningDisplayOff();

  m_NEContourClipFilterE = vtkClipPolyData::New();
  m_NEContourClipFilterE->SetInputConnection(m_NEContourClipFilterN->GetOutputPort()); // second cut plane
  m_NEContourClipFilterE->SetClipFunction(m_CuttingPlaneE);
  m_NEContourClipFilterE->GlobalWarningDisplayOff();

  m_NEContourTransform = vtkTransform::New() ;
  m_NEContourTransform->Identity() ;

  m_NEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_NEContourTransformPolyDataFilter->SetInputConnection(m_NEContourClipFilterE->GetOutputPort());
  m_NEContourTransformPolyDataFilter->SetTransform(m_NEContourTransform) ;

  m_NEContourTubeFilter =  vtkTubeFilter::New();
  m_NEContourTubeFilter->SetInputConnection(m_NEContourTransformPolyDataFilter->GetOutputPort());
  m_NEContourTubeFilter->SetRadius(m_TubeSize);
  m_NEContourTubeFilter->SetNumberOfSides(12);

  m_NEContourMapper = vtkPolyDataMapper::New();
  m_NEContourMapper->SetInputConnection(m_NEContourTubeFilter->GetOutputPort());

  m_NEContourActor = vtkActor::New();
  m_NEContourActor->SetMapper(m_NEContourMapper);
  m_NEContourActor->GetProperty()->SetColor(1.0, 1.0, 0.0);
  m_NEContourActor->SetVisibility(m_ContourOn);

  m_Renderer->AddActor(m_NEContourActor);



  //----------------------------------------------------------------------------
  // north-west contour visual pipe
  //----------------------------------------------------------------------------
  m_NWContourClipFilterN = vtkClipPolyData::New();
  m_NWContourClipFilterN->SetInputConnection(m_ContourTransformFilter->GetOutputPort());
  m_NWContourClipFilterN->SetClipFunction(m_CuttingPlaneN);
  m_NWContourClipFilterN->GlobalWarningDisplayOff();

  m_NWContourClipFilterW = vtkClipPolyData::New();
  m_NWContourClipFilterW->SetInputConnection(m_NWContourClipFilterN->GetOutputPort());
  m_NWContourClipFilterW->SetClipFunction(m_CuttingPlaneW);
  m_NWContourClipFilterW->GlobalWarningDisplayOff();

  m_NWContourTransform = vtkTransform::New() ;
  m_NWContourTransform->Identity() ;

  m_NWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_NWContourTransformPolyDataFilter->SetInputConnection(m_NWContourClipFilterW->GetOutputPort());
  m_NWContourTransformPolyDataFilter->SetTransform(m_NWContourTransform) ;

  m_NWContourTubeFilter =  vtkTubeFilter::New();
  m_NWContourTubeFilter->SetInputConnection(m_NWContourTransformPolyDataFilter->GetOutputPort());
  m_NWContourTubeFilter->SetRadius(m_TubeSize);
  m_NWContourTubeFilter->SetNumberOfSides(12);

  m_NWContourMapper = vtkPolyDataMapper::New();
  m_NWContourMapper->SetInputConnection(m_NWContourTubeFilter->GetOutputPort());

  m_NWContourActor = vtkActor::New();
  m_NWContourActor->SetMapper(m_NWContourMapper);
  m_NWContourActor->GetProperty()->SetColor(0.0, 1.0, 1.0);
  m_NWContourActor->SetVisibility(m_ContourOn);

  m_Renderer->AddActor(m_NWContourActor);


  //----------------------------------------------------------------------------
  // south-east contour visual pipe
  //----------------------------------------------------------------------------
  m_SEContourClipFilterS = vtkClipPolyData::New();
  m_SEContourClipFilterS->SetInputConnection(m_ContourTransformFilter->GetOutputPort());
  m_SEContourClipFilterS->SetClipFunction(m_CuttingPlaneS);
  m_SEContourClipFilterS->GlobalWarningDisplayOff();

  m_SEContourClipFilterE = vtkClipPolyData::New();
  m_SEContourClipFilterE->SetInputConnection(m_SEContourClipFilterS->GetOutputPort());
  m_SEContourClipFilterE->SetClipFunction(m_CuttingPlaneE);
  m_SEContourClipFilterE->GlobalWarningDisplayOff();

  m_SEContourTransform = vtkTransform::New() ;
  m_SEContourTransform->Identity() ;

  m_SEContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_SEContourTransformPolyDataFilter->SetInputConnection(m_SEContourClipFilterE->GetOutputPort());
  m_SEContourTransformPolyDataFilter->SetTransform(m_SEContourTransform) ;

  m_SEContourTubeFilter =  vtkTubeFilter::New();
  m_SEContourTubeFilter->SetInputConnection(m_SEContourTransformPolyDataFilter->GetOutputPort());
  m_SEContourTubeFilter->SetRadius(m_TubeSize);
  m_SEContourTubeFilter->SetNumberOfSides(12);

  m_SEContourMapper = vtkPolyDataMapper::New();
  m_SEContourMapper->SetInputConnection(m_SEContourTubeFilter->GetOutputPort());

  m_SEContourActor = vtkActor::New();
  m_SEContourActor->SetMapper(m_SEContourMapper);
  m_SEContourActor->GetProperty()->SetColor(0.0, 1.0, 1.0);
  m_SEContourActor->SetVisibility(m_ContourOn);

  m_Renderer->AddActor(m_SEContourActor);


  //----------------------------------------------------------------------------
  // south-west contour visual pipe
  //----------------------------------------------------------------------------
  m_SWContourClipFilterS = vtkClipPolyData::New();
  m_SWContourClipFilterS->SetInputConnection(m_ContourTransformFilter->GetOutputPort());
  m_SWContourClipFilterS->SetClipFunction(m_CuttingPlaneS);
  m_SWContourClipFilterS->GlobalWarningDisplayOff();

  m_SWContourClipFilterW = vtkClipPolyData::New();
  m_SWContourClipFilterW->SetInputConnection(m_SWContourClipFilterS->GetOutputPort());
  m_SWContourClipFilterW->SetClipFunction(m_CuttingPlaneW);
  m_SWContourClipFilterW->GlobalWarningDisplayOff();

  m_SWContourTransform = vtkTransform::New() ;
  m_SWContourTransform->Identity() ;

  m_SWContourTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_SWContourTransformPolyDataFilter->SetInputConnection(m_SWContourClipFilterW->GetOutputPort());
  m_SWContourTransformPolyDataFilter->SetTransform(m_SWContourTransform) ;

  m_SWContourTubeFilter =  vtkTubeFilter::New();
  m_SWContourTubeFilter->SetInputConnection(m_SWContourTransformPolyDataFilter->GetOutputPort());
  m_SWContourTubeFilter->SetRadius(m_TubeSize);
  m_SWContourTubeFilter->SetNumberOfSides(12);

  m_SWContourMapper = vtkPolyDataMapper::New();
  m_SWContourMapper->SetInputConnection(m_SWContourTubeFilter->GetOutputPort());

  m_SWContourActor = vtkActor::New();
  m_SWContourActor->SetMapper(m_SWContourMapper);
  m_SWContourActor->GetProperty()->SetColor(1.0, 1.0, 0.0);
  m_SWContourActor->SetVisibility(m_ContourOn);

  m_Renderer->AddActor(m_SWContourActor);




  //----------------------------------------------------------------------------
  // Display text information visual pipes
  // Just set the mapper input to the required text
  //----------------------------------------------------------------------------

  m_TextMapperX = vtkTextMapper::New();
  m_TextMapperX->GetTextProperty()->SetFontSize(10);
  m_TextMapperX->GetTextProperty()->SetFontFamilyToCourier();

  m_ScaledTextActorX = vtkScaledTextActor::New();
  m_ScaledTextActorX->SetMapper(m_TextMapperX);
  m_ScaledTextActorX->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_ScaledTextActorX->SetVisibility(m_TextXOn) ;
  m_Renderer->AddActor2D(m_ScaledTextActorX);

  m_TextMapperY = vtkTextMapper::New();
  m_TextMapperY->GetTextProperty()->SetFontSize(10);
  m_TextMapperY->GetTextProperty()->SetFontFamilyToCourier();

  m_ScaledTextActorY = vtkScaledTextActor::New();
  m_ScaledTextActorY->SetMapper(m_TextMapperY);
  m_ScaledTextActorY->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_ScaledTextActorY->SetVisibility(m_TextYOn) ;
  m_Renderer->AddActor2D(m_ScaledTextActorY);
}




//------------------------------------------------------------------------------
// Destructor
albaOpMML3ModelView2DPipe::~albaOpMML3ModelView2DPipe()
//------------------------------------------------------------------------------
{
  // look up table
  m_Lut->Delete() ;


  // matrices
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_Mat[i]->Delete() ;
    m_Invmat[i]->Delete() ;
  }
  delete [] m_Mat ;
  delete [] m_Invmat ;


  // muscle polydata
  m_MuscleTransform->Delete() ;
  m_MuscleTransformFilter->Delete() ;
  m_MuscleMapper->Delete() ;
  m_MuscleActor->Delete() ;


  // slice
  m_SliceSource->Delete() ;
  m_SliceTransformScale->Delete() ;
  m_SliceTransformScaleFilter->Delete() ;

  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceTransform[i]->Delete() ;
    m_SliceTransformFilter[i]->Delete() ;
    m_SliceProbeFilter[i]->Delete() ;
    m_SliceInvTransform[i]->Delete() ;
    m_SliceInvTransformFilter[i]->Delete() ;

    m_SliceMapper[i]->Delete() ;
    m_SliceActor[i]->Delete() ;
  }
  delete [] m_SliceTransform ;
  delete [] m_SliceTransformFilter ;
  delete [] m_SliceProbeFilter ;
  delete [] m_SliceInvTransform ;
  delete [] m_SliceInvTransformFilter ;
  delete [] m_SliceMapper ;
  delete [] m_SliceActor ;

  
  // contour transform
  m_ContourAxesTransform->Delete() ;

  // east contour axis
  m_ContourPosXAxisLineSource->Delete() ;
  m_ContourPosXAxisTransformPDFilter->Delete() ;
  m_ContourPosXAxisTubeFilter->Delete() ;
  m_ContourPosXAxisMapper->Delete() ;
  m_ContourPosXAxisActor->Delete() ;

  // north contour axis
  m_ContourPosYAxisLineSource->Delete() ;
  m_ContourPosYAxisTransformPDFilter->Delete() ;
  m_ContourPosYAxisTubeFilter->Delete() ;
  m_ContourPosYAxisMapper->Delete() ;
  m_ContourPosYAxisActor->Delete() ;

  // west contour axis
  m_ContourNegXAxisLineSource->Delete() ;
  m_ContourNegXAxisTransformPDFilter->Delete() ;
  m_ContourNegXAxisTubeFilter->Delete() ;
  m_ContourNegXAxisMapper->Delete() ;
  m_ContourNegXAxisActor->Delete() ;

  // south contour axis
  m_ContourNegYAxisLineSource->Delete() ;
  m_ContourNegYAxisTransformPDFilter->Delete() ;
  m_ContourNegYAxisTubeFilter->Delete() ;
  m_ContourNegYAxisMapper->Delete() ;
  m_ContourNegYAxisActor->Delete() ;


  // global transform
  m_GlobalAxesTransform->Delete() ;

  // east global axis
  m_GlobalPosXAxisLineSource->Delete() ;
  m_GlobalPosXAxisTransformPDFilter->Delete() ;
  m_GlobalPosXAxisTubeFilter->Delete() ;
  m_GlobalPosXAxisMapper->Delete() ;
  m_GlobalPosXAxisActor->Delete() ;

  // north global axis
  m_GlobalPosYAxisLineSource->Delete() ;
  m_GlobalPosYAxisTransformPDFilter->Delete() ;
  m_GlobalPosYAxisTubeFilter->Delete() ;
  m_GlobalPosYAxisMapper->Delete() ;
  m_GlobalPosYAxisActor->Delete() ;

  // west global axis
  m_GlobalNegXAxisLineSource->Delete() ;
  m_GlobalNegXAxisTransformPDFilter->Delete() ;
  m_GlobalNegXAxisTubeFilter->Delete() ;
  m_GlobalNegXAxisMapper->Delete() ;
  m_GlobalNegXAxisActor->Delete() ;

  // south global axis
  m_GlobalNegYAxisLineSource->Delete() ;
  m_GlobalNegYAxisTransformPDFilter->Delete() ;
  m_GlobalNegYAxisTubeFilter->Delete() ;
  m_GlobalNegYAxisMapper->Delete() ;
  m_GlobalNegYAxisActor->Delete() ;

  // cutter and clipping planes
  m_ContourPlane->Delete() ;
  m_ContourCutterTransform->Delete() ;
  m_ContourCutter->Delete() ;
  m_ContourTransform->Delete() ;
  m_ContourTransformFilter->Delete() ;
  m_CuttingPlanesTransform->Delete() ;
  m_CuttingPlaneN->Delete() ;
  m_CuttingPlaneS->Delete() ;
  m_CuttingPlaneE->Delete() ;
  m_CuttingPlaneW->Delete() ; 

  // north-east contour
  m_NEContourClipFilterN->Delete() ; 
  m_NEContourClipFilterE->Delete() ; 
  m_NEContourTubeFilter->Delete() ;  
  m_NEContourTransform->Delete() ;
  m_NEContourTransformPolyDataFilter->Delete() ;
  m_NEContourMapper->Delete() ; 
  m_NEContourActor->Delete() ; 

  // north-west contour
  m_NWContourClipFilterN->Delete() ; 
  m_NWContourClipFilterW->Delete() ;
  m_NWContourTubeFilter->Delete() ;  
  m_NWContourTransform->Delete() ;
  m_NWContourTransformPolyDataFilter->Delete() ; 
  m_NWContourMapper->Delete() ; 
  m_NWContourActor->Delete() ; 

  // south-east contour
  m_SEContourClipFilterS->Delete() ; 
  m_SEContourClipFilterE->Delete() ; 
  m_SEContourTubeFilter->Delete() ;  
  m_SEContourTransform->Delete() ;
  m_SEContourTransformPolyDataFilter->Delete() ;
  m_SEContourMapper->Delete() ;
  m_SEContourActor->Delete() ;

  // south-west contour
  m_SWContourClipFilterS->Delete() ;
  m_SWContourClipFilterW->Delete() ; 
  m_SWContourTubeFilter->Delete() ;  
  m_SWContourTransform->Delete() ;
  m_SWContourTransformPolyDataFilter->Delete() ; 
  m_SWContourMapper->Delete() ; 
  m_SWContourActor->Delete() ;


  // display information
  m_TextMapperX->Delete() ; 
  m_ScaledTextActorX->Delete() ; 
  m_TextMapperY->Delete() ; 
  m_ScaledTextActorY->Delete() ;
}




//------------------------------------------------------------------------------
// Set visibility
void albaOpMML3ModelView2DPipe::SetVisibility(int visibility)
//------------------------------------------------------------------------------
{
  // set overall visibility state
  m_CurrentVisibility = visibility ;

  if (m_MusclePolyDataOn)
    m_MuscleActor->SetVisibility(visibility) ;

  // turn off all slices except the current id
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceActor[i]->SetVisibility(0) ;
  }
  m_SliceActor[m_CurrentSliceId]->SetVisibility(visibility) ;

  if (m_GlobalAxesOn){
    m_GlobalPosXAxisActor->SetVisibility(visibility) ;
    m_GlobalPosYAxisActor->SetVisibility(visibility) ;
    m_GlobalNegXAxisActor->SetVisibility(visibility) ;
    m_GlobalNegYAxisActor->SetVisibility(visibility) ;
  }

  if (m_ContourAxesOn){
    m_ContourPosXAxisActor->SetVisibility(visibility) ;
    m_ContourPosYAxisActor->SetVisibility(visibility) ;
    m_ContourNegXAxisActor->SetVisibility(visibility) ;
    m_ContourNegYAxisActor->SetVisibility(visibility) ;
  }

  if (m_ContourOn){
    m_NEContourActor->SetVisibility(visibility) ;
    m_NWContourActor->SetVisibility(visibility) ;
    m_SEContourActor->SetVisibility(visibility) ;
    m_SWContourActor->SetVisibility(visibility) ;
  }

  if (m_TextXOn)
    m_ScaledTextActorX->SetVisibility(visibility) ;
  if (m_TextYOn)
    m_ScaledTextActorY->SetVisibility(visibility) ;
}




//------------------------------------------------------------------------------
// Update
void albaOpMML3ModelView2DPipe::Update()
//------------------------------------------------------------------------------
{
  m_Renderer->GetRenderWindow()->Render() ;
}




//------------------------------------------------------------------------------
// Force update of all slices and contours.
// Call this once to pre-process the slices, to avoid sticky processing when moving slider.
void albaOpMML3ModelView2DPipe::UpdateAllSlices()
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    m_SliceActor[i]->SetVisibility(1) ;    // temporarily switch on actors so that they can update
  }
  m_Renderer->GetRenderWindow()->Render() ;

  // set visibility back to normal
  SetVisibility(m_CurrentVisibility) ;
  m_Renderer->GetRenderWindow()->Render() ;
}



//------------------------------------------------------------------------------
// Set current slice id
void albaOpMML3ModelView2DPipe::SetCurrentSliceId(int i)
//------------------------------------------------------------------------------
{
  m_CurrentSliceId = i ;
  SetVisibility(m_CurrentVisibility) ;

  // plug the corresponding matrix into the muscle transform
  m_MuscleTransform->SetMatrix(m_Invmat[i]) ;

  // plug the matrix into the contour cutter transforms
  m_ContourCutterTransform->SetMatrix(m_Invmat[i]) ;  // positions cutting plane in patient space (inverse because it's vtkCutter)
  m_ContourTransform->SetMatrix(m_Invmat[i]) ; // transforms contour into slice coords

  Update() ;
}



//------------------------------------------------------------------------------
// Set transformation matrix which positions slice
void albaOpMML3ModelView2DPipe::SetSliceTransform(int i, vtkMatrix4x4 *mat)
//------------------------------------------------------------------------------
{
  m_Mat[i]->DeepCopy(mat) ;

  vtkMatrix4x4 *invMat = vtkMatrix4x4::New() ;
  vtkMatrix4x4::Invert(mat, invMat) ;
  m_Invmat[i]->DeepCopy(invMat) ;

  m_SliceTransform[i]->SetMatrix(mat) ;
  m_SliceInvTransform[i]->SetMatrix(invMat) ;

  invMat->Delete() ;

}





//------------------------------------------------------------------------------
// set radius of tube filter
void albaOpMML3ModelView2DPipe::SetTubeRadius(double r)
//------------------------------------------------------------------------------
{
  m_TubeSize = r ;

  m_GlobalPosXAxisTubeFilter->SetRadius(r) ;
  m_GlobalPosYAxisTubeFilter->SetRadius(r) ;
  m_GlobalNegXAxisTubeFilter->SetRadius(r) ;
  m_GlobalNegYAxisTubeFilter->SetRadius(r) ;

  m_ContourPosXAxisTubeFilter->SetRadius(r) ;
  m_ContourPosYAxisTubeFilter->SetRadius(r) ;
  m_ContourNegXAxisTubeFilter->SetRadius(r) ;
  m_ContourNegYAxisTubeFilter->SetRadius(r) ;

  m_NEContourTubeFilter->SetRadius(r) ;
  m_NWContourTubeFilter->SetRadius(r) ;
  m_SEContourTubeFilter->SetRadius(r) ;
  m_SWContourTubeFilter->SetRadius(r) ;
}


//------------------------------------------------------------------------------
// set x text
void albaOpMML3ModelView2DPipe::SetTextXVisibility(int visibility)
//------------------------------------------------------------------------------
{
  m_TextXOn = true ;
  m_ScaledTextActorX->SetVisibility(1) ;
}


//------------------------------------------------------------------------------
// set y text
void albaOpMML3ModelView2DPipe::SetTextYVisibility(int visibility)
//------------------------------------------------------------------------------
{
  m_TextYOn = true ;
  m_ScaledTextActorY->SetVisibility(1) ;
}



//------------------------------------------------------------------------------
// set lut range
void albaOpMML3ModelView2DPipe::SetLutRange(double scalarMin, double scalarMax)
//------------------------------------------------------------------------------
{
  m_LutWindow = (scalarMax -  scalarMin);
  m_LutLevel = (scalarMax + scalarMin) / 2.0 ;

  m_Lut->SetWindow(m_LutWindow);
  m_Lut->SetLevel(m_LutLevel);
}






//----------------------------------------------------------------------------
// set up contour axes
void albaOpMML3ModelView2DPipe::SetUpContourCoordinateAxes()
//----------------------------------------------------------------------------
{
  // east
  m_ContourPosXAxisTubeFilter->SetRadius(m_TubeSize);
  m_ContourPosXAxisTubeFilter->SetNumberOfSides(12);
  m_ContourPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosXAxisLineSource->SetPoint2(m_AxisLengthX, 0.0, 0.0);
  m_ContourPosXAxisActor->GetProperty()->SetColor(0.0, 0.0, 1.0); // blue

  // north
  m_ContourPosYAxisTubeFilter->SetRadius(m_TubeSize);
  m_ContourPosYAxisTubeFilter->SetNumberOfSides(12);
  m_ContourPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosXAxisLineSource->SetPoint2(0, m_AxisLengthY, 0.0);
  m_ContourPosYAxisActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // red

  // west
  m_ContourNegXAxisTubeFilter->SetRadius(m_TubeSize);
  m_ContourNegXAxisTubeFilter->SetNumberOfSides(12);
  m_ContourNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosXAxisLineSource->SetPoint2(-m_AxisLengthX, 0.0, 0.0);
  m_ContourNegXAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0); // magenta

  // south
  m_ContourNegYAxisTubeFilter->SetRadius(m_TubeSize);
  m_ContourNegYAxisTubeFilter->SetNumberOfSides(12);
  m_ContourNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_ContourPosXAxisLineSource->SetPoint2(0, -m_AxisLengthY, 0.0);
  m_ContourNegYAxisActor->GetProperty()->SetColor(0.0, 1.0, 0.0); // green
}




//----------------------------------------------------------------------------
// set up global axes
void albaOpMML3ModelView2DPipe::SetUpGlobalCoordinateAxes()
//----------------------------------------------------------------------------
{
  // east
  m_GlobalPosXAxisTubeFilter->SetRadius(m_TubeSize);
  m_GlobalPosXAxisTubeFilter->SetNumberOfSides(12);
  m_GlobalPosXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_GlobalPosXAxisLineSource->SetPoint2(m_AxisLengthX, 0.0, 0.0);
  m_GlobalPosXAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

  // north
  m_GlobalPosYAxisTubeFilter->SetRadius(m_TubeSize);
  m_GlobalPosYAxisTubeFilter->SetNumberOfSides(12);
  m_GlobalPosYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_GlobalPosYAxisLineSource->SetPoint2(0.0, m_AxisLengthY, 0.0);
  m_GlobalPosYAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

  // west
  m_GlobalNegXAxisTubeFilter->SetRadius(m_TubeSize);
  m_GlobalNegXAxisTubeFilter->SetNumberOfSides(12);
  m_GlobalNegXAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_GlobalNegXAxisLineSource->SetPoint2(-m_AxisLengthX, 0.0, 0.0);
  m_GlobalNegXAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow

  // south
  m_GlobalNegYAxisTubeFilter->SetRadius(m_TubeSize);
  m_GlobalNegYAxisTubeFilter->SetNumberOfSides(12);
  m_GlobalNegYAxisLineSource->SetPoint1(0.0, 0.0, 0.0);
  m_GlobalNegYAxisLineSource->SetPoint2(0.0, -m_AxisLengthY, 0.0);
  m_GlobalNegYAxisActor->GetProperty()->SetColor(1.0, 1.0, 0.0); // yellow
}




//------------------------------------------------------------------------------
// Set resolution of slice
void albaOpMML3ModelView2DPipe::SetSliceResolution(int resX, int resY)
//------------------------------------------------------------------------------
{
  m_ResX = resX ;
  m_ResY = resY ;
  m_SliceSource->SetXResolution(resX) ;
  m_SliceSource->SetYResolution(resY) ;
}


//------------------------------------------------------------------------------
// Set size of slice.
// This also sets the length of the axes and the tube size
void albaOpMML3ModelView2DPipe::SetSliceSize(double sizeX, double sizeY)
//------------------------------------------------------------------------------
{
  m_SizeX = sizeX ;
  m_SizeY = sizeY ;
  m_SliceTransformScale->Scale(sizeX, sizeY, 1.0) ;


  // adjust size of axes
  m_AxisLengthX = sizeX / 4.0 ;
  m_AxisLengthY = sizeY / 4.0 ;

  m_ContourPosXAxisLineSource->SetPoint2(m_AxisLengthX, 0.0, 0.0);
  m_ContourPosYAxisLineSource->SetPoint2(0.0, m_AxisLengthY, 0.0);
  m_ContourNegXAxisLineSource->SetPoint2(-m_AxisLengthX, 0.0, 0.0);
  m_ContourNegYAxisLineSource->SetPoint2(0.0, -m_AxisLengthY, 0.0);

  m_GlobalPosXAxisLineSource->SetPoint2(m_AxisLengthX, 0.0, 0.0);
  m_GlobalPosYAxisLineSource->SetPoint2(0.0, m_AxisLengthY, 0.0);
  m_GlobalNegXAxisLineSource->SetPoint2(-m_AxisLengthX, 0.0, 0.0);
  m_GlobalNegYAxisLineSource->SetPoint2(0.0, -m_AxisLengthY, 0.0);


  // set the tube size
  double meanLength = (m_AxisLengthX + m_AxisLengthY) / 2.0 ;
  SetTubeRadius(meanLength/100.0) ;
}




//------------------------------------------------------------------------------
// Set visibility of contour axes
void albaOpMML3ModelView2DPipe::SetContourAxesVisibility(int visibility)
//------------------------------------------------------------------------------
{
  m_ContourAxesOn = visibility ;
  m_GlobalAxesOn = 1-visibility ;
  SetVisibility(m_CurrentVisibility) ; // update visibility
}


//------------------------------------------------------------------------------
// Set visibility of global axes
void albaOpMML3ModelView2DPipe::SetGlobalAxesVisibility(int visibility)
//------------------------------------------------------------------------------
{
  m_ContourAxesOn = 1-visibility ;
  m_GlobalAxesOn = visibility ;
  SetVisibility(m_CurrentVisibility) ; // update visibility
}


//------------------------------------------------------------------------------
// Transform a point in coords of current slice to patient coords.
// Useful for positioning the camera
void albaOpMML3ModelView2DPipe::TransformPointToPatient(double *p)
//------------------------------------------------------------------------------
{
  vtkTransform *t = vtkTransform::New() ;
  t->SetMatrix(m_Invmat[m_CurrentSliceId]) ;
  t->TransformDoublePoint(p) ;
  t->Delete() ;
}


//------------------------------------------------------------------------------
// Set transform of contour axes and contour clipping planes
void albaOpMML3ModelView2DPipe::SetContourAxesTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_ContourAxesTransform->SetMatrix(transform->GetMatrix()) ;
}


//------------------------------------------------------------------------------
// Set transform of global axes
void albaOpMML3ModelView2DPipe::SetGlobalAxesTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_GlobalAxesTransform->SetMatrix(transform->GetMatrix()) ;
}




//------------------------------------------------------------------------------
// Get the bounds of the contour quadrant
void albaOpMML3ModelView2DPipe::GetOriginalSegmentBoundsNE(double *bounds) 
//------------------------------------------------------------------------------
{
  m_NEContourClipFilterE->GetOutput()->GetBounds(bounds) ;
}



//------------------------------------------------------------------------------
// Get the bounds of the contour quadrant
void albaOpMML3ModelView2DPipe::GetOriginalSegmentBoundsNW(double *bounds) 
//------------------------------------------------------------------------------
{
  m_NWContourClipFilterW->GetOutput()->GetBounds(bounds) ;
}


//------------------------------------------------------------------------------
// Get the bounds of the contour quadrant
void albaOpMML3ModelView2DPipe::GetOriginalSegmentBoundsSE(double *bounds) 
//------------------------------------------------------------------------------
{
  m_SEContourClipFilterE->GetOutput()->GetBounds(bounds) ;
}



//------------------------------------------------------------------------------
// Get the bounds of the contour quadrant
void albaOpMML3ModelView2DPipe::GetOriginalSegmentBoundsSW(double *bounds) 
//------------------------------------------------------------------------------
{
  m_SWContourClipFilterW->GetOutput()->GetBounds(bounds) ;
}



//------------------------------------------------------------------------------
// Get center of contour quadrant after clipping, before transform
void albaOpMML3ModelView2DPipe::GetOriginalSegmentCenterNE(double *center) 
//------------------------------------------------------------------------------
{
  double bounds[6] ;
  GetOriginalSegmentBoundsNE(bounds) ;
  center[0] = (bounds[0] + bounds[1]) / 2.0 ;
  center[1] = (bounds[2] + bounds[3]) / 2.0 ;
  center[2] = (bounds[4] + bounds[5]) / 2.0 ;
}


//------------------------------------------------------------------------------
// Get center of contour quadrant after clipping, before transform
void albaOpMML3ModelView2DPipe::GetOriginalSegmentCenterNW(double *center) 
//------------------------------------------------------------------------------
{
  double bounds[6] ;
  GetOriginalSegmentBoundsNW(bounds) ;
  center[0] = (bounds[0] + bounds[1]) / 2.0 ;
  center[1] = (bounds[2] + bounds[3]) / 2.0 ;
  center[2] = (bounds[4] + bounds[5]) / 2.0 ;
}


//------------------------------------------------------------------------------
// Get center of contour quadrant after clipping, before transform
void albaOpMML3ModelView2DPipe::GetOriginalSegmentCenterSE(double *center) 
//------------------------------------------------------------------------------
{
  double bounds[6] ;
  GetOriginalSegmentBoundsSE(bounds) ;
  center[0] = (bounds[0] + bounds[1]) / 2.0 ;
  center[1] = (bounds[2] + bounds[3]) / 2.0 ;
  center[2] = (bounds[4] + bounds[5]) / 2.0 ;
}


//------------------------------------------------------------------------------
// Get center of contour quadrant after clipping, before transform
void albaOpMML3ModelView2DPipe::GetOriginalSegmentCenterSW(double *center) 
//------------------------------------------------------------------------------
{
  double bounds[6] ;
  GetOriginalSegmentBoundsSW(bounds) ;
  center[0] = (bounds[0] + bounds[1]) / 2.0 ;
  center[1] = (bounds[2] + bounds[3]) / 2.0 ;
  center[2] = (bounds[4] + bounds[5]) / 2.0 ;
}



//------------------------------------------------------------------------------
// Get the bounds of the current contour 
void albaOpMML3ModelView2DPipe::GetCurrentContourBounds(double *bounds) 
//------------------------------------------------------------------------------
{
  double bNE[6], bSE[6], bSW[6], bNW[6] ;

  m_NEContourTransformPolyDataFilter->GetOutput()->GetBounds(bNE) ;
  m_NWContourTransformPolyDataFilter->GetOutput()->GetBounds(bNW) ;
  m_SEContourTransformPolyDataFilter->GetOutput()->GetBounds(bSE) ;
  m_SWContourTransformPolyDataFilter->GetOutput()->GetBounds(bSW) ;

  bounds[0] = std::min(bNE[0], bSE[0]) ;
  bounds[0] = std::min(bounds[0], bSW[0]) ;
  bounds[0] = std::min(bounds[0], bNW[0]) ;

  bounds[1] = std::max(bNE[1], bSE[1]) ;
  bounds[1] = std::max(bounds[1], bSW[1]) ;
  bounds[1] = std::max(bounds[1], bNW[1]) ;

  bounds[2] = std::min(bNE[2], bSE[2]) ;
  bounds[2] = std::min(bounds[2], bSW[2]) ;
  bounds[2] = std::min(bounds[2], bNW[2]) ;

  bounds[3] = std::max(bNE[3], bSE[3]) ;
  bounds[3] = std::max(bounds[3], bSW[3]) ;
  bounds[3] = std::max(bounds[3], bNW[3]) ;

  bounds[4] = std::min(bNE[4], bSE[4]) ;
  bounds[4] = std::min(bounds[4], bSW[4]) ;
  bounds[4] = std::min(bounds[4], bNW[4]) ;

  bounds[5] = std::max(bNE[5], bSE[5]) ;
  bounds[5] = std::max(bounds[5], bSW[5]) ;
  bounds[5] = std::max(bounds[5], bNW[5]) ;
}


//------------------------------------------------------------------------------
// Get the center of the current contour 
void albaOpMML3ModelView2DPipe::GetCurrentContourCenter(double *center) 
//------------------------------------------------------------------------------
{
  double bounds[6] ;
  GetCurrentContourBounds(bounds) ;
  center[0] = (bounds[0] + bounds[1]) / 2.0 ;
  center[1] = (bounds[2] + bounds[3]) / 2.0 ;
  center[2] = (bounds[4] + bounds[5]) / 2.0 ;
}




//------------------------------------------------------------------------------
// Set the transform of quadrant cutting planes
// This is the method which applies the effect of the widget to the contour
// Note: This will be applied to an implicit function vtkPlane, so we must give it the inverse matrix.
void albaOpMML3ModelView2DPipe::SetCuttingPlanesTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  vtkMatrix4x4 *mat = vtkMatrix4x4::New() ;
  mat->DeepCopy(transform->GetMatrix()) ;

  vtkMatrix4x4 *invMat = vtkMatrix4x4::New() ;
  mat->Invert(mat, invMat) ;

  m_CuttingPlanesTransform->SetMatrix(invMat) ;

  mat->Delete() ;
  invMat->Delete() ;
}


//------------------------------------------------------------------------------
// Set the transform of the contour quadrant
// This is the method which applies the effect of the widget to the contour
void albaOpMML3ModelView2DPipe::SetNEContourTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_NEContourTransform->SetMatrix(transform->GetMatrix()) ;
}

//------------------------------------------------------------------------------
// Set the transform of the contour quadrant
// This is the method which applies the effect of the widget to the contour
void albaOpMML3ModelView2DPipe::SetSEContourTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_SEContourTransform->SetMatrix(transform->GetMatrix()) ;
}

//------------------------------------------------------------------------------
// Set the transform of the contour quadrant
// This is the method which applies the effect of the widget to the contour
void albaOpMML3ModelView2DPipe::SetNWContourTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_NWContourTransform->SetMatrix(transform->GetMatrix()) ;
}

//------------------------------------------------------------------------------
// Set the transform of the contour quadrant
// This is the method which applies the effect of the widget to the contour
void albaOpMML3ModelView2DPipe::SetSWContourTransform(vtkTransform *transform)
//------------------------------------------------------------------------------
{
  m_SWContourTransform->SetMatrix(transform->GetMatrix()) ;
} 






//------------------------------------------------------------------------------
// Calculate robust center of the current (untransformed) contour
void albaOpMML3ModelView2DPipe::CalculateRobustCenterOfContour(double *pos) const
//------------------------------------------------------------------------------
{
  vtkPoints *contourPoints = m_ContourTransformFilter->GetOutput()->GetPoints() ;

  int numPts = contourPoints->GetNumberOfPoints() ;

  if (numPts == 0){
    // no points - set to zero and return
    pos[0] = 0.0 ;
    pos[1] = 0.0 ;
    pos[2] = 0.0 ;
    return ;
  }

  std::vector<double> posx ;
  std::vector<double> posy ;
  std::vector<double> posz ;

  // list the x, y and z values
  for (int i = 0 ;  i < numPts ;  i++){
    double pt[3] ;
    contourPoints->GetPoint(i, pt) ;
    posx.push_back(pt[0]) ;
    posy.push_back(pt[1]) ;
    posz.push_back(pt[2]) ;
  }

  // sort the lists
  std::sort(posx.begin(), posx.end()) ;
  std::sort(posy.begin(), posy.end()) ;
  std::sort(posz.begin(), posz.end()) ;

  // cut-off points in list
  int nLo = (int)(0.1 * (double)numPts) ;
  int nHi = (int)(0.9 * (double)numPts) ;

  // Return value is bounds of data with extreme values removed
  // This works better than just returning the median.
  pos[0] = (posx.at(nLo) + posx.at(nHi)) / 2.0  ;
  pos[1] = (posy.at(nLo) + posy.at(nHi)) / 2.0  ;
  pos[2] = (posz.at(nLo) + posz.at(nHi)) / 2.0  ;
}




//------------------------------------------------------------------------------
// Print self
void albaOpMML3ModelView2DPipe::PrintSelf(ostream& os, int indent)
//------------------------------------------------------------------------------
{
  Update() ;

  os << "MML Visual pipe 2d" << std::endl ;
  os << std::endl ;

  os << "visibility = " << m_CurrentVisibility << std::endl ;
  os << std::endl ;

  os << "current slice = " << m_CurrentSliceId << std::endl ;
  os << std::endl ;

  os << "tube size = " << m_TubeSize << std::endl ;
  os << std::endl ;

  os << "axis length = " << m_AxisLengthX << " " << m_AxisLengthY << std::endl ;
  os << std::endl ;

  double *b ;
  double *c ;

  // muscle
  b = m_MuscleActor->GetBounds() ;
  c = m_MuscleActor->GetCenter() ;
  os << "muscle " << "visibility = " << m_MuscleActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  // total contour
  GetCurrentContourBounds(b) ;
  GetCurrentContourCenter(c) ;
  os << "total contour " << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;


  /* // all slices
  for (int i = 0 ;  i < m_NumberOfSlices ;  i++){
    b = m_SliceActor[i]->GetBounds() ;
    c = m_SliceActor[i]->GetCenter() ;
    os << "slice " << i << " " << "visibility = " << m_SliceActor[i]->GetVisibility() << std::endl ;
    os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
    os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
    os << std::endl ;
  } */


  // current slice
  b = m_SliceActor[m_CurrentSliceId]->GetBounds() ;
  c = m_SliceActor[m_CurrentSliceId]->GetCenter() ;
  os << "current slice " << m_CurrentSliceId << " " << "visibility = " << m_SliceActor[m_CurrentSliceId]->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;


  // global axes
  b = m_GlobalPosXAxisActor->GetBounds() ;
  os << "globalPosXAxis " << "visibility = " << m_GlobalPosXAxisActor->GetVisibility() << std::endl ;
  os << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_GlobalPosYAxisActor->GetBounds() ;
  os << "globalPosYAxis " << "visibility = " << m_GlobalPosYAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_GlobalNegXAxisActor->GetBounds() ;
  os << "globalNegXAxis " << "visibility = " << m_GlobalNegXAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_GlobalNegYAxisActor->GetBounds() ;
  os << "globalNegYAxis " << "visibility = " << m_GlobalNegYAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << std::endl ;


  // contour axes
  b = m_ContourPosXAxisActor->GetBounds() ;
  os << "contourPosXAxis " << "visibility = " << m_ContourPosXAxisActor->GetVisibility() << std::endl ;
  os << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_ContourPosYAxisActor->GetBounds() ;
  os << "contourPosYAxis " << "visibility = " << m_ContourPosYAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_ContourNegXAxisActor->GetBounds() ;
  os << "contourNegXAxis " << "visibility = " << m_ContourNegXAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;

  b = m_ContourNegYAxisActor->GetBounds() ;
  os << "contourNegYAxis " << "visibility = " << m_ContourNegYAxisActor->GetVisibility()
    << " bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << std::endl ;


  // contour segments
  b = m_NEContourActor->GetBounds() ;
  c = m_NEContourActor->GetCenter() ;
  os << "NEContour " << "visibility = " << m_NEContourActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  b = m_NWContourActor->GetBounds() ;
  c = m_NWContourActor->GetCenter() ;
  os << "NWContour " << "visibility = " << m_NWContourActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  b = m_SEContourActor->GetBounds() ;
  c = m_SEContourActor->GetCenter() ;
  os << "SEContour " << "visibility = " << m_SEContourActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;

  b = m_SWContourActor->GetBounds() ;
  c = m_SWContourActor->GetCenter() ;
  os << "SWContour " << "visibility = " << m_SWContourActor->GetVisibility() << std::endl ;
  os << "bounds " << b[0] << " " << b[1] << " " << b[2] << " " << b[3] << " " << b[4] << " " << b[5] << std::endl ;
  os << "center " << c[0] << " " << c[1] << " " << c[2] << std::endl ;
  os << std::endl ;


  // camera
  double fp[3], campos[3] ;
  m_Renderer->GetActiveCamera()->GetFocalPoint(fp) ;
  m_Renderer->GetActiveCamera()->GetPosition(campos) ;
  os << "camera focus " << fp[0] << " " << fp[1] << " " << fp[2] << std::endl ;
  os << "camera pos " << campos[0] << " " << campos[1] << " " << campos[2] << std::endl ;
  os << std::endl ;


  // text
  os << "Text X " << "visibility = " << m_ScaledTextActorX->GetVisibility() << std::endl ;
  os << "Text Y " << "visibility = " << m_ScaledTextActorY->GetVisibility() << std::endl ;
  os << std::endl ;
  os << std::endl ;
}
