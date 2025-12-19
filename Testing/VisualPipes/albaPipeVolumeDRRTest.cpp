/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeDRRTest
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
#include "albaPipeVolumeDRRTest.h"
#include "albaPipeVolumeDRR.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>
#include "vtkImageData.h"

enum TESTS_PIPE_SURFACE
{
  BASE_TEST,
  COLOR_TEST,
  EXPOSURE_TEST,
  GAMMA_TEST,
  CAMERA_ANGLE_TEST,
  CAMERA_POSITION_TEST,
  CAMERA_FOCUS_TEST,
  CAMERA_ROLL_TEST,
  RESAMPLE_FACTOR_TEST,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipeVolumeDRRTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRRTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRRTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVolumeDRRTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeVolumeDRR *pipeDRR = new albaPipeVolumeDRR;
  pipeDRR->Create(sceneNode);
  pipeDRR->m_RenFront = m_Renderer;
  pipeDRR->SetGamma(2.2);
  double position[3] = {70.,111,213};
  double focus[3] = {70.,71., 51.};
  pipeDRR->SetCameraPosition(position);
  pipeDRR->SetCameraFocus(focus);
  pipeDRR->SetCameraAngle(20);
  pipeDRR->SetCameraRoll(0);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeDRR->GetAssemblyFront()->GetVolumes(actorList);

  const char *strings[NUMBER_OF_TEST];
  strings[0] = "BASE_TEST";
  strings[1] = "COLOR_TEST";
  strings[2] = "EXPOSURE_TEST";
  strings[3] = "GAMMA_TEST";
  strings[4] = "CAMERA_ANGLE_TEST";
  strings[5] = "CAMERA_POSITION_TEST";
  strings[6] = "CAMERA_FOCUS_TEST";
  strings[7] = "CAMERA_ROLL_TEST";
  strings[8] = "RESAMPLE_FACTOR_TEST";

  for (int i = 0; i < NUMBER_OF_TEST; i++)
  {
    if (i == BASE_TEST)
    {
    }
    else if (i == COLOR_TEST)
    {
      pipeDRR->SetColor(wxColour(200, 0, 0));
    }
    else if (i == EXPOSURE_TEST)
    {
      double value[2] = { -0.6,-0.5 };
      pipeDRR->SetExposureCorrection(value);
    }
    else if (i == GAMMA_TEST)
    {
      pipeDRR->SetGamma(0.1);
    }
    else if (i == CAMERA_ANGLE_TEST)
    {
      pipeDRR->SetCameraAngle(10);
    }
    else if (i == CAMERA_POSITION_TEST)
    {
      double newPosition[3] = { 40,40,100 };
      pipeDRR->SetCameraPosition(newPosition);
    }
    else if (i == CAMERA_FOCUS_TEST)
    {
      double newFocus[3] = { 70,71,31 };
    }
    else if (i == CAMERA_ROLL_TEST)
    {
      pipeDRR->SetCameraRoll(20);
    }
    else if (i == RESAMPLE_FACTOR_TEST)
    {
      pipeDRR->SetResampleFactor(0.5);
    }

		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		while (actor)
		{
			m_Renderer->AddVolume(actor);
			actor = actorList->GetNextProp();
		}

    vtkVolume *volumeActor;
    volumeActor = (vtkVolume *) SelectActorToControl(actorList, 0);
    CPPUNIT_ASSERT(volumeActor != NULL);

		printf("\n Visualization: %s \n", strings[i]);

		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
    COMPARE_IMAGES("TestPipeExecution", i);

    if(i == COLOR_TEST)
    {
      pipeDRR->SetColor(wxColour(128,128,128));
    }
    else if(i == EXPOSURE_TEST)
    {
      double value[2] = {0,0};
      pipeDRR->SetExposureCorrection(value);
    }
    else if(i == GAMMA_TEST)
    {
      pipeDRR->SetGamma(2.2);
    }
    else if(i == CAMERA_ANGLE_TEST)
    {
      pipeDRR->SetCameraAngle(20);
    }
    else if(i == CAMERA_POSITION_TEST)
    {
      pipeDRR->SetCameraPosition(position);
    }
    else if(i == CAMERA_FOCUS_TEST)
    {
      pipeDRR->SetCameraFocus(focus);
    }
    else if(i == CAMERA_ROLL_TEST)
    {
      pipeDRR->SetCameraRoll(0);
    }
    else if(i == RESAMPLE_FACTOR_TEST)
    {
      pipeDRR->SetResampleFactor(1.);
    }
	}
	
  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(volume);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeVolumeDRRTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
