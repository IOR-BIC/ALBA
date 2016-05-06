/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceTexturedTest
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
#include "mafPipeSurfaceTexturedTest.h"
#include "mafPipeSurfaceTextured.h"

#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"

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
#include "vtkPolyData.h"
#include "vtkJPEGReader.h"
#include "vtkDoubleArray.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkSphereSource.h"
#include <iostream>

enum PIPE_SURFACE_ACTORS
  {
    PIPE_SURFACE_ACTOR,
    PIPE_SURFACE_ACTOR_WIRED,
    PIPE_SURFACE_ACTOR_OUTLINE_CORNER,
    PIPE_SURFACE_NUMBER_OF_ACTORS,
  };

enum TESTS_PIPE_SURFACE
{
  USE_TEXTURE_PLANE_MAPPING,
  USE_TEXTURE_CYLINDER_MAPPING,
  USE_TEXTURE_SPHERE_MAPPING,
  NUMBER_OF_TEST_TEXTURES,
	SCALAR_TEST,
  VTK_PROPERTY_TEST,
  NUMBER_OF_TEST_CLASSIC,
};


//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::TestPipeTextureExecution()
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
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	mafString filenameImage=MAF_DATA_ROOT;
	filenameImage<<"/Test_PipeSurfaceTextured/texture1.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

  mafVMESurface *surface;
  mafNEW(surface);

	vtkCubeSource *cube = vtkCubeSource::New();
	cube->Update();
	cube->GetOutput()->Update();

  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetResolution(64);
  cylinder->Update();
  cylinder->GetOutput()->Update();


  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(64);
  sphere->SetPhiResolution(64);
  sphere->Update();
  sphere->GetOutput()->Update();

  
  surface->SetData((vtkPolyData*)cube->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
	surface->GetMaterial()->SetMaterialTexture((vtkImageData*)ImporterImage->GetOutput());
  surface->Update();

  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceTextured *pipeSurface = new mafPipeSurfaceTextured;
  pipeSurface->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSurface->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }
	
	const char *strings[NUMBER_OF_TEST_TEXTURES];
	
	strings[0] = "USE_TEXTURE_PLANE_MAPPING";
	strings[1] = "USE_TEXTURE_CYLINDER_MAPPING";
	strings[2] = "USE_TEXTURE_SPHERE_MAPPING";
  
  for(int i=0;i<NUMBER_OF_TEST_TEXTURES;i++)
  {
    if(i == USE_TEXTURE_PLANE_MAPPING)
    {
      surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;
      pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
    else if(i == USE_TEXTURE_CYLINDER_MAPPING)
    {
      surface->SetData((vtkPolyData*)cylinder->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

			surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::CYLINDER_MAPPING;
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
    else if(i == USE_TEXTURE_SPHERE_MAPPING)
    {
      surface->SetData((vtkPolyData*)sphere->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

			surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::SPHERE_MAPPING;
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
		
		
    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

    m_RenderWindow->Render();
	  printf("\n Visualizzazione: %s \n", strings[i]);
    CompareImages(i);
  }

  vtkDEL(actorList);

  delete sceneNode;

  mafDEL(surface);
  vtkDEL(cube);
  vtkDEL(cylinder);
  vtkDEL(sphere);
	vtkDEL(ImporterImage);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::TestPipeClassicExecution()
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
	filename<<"/Test_PipeSurfaceTextured/surface0.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	mafVMESurface *surface;
	mafNEW(surface);
	surface->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
	surface->GetOutput()->Update();
	surface->GetMaterial();
	
	//Setting standard material to avoid random color selection
	surface->GetMaterial()->m_Diffuse[0]=0.3;
	surface->GetMaterial()->m_Diffuse[1]=0.6;
	surface->GetMaterial()->m_Diffuse[2]=0.9;
	surface->GetMaterial()->UpdateProp();

	surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	surface->Update();

	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *sceneNode;
	sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);

	/////////// Pipe Instance and Creation ///////////
	mafPipeSurfaceTextured *pipeSurface = new mafPipeSurfaceTextured;
	pipeSurface->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeSurface->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while(actor)
	{   
		m_Renderer->AddActor(actor);
		m_RenderWindow->Render();

		actor = actorList->GetNextProp();
	}

	const char *strings[2];
	strings[0] = "SCALAR_TEST";
	strings[1] = "VTK_PRPOERTY_TEST";

	for(int i=SCALAR_TEST;i<NUMBER_OF_TEST_CLASSIC;i++)
	{
		if(i == SCALAR_TEST)
		{
			pipeSurface->SetScalarVisibilityOn();
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_SCALAR_VISIBILITY));
		}
		else if(i == VTK_PROPERTY_TEST)
		{
			pipeSurface->SetUseVtkPropertyOn();
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_USE_VTK_PROPERTY));
		}

		vtkActor *surfaceActor;
		surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
		CPPUNIT_ASSERT(surfaceActor != NULL);

		m_RenderWindow->Render();
		printf("\n Visualizzazione: %s \n", strings[i-SCALAR_TEST]);
		CompareImages(i);

		if(i == SCALAR_TEST)
		{
			pipeSurface->SetScalarVisibilityOff();
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_SCALAR_VISIBILITY));
		}
		else if(i == VTK_PROPERTY_TEST)
		{
			pipeSurface->SetUseVtkPropertyOff();
			pipeSurface->OnEvent(&mafEvent(this, mafPipeSurfaceTextured::ID_USE_VTK_PROPERTY));
		}
	}

	vtkDEL(actorList);

	delete sceneNode;

	mafDEL(surface);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTexturedTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeSurfaceTextured/";
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
    imageFile<<"/Test_PipeSurfaceTextured/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeSurfaceTextured/";
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
  imageFileOrig<<"/Test_PipeSurfaceTextured/";
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
vtkProp *mafPipeSurfaceTexturedTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
