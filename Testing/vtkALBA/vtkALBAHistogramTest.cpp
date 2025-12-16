/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAHistogramTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAHistogram.h"
#include "vtkALBAHistogramTest.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkDataSetReader.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkALBAHistogramTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_Renderer->SetBackground(0.0, 0.0, 0.0);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);
}
//----------------------------------------------------------------------------
void vtkALBAHistogramTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkALBAHistogramTest::RenderData(vtkActor2D *actor, char* testName)
//------------------------------------------------------------
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  m_Renderer->AddActor2D(actor);
  m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkALBAHistogramTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBAHistogram *to = vtkALBAHistogram::New();
  to->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramPointRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data,0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::POINT_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("POINT REPRESENTATION");

  RenderData(actor, "TestHistogramPointRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramLineRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data,0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::LINE_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("LINE REPRESENTATION");

  RenderData(actor, "TestHistogramLineRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramBarRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data, 0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("BAR REPRESENTATION");

  RenderData(actor, "TestHistogramBarRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramLogaritmicProperties()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data,0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST LOGARITMIC PROPERTIES");

  actor->SetLogHistogram(true);
  actor->SetLogScaleConstant(true);

  RenderData(actor, "TestHistogramLogaritmicProperties");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramScaleFactorColorProperties()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data, 0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST SCALE PROPERTIES");

  actor->AutoscaleHistogramOff();
  actor->SetScaleFactor(0.1);
  actor->SetColor(1.0,0.0,0.0);
  actor->Modified();

  RenderData(actor, "TestHistogramScaleFactorColorProperties");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkALBAHistogramTest::TestHistogramGetAttributesAndUpdateLines()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << ALBA_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data, 0);
  actor->SetHisctogramRepresentation(vtkALBAHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST ATTRIBUTES AND UPDATE LINES");

	double sr[2] = { 50,80 };
  actor->UpdateLines(sr);
  actor->ShowLinesOn();

  RenderData(actor, "TestHistogramGetAttributesAndUpdateLines");

  int hv[2] = {10,20};
  int value = actor->GetHistogramValue(hv[0], hv[1]);
  CPPUNIT_ASSERT(value >= 0 && value <= data->GetSize()) ;
  value = actor->GetScalarValue(hv[0], hv[1]);
  double range[2];
  data->GetRange(range);
  CPPUNIT_ASSERT(value >= range[0] && value <= range[1]) ;

  actor->Delete();
  reader->Delete();
}

//----------------------------------------------------------------------------
void vtkALBAHistogramTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkALBAHistogram *actor;
  actor = vtkALBAHistogram::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}