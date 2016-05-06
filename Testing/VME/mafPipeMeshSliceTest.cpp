/*=========================================================================

 Program: MAF2
 Module: mafPipeMeshSliceTest
 Authors: Daniele Giunchi
 
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
#include "mafPipeMeshSliceTest.h"
#include "mafPipeMeshSlice.h"

#include "mafSceneNode.h"
#include "mafVMEMesh.h"

#include "mafOpImporterVTK.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "vtkMAFAssembly.h"


#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "mmaMaterial.h"


// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum PIPE_MESH_ACTORS
  {
    PIPE_MESH_ACTOR,
    PIPE_MESH_ACTOR_WIRED,
    PIPE_MESH_ACTOR_OUTLINE_CORNER,
    PIPE_MESH_NUMBER_OF_ACTORS,
  };

//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{

  ///////////////// render stuff /////////////////////////
  
  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);

  

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////



  ////// Create VME (import vtkData) ////////////////////
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafOpImporterVTK *Importer=new mafOpImporterVTK("importer");
  mafString filename=MAF_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  mafVMEMesh *mesh;
  mesh =mafVMEMesh::SafeDownCast(Importer->GetOutput());
	mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
  

  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,mesh, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeMeshSlice *pipeMeshSlice = new mafPipeMeshSlice;
  pipeMeshSlice->SetScalarMapActive(1);
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin);
  pipeMeshSlice->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }
	
	const char *strings[5];
	strings[0] = "Id"; //point 

	strings[1] = "Material"; //cell 
	strings[2] = "EX";
	strings[3] = "NUXY";
	strings[4] = "DENS";

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
  {
    double controlValues[2] = {-9999,-9999};
    switch(arrayIndex)
    {
    case 0:
      {
        controlValues[0] = 1.0;
        controlValues[1] = 12.0;
      }
      break;
    case 1:
      {
        controlValues[0] = 2.0; 
        controlValues[1] = 3.0;
      }
      break;
    case 2:
      {
        controlValues[0] = 1000.0;
        controlValues[1] = 200000.0;
      }
      break;
    case 3:
      {
        controlValues[0] = 0.33;
        controlValues[1] =  0.39 ;
      }
      break;
    case 4:
      {
        controlValues[0] = 0.107;
        controlValues[1] = 1.07;
      }
      break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&mafEvent(this, mafPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    ProceduralControl(controlValues, meshActor);
    m_RenderWindow->Render();
		printf("\n Visualizzazione: %s \n", strings[arrayIndex]);
    CompareImages(arrayIndex);
  }

	delete sceneNode;

  vtkDEL(actorList);
  cppDEL(Importer);
  mafDEL(storage);

}
//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');

  name = name.substr(0, pointIndex);


  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/FEM/pipemesh/";
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
    imageFile<<"/FEM/pipemesh/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/FEM/pipemesh/";
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
  imageFileOrig<<"/FEM/pipemesh/";
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
  vtkDEL(rO);
  vtkDEL(rC);
  vtkDEL(imageMath);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
void mafPipeMeshSliceTest::ProceduralControl(double controlRangeMapper[2],vtkProp *propToControl)
//----------------------------------------------------------------------------
{
  //procedural control

  double sr[2];
  ((vtkActor* )propToControl)->GetMapper()->GetScalarRange(sr);
  CPPUNIT_ASSERT(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1] || 1);
  //end procedural control

}
//----------------------------------------------------------------------------
vtkProp *mafPipeMeshSliceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
