/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtendedGlyph3DTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAExtendedGlyph3DTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAExtendedGlyph3D.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkArrowSource.h"

#include <string>

enum ID_TEST_LIST
{
  ID_EXECUTION_TEST = 0,
};

static int TestNumber = 0;

//-----------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::BeforeTest()
//-----------------------------------------------------------
{
	// create windows
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);
	m_RenderWindow->AddRenderer(m_Renderer);

	// prepare for rendering
	m_Renderer->SetBackground(0.05f, 0.05f, 0.05f);
	m_RenderWindow->SetSize(1024, 768);

	TestNumber++;

  CreatePointsSet();
}
//-----------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::AfterTest()
//-----------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);

  m_Points->Delete();

}
//-----------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestFixture()
//-----------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter;

  //////////////////////////////////////////////////////////////////////////
  filter = vtkALBAExtendedGlyph3D::New();
  filter->Delete();
  //////////////////////////////////////////////////////////////////////////
  vtkALBASmartPointer<vtkALBAExtendedGlyph3D> filterSmart;
  //////////////////////////////////////////////////////////////////////////

}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetNumberOfSources()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  for(int i=1;i<10;i++)
  {
    filter->SetNumberOfSources(i);

    CPPUNIT_ASSERT( filter->GetNumberOfSources() == i );
  }

  filter->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::CreatePointsSet()
//----------------------------------------------------------------------------
{  
  vtkPoints *pts = vtkPoints::New();
  m_Points = vtkPolyData::New();

  pts->InsertNextPoint(0,0,0);
  pts->InsertNextPoint(0,10,0);
  pts->InsertNextPoint(10,0,0);
//   pts->InsertNextPoint(0,1,0);
//   pts->InsertNextPoint(0,0,1);

  m_Points->SetPoints(pts);
  

  pts->Delete();

}

//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetSource()
//----------------------------------------------------------------------------
{  
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();
  vtkConeSource *cone = vtkConeSource::New();
  cone->Update();
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->Update();

  filter->SetNumberOfSources(2);
  filter->SetSource(cone->GetOutput());
  filter->SetSource(1,cube->GetOutput());

  CPPUNIT_ASSERT( filter->GetSource() == cone->GetOutput() );
  CPPUNIT_ASSERT( filter->GetSource(1) == cube->GetOutput() );

  filter->Delete();
  cone->Delete();
  cube->Delete();

}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetScaling()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(1.0);
  scalars->InsertNextTuple1(2.0);
  scalars->InsertNextTuple1(3.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->SetScalars(scalars);
  

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->ScalingOff();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaling() == false );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] == scalars->GetRange()[0] );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] == scalars->GetRange()[1] );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(filter->GetOutputPort());
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenderWindow->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetScaling", m_TestNumber);

  filter->ScalingOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaling() == true );

	m_RenderWindow->Render();

  TestNumber++;
  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetScaling", m_TestNumber);

	actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetScaleFactor()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(1.0);
  scalars->InsertNextTuple1(2.0);
  scalars->InsertNextTuple1(3.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->SetScalars(scalars);
  

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->SetScaleFactor(2.0);
  filter->ScalingOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaleFactor() == 2.0 );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(filter->GetOutputPort());
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
	m_RenderWindow->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetScaleFactor", m_TestNumber);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetRange()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");
  

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->SetRange(15.0,20.0);
  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 15.0 && filter->GetRange()[1] == 20.0 );

  filter->Delete();
  sphere->Delete();
  scalars->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetGeneratePointIds()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->GeneratePointIdsOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetPointData()->GetArray("InputPointIds") != NULL );

  filter->Delete();
  sphere->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetPointIdsName()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->GeneratePointIdsOn();
  filter->SetPointIdsName("TEST");
  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetPointData()->GetArray("TEST") != NULL );

  filter->Delete();
  sphere->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetScalarVisibility()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(0.0);//0 value make the point not visible
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->ScalarVisibilityOn();
  filter->Update();  

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(filter->GetOutputPort());
  mapper->SetScalarVisibility(true);
  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(filter->GetOutput()->GetScalarRange()[0],filter->GetOutput()->GetScalarRange()[1]);
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
	m_RenderWindow->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
 	COMPARE_IMAGES("TestSetScalarVisibility", m_TestNumber);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetOrient()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkArrowSource *arrow = vtkArrowSource::New();
  arrow->Update();

  vtkDoubleArray *normals = vtkDoubleArray::New();
  normals->SetNumberOfComponents(3);
  normals->InsertNextTuple3(1,0,0);
  normals->InsertNextTuple3(0,1,0);
  normals->InsertNextTuple3(0,0,1);
  normals->SetName("NORMALS");

  m_Points->GetPointData()->AddArray(normals);
  m_Points->GetPointData()->SetActiveNormals("NORMALS");

  vtkDataArray *n = m_Points->GetPointData()->GetNormals();

  filter->SetSource(arrow->GetOutput());
  filter->SetInputData(m_Points);
  filter->SetVectorModeToUseNormal();
  filter->OrientOn();
  filter->Update();  

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(filter->GetOutputPort());
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenderWindow->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetOrient", m_TestNumber);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  arrow->Delete();
  normals->Delete();
}
//----------------------------------------------------------------------------
void vtkALBAExtendedGlyph3DTest::TestSetClamping()
//----------------------------------------------------------------------------
{
  vtkALBAExtendedGlyph3D *filter = vtkALBAExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");

  filter->SetSource(sphere->GetOutput());
  filter->SetInputData(m_Points);
  filter->SetRange(15.0,20.0);
  filter->ClampingOn();//Clamping normalize to [0,1] the scale values
  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 15.0 && filter->GetRange()[1] == 20.0 );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] >= 0.0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] <= 1.0 );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(filter->GetOutputPort());
  mapper->SetScalarVisibility(true);
  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(filter->GetOutput()->GetScalarRange()[0],filter->GetOutput()->GetScalarRange()[1]);
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
	m_RenderWindow->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetClamping", m_TestNumber);

  filter->SetRange(12.0,20.0);

  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 12.0 && filter->GetRange()[1] == 20.0 );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] >= 0.0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] <= 1.0 );

  m_RenderWindow->Render();

  TestNumber++;
  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
	COMPARE_IMAGES("TestSetClamping", m_TestNumber);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();
}
