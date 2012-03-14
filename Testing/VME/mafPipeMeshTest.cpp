/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMeshTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-24 13:33:00 $
Version:   $Revision: 1.5.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafPipeMeshTest.h"
#include "mafPipeMesh.h"

#include "mafSceneNode.h"
#include "mafVMEMesh.h"

#include "mafVMERoot.h"
#include "vtkMAFAssembly.h"


#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
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
void mafPipeMeshTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeMeshTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeshTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeshTest::TestPipeExecution()
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
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEMesh *mesh;
  mafNEW(mesh);
  mesh->SetData(Importer->GetOutput(),0.0);
  mesh->GetOutput()->Update();
	mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,mesh, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeMesh *pipeMesh = new mafPipeMesh;
  pipeMesh->SetScalarMapActive(1);
  pipeMesh->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMesh->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }
	
	const char *strings[5];
	strings[0] = "id"; //point 

	strings[1] = "material"; //cell 
	strings[2] = "EX";
	strings[3] = "NUXY";
	strings[4] = "DENS";

  for(int arrayIndex=0; arrayIndex<pipeMesh->GetNumberOfArrays(); arrayIndex++)
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
    pipeMesh->SetActiveScalar(arrayIndex);
    pipeMesh->OnEvent(&mafEvent(this, mafPipeMesh::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    ProceduralControl(controlValues, meshActor);
    m_RenderWindow->Render();
		printf("\n Visualizzazione: %s \n", strings[arrayIndex]);
    mafSleep(500);
    CompareImages(arrayIndex);
  }

  vtkDEL(actorList);

  delete pipeMesh;
  delete sceneNode;

  mafDEL(mesh);
  vtkDEL(Importer);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeMeshTest::CompareImages(int scalarIndex)
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
  vtkDEL(imageMath);
  vtkDEL(rC);
  vtkDEL(rO);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
void mafPipeMeshTest::ProceduralControl(double controlRangeMapper[2],vtkProp *propToControl)
//----------------------------------------------------------------------------
{
  //procedural control

  double sr[2];
  ((vtkActor* )propToControl)->GetMapper()->GetScalarRange(sr);
  CPPUNIT_ASSERT(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1]);
  //end procedural control

}
//----------------------------------------------------------------------------
vtkProp *mafPipeMeshTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
