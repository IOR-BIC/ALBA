/*=========================================================================

 Program: MAF2
 Module: vtkMAFCollisionDetectionFilterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFCollisionDetectionFilter.h"
#include "vtkMAFCollisionDetectionFilterTest.h"

#include "vtkMAFSmartPointer.h"
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

static int index = 0;

//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::BeforeTest()
//-------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);
	vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(600, 0);
	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::AfterTest()
//-------------------------------------------------------------------------
{
	m_Renderer->RemoveAllProps();
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
	vtkDEL(m_RenderWindowInteractor);
}

//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFCollisionDetectionFilter> filter1;

  vtkMAFCollisionDetectionFilter *filter2 = vtkMAFCollisionDetectionFilter::New();
  
  filter2->Delete();
}
//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::Test()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> s1;
  s1->SetRadius(10.0);
  s1->SetCenter(0.0,0.0,0.0);
  s1->SetPhiResolution(100);
  s1->SetThetaResolution(100);
  s1->Update();

  vtkMAFSmartPointer<vtkSphereSource> s2;
  s2->SetRadius(10.0);
  s2->SetCenter(10.0,10.0,10.0);
  s2->SetPhiResolution(100);
  s2->SetThetaResolution(100);
  s2->Update();

  vtkMAFSmartPointer<vtkMatrix4x4> m1;
  vtkMAFSmartPointer<vtkMatrix4x4> m2;

  long timeStart = time(NULL);
  vtkMAFSmartPointer<vtkMAFCollisionDetectionFilter> filter;
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

  vtkMAFSmartPointer<vtkPolyData> output0;
  output0->DeepCopy(filter->GetOutput(0));
  output0->Update();
  vtkMAFSmartPointer<vtkDoubleArray> contactScalars;
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

  vtkMAFSmartPointer<vtkPolyDataWriter> w;
  w->SetInput(output0);
  w->SetFileName("test0.vtk");
  w->Write();

  return;

  w->SetInput(filter->GetOutput(1));
  w->SetFileName("test1.vtk");
  w->Write();

  vtkMAFSmartPointer<vtkProperty> p;
  p->SetOpacity(0.5);

  AddPolydataToVisualize(s1->GetOutput(),p);
  AddPolydataToVisualize(s2->GetOutput(),p);
  AddPolydataToVisualize(filter->GetOutput(2));

  int steps = 50;
  for (int t = 0; t < steps; t++) 
  {
    m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
    m_RenderWindow->Render();
		COMPARE_IMAGES("Test", index);
    index++;
  }
}

//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::Visualize( vtkActor *actor )
//-------------------------------------------------------------------------
{
  m_Renderer->AddActor(actor);
  m_RenderWindow->Render();
}
//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::AddPolydataToVisualize(vtkPolyData *data, vtkProperty *property /* = NULL */)
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInput(data);
  mapper->Update();

  vtkMAFSmartPointer<vtkActor> actor;
  if (property != NULL)
  {
    actor->SetProperty(property);
  }
  actor->SetMapper(mapper);

  Visualize(actor);
}
//-------------------------------------------------------------------------
void vtkMAFCollisionDetectionFilterTest::TestChangingMatrix()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> s1;
  s1->SetRadius(10.0);
  s1->SetCenter(0.0,0.0,0.0);
  s1->SetPhiResolution(50);
  s1->SetThetaResolution(50);
  s1->Update();

  vtkMAFSmartPointer<vtkSphereSource> s2;
  s2->SetRadius(10.0);
  s2->SetCenter(10.0,10.0,10.0);
  s2->SetPhiResolution(50);
  s2->SetThetaResolution(50);
  s2->Update();

  vtkMAFSmartPointer<vtkMatrix4x4> m1;
  vtkMAFSmartPointer<vtkMatrix4x4> m2;

  clock_t timeStart = clock();
  vtkMAFSmartPointer<vtkMAFCollisionDetectionFilter> filter;
  filter->SetInput(0,s1->GetOutput());
  filter->SetInput(1,s2->GetOutput());
  filter->SetMatrix(0,m1);
  filter->SetMatrix(1,m2);
  filter->Update();
  clock_t timeEnd = clock();

  double timeToCompute = (double)(timeEnd - timeStart)/CLOCKS_PER_SEC;

  printf("\n\t%.3f sec. to compute collision detection\n",timeToCompute);

  vtkMAFSmartPointer<vtkProperty> p;
  p->SetOpacity(0.5);

  AddPolydataToVisualize(s1->GetOutput(),p);
  AddPolydataToVisualize(s2->GetOutput(),p);
  AddPolydataToVisualize(filter->GetOutput(2));

  int steps = 50;
  for (int t = 0; t < steps; t++) 
  {
    m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
    m_RenderWindow->Render();
		COMPARE_IMAGES("TestChangingMatrix", index);
    index++;
  }

  for (int i=0;i<10;i++)
  {
    vtkMatrix4x4 *m3 = vtkMatrix4x4::New();
    vtkMAFSmartPointer<vtkTransform> transform;
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
      m_Renderer->GetActiveCamera()->Azimuth(-(180.0/steps));
      m_RenderWindow->Render();
			COMPARE_IMAGES("TestChangingMatrix", index);
      index++;
    }

    m3->Delete();
  }
}