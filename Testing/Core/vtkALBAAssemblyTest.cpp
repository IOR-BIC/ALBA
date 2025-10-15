/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAAssemblyTest
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

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAAssemblyTest.h"
#include "vtkALBAAssembly.h"
#include "albaVMEGeneric.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkProp3DCollection.h"
#include "vtkCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkTimerLog.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkCubeSource.h"

#include <vtkOpenGLHelper.h>

//force moudule init to avoid a crash on shallowcopy
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);

//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();
  vtkDEL(assembly);
  
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::PrintSelfTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();
  assembly->PrintSelf(std::cout, vtkIndent(3));
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::SetGetVmeTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();
  albaVMEGeneric *vme;

  albaNEW(vme);
  assembly->SetVme(vme);
  CPPUNIT_ASSERT(vme == assembly->GetVme());

  albaDEL(vme);

  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::AddRemovePartTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkActor *actor;
  vtkNEW(actor);

  assembly->AddPart(actor);
  vtkProp3DCollection * actors = assembly->GetParts();
  
  CPPUNIT_ASSERT(actor == actors->GetItemAsObject(0));

  assembly->RemovePart(actor);

  CPPUNIT_ASSERT(NULL == actors->GetItemAsObject(0));

  vtkDEL(actor);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetPartsTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkNEW(actor1);
  assembly->AddPart(actor1);

  vtkActor *actor2;
  vtkNEW(actor2);
  assembly->AddPart(actor2);


  vtkProp3DCollection * actors = assembly->GetParts();

  CPPUNIT_ASSERT(actor1 == actors->GetItemAsObject(0));
  CPPUNIT_ASSERT(actor2 == actors->GetItemAsObject(1));

  assembly->RemovePart(actor1);

  CPPUNIT_ASSERT(actor2 == actors->GetItemAsObject(0));

  assembly->RemovePart(actor2);

  CPPUNIT_ASSERT(NULL == actors->GetItemAsObject(0));

  vtkDEL(actor1);
  vtkDEL(actor2);

  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetActorsTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkVolume *volume;
  vtkActor *actor;

  vtkNEW(volume);
  vtkNEW(actor);

  assembly->AddPart(volume);
  assembly->AddPart(actor);

  vtkProp3DCollection * actors;
  vtkNEW(actors);
  assembly->GetActors(actors);
  CPPUNIT_ASSERT(actor == actors->GetItemAsObject(0));
  CPPUNIT_ASSERT(NULL == actors->GetItemAsObject(1));

  assembly->RemovePart(volume);
  assembly->RemovePart(actor);

  vtkDEL(volume);
  vtkDEL(actor);
  vtkDEL(actors);

  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetVolumesTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkVolume *volume;
  vtkActor *actor;

  vtkNEW(volume);
  vtkNEW(actor);

  assembly->AddPart(volume);
  assembly->AddPart(actor);

  vtkProp3DCollection * volumes;
  vtkNEW(volumes);
  assembly->GetVolumes(volumes);
  CPPUNIT_ASSERT(volume == volumes->GetItemAsObject(0));
  CPPUNIT_ASSERT(NULL == volumes->GetItemAsObject(1));

  assembly->RemovePart(volume);
  assembly->RemovePart(actor);

  vtkDEL(volume);
  vtkDEL(actor);
  vtkDEL(volumes);

  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::RenderOpaqueGeometryTest()
//----------------------------------------------------------------------------
{
  vtkTimerLog::SetLogging(0); // Must shutdown logging otherwise it will generate leaks

  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);


  PrepareToRender(m_Renderer,m_RenderWindow);

  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkSphereSource *sph_source1;
  vtkNEW(sph_source1);

  sph_source1->SetRadius(.5);
  sph_source1->SetCenter(-.25,0,0);
  sph_source1->Update();

  vtkPolyDataMapper *mapper1;
  vtkNEW(mapper1);

  mapper1->SetInputConnection(sph_source1->GetOutputPort());

  // translucent
  vtkActor *sphere1;
  vtkNEW(sphere1);

  sphere1->SetMapper(mapper1);
  sphere1->GetProperty()->SetAmbient(1.f);
  sphere1->GetProperty()->SetDiffuse(0.f);
  sphere1->GetProperty()->SetOpacity(.5);


  vtkSphereSource *sph_source2;
  vtkNEW(sph_source2);

  sph_source2->SetRadius(.5);
  sph_source2->SetCenter(.25,0,0);
  sph_source2->Update();

  vtkPolyDataMapper *mapper2;
  vtkNEW(mapper2);

  mapper2->SetInputConnection(sph_source2->GetOutputPort());

  // opaque
  vtkActor *sphere2;
  vtkNEW(sphere2);

  sphere2->SetMapper(mapper2);
  sphere2->GetProperty()->SetAmbient(1.f);
  sphere2->GetProperty()->SetDiffuse(0.f);
  sphere2->GetProperty()->SetOpacity(1);

  assembly->AddPart(sphere1);
  assembly->AddPart(sphere2);

	m_Renderer->AddActor(assembly);
	assembly->GetBounds();

  CPPUNIT_ASSERT(assembly->RenderOpaqueGeometry((vtkViewport*)m_Renderer) == 1);
  m_RenderWindow->Render();

	COMPARE_IMAGES("RenderOpaqueGeometryTest");

  assembly->ReleaseGraphicsResources(m_RenderWindow);
  assembly->RemovePart(sphere1);
  assembly->RemovePart(sphere2);

  vtkDEL(sph_source1);
  vtkDEL(sphere1);
  vtkDEL(mapper1);
  vtkDEL(sph_source2);
  vtkDEL(sphere2);
  vtkDEL(mapper2);
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::RenderTranslucentPolygonalGeometryTest()
//----------------------------------------------------------------------------
{
  vtkTimerLog::SetLogging(0); // Must shutdown logging otherwise it will generate leaks

  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);

  PrepareToRender(m_Renderer,m_RenderWindow);

  vtkALBAAssembly *assembly = vtkALBAAssembly::New();
  
  vtkSphereSource *sph_source1;
  vtkNEW(sph_source1);

  sph_source1->SetRadius(.5);
  sph_source1->SetCenter(-.25,0,0);
  sph_source1->Update();

  vtkPolyDataMapper *mapper1;
  vtkNEW(mapper1);

  mapper1->SetInputConnection(sph_source1->GetOutputPort());

  // translucent
  vtkActor *sphere1;
  vtkNEW(sphere1);

  sphere1->SetMapper(mapper1);
  sphere1->GetProperty()->SetAmbient(0.5f);
  sphere1->GetProperty()->SetDiffuse(0.5f);
  sphere1->GetProperty()->SetOpacity(.5);


  vtkSphereSource *sph_source2;
  vtkNEW(sph_source2);

  sph_source2->SetRadius(.5);
  sph_source2->SetCenter(.25,0,0);
  sph_source2->Update();

  vtkPolyDataMapper *mapper2;
  vtkNEW(mapper2);

  mapper2->SetInputConnection(sph_source2->GetOutputPort());

  // opaque
  vtkActor *sphere2;
  vtkNEW(sphere2);

  sphere2->SetMapper(mapper2);
  sphere2->GetProperty()->SetAmbient(0.5f);
  sphere2->GetProperty()->SetDiffuse(0.5f);
  sphere2->GetProperty()->SetOpacity(1);

  assembly->AddPart(sphere1);
  assembly->AddPart(sphere2);

  m_Renderer->AddActor(assembly);
	assembly->GetBounds();

  CPPUNIT_ASSERT(assembly->RenderTranslucentPolygonalGeometry((vtkViewport*)m_Renderer) == 1);
  m_RenderWindow->Render();

	COMPARE_IMAGES("RenderTranslucentGeometry");

	assembly->ReleaseGraphicsResources(m_RenderWindow);
  assembly->RemovePart(sphere1);
  assembly->RemovePart(sphere2);

  vtkDEL(sph_source1);
  vtkDEL(sphere1);
  vtkDEL(mapper1);
  vtkDEL(sph_source2);
  vtkDEL(sphere2);
  vtkDEL(mapper2);
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::InitPathTraversalTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkNEW(actor1);
  vtkActor *actor2;
  vtkNEW(actor2);
  vtkActor *actor3;
  vtkNEW(actor3);

  assembly->AddPart(actor1);
  assembly->AddPart(actor2);
  assembly->AddPart(actor3);

  // simulate a traversal
  assembly->InitPathTraversal();
  assembly->GetNextPath(); // return the current path and go to the next path

  // the init traversal method must initialize to the first path
  assembly->InitPathTraversal();
  vtkAssemblyPath *path = assembly->GetNextPath();

  CPPUNIT_ASSERT(path->GetNumberOfItems() == 2);
  CPPUNIT_ASSERT(path->GetFirstNode()->GetViewProp() == assembly);

  vtkDEL(actor1);
  vtkDEL(actor2);
  vtkDEL(actor3);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetNextPathTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  
  vtkALBAAssembly *assembly1 = vtkALBAAssembly::New();
  vtkALBAAssembly *assembly2 = vtkALBAAssembly::New();
  vtkALBAAssembly *assembly3 = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkNEW(actor1);
  vtkActor *actor2;
  vtkNEW(actor2);
  vtkActor *actor3;
  vtkNEW(actor3);

  assembly1->AddPart(actor1);
  assembly2->AddPart(actor2);
  assembly3->AddPart(actor3);

  assembly->AddPart(assembly1);
  assembly->AddPart(assembly2);
  assembly->AddPart(assembly3);

  // simulate a traversal
  assembly->InitPathTraversal();

  vtkAssemblyPath *path = assembly->GetNextPath();
  path->InitTraversal();
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly);
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly1);


  path = assembly->GetNextPath();
  path->InitTraversal();
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly);
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly2);

  path = assembly->GetNextPath();
  path->InitTraversal();
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly);
  CPPUNIT_ASSERT(path->GetNextNode()->GetViewProp() == assembly3);

  vtkDEL(actor1);
  vtkDEL(actor2);
  vtkDEL(actor3);
  vtkDEL(assembly1);
  vtkDEL(assembly2);
  vtkDEL(assembly3);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetNumberOfPathsTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkNEW(actor1);
  vtkActor *actor2;
  vtkNEW(actor2);
  vtkActor *actor3;
  vtkNEW(actor3);

  assembly->AddPart(actor1);
  assembly->AddPart(actor2);
  assembly->AddPart(actor3);

  CPPUNIT_ASSERT(3 == assembly->GetNumberOfPaths());

  vtkDEL(actor1);
  vtkDEL(actor2);
  vtkDEL(actor3);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetBoundsTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkCubeSource *cub_source1;
  vtkNEW(cub_source1);

  double b1[6];
  b1[0] = -2;
  b1[1] = 0;
  b1[2] = -1;
  b1[3] = 1;
  b1[4] = -1;
  b1[5] = 1;
  cub_source1->SetBounds(b1);
  cub_source1->Update();

  vtkPolyDataMapper *mapper1;
  vtkNEW(mapper1);

  mapper1->SetInputConnection(cub_source1->GetOutputPort());

  vtkActor *cube1;
  vtkNEW(cube1);

  cube1->SetMapper(mapper1);

  vtkCubeSource *cub_source2;
  vtkNEW(cub_source2);

  double b2[6];
  b2[0] = 0;
  b2[1] = 2;
  b2[2] = -1;
  b2[3] = 1;
  b2[4] = -1;
  b2[5] = 1;
  cub_source2->SetBounds(b2);
  cub_source2->Update();

  vtkPolyDataMapper *mapper2;
  vtkNEW(mapper2);

  mapper2->SetInputConnection(cub_source2->GetOutputPort());

  vtkActor *cube2;
  vtkNEW(cube2);

  cube2->SetMapper(mapper2);

  assembly->AddPart(cube1);
  assembly->AddPart(cube2);

  double b[6];

  assembly->GetBounds(b);

  CPPUNIT_ASSERT(b[0] = -2 && b[1] == 2 && b[2] == -1 && b[3] == 1 && b[4] == -1 && b[5] == 1);

  assembly->RemovePart(cube1);
  assembly->RemovePart(cube2);

  vtkDEL(cub_source1);
  vtkDEL(cube1);
  vtkDEL(mapper1);
  vtkDEL(cub_source2);
  vtkDEL(cube2);
  vtkDEL(mapper2);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::GetMTimeTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkActor *actor2;
  vtkNEW(actor1);
  vtkNEW(actor2);

  assembly->AddPart(actor1);
  assembly->AddPart(actor2);

  actor1->Modified();
  actor2->Modified();

  CPPUNIT_ASSERT(assembly->GetMTime() == actor2->GetMTime());

  assembly->RemovePart(actor1);
  assembly->RemovePart(actor2);

  vtkDEL(actor1);
  vtkDEL(actor2);
  vtkDEL(assembly);
}
//----------------------------------------------------------------------------
void vtkALBAAssemblyTest::ShallowCopyTest()
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *assembly1 = vtkALBAAssembly::New();
  vtkALBAAssembly *assembly2 = vtkALBAAssembly::New();

  vtkActor *actor1;
  vtkActor *actor2;
  vtkNEW(actor1);
  vtkNEW(actor2);

  assembly1->AddPart(actor1);
  assembly1->AddPart(actor2);

  assembly2->ShallowCopy(assembly1);

  vtkProp3DCollection *parts1 = assembly1->GetParts();
  vtkProp3DCollection *parts2 = assembly2->GetParts();

  CPPUNIT_ASSERT(parts1->GetNumberOfItems() == parts2->GetNumberOfItems());

  parts1->InitTraversal();
  parts2->InitTraversal();
  for (int i=0; i<parts1->GetNumberOfItems(); i++)
  {
    CPPUNIT_ASSERT(parts1->GetNextProp3D() == parts2->GetNextProp3D());
  }

  vtkDEL(actor1);
  vtkDEL(actor2);
  vtkDEL(assembly1);
  vtkDEL(assembly2);
}
//------------------------------------------------------------
void vtkALBAAssemblyTest::PrepareToRender(vtkRenderer *renderer, vtkRenderWindow *render_window)
//------------------------------------------------------------
{
  renderer->SetBackground(0.0, 0.0, 0.0);

  vtkCamera *camera = renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  render_window->AddRenderer(renderer);
  render_window->SetSize(640, 480);
  render_window->SetPosition(100,0);
}