/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEImageTest
 Authors: Daniele Giunchi
 
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
#include "albaVMETests.h"

#include "albaVMEImageTest.h"
#include <wx/dir.h>

#include "albaVMEImage.h"
#include "albaVMEFactory.h"
#include "albaCoreFactory.h"
#include "albaVMEIterator.h"
#include "albaVMEPolyline.h"
#include "albaTransform.h"
#include "albaVMEItem.h"
#include "albaDataVector.h"

#include "vtkALBASmartPointer.h"
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
void albaVMEImageTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VmeRoot = NULL;
}

//----------------------------------------------------------------------------
void albaVMEImageTest::TestSetData()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMEImage> vmeImage;

  // We'll create the building blocks of polydata including data attributes.
  vtkALBASmartPointer<vtkImageData> image;
  
  vtkALBASmartPointer<vtkFloatArray> scalars;

  int dimensions[3] = {10,10,1};
  double spacing[3] = {1.0,1.0,1.0};

  // Load the point, cell, and data attributes.
	int i;
  for (i=0; i<100; i++) scalars->InsertTuple1(i,i);

  // We now assign the spacing,dimensions, attributes to vtkImageData.
  image->SetDimensions(dimensions);
  image->SetSpacing(spacing);
  image->GetPointData()->SetScalars(scalars);

	int result=vmeImage->SetData(image,0);
	CPPUNIT_ASSERT(result == ALBA_OK);

  //it doesn't exist a condition of bad vtkImageData.
}

//----------------------------------------------------------------------------
void albaVMEImageTest::TestVMEImageVisualization()
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

  albaSmartPointer<albaVMEImage> imageVME;

  vtkALBASmartPointer<vtkImageData> imageData;
  int dimensions[3] = {10,10,1};
  double spacing[3] = {1.0,1.0,1.0};

  vtkALBASmartPointer<vtkFloatArray> scalars;
  // Load the point, cell, and data attributes.
  int i;
  for (i=0; i<100; i++) scalars->InsertTuple1(i,i);

  // We now assign the spacing,dimensions, attributes to vtkImageData.
  imageData->SetDimensions(dimensions);
  imageData->SetSpacing(spacing);
  imageData->GetPointData()->SetScalars(scalars);

  imageVME->SetData(imageData,-1);

  mapper->SetInputData(imageVME->GetImageOutput()->GetVTKData());
  mapper->SetScalarModeToDefault();
  mapper->SetScalarRange(scalars->GetRange());

  renWin->Render();
}

//----------------------------------------------------------------------------
void albaVMEImageTest::TestTimeVaryingVMEImageTree()
//----------------------------------------------------------------------------
{
  CreateVMETestTree();
  PlayTree(m_VmeRoot, false);
}


//---------------------------------------------------------------------------
int albaVMEImageTest::PlayTree(albaVMERoot *root, bool ignoreVisibleToTraverse)
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
        mapper->SetScalarModeToDefault();
        mapper->SetScalarRange(0,100);

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
    //iren->Start();
  }

  // this destroy m_VmeRoot also... 
  iter->Delete();

  // these pointers are not NULL but all of their members are not valid anymore... ? 
  CPPUNIT_ASSERT(m_VmeRoot);

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVMEImageTest::CreateVMETestTree()
//----------------------------------------------------------------------------
{

  m_VmeRoot = albaVMERoot::New();
 // albaSmartPointer<albaVMERoot> m_VmeRoot;

 // plug the custom attribute in the Node Factory
  albaPlugAttribute<albaClientData>("Simple attribute for attaching actors to VMEs");

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2);

  vtkALBASmartPointer<vtkTextSource> text;

  text->SetText("VME Tree Test");
  text->Update();

  // this will be destroyed when the tree is destroyed: no need to call delete... 
  albaVMEGeneric  *FirstChild = albaVMEGeneric::New();

  albaSmartPointer<albaVMEImage> image1;
  albaSmartPointer<albaVMEImage> image2;
  albaSmartPointer<albaVMEImage> image3;

  FirstChild->SetName("title");
  image1->SetName("image1");
  image2->SetName("image2");
  image3->SetName("image3");

  albaTransform trans;
  FirstChild->SetData(axes->GetOutput(),0);


  m_VmeRoot->AddChild(FirstChild);

  FirstChild->AddChild(image1);
  FirstChild->AddChild(image2);
  FirstChild->AddChild(image3);
  int i;
  vtkALBASmartPointer<vtkImageData> texture1;
  texture1->SetSpacing(1.0,1.0,1.0);
  texture1->SetDimensions(10,10,1);
  vtkALBASmartPointer<vtkFloatArray> scalars1;
  for (i=0; i<100; i++) scalars1->InsertTuple1(i,i);
  texture1->GetPointData()->SetScalars(scalars1);

  vtkALBASmartPointer<vtkImageData> texture2;
  texture2->SetSpacing(2.0,2.0,2.0);
  texture2->SetDimensions(10,10,1);
  vtkALBASmartPointer<vtkFloatArray> scalars2;
  for (i=0; i<100; i++) scalars2->InsertTuple1(i,i/2);
  texture2->GetPointData()->SetScalars(scalars2);


  vtkALBASmartPointer<vtkImageData> texture3;
  texture3->SetSpacing(3.0,3.0,3.0);
  texture3->SetDimensions(10,10,1);
  vtkALBASmartPointer<vtkFloatArray> scalars3;
  for (i=0; i<100; i++) scalars3->InsertTuple1(i,i);
  texture3->GetPointData()->SetScalars(scalars3);


  for (i=0;i<100;i++)
  {
    trans.Identity();
    trans.RotateY(i*1.6-180.0,POST_MULTIPLY);

    FirstChild->SetPose(trans.GetMatrix(),i);

    // The image1
    image1->SetData(texture1, -1);

    trans.Identity();
    trans.Translate(0,.02*i,0,POST_MULTIPLY);
    image1->SetPose(trans.GetMatrix(),i*.5+25);

    // the image2
    
    image2->SetData(texture2,200-i*2);
    trans.Identity();
    trans.Translate(2-(double)i/50.0,10,10,POST_MULTIPLY);

    image2->SetPose(trans.GetMatrix(),i*.5+75);

    
    image3->SetData(texture3,log10((double)(100-i))*50);
    
  }

  trans.Identity();
  trans.RotateZ(90,POST_MULTIPLY);
  trans.Translate(-.75,0,0,POST_MULTIPLY);
  image3->SetPose(trans.GetMatrix(),0); // set pose at time 0

  albaDEL(FirstChild);

}
