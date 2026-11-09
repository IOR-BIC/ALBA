/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPickHelperTest
 Authors: Gianluigi Crimi

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
#include "albaPickHelperTest.h"
#include "albaPickHelper.h"
#include "albaVMESurface.h"
#include "albaVMEOutput.h"
#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
vtkActor *albaPickHelperTest::NewSphereActor()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(10);
  sphere->SetThetaResolution(16);
  sphere->SetPhiResolution(16);

  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(sphere->GetOutputPort());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  return actor;
}

//----------------------------------------------------------------------------
void albaPickHelperTest::TestPickCellOnVME()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  albaVMESurface *vme;
  albaNEW(vme);
  vtkActor *actor = NewSphereActor();
  vtkALBASmartPointer<vtkALBAAssembly> assembly;
  assembly->AddPart(actor);
  assembly->SetVme(vme);
  m_Renderer->AddActor(assembly);
  m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  int *size = m_RenderWindow->GetSize();
  double pos[3];
  albaVME *pickedVme = NULL;
  vtkIdType cellId = -1;
  CPPUNIT_ASSERT(albaPickHelper::PickCell(m_RenderWindow, size[0] / 2, size[1] / 2, pos, &pickedVme, &cellId));
  CPPUNIT_ASSERT(pickedVme == vme);
  CPPUNIT_ASSERT(cellId >= 0);
  // the picked position lies on the sphere surface
  CPPUNIT_ASSERT(fabs(vtkMath::Norm(pos) - 10.0) < 0.5);

  m_Renderer->RemoveAllViewProps();
  actor->Delete();
  albaDEL(vme);
}

//----------------------------------------------------------------------------
void albaPickHelperTest::TestPickCellMiss()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  vtkActor *actor = NewSphereActor();
  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  // a corner of the window shows only the background
  double pos[3] = {1, 1, 1};
  albaVME *pickedVme = (albaVME *)this;
  vtkIdType cellId = 0;
  CPPUNIT_ASSERT(!albaPickHelper::PickCell(m_RenderWindow, 2, 2, pos, &pickedVme, &cellId));
  CPPUNIT_ASSERT(pos[0] == 0 && pos[1] == 0 && pos[2] == 0);
  CPPUNIT_ASSERT(pickedVme == NULL);
  CPPUNIT_ASSERT(cellId == -1);

  // no render window or no view: nothing is picked
  CPPUNIT_ASSERT(!albaPickHelper::PickCell((vtkRenderWindow *)NULL, 2, 2, pos));
  CPPUNIT_ASSERT(!albaPickHelper::PickCell((albaView *)NULL, 2, 2, pos));

  m_Renderer->RemoveAllViewProps();
  actor->Delete();
}

//----------------------------------------------------------------------------
void albaPickHelperTest::TestPickCellPropWithoutVME()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  // a prop not inside a vtkALBAAssembly is picked, but has no VME
  vtkActor *actor = NewSphereActor();
  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  int *size = m_RenderWindow->GetSize();
  double pos[3];
  albaVME *pickedVme = (albaVME *)this;
  CPPUNIT_ASSERT(albaPickHelper::PickCell(m_RenderWindow, size[0] / 2, size[1] / 2, pos, &pickedVme));
  CPPUNIT_ASSERT(pickedVme == NULL);
  CPPUNIT_ASSERT(fabs(vtkMath::Norm(pos) - 10.0) < 0.5);

  m_Renderer->RemoveAllViewProps();
  actor->Delete();
}

//----------------------------------------------------------------------------
void albaPickHelperTest::TestPickCellStopsAtFirstRenderer()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  vtkActor *actor = NewSphereActor();
  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera();

  // an empty renderer after the one showing the sphere: a picking continuing on it
  // would reset the picked position
  vtkALBASmartPointer<vtkRenderer> frontRenderer;
  m_RenderWindow->SetNumberOfLayers(2);
  frontRenderer->SetLayer(1);
  m_RenderWindow->AddRenderer(frontRenderer);
  m_RenderWindow->Render();

  int *size = m_RenderWindow->GetSize();
  double pos[3];
  CPPUNIT_ASSERT(albaPickHelper::PickCell(m_RenderWindow, size[0] / 2, size[1] / 2, pos));
  CPPUNIT_ASSERT(fabs(vtkMath::Norm(pos) - 10.0) < 0.5);

  m_RenderWindow->RemoveRenderer(frontRenderer);
  m_Renderer->RemoveAllViewProps();
  actor->Delete();
}

//----------------------------------------------------------------------------
void albaPickHelperTest::TestGetPointScalar()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(10);
  sphere->Update();

  // the scalar of each point is its id
  vtkALBASmartPointer<vtkPolyData> data;
  data->DeepCopy(sphere->GetOutput());
  vtkALBASmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfTuples(data->GetNumberOfPoints());
  for (vtkIdType i = 0; i < data->GetNumberOfPoints(); i++)
    scalars->SetValue(i, i);
  data->GetPointData()->SetScalars(scalars);

  albaVMESurface *vme;
  albaNEW(vme);
  vme->SetData(data, 0.0);
  vme->Update();

  double point[3], scalar;
  data->GetPoint(5, point);
  CPPUNIT_ASSERT(albaPickHelper::GetPointScalar(vme, point, scalar) == 5);
  CPPUNIT_ASSERT(scalar == 5.0);

  // a position outside the bounds is clamped into them
  double outside[3] = {0, 0, 1000};
  CPPUNIT_ASSERT(albaPickHelper::GetPointScalar(vme, outside, scalar, true) >= 0);

  // no VME: no point
  CPPUNIT_ASSERT(albaPickHelper::GetPointScalar(NULL, point, scalar) == -1);
  CPPUNIT_ASSERT(scalar == 0.0);

  albaDEL(vme);
}
