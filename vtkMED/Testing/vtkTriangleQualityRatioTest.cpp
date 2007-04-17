/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkTriangleQualityRatioTest.cpp,v $
Language:  C++
Date:      $Date: 2007-04-17 11:04:58 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkTriangleQualityRatio.h"
#include "vtkTriangleQualityRatioTest.h"

#include "vtkMAFSmartPointer.h"
#include "mafString.h"
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
	int pointId[3];
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

	dataset->Update();
	

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
  vtkMAFSmartPointer<vtkLookupTable> defaultLut;
  vtkMAFSmartPointer<vtkLookupTable> reverseLut;

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

  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
  mapper->ScalarVisibilityOn();

  mapper->SetScalarModeToUseCellData();
  
  
  mapper->UseLookupTableScalarRangeOff();
  mapper->SetScalarRange(reverseLut->GetTableRange());
  mapper->SetLookupTable(reverseLut);
  mapper->SetInput(data);

  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();

  mafSleep(500);
}