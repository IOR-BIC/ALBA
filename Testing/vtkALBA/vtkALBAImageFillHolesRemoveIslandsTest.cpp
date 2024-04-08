/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImageFillHolesRemoveIslandsTest
 Authors: Alberto Losi
 
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
#include "vtkALBAImageFillHolesRemoveIslands.h"
#include "vtkALBAImageFillHolesRemoveIslandsTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkBMPReader.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"

#include "albaString.h"

//------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::TestFixture()
//------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	m_Renderer->SetBackground(0.0, 0.0, 0.0);

	m_TestName = "Test";
}
//----------------------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::RenderData(vtkActor *actor)
//------------------------------------------------------------
{
  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(m_TestName);

  renderWindowInteractor->Delete();
}

//------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBAImageFillHolesRemoveIslands *to = vtkALBAImageFillHolesRemoveIslands::New();
  to->Delete();
}

//--------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::TestFillHoles()
//--------------------------------------------
{
  m_Algorithm = vtkALBAImageFillHolesRemoveIslands::FILL_HOLES;
	m_TestName = "TestFillHoles";
  TestAlgorithm();
}
//--------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::TestRemoveIslands()
//--------------------------------------------
{
  m_Algorithm = vtkALBAImageFillHolesRemoveIslands::REMOVE_ISLANDS;
	m_TestName = "TestRemoveIslands";
  TestAlgorithm();
}

//--------------------------------------------
void vtkALBAImageFillHolesRemoveIslandsTest::TestAlgorithm()
//--------------------------------------------
{
  albaString filename = ALBA_DATA_ROOT;
  filename<<"/Test_ImageFillHolesRemoveIslands/test.bmp";

  //load an image
  vtkBMPReader *r = vtkBMPReader::New();
  r->SetFileName(filename.GetCStr());
  r->Allow8BitBMPOn();
  r->SetDataScalarTypeToUnsignedChar();
  r->Update();

  //create vtkImageData from zero and set the correct parameters (spacing, dimension) ...
  vtkImageData *originalImage = vtkImageData::New();
  originalImage->SetSpacing(r->GetOutput()->GetSpacing());
  originalImage->SetDimensions(r->GetOutput()->GetDimensions());
  //originalImage->AllocateScalars();
  originalImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);

  // and scalar
  vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
  scalars->SetNumberOfComponents(1);
  scalars->SetName("SCALARS");
  for(int i=0; i<originalImage->GetNumberOfPoints();i++)
  {
    scalars->InsertNextTuple1(r->GetOutput()->GetPointData()->GetScalars()->GetTuple(i)[0]);
  }
  originalImage->GetPointData()->SetScalars(scalars);

  r->Delete();

  vtkALBAImageFillHolesRemoveIslands *filter = vtkALBAImageFillHolesRemoveIslands::New();
  filter->SetInputData(originalImage);
  filter->SetEdgeSize(1);
  filter->SetAlgorithm(m_Algorithm);
  filter->Update();

  vtkImageData *outputImage = vtkImageData::New();
  outputImage->DeepCopy(filter->GetOutput());

  vtkPlaneSource *imagePlane = vtkPlaneSource::New();
  imagePlane->SetOrigin(0.,0.,0.);
  imagePlane->SetPoint1(1.,0.,0.);
  imagePlane->SetPoint2(0.,1.,0.);

  double w,l, range[2];
  outputImage->GetScalarRange(range);
  w = range[1] - range[0];
  l = (range[1] + range[0]) * .5;

  vtkWindowLevelLookupTable *imageLUT = vtkWindowLevelLookupTable::New();
  imageLUT->SetWindow(w);
  imageLUT->SetLevel(l);
  //m_ImageLUT->InverseVideoOn();
  imageLUT->Build();

  vtkTexture *imageTexture = vtkTexture::New();
  imageTexture->RepeatOff();
  imageTexture->InterpolateOn();
  imageTexture->SetQualityTo32Bit();
  imageTexture->SetInputConnection(filter->GetOutputPort());


  imageTexture->SetLookupTable(imageLUT);
  imageTexture->MapColorScalarsThroughLookupTableOn();
  imageTexture->Modified();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(imagePlane->GetOutputPort());
  mapper->ImmediateModeRenderingOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->SetTexture(imageTexture);

  RenderData(actor);

  scalars->Delete();
  actor->Delete();
  mapper->Delete();
  imageTexture->Delete();
  imageLUT->Delete();
  imagePlane->Delete();
  outputImage->Delete();
  originalImage->Delete();
  filter->Delete();
}