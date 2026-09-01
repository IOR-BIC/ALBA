/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGizmoTest
 Authors: Stefano Perticoni
 
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
#include "albaVMETests.h"

#include <cppunit/config/SourcePrefix.h>
#include "albaVMEGizmoTest.h"

#include "albaVMEFactory.h"
#include "albaCoreFactory.h"
#include "albaVMEIterator.h"
#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaTransform.h"
#include "albaVMEItem.h"
#include "albaDataVector.h"
#include "albaDirectory.h"
#include "albaVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"

#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkTextSource.h"
#include "vtkCylinderSource.h"
#include "vtkAxes.h"
#include "vtkFileOutputWindow.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include <wx/dir.h>

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
int albaVMEGizmoTest::PlayTree(albaVMERoot *root, bool ignoreVisibleToTraverse)
//----------------------------------------------------------------------------
{
  //
  // Test display of generic VME tree of surfaces
  //
  
  vtkALBASmartPointer<vtkRenderWindow> renWin;
  vtkALBASmartPointer<vtkRenderer> renderer;
  vtkALBASmartPointer<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  albaVMEIterator *iter=root->NewIterator();
  iter->IgnoreVisibleToTraverse(ignoreVisibleToTraverse);

  // connect VME to assemblies and put root assembly into the renderer
  for (albaVME *node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (node)
    {
      if (node->IsALBAType(albaVMERoot))
      {
        vtkALBASmartPointer<vtkAssembly> vmeasm;
        albaClientData *clientdata=albaClientData::SafeDownCast(node->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=albaClientData::New(); // note ref count already set to 0
          clientdata->SetName("ClientData");
 
          node->SetAttribute("ClientData",clientdata);
        }

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        renderer->AddActor(vmeasm);
      }
      else
      {
        vtkDataSet *vmedata=node->GetOutput()->GetVTKData();
        vtkALBASmartPointer<vtkDataSetMapper> mapper;
        mapper->SetInputData((vtkPolyData *)vmedata);

        vtkALBASmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);

        vtkALBASmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        vmeasm->SetUserTransform(node->GetOutput()->GetTransform()->GetVTKTransform());
        albaClientData *clientdata=albaClientData::SafeDownCast(node->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=albaClientData::New(); // note ref count initially set to 0
          clientdata->SetName("ClientData");
          node->SetAttribute("ClientData",clientdata);
        }

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        albaClientData *pclientdata=albaClientData::SafeDownCast(node->GetParent()->GetAttribute("ClientData"));
        CPPUNIT_ASSERT(pclientdata);
        vtkAssembly *pvmeasm=(vtkAssembly *)pclientdata->m_Prop3D;
        pvmeasm->AddPart(vmeasm);
      }
    }    
  }

  albaTimeStamp treeTimeBounds[2];
  root->GetOutput()->GetTimeBounds(treeTimeBounds);

  CPPUNIT_ASSERT(treeTimeBounds[0]==0);
  CPPUNIT_ASSERT(treeTimeBounds[1]==200);

  float spacing=(treeTimeBounds[1]-treeTimeBounds[0])/100;

  int step=1;
  for (int t = 0; t < 10; t+=step) 
  {
    if (t==10)
    {
      step=-1;
      continue;
    }

    // Update time into all the tree
    root->SetTreeTime(t*spacing);

    renderer->ResetCameraClippingRange();
    renWin->Render();
  }


  // this destroy m_VmeRoot also... 
  iter->Delete();

  // these pointers are not NULL but all of their members are not valid anymore... ? 
  CPPUNIT_ASSERT(m_FirstChild);
  CPPUNIT_ASSERT(m_VmeRoot);

  return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaVMEGizmoTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaVMEGizmoTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VmeRoot = NULL;
  m_FirstChild = NULL;

  // initialized the VME factory
  albaVMEFactory::Initialize();
}

//----------------------------------------------------------------------------
void albaVMEGizmoTest::AfterTest()
//----------------------------------------------------------------------------
{
    albaDEL(m_FirstChild);
}

//----------------------------------------------------------------------------
void albaVMEGizmoTest::TestGizmoCreateAndDestroy()
//----------------------------------------------------------------------------
{
  // Use New() to build the gizmo because it isn't into the factory (gizmo hasn't to be saved!!)
  albaVMEGizmo *gizmo = albaVMEGizmo::New();
  
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();
  CPPUNIT_ASSERT(sphere->GetOutput());
  gizmo->SetData(sphere->GetOutput());
  sphere->Delete();  // data is deleted by the gizmo.
  
  gizmo->Delete();
}
//----------------------------------------------------------------------------
void albaVMEGizmoTest::TestGizmoVisualizatioAlone()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkRenderWindow> renWin;
  vtkALBASmartPointer<vtkRenderer> renderer;
  vtkALBASmartPointer<vtkRenderWindowInteractor> iren;
  vtkALBASmartPointer<vtkActor> actor;
  vtkALBASmartPointer<vtkDataSetMapper> mapper;
  actor->SetMapper(mapper);
  renderer->AddActor(actor);

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  albaSmartPointer<albaVMEGizmo> gizmo;
  
  vtkALBASmartPointer<vtkSphereSource> sphere;

  gizmo->SetData(sphere->GetOutput());

  mapper->SetInputData(gizmo->GetSurfaceOutput()->GetVTKData());
  
  renWin->Render();
  
}

//----------------------------------------------------------------------------
void albaVMEGizmoTest::TestGizmoVisualizationInVMETree()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  CPPUNIT_ASSERT( PlayTree(m_VmeRoot, true) == ALBA_OK) ;
}

//----------------------------------------------------------------------------
void albaVMEGizmoTest::TestGizmoVisibleToTraverse()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  CPPUNIT_ASSERT( PlayTree(m_VmeRoot, false) == ALBA_OK) ;
}
//----------------------------------------------------------------------------
void albaVMEGizmoTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{
  m_VmeRoot = albaVMERoot::New();

  // plug the custom attribute in the Node Factory
  albaPlugAttribute<albaClientData>("Simple attribute for attaching actors to VMEs");

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkALBASmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  m_FirstChild = albaVMEGeneric::New();

  albaSmartPointer<albaVMEGizmo> sphereGizmo;
  albaSmartPointer<albaVMESurface> vcone;
  albaSmartPointer<albaVMESurface> vmorph;

  m_FirstChild->SetName("title");
  sphereGizmo->SetName("sphere gizmo");
  
  vcone->SetName("cone");
  vmorph->SetName("morph");

  albaTransform trans;
  m_FirstChild->SetData(axes->GetOutput(),0);

  
  m_VmeRoot->AddChild(m_FirstChild);

  m_FirstChild->AddChild(sphereGizmo);
  m_FirstChild->AddChild(vcone);
  m_FirstChild->AddChild(vmorph);

  vtkALBASmartPointer<vtkSphereSource> sphere;
  vtkALBASmartPointer<vtkConeSource> cone;
	cone->Update();
	sphere->Update();

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    m_FirstChild->SetPose(trans.GetMatrix(),i);

    // The sphere
    sphere->SetRadius(.1+.01*i);
    sphereGizmo->SetData(sphere->GetOutput());

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    sphereGizmo->SetPose(trans.GetMatrix(),i*.5+25);

    // the cone
    cone->SetResolution(103-i);

    vcone->SetData(cone->GetOutput(),200-i*2);
    trans.Identity();
    trans.Translate(2-(double)i/50.0,0,0,POST_MULTIPLY);

    vcone->SetPose(trans.GetMatrix(),i*.5+75);

    vtkPolyDataAlgorithm *morph;

    // the morphing tube
    if (i<50)
    {
      vtkCylinderSource *cyl=vtkCylinderSource::New();

      cyl->SetResolution(52-i);
      morph=cyl;
    }
    else
    {
      vtkCubeSource *cube=vtkCubeSource::New();

      cube->SetYLength((100.0-(double)i)/100.0+.5);
      cube->SetXLength(1);
      cube->SetZLength(1);
      morph=cube;
    }

		morph->Update();
    vmorph->SetData(morph->GetOutput(),log10((double)(100-i))*50);
    morph->Delete();
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  vmorph->SetPose(trans.GetMatrix(),0); // set pose at time 0
}
