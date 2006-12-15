/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkContourVolumeMapperTest.cpp,v $
Language:  C++
Date:      $Date: 2006-12-15 10:20:23 $
Version:   $Revision: 1.6 $
Authors:   Matteo Giacomoni, Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFSmartPointer.h"

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
#include "vtkStructuredPoints.h"
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

#include "vtkContourVolumeMapper.h"
#include "vtkVolumeSlicer.h"

#include "vtkContourVolumeMapperTest.h"

static bool ExtractModel   = true;
static bool CleanModel     = false;

void vtkContourVolumeMapperTest::setUp()
{
  m_RGData = NULL;
  m_ImageData = NULL;

  CreateDataRG();
  CreateDataID();
}

void vtkContourVolumeMapperTest::tearDown()
{
  vtkDEL(m_RGData);
  vtkDEL(m_ImageData);

}


void vtkContourVolumeMapperTest::TestFixture()
{

}

//-----------------------------------------------------------
void vtkContourVolumeMapperTest::TestRectilinearGrid() 
//-----------------------------------------------------------
{
	char filename[]   = "cubePolyFromRG";
	//------------------ create objects
  vtkContourVolumeMapper *mapper = vtkContourVolumeMapper::New();
  vtkMAFSmartPointer<vtkVolume> volume;
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
  
	vtkRectilinearGrid *data = vtkRectilinearGrid::New();
	
  data->DeepCopy(m_RGData);

  data->GetDimensions(dims);
	mapper->SetInput(data);
	data->GetScalarRange(b);
	mapper->SetMaxScalar(b[1]);
	CPPUNIT_ASSERT(data->GetNumberOfPoints()==(dims[0] * dims[1] * dims[2]));
	data->Delete();


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

  vtkVolumeSlicer *slicer = NULL, *pslicer = NULL;

  mapper->SetEnableContourAnalysis(CleanModel);
    
  renderer->GetActiveCamera()->SetViewAngle(vtkMath::RadiansToDegrees() * 2.f * atan(renWin->GetSize()[1] * 0.27 / (2.f * 700.f)));
  renderer->GetActiveCamera()->SetEyeAngle(2.5);
  renderer->ResetCamera();
  vtkCamera *camera = renderer->GetActiveCamera();
  renderer->ResetCamera(mapper->GetBounds());
  camera->Azimuth(45);
  for (double value = 0; value < dims[2]; value+=1.0  ) {
			mapper->SetContourValue(value);
			vtkPolyData *polydata = mapper->GetOutput(0);
			CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==((dims[0]*dims[1])));
       // renWin->Render();
	}

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
  mafSleep(1000);
}


//-----------------------------------------------------------
void vtkContourVolumeMapperTest::TestImageData() 
//-----------------------------------------------------------
{
  char filename[]   = "cubePolyFromID";
  //------------------ create objects
  vtkContourVolumeMapper *mapper = vtkContourVolumeMapper::New();
  vtkMAFSmartPointer<vtkVolume> volume;
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

  vtkImageData *data = vtkImageData::New();

  data->DeepCopy(m_ImageData);

  data->GetDimensions(dims);
  mapper->SetInput(data);
  data->GetScalarRange(b);
  mapper->SetMaxScalar(b[1]);
  CPPUNIT_ASSERT(data->GetNumberOfPoints()==(dims[0] * dims[1] * dims[2]));
  data->Delete();

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

  vtkVolumeSlicer *slicer = NULL, *pslicer = NULL;

  mapper->SetEnableContourAnalysis(CleanModel);

  renderer->GetActiveCamera()->SetViewAngle(vtkMath::RadiansToDegrees() * 2.f * atan(renWin->GetSize()[1] * 0.27 / (2.f * 700.f)));
  renderer->GetActiveCamera()->SetEyeAngle(2.5);
  renderer->ResetCamera();
  vtkCamera *camera = renderer->GetActiveCamera();
  renderer->ResetCamera(mapper->GetBounds());
  camera->Azimuth(45);
  for (double value = 0; value < dims[2]; value+=1.0  ) {
    mapper->SetContourValue(value);
    vtkPolyData *polydata = mapper->GetOutput(0);
    CPPUNIT_ASSERT(polydata->GetNumberOfPoints()==((dims[0]*dims[1])));
    // renWin->Render();
  }

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
  mafSleep(1000);
}

//-----------------------------------------------------------
void vtkContourVolumeMapperTest::CreateDataRG()
//-----------------------------------------------------------
{
    m_RGData = vtkRectilinearGrid::New();
		int dims[3];
		m_RGData->SetExtent(0, 8, 0, 8, 0, 8);
    m_RGData->GetDimensions(dims);
    vtkFloatArray *xyarray = vtkFloatArray::New();
    xyarray->Allocate(dims[0]);
    xyarray->SetNumberOfTuples(dims[0]);
    for (int x = 0; x < dims[0]; x++) {
      float fx = float(x) * 0.5f;
      xyarray->SetTuple(x, &fx);
    }
    m_RGData->SetXCoordinates(xyarray);
    m_RGData->SetYCoordinates(xyarray);
    m_RGData->SetZCoordinates(xyarray);
    xyarray->Delete();

    m_RGData->GetPointData()->SetScalars(vtkShortArray::New());
    m_RGData->GetPointData()->GetScalars()->Allocate(dims[0] * dims[1] * dims[2]);
    m_RGData->GetPointData()->GetScalars()->SetNumberOfComponents(1);
    m_RGData->GetPointData()->GetScalars()->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
    short* ptr = (short*)m_RGData->GetPointData()->GetScalars()->GetVoidPointer(0);
    for (int z = 0; z < dims[2]; z++)
      for (int y = 0; y < dims[1]; y++)
        for (int x = 0; x < dims[0]; x++)
          //*(ptr++) = abs(x - (dims[0] >> 1)) << 1;
					*(ptr++)=z;

}
//-----------------------------------------------------------
void vtkContourVolumeMapperTest::CreateDataID()
//-----------------------------------------------------------
{
		m_ImageData = vtkImageData::New();
		int dims[3], dataExtent[6];
		m_ImageData->SetExtent(0, 64, 0, 64, 0, 64);
    m_ImageData->GetDimensions(dims);
		m_ImageData->AllocateScalars();
    m_ImageData->SetSpacing(1, 1, 1);
    m_ImageData->GetExtent(dataExtent);
    m_ImageData->SetWholeExtent(dataExtent);

    m_ImageData->GetPointData()->SetScalars(vtkShortArray::New());
    m_ImageData->GetPointData()->GetScalars()->Allocate(dims[0] * dims[1] * dims[2]);
    m_ImageData->GetPointData()->GetScalars()->SetNumberOfComponents(1);
    m_ImageData->GetPointData()->GetScalars()->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
    short* ptr = (short*)m_ImageData->GetPointData()->GetScalars()->GetVoidPointer(0);
    for (int z = 0; z < dims[2]; z++)
      for (int y = 0; y < dims[1]; y++)
        for (int x = 0; x < dims[0]; x++)
					*(ptr++)=z;

}
