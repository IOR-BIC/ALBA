/*=========================================================================

 Program: MAF2
 Module: vtkMAFHistogramTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFHistogram.h"
#include "vtkMAFHistogramTest.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkDataSetReader.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkMAFHistogramTest::BeforeTest()
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
void vtkMAFHistogramTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFHistogramTest::RenderData(vtkActor2D *actor, char* testName)
//------------------------------------------------------------
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  m_Renderer->AddActor2D(actor);
  m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkMAFHistogramTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFHistogram *to = vtkMAFHistogram::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramPointRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::POINT_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("POINT REPRESENTATION");

  RenderData(actor, "TestHistogramPointRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramLineRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::LINE_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("LINE REPRESENTATION");

  RenderData(actor, "TestHistogramLineRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramBarRepresentation()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("BAR REPRESENTATION");

  RenderData(actor, "TestHistogramBarRepresentation");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramLogaritmicProperties()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST LOGARITMIC PROPERTIES");

  actor->SetLogHistogram(TRUE);
  actor->SetLogScaleConstant(TRUE);

  RenderData(actor, "TestHistogramLogaritmicProperties");
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramScaleFactorColorProperties()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

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
void vtkMAFHistogramTest::TestHistogramGetAttributesAndUpdateLines()
//--------------------------------------------
{
  // set filename
  std::ostringstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str().c_str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST ATTRIBUTES AND UPDATE LINES");

  actor->UpdateLines(50,80);
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
void vtkMAFHistogramTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}