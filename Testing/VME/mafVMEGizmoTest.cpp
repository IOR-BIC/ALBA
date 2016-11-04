/*=========================================================================

 Program: MAF2
 Module: mafVMEGizmoTest
 Authors: Stefano Perticoni
 
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
#include "mafVMETests.h"

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEGizmoTest.h"

#include "mafVMEFactory.h"
#include "mafCoreFactory.h"
#include "mafVMEIterator.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafTransform.h"
#include "mafVMEItem.h"
#include "mafDataVector.h"
#include "mafDirectory.h"
#include "mafVMEGizmo.h"

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
#include "vtkMAFSmartPointer.h"

#include <wx/dir.h>

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
int mafVMEGizmoTest::PlayTree(mafVMERoot *root, bool ignoreVisibleToTraverse)
//----------------------------------------------------------------------------
{
  //
  // Test display of generic VME tree of surfaces
  //
  
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  mafVMEIterator *iter=root->NewIterator();
  iter->IgnoreVisibleToTraverse(ignoreVisibleToTraverse);

  // connect VME to assemblies and put root assembly into the renderer
  for (mafVME *node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (node)
    {
      if (node->IsMAFType(mafVMERoot))
      {
        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        mafClientData *clientdata=mafClientData::SafeDownCast(node->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count already set to 0
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
        vtkMAFSmartPointer<vtkDataSetMapper> mapper;
        mapper->SetInput((vtkPolyData *)vmedata);

        vtkMAFSmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);

        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        vmeasm->SetUserTransform(node->GetOutput()->GetTransform()->GetVTKTransform());
        mafClientData *clientdata=mafClientData::SafeDownCast(node->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count initially set to 0
          clientdata->SetName("ClientData");
          node->SetAttribute("ClientData",clientdata);
        }

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        mafClientData *pclientdata=mafClientData::SafeDownCast(node->GetParent()->GetAttribute("ClientData"));
        CPPUNIT_ASSERT(pclientdata);
        vtkAssembly *pvmeasm=(vtkAssembly *)pclientdata->m_Prop3D;
        pvmeasm->AddPart(vmeasm);
      }
    }    
  }

  mafTimeStamp treeTimeBounds[2];
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

  return MAF_OK;
}
//----------------------------------------------------------------------------
void mafVMEGizmoTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEGizmoTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VmeRoot = NULL;
  m_FirstChild = NULL;

  // initialized the VME factory
  mafVMEFactory::Initialize();
}

//----------------------------------------------------------------------------
void mafVMEGizmoTest::AfterTest()
//----------------------------------------------------------------------------
{
    mafDEL(m_FirstChild);
}

//----------------------------------------------------------------------------
void mafVMEGizmoTest::TestGizmoCreateAndDestroy()
//----------------------------------------------------------------------------
{
  // Use New() to build the gizmo because it isn't into the factory (gizmo hasn't to be saved!!)
  mafVMEGizmo *gizmo = mafVMEGizmo::New();
  
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();
  CPPUNIT_ASSERT(sphere->GetOutput());
  gizmo->SetData(sphere->GetOutput());
  sphere->Delete();  // data is deleted by the gizmo.
  
  gizmo->Delete();
}
//----------------------------------------------------------------------------
void mafVMEGizmoTest::TestGizmoVisualizatioAlone()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkRenderWindow> renWin;
  vtkMAFSmartPointer<vtkRenderer> renderer;
  vtkMAFSmartPointer<vtkRenderWindowInteractor> iren;
  vtkMAFSmartPointer<vtkActor> actor;
  vtkMAFSmartPointer<vtkDataSetMapper> mapper;
  actor->SetMapper(mapper);
  renderer->AddActor(actor);

  renWin->AddRenderer(renderer);
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.1, 0.1, 0.1);
  renWin->SetSize(640, 480);
  renWin->SetPosition(400,0);
  renWin->StereoCapableWindowOn();

  mafSmartPointer<mafVMEGizmo> gizmo;
  
  vtkMAFSmartPointer<vtkSphereSource> sphere;

  gizmo->SetData(sphere->GetOutput());

  mapper->SetInput(gizmo->GetSurfaceOutput()->GetVTKData());
  
  renWin->Render();
  
}

//----------------------------------------------------------------------------
void mafVMEGizmoTest::TestGizmoVisualizationInVMETree()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  CPPUNIT_ASSERT( PlayTree(m_VmeRoot, true) == MAF_OK) ;
}

//----------------------------------------------------------------------------
void mafVMEGizmoTest::TestGizmoVisibleToTraverse()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  CPPUNIT_ASSERT( PlayTree(m_VmeRoot, false) == MAF_OK) ;
}
//----------------------------------------------------------------------------
void mafVMEGizmoTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{
  m_VmeRoot = mafVMERoot::New();

  // plug the custom attribute in the Node Factory
  mafPlugAttribute<mafClientData>("Simple attribute for attaching actors to VMEs");

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkMAFSmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  m_FirstChild = mafVMEGeneric::New();

  mafSmartPointer<mafVMEGizmo> sphereGizmo;
  mafSmartPointer<mafVMESurface> vcone;
  mafSmartPointer<mafVMESurface> vmorph;

  m_FirstChild->SetName("title");
  sphereGizmo->SetName("sphere gizmo");
  
  vcone->SetName("cone");
  vmorph->SetName("morph");

  mafTransform trans;
  m_FirstChild->SetData(axes->GetOutput(),0);

  
  m_VmeRoot->AddChild(m_FirstChild);

  m_FirstChild->AddChild(sphereGizmo);
  m_FirstChild->AddChild(vcone);
  m_FirstChild->AddChild(vmorph);

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  vtkMAFSmartPointer<vtkConeSource> cone;

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

    vtkPolyDataSource *morph;

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

    vmorph->SetData(morph->GetOutput(),log10((double)(100-i))*50);
    morph->Delete();
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  vmorph->SetPose(trans.GetMatrix(),0); // set pose at time 0
}
