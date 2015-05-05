/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceTest
 Authors: Matteo Giacomoni, Stefano Perticoni, Paolo Quadrani
 
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

#include "mafVMESurfaceTest.h"
#include <wx/dir.h>

#include "mafVMESurface.h"
#include "mafVMEFactory.h"
#include "mafCoreFactory.h"
#include "mafNodeIterator.h"
#include "mafVMEPolyline.h"
#include "mafTransform.h"
#include "mafVMEItem.h"
#include "mafDataVector.h"

#include "vtkMAFSmartPointer.h"
#include "vtkAssembly.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkTextSource.h"
#include "vtkCylinderSource.h"
#include "vtkAxes.h"
#include "vtkFileOutputWindow.h"
#include "vtkSphereSource.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"

#include <iostream>
#include <set>


//----------------------------------------------------------------------------
void mafVMESurfaceTest::setUp()
//----------------------------------------------------------------------------
{
  m_VmeRoot = NULL;
}

//----------------------------------------------------------------------------
void mafVMESurfaceTest::tearDown()
//----------------------------------------------------------------------------
{


}

//----------------------------------------------------------------------------
void mafVMESurfaceTest::TestSetData()
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafVMESurface> vmeSurface;

  // We'll create the building blocks of polydata including data attributes.
  vtkMAFSmartPointer<vtkPolyData> cube;
  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> polys;
  vtkMAFSmartPointer<vtkFloatArray> scalars;

  // Load the point, cell, and data attributes.
	int i;
	static float x[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
                        {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
  static vtkIdType pts[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
                        {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
  for (i=0; i<8; i++) points->InsertPoint(i,x[i]);
  for (i=0; i<6; i++) polys->InsertNextCell(4,pts[i]);
  for (i=0; i<8; i++) scalars->InsertTuple1(i,i);

  // We now assign the pieces to the vtkPolyData.
  cube->SetPoints(points);
  cube->SetPolys(polys);
  cube->GetPointData()->SetScalars(scalars);

	int result=vmeSurface->SetData(cube,0);
	CPPUNIT_ASSERT(result == MAF_OK);

	// create bad polydata for mafVMESurface
	vtkMAFSmartPointer<vtkPolyData> bad_polyData;
  bad_polyData->SetPoints(points);
	result = vmeSurface->SetData(bad_polyData,0);
	CPPUNIT_ASSERT(result == MAF_ERROR);
}

//----------------------------------------------------------------------------
void mafVMESurfaceTest::TestVMESurfaceVisualization()
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

  mafSmartPointer<mafVMESurface> sphereSurfaceVME;

  vtkMAFSmartPointer<vtkSphereSource> sphereSource;

  sphereSurfaceVME->SetData(sphereSource->GetOutput(),-1);

  mapper->SetInput(sphereSurfaceVME->GetSurfaceOutput()->GetVTKData());

  renWin->Render();
  CPPUNIT_ASSERT(true);
}

//----------------------------------------------------------------------------
void mafVMESurfaceTest::TestTimeVaryingVMESurfaceTree()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  PlayTree(m_VmeRoot, FALSE);
}


//---------------------------------------------------------------------------
int mafVMESurfaceTest::PlayTree(mafVMERoot *root, bool ignoreVisibleToTraverse)
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

  mafNodeIterator *iter=root->NewIterator();
  iter->IgnoreVisibleToTraverse(ignoreVisibleToTraverse);

  // connect VME to assemblies and put root assembly into the renderer
  for (mafNode *node=iter->GetFirstNode();node;node=iter->GetNextNode())
  {
    if (mafVME *vme=mafVME::SafeDownCast(node))
    {
      if (vme->IsMAFType(mafVMERoot))
      {
        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        mafClientData *clientdata=mafClientData::SafeDownCast(vme->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count already set to 0
          clientdata->SetName("ClientData");

          vme->SetAttribute("ClientData",clientdata);
        }

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        renderer->AddActor(vmeasm);
      }
      else
      {
        vtkDataSet *vmedata=vme->GetOutput()->GetVTKData();
        vtkMAFSmartPointer<vtkDataSetMapper> mapper;
        mapper->SetInput((vtkPolyData *)vmedata);

        vtkMAFSmartPointer<vtkActor> vmeact;
        vmeact->SetMapper(mapper);

        vtkMAFSmartPointer<vtkAssembly> vmeasm;
        vmeasm->AddPart(vmeact);
        vmeasm->SetUserTransform(vme->GetOutput()->GetTransform()->GetVTKTransform());
        mafClientData *clientdata=mafClientData::SafeDownCast(vme->GetAttribute("ClientData"));
        if (!clientdata)
        {
          clientdata=mafClientData::New(); // note ref count initially set to 0
          clientdata->SetName("ClientData");
          vme->SetAttribute("ClientData",clientdata);
        }

        CPPUNIT_ASSERT(clientdata!=NULL);
        clientdata->m_Prop3D=vmeasm;

        mafClientData *pclientdata=mafClientData::SafeDownCast(vme->GetParent()->GetAttribute("ClientData"));
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
  CPPUNIT_ASSERT(m_VmeRoot);

  return MAF_OK;
}

//----------------------------------------------------------------------------
void mafVMESurfaceTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{

  m_VmeRoot = mafVMERoot::New();
 // mafSmartPointer<mafVMERoot> m_VmeRoot;

  vtkMAFSmartPointer<vtkFileOutputWindow> log;
  log->SetInstance(log);
  log->SetFileName("testMSF.log"); // log of VTK error messages

  // plug the custom attribute in the Node Factory
  mafPlugAttribute<mafClientData>("Simple attribute for attaching actors to VMEs");

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkMAFSmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  mafVMEGeneric  *FirstChild = mafVMEGeneric::New();

  mafSmartPointer<mafVMESurface> vsphere;
  mafSmartPointer<mafVMESurface> vcone;
  mafSmartPointer<mafVMESurface> vmorph;

  FirstChild->SetName("title");
  vsphere->SetName("sphere");

  vcone->SetName("cone");
  vmorph->SetName("morph");

  mafTransform trans;
  FirstChild->SetData(axes->GetOutput(),0);


  m_VmeRoot->AddChild(FirstChild);

  FirstChild->AddChild(vsphere);
  FirstChild->AddChild(vcone);
  FirstChild->AddChild(vmorph);

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  vtkMAFSmartPointer<vtkConeSource> cone;

  int i;
  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    FirstChild->SetPose(trans.GetMatrix(),i);

    // The sphere: time fixed
    sphere->SetRadius(.1+.01*i);
    vsphere->SetData(sphere->GetOutput(), -1);

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    vsphere->SetPose(trans.GetMatrix(),i*.5+25);

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

  mafDEL(FirstChild);

}
