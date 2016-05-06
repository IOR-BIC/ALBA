/*=========================================================================

 Program: MAF2
 Module: mafVisualPipeSlicerSliceTest
 Authors: Simone Brazzale
 
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
#include "mafVisualPipeSlicerSliceTest.h"

#include "mafVisualPipeSlicerSlice.h"
#include "mafSceneNode.h"
#include "mafOpImporterVTK.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkMAFAssembly.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESlicer.h"
#include "mmaMaterial.h"

//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////
  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);
  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  /////// Create VME (import vtkData) ////////////////////
  mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	mafVMERoot *root=storage->GetRoot();

	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/VTK_Volumes/volume.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

  mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->ReparentTo(root);
	volume->Update();

	double centerV[3], rotV[3];
	mafOBB b;
	rotV[0] = rotV[1] = rotV[2] = 0;
	volume->GetOutput()->GetVMELocalBounds(b);
	b.GetCenter(centerV);

	mafVMESlicer *slicer;
	mafNEW(slicer);

	//Setting standard material to avoid random color selection
	slicer->GetMaterial()->m_Diffuse[0]=0.3;
	slicer->GetMaterial()->m_Diffuse[1]=0.6;
	slicer->GetMaterial()->m_Diffuse[2]=0.9;
	slicer->GetMaterial()->UpdateProp();

	slicer->ReparentTo(volume);
  slicer->SetSlicedVMELink(volume);

	slicer->SetPose(centerV,rotV,0);
	slicer->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL, rootscenenode, slicer, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafVisualPipeSlicerSlice *pipe = new mafVisualPipeSlicerSlice();
  pipe->Create(sceneNode);
  pipe->m_RenFront = m_Renderer;

  CPPUNIT_ASSERT(pipe->GetAssemblyFront()!=NULL);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();
    actor = actorList->GetNextProp();
  }

  vtkActor *meshActor;
  meshActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(meshActor != NULL);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  CompareImages(1);

  vtkDEL(actorList);

  cppDEL(sceneNode);
  cppDEL(rootscenenode);

  volume->ReparentTo(NULL);
  slicer->ReparentTo(NULL);
  
  mafDEL(slicer);
  cppDEL(importer);
  mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVisualPipeSlicerSliceTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_VisualPipeSlicerSlice/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<scalarIndex;
  controlOriginFile<<".jpg";

  fstream controlStream;
  controlStream.open(controlOriginFile.GetCStr()); 

  // visualization control
  m_RenderWindow->OffScreenRenderingOn();
  vtkWindowToImageFilter *w2i;
  vtkNEW(w2i);
  w2i->SetInput(m_RenderWindow);
  //w2i->SetMagnification(magnification);
  w2i->Update();
  m_RenderWindow->OffScreenRenderingOff();

  //write comparing image
  vtkJPEGWriter *w;
  vtkNEW(w);
  w->SetInput(w2i->GetOutput());
  mafString imageFile=MAF_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_VisualPipeSlicerSlice/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_VisualPipeSlicerSlice/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<scalarIndex;
  imageFile<<".jpg";
  w->SetFileName(imageFile.GetCStr());
  w->Write();

  if(!controlStream)
  {
    controlStream.close();
    vtkDEL(w);
    vtkDEL(w2i);

    return;
  }
  controlStream.close();

  //read original Image
  vtkJPEGReader *rO;
  vtkNEW(rO);
  mafString imageFileOrig=MAF_DATA_ROOT;
  imageFileOrig<<"/Test_VisualPipeSlicerSlice/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<scalarIndex;
  imageFileOrig<<".jpg";
  rO->SetFileName(imageFileOrig.GetCStr());
  rO->Update();

  vtkImageData *imDataOrig = rO->GetOutput();

  //read compared image
  vtkJPEGReader *rC;
  vtkNEW(rC);
  rC->SetFileName(imageFile.GetCStr());
  rC->Update();

  vtkImageData *imDataComp = rC->GetOutput();

  vtkImageMathematics *imageMath = vtkImageMathematics::New();
  imageMath->SetInput1(imDataOrig);
  imageMath->SetInput2(imDataComp);
  imageMath->SetOperationToSubtract();
  imageMath->Update();

  double srR[2] = {-1,1};
  imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(srR);

  CPPUNIT_ASSERT(srR[0] == 0.0 && srR[1] == 0.0);

  // end visualization control
  vtkDEL(imageMath);
  vtkDEL(rC);
  vtkDEL(rO);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
vtkProp *mafVisualPipeSlicerSliceTest::SelectActorToControl(vtkPropCollection *propList, int index)
//----------------------------------------------------------------------------
{
  propList->InitTraversal();
  vtkProp *actor = propList->GetNextProp();
  int count = 0;
  while(actor)
  {   
    if(count == index) return actor;
    actor = propList->GetNextProp();
    count ++;
  }
  return NULL;
}