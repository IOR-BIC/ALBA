/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAContourVolumeMapperTest
 Authors: Matteo Giacomoni, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBASmartPointer.h"

#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetReader.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkMath.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataNormals.h"
#include "vtkStructuredPointsReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkFloatArray.h"
#include "vtkShortArray.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkPlane.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkProperty.h"

#include "vtkALBAContourVolumeMapper.h"
#include "vtkALBAVolumeSlicer.h"

#include "vtkALBAContourVolumeMapperTest.h"

static bool ExtractModel   = true;
static bool CleanModel     = false;

void vtkALBAContourVolumeMapperTest::BeforeTest()
{
  m_RGData = NULL;
  m_ImageData = NULL;

  CreateDataRG();
  CreateDataID();
}

void vtkALBAContourVolumeMapperTest::AfterTest()
{
  m_RGData->Delete() ;
  m_ImageData->Delete() ;

  m_RGData = NULL ;
  m_ImageData = NULL ;

}


void vtkALBAContourVolumeMapperTest::TestFixture()
{

}

//------------------------------------------------------------------------------
// Test the mapper with rect. grid data
void vtkALBAContourVolumeMapperTest::TestRectilinearGrid() 
//------------------------------------------------------------------------------
{
  char filename[]   = "cubePolyFromRG";
  //------------------ create objects
  vtkALBAContourVolumeMapper *mapper = vtkALBAContourVolumeMapper::New();
  vtkALBASmartPointer<vtkVolume> volume;
  volume->SetMapper(mapper);

  // create windows
  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddVolume(volume);
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  // prepare for rendering
  iren->SetStillUpdateRate(0.05f);
  iren->SetDesiredUpdateRate(15.f);
  vtkInteractorStyleSwitch *style = vtkInteractorStyleSwitch::New();
  iren->SetInteractorStyle(style);
  style->SetCurrentStyleToTrackballActor();
  style->Delete();

  renderer->SetBackground(0.05f, 0.05f, 0.05f);
  renWin->SetSize(1024, 768);

  int dims[3];
  double b[2];

  //------------------ load (create) data
  m_RGData->GetDimensions(dims);
  mapper->SetInput(m_RGData);
  m_RGData->GetScalarRange(b);
  mapper->SetMaxScalar(b[1]);
  CPPUNIT_ASSERT(m_RGData->GetNumberOfPoints()==(dims[0] * dims[1] * dims[2]));


  vtkOutlineCornerFilter *filter = vtkOutlineCornerFilter::New();
  filter->SetInput(mapper->GetInput());
  vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
  omapper->SetInput(filter->GetOutput());
  vtkActor *oactor = vtkActor::New();
  oactor->SetMapper(omapper);
  oactor->VisibilityOn();
  oactor->PickableOff();
  renderer->AddActor(oactor);
  filter->Delete();
  omapper->Delete();
  oactor->SetUserMatrix(volume->GetMatrix());
  oactor->Delete();

  vtkALBAVolumeSlicer *slicer = NULL, *pslicer = NULL;

  mapper->SetEnableContourAnalysis(CleanModel);

  renderer->GetActiveCamera()->SetViewAngle(vtkMath::RadiansToDegrees() * 2.f * atan(renWin->GetSize()[1] * 0.27 / (2.f * 700.f)));
  renderer->GetActiveCamera()->SetEyeAngle(2.5);
  renderer->ResetCamera();
  vtkCamera *camera = renderer->GetActiveCamera();
  renderer->ResetCamera(mapper->GetBounds());
  camera->Azimuth(45);


  // switch off the auto lod so that we can control it manually
  mapper->AutoLODCreateOff() ;

  // This loop tests that the output has the correct no. of points
  for (int lod = 0 ;  lod < vtkALBAContourVolumeMapperNamespace::NumberOfLods ;  lod++){
    // get the step lodxy for this lod
    int lodxy, lodz ;
    mapper->CalculateLodIncrements(lod, &lodxy, &lodz) ;

    // predict how many points in polydata output
    int nx = (dims[0]+lodxy-1)/lodxy ;
    int ny = (dims[1]+lodxy-1)/lodxy ;
    int npts = nx*ny ;

    // test each z plane in the volume
    for (double value = 0; value < dims[2]; value+=1.0  ) {
      mapper->SetContourValue(value);
      vtkPolyData *polydata = mapper->GetOutput(lod);

      int nactual = polydata->GetNumberOfPoints() ;
      CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==npts);
      // renWin->Render();

      polydata->Delete() ;
    }
  }

  // switch auto lod back on
  mapper->AutoLODCreateOn() ;


  if (ExtractModel && filename) {
    vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
    vtkPolyData *polydata = mapper->GetOutput(0);
    writer->SetInput(polydata);
    char newfilename[512];
    sprintf(newfilename, "%s.polydata", filename);
    writer->SetFileName(newfilename);
    writer->SetFileTypeToASCII();
    writer->Write();

    writer->Delete();
    polydata->Delete();
  }

  //------------------ delete objects
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  mapper->Delete();

  // this seems to enable leaks printing!
}


//------------------------------------------------------------------------------
// Test the mapper with image data
void vtkALBAContourVolumeMapperTest::TestImageData() 
//------------------------------------------------------------------------------
{
  char filename[]   = "cubePolyFromID";
  //------------------ create objects
  vtkALBAContourVolumeMapper *mapper = vtkALBAContourVolumeMapper::New();
  vtkALBASmartPointer<vtkVolume> volume;
  volume->SetMapper(mapper);

  // create windows
  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddVolume(volume);
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  // prepare for rendering
  iren->SetStillUpdateRate(0.05f);
  iren->SetDesiredUpdateRate(15.f);
  vtkInteractorStyleSwitch *style = vtkInteractorStyleSwitch::New();
  iren->SetInteractorStyle(style);
  style->SetCurrentStyleToTrackballActor();
  style->Delete();

  renderer->SetBackground(0.05f, 0.05f, 0.05f);
  renWin->SetSize(1024, 768);

  int dims[3];
  double b[2];

  //------------------ load (create) data
  m_ImageData->GetDimensions(dims);
  mapper->SetInput(m_ImageData);
  m_ImageData->GetScalarRange(b);
  mapper->SetMaxScalar(b[1]);
  CPPUNIT_ASSERT(m_ImageData->GetNumberOfPoints()==(dims[0] * dims[1] * dims[2]));

  vtkOutlineCornerFilter *filter = vtkOutlineCornerFilter::New();
  filter->SetInput(mapper->GetInput());
  vtkPolyDataMapper *omapper = vtkPolyDataMapper::New();
  omapper->SetInput(filter->GetOutput());
  vtkActor *oactor = vtkActor::New();
  oactor->SetMapper(omapper);
  oactor->VisibilityOn();
  oactor->PickableOff();
  renderer->AddActor(oactor);
  filter->Delete();
  omapper->Delete();
  oactor->SetUserMatrix(volume->GetMatrix());
  oactor->Delete();

  vtkALBAVolumeSlicer *slicer = NULL, *pslicer = NULL;

  mapper->SetEnableContourAnalysis(CleanModel);

  renderer->GetActiveCamera()->SetViewAngle(vtkMath::RadiansToDegrees() * 2.f * atan(renWin->GetSize()[1] * 0.27 / (2.f * 700.f)));
  renderer->GetActiveCamera()->SetEyeAngle(2.5);
  renderer->ResetCamera();
  vtkCamera *camera = renderer->GetActiveCamera();
  renderer->ResetCamera(mapper->GetBounds());
  camera->Azimuth(45);


  // switch off the auto lod so that we can control it manually
  mapper->AutoLODCreateOff() ;

  // This loop tests that the output has the correct no. of points
  for (int lod = 0 ;  lod < vtkALBAContourVolumeMapperNamespace::NumberOfLods ;  lod++){
    // get the step lodxy for this lod
    int lodxy, lodz ;
    mapper->CalculateLodIncrements(lod, &lodxy, &lodz) ;

    // predict how many points in polydata output
    int nx = (dims[0]+lodxy-1)/lodxy ;
    int ny = (dims[1]+lodxy-1)/lodxy ;
    int npts = nx*ny ;

    // test each z plane in the volume
    for (double value = 0; value < dims[2]; value+=1.0  ) {
      mapper->SetContourValue(value);
      vtkPolyData *polydata = mapper->GetOutput(lod);
      CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==npts);
      // renWin->Render();

      polydata->Delete() ;   
    }
  }

  // switch auto lod back on
  mapper->AutoLODCreateOn() ;

  if (ExtractModel && filename) {
    vtkPolyDataWriter *writer = vtkPolyDataWriter::New();
    vtkPolyData *polydata = mapper->GetOutput(0);
    writer->SetInput(polydata);
    char newfilename[512];
    sprintf(newfilename, "%s.polydata", filename);
    writer->SetFileName(newfilename);
    writer->SetFileTypeToASCII();
    writer->Write();

    writer->Delete();
    polydata->Delete();
  }

  //------------------ delete objects
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  mapper->Delete();

  // this seems to enable leaks printing!
}



//------------------------------------------------------------------------------
// Create the Rectlinear Grid volume
void vtkALBAContourVolumeMapperTest::CreateDataRG()
//------------------------------------------------------------------------------
{
  int dims[3] = {9, 9, 9} ;

  // do nothing if data already created
  if (m_RGData != NULL)
    return ;

  // create data set
  m_RGData = vtkRectilinearGrid::New();
  m_RGData->Initialize() ;
  m_RGData->SetDimensions(dims) ;

  // create data array of desired type
  vtkShortArray *DA = vtkShortArray::New() ;

  // create arrays for coords of points
  int x, y, z ;
  vtkFloatArray *xCoords = vtkFloatArray::New() ;
  vtkFloatArray *yCoords = vtkFloatArray::New() ;
  vtkFloatArray *zCoords = vtkFloatArray::New() ;
  for (x = 0 ;  x < dims[0] ;  x++)
    xCoords->InsertNextValue((float)x) ;
  for (y = 0 ;  y < dims[0] ;  y++)
    yCoords->InsertNextValue((float)y) ;
  for (z = 0 ;  z < dims[0] ;  z++)
    zCoords->InsertNextValue((float)z) ;

  // write data into array
  DA->SetNumberOfComponents(1) ;
  float r ;
  float c = 255.0 / (float)(dims[2]-1) ;
  for (z = 0 ;  z < dims[2] ;  z++){
    r = c * z ;
    for (y = 0 ;  y < dims[1] ;  y++){
      for (x = 0 ;  x < dims[0] ;  x++){
        DA->InsertNextTuple1(r) ;
      }
    }
  }

  // assign scalar array and coords to rect grid
  m_RGData->GetPointData()->SetScalars(DA) ;
  m_RGData->SetXCoordinates(xCoords) ;
  m_RGData->SetYCoordinates(yCoords) ;
  m_RGData->SetZCoordinates(zCoords) ;

  // delete local arrays
  DA->Delete() ;
  xCoords->Delete() ;
  yCoords->Delete() ;
  zCoords->Delete() ;
}


//-----------------------------------------------------------
void vtkALBAContourVolumeMapperTest::CreateDataID()
//-----------------------------------------------------------
{
  int dims[3] = {64, 64, 64} ;
  double spacing[3] = {1.0, 1.0, 1.0} ;

  // do nothing if data already created
  if (m_ImageData != NULL)
    return ;

  // create data set
  m_ImageData = vtkImageData::New();
  m_ImageData->Initialize() ;
  m_ImageData->SetDimensions(dims) ;
  m_ImageData->SetSpacing(spacing);

  // create data array of desired type
  vtkShortArray *DA = vtkShortArray::New() ;

  // write data into array
  DA->SetNumberOfComponents(1) ;
  float r ;
  float c = 255.0 / (float)(dims[2]-1) ;
  for (int z = 0 ;  z < dims[2] ;  z++){
    r = c*z ;
    for (int y = 0 ;  y < dims[1] ;  y++){
      for (int x = 0 ;  x < dims[0] ;  x++){
        DA->InsertNextTuple1(r) ;
      }
    }
  }

  // assign scalar array and coords to image
  m_ImageData->GetPointData()->SetScalars(DA) ;

  // delete local arrays
  DA->Delete() ;
}
