/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkTriangleQualityRatioTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkTriangleQualityRatio.h"
#include "vtkTriangleQualityRatioTest.h"

#include "vtkALBASmartPointer.h"
#include "albaString.h"
#include "vtkSphereSource.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"

#define EPSILON 0.01

//-------------------------------------------------------------------------
void vtkTriangleQualityRatioTest::QualityTestValues()
//-------------------------------------------------------------------------
{
	vtkPolyData *dataset = vtkPolyData::New();

	vtkPoints   *points = vtkPoints::New();
	vtkCellArray   *cellArray = vtkCellArray::New();
	dataset->SetPoints(points);
	dataset->SetLines(cellArray);

	double zero = 0.0;
	vtkIdType pointId[3];
	points->InsertNextPoint(zero,zero,zero);
	points->InsertNextPoint(0.5,1.0,zero);
	points->InsertNextPoint(1.0,zero,zero);

	pointId[0] = 0;
	pointId[1] = 1;
	pointId[2] = 2;
	cellArray->InsertNextCell(3 , pointId);

	points->InsertNextPoint(2.0,zero,zero);
	points->InsertNextPoint(2.5,10.0,zero);
	points->InsertNextPoint(3.0,zero,zero);

	pointId[0] = 3;
	pointId[1] = 4;
	pointId[2] = 5;
	cellArray->InsertNextCell(3 , pointId);

	points->InsertNextPoint(4.0,zero,zero);
	points->InsertNextPoint(4.5,0.01,zero);
	points->InsertNextPoint(5.0,zero,zero);

	pointId[0] = 6;
	pointId[1] = 7;
	pointId[2] = 8;
	cellArray->InsertNextCell(3 , pointId);	

	vtkTriangleQualityRatio *tqr = vtkTriangleQualityRatio::New();
	tqr->SetInput(dataset);
	tqr->Update();

	double averageRatio = tqr->GetMeanRatio();
	double maxRatio = tqr->GetMaxRatio();
	double minRatio = tqr->GetMinRatio();

	CPPUNIT_ASSERT((maxRatio <= 0.95 + EPSILON) && (maxRatio >= 0.95 - EPSILON));
	CPPUNIT_ASSERT((minRatio <= 0.016 + EPSILON) && (minRatio >= 0.016 - EPSILON));
	CPPUNIT_ASSERT((averageRatio <= 0.37 + EPSILON) && (averageRatio >= 0.37 - EPSILON));


	vtkDEL(tqr);
	vtkDEL(points);
	vtkDEL(cellArray);
	vtkDEL(dataset);
	
}


//-------------------------------------------------------------------------
void vtkTriangleQualityRatioTest::QualityTestRender()
//-------------------------------------------------------------------------
{
	vtkSphereSource *ss = vtkSphereSource::New();
	ss->SetRadius(1);
	ss->Update();

	vtkTriangleQualityRatio *tqr = vtkTriangleQualityRatio::New();
	tqr->SetInput(ss->GetOutput());
	tqr->Update();


	RenderData(tqr->GetOutput());

	vtkDEL(tqr);
  vtkDEL(ss);
}
//------------------------------------------------------------------------
void vtkTriangleQualityRatioTest::RenderData(vtkPolyData *data)
//------------------------------------------------------------------------
{

  //----------------------------
  // lookup table stuff
  //----------------------------

  // i reverse the color table  to use blue for the lowest values and red for the highest
  vtkALBASmartPointer<vtkLookupTable> defaultLut;
  vtkALBASmartPointer<vtkLookupTable> reverseLut;

  int colNum = 256;

  defaultLut->SetNumberOfColors(colNum);
  defaultLut->SetHueRange(0.0 ,.66667);
  double range[2] = {0,0};

  data->GetCellData()->GetScalars()->GetRange(range);

  defaultLut->SetTableRange(range);
  defaultLut->Build();

  reverseLut->SetNumberOfColors(colNum);
  reverseLut->SetTableRange(defaultLut->GetTableRange());
  for(int i = 0;i < colNum; i++)
  {
    reverseLut->SetTableValue(i, defaultLut->GetTableValue(colNum-i));
  }

  //----------------------------

  vtkALBASmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkALBASmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->ScalarVisibilityOn();

  mapper->SetScalarModeToUseCellData();
  
  
  mapper->UseLookupTableScalarRangeOff();
  mapper->SetScalarRange(reverseLut->GetTableRange());
  mapper->SetLookupTable(reverseLut);
  mapper->SetInputData(data);

  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();
}