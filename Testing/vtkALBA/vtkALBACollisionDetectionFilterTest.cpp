/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACollisionDetectionFilterTest
 Authors: Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBACollisionDetectionFilter.h"
#include "vtkALBACollisionDetectionFilterTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkMatrix4x4.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkPolyDataWriter.h"
#include "vtkProperty.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include "vtkPolyDataWriter.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkIntArray.h"

#include <ctime>
#include <time.h>
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

static int index = 0;

//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::BeforeTest()
//-------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::AfterTest()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkALBACollisionDetectionFilter> filter1;

  vtkALBACollisionDetectionFilter *filter2 = vtkALBACollisionDetectionFilter::New();
  
  filter2->Delete();
}
//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::Test()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> s1;
  s1->SetRadius(10.0);
  s1->SetCenter(0.0,0.0,0.0);
  s1->SetPhiResolution(100);
  s1->SetThetaResolution(100);
  s1->Update();

  vtkALBASmartPointer<vtkSphereSource> s2;
  s2->SetRadius(10.0);
  s2->SetCenter(10.0,10.0,10.0);
  s2->SetPhiResolution(100);
  s2->SetThetaResolution(100);
  s2->Update();

  vtkALBASmartPointer<vtkMatrix4x4> m1;
  vtkALBASmartPointer<vtkMatrix4x4> m2;

  long timeStart = time(NULL);
  vtkALBASmartPointer<vtkALBACollisionDetectionFilter> filter;
  filter->SetInput(0,s1->GetOutput());
  filter->SetInput(1,s2->GetOutput());
  filter->SetMatrix(0,m1);
  filter->SetMatrix(1,m2);
  filter->GenerateScalarsOn();
  // filter->SetCollisionModeToFirstContact();
  filter->Update();
  long timeEnd = time(NULL);

  long timeToCompute = (timeEnd - timeStart);

  printf("\n\t%d sec. to compute collision detection\n",timeToCompute);

  vtkALBASmartPointer<vtkPolyData> output0;
  output0->DeepCopy(filter->GetOutput(0));
 
  vtkALBASmartPointer<vtkDoubleArray> contactScalars;
  contactScalars->SetName("CONTACT");
  contactScalars->SetNumberOfTuples(filter->GetOutput(0)->GetNumberOfCells());
  for (int i=0;i<filter->GetOutput(0)->GetNumberOfCells();i++)
  {
    contactScalars->SetTuple1(i,0.0);
  }
  vtkDataArray *array = output0->GetFieldData()->GetArray("ContactCells");
  for (int i=0;i<array->GetNumberOfTuples();i++)
  {
    int value = (int)array->GetTuple1(i);
    contactScalars->SetTuple1(value,1.0);
  }

  output0->GetCellData()->AddArray(contactScalars);

  vtkALBASmartPointer<vtkPolyDataWriter> w;
  w->SetInputConnection(filter->GetOutputPort(0));
  w->SetFileName("test0.vtk");
  w->Write();

  return;

  w->SetInputConnection(filter->GetOutputPort(1));
  w->SetFileName("test1.vtk");
  w->Write();

  vtkALBASmartPointer<vtkProperty> p;
  p->SetOpacity(0.5);

  AddPolydataToVisualize(s1->GetOutput(),p);
  AddPolydataToVisualize(s2->GetOutput(),p);
  AddPolydataToVisualize(filter->GetOutput(2));

  int steps = 50;
  for (int t = 0; t < steps; t++) 
  {
		m_Renderer->ResetCamera();
    m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
    m_RenderWindow->Render();
		COMPARE_IMAGES("Test", index);
    index++;
  }
}

//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::Visualize( vtkActor *actor )
//-------------------------------------------------------------------------
{
  m_Renderer->AddActor(actor);
}
//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::AddPolydataToVisualize(vtkPolyData *data, vtkProperty *property /* = NULL */)
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputData(data);
  mapper->Update();

  vtkALBASmartPointer<vtkActor> actor;
  if (property != NULL)
  {
    actor->SetProperty(property);
  }
  actor->SetMapper(mapper);

  Visualize(actor);
}
//-------------------------------------------------------------------------
void vtkALBACollisionDetectionFilterTest::TestChangingMatrix()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> s1;
  s1->SetRadius(10.0);
  s1->SetCenter(0.0,0.0,0.0);
  s1->SetPhiResolution(50);
  s1->SetThetaResolution(50);
  s1->Update();

  vtkALBASmartPointer<vtkSphereSource> s2;
  s2->SetRadius(10.0);
  s2->SetCenter(10.0,10.0,10.0);
  s2->SetPhiResolution(50);
  s2->SetThetaResolution(50);
  s2->Update();

  vtkALBASmartPointer<vtkMatrix4x4> m1;
  vtkALBASmartPointer<vtkMatrix4x4> m2;

  clock_t timeStart = clock();
  vtkALBASmartPointer<vtkALBACollisionDetectionFilter> filter;
  filter->SetInput(0,s1->GetOutput());
  filter->SetInput(1,s2->GetOutput());
  filter->SetMatrix(0,m1);
  filter->SetMatrix(1,m2);
  filter->Update();
  clock_t timeEnd = clock();

  double timeToCompute = (double)(timeEnd - timeStart)/CLOCKS_PER_SEC;

  printf("\n\t%.3f sec. to compute collision detection\n",timeToCompute);

  vtkALBASmartPointer<vtkProperty> p;
  p->SetOpacity(0.5);

  AddPolydataToVisualize(s1->GetOutput(),p);
  AddPolydataToVisualize(s2->GetOutput(),p);
  AddPolydataToVisualize(filter->GetOutput(2));

  int steps = 50;
  for (int t = 0; t < steps; t++) 
  {
		m_Renderer->ResetCamera();
    m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
    m_RenderWindow->Render();
		COMPARE_IMAGES("TestChangingMatrix", index);
    index++;
  }

  for (int i=0;i<10;i++)
  {
    vtkMatrix4x4 *m3 = vtkMatrix4x4::New();
    vtkALBASmartPointer<vtkTransform> transform;
    transform->SetMatrix(m3);
    transform->Translate(i/10.0,i/10.0,i/10.0);

    timeStart = timeStart = clock();
    filter->SetMatrix(0,transform->GetMatrix());
    filter->Modified();
    filter->Update();
    filter->Update();
    timeEnd = timeEnd = clock();

    timeToCompute = (double)(timeEnd - timeStart)/CLOCKS_PER_SEC;

    printf("\n\t%.3f sec. to compute collision detection\n",timeToCompute);

    for (int t = 0; t < steps; t++) 
    {
			m_Renderer->ResetCamera();
      m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestChangingMatrix", index);
      index++;
    }

    m3->Delete();
  }
}