/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointSetTest
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
#include "albaPipePointSetTest.h"
#include "albaPipePointSet.h"

#include "albaSceneNode.h"
#include "albaVMEPointSet.h"
#include "mmaMaterial.h"
#include "vtkALBAAssembly.h"

#include "vtkMapper.h"
#include "vtkPointData.h"

#include "vtkCamera.h"
#include "vtkProp3DCollection.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkDelaunay2D.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>
#include <fstream>
//----------------------------------------------------------------------------
void albaPipePointSetTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipePointSetTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipePointSetTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipePointSetTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  vtkPoints *points = vtkPoints::New();
  points->InsertNextPoint(0.,0.,0.);
  points->InsertNextPoint(1.,0.,0.);
  points->InsertNextPoint(0.,1.,0.);
  points->InsertNextPoint(1.,1.,0.);
  points->InsertNextPoint(0.,0.,1.);
  points->InsertNextPoint(1.,0.,1.);
  points->InsertNextPoint(0.,1.,1.);
  points->InsertNextPoint(1.,1.,1.);

  /*vtkCellArray *cellArray = vtkCellArray::New();
  int index[2];
  index[0] = 0;index[1] = 1;
  cellArray->InsertNextCell(2,index);
  index[0] = 1;index[1] = 2;
  cellArray->InsertNextCell(2,index);
  index[0] = 2;index[1] = 3;
  cellArray->InsertNextCell(2,index);
  index[0] = 3;index[1] = 4;
  cellArray->InsertNextCell(2,index);
  index[0] = 4;index[1] = 5;
  cellArray->InsertNextCell(2,index);
  index[0] = 5;index[1] = 6;
  cellArray->InsertNextCell(2,index);
  index[0] = 6;index[1] = 7;
  cellArray->InsertNextCell(2,index);*/

  vtkPolyData *polyData = vtkPolyData::New();
  polyData->SetPoints(points);
  //polyData->SetLines(cellArray);
  polyData->Update();

  vtkDelaunay2D *delaunay = vtkDelaunay2D::New();
  delaunay->SetInput(polyData);
  delaunay->SetAlpha(5);
  delaunay->SetTolerance(2);
  delaunay->Update();
  
  albaVMEPointSet *pointsetInput;
  albaNEW(pointsetInput);
  pointsetInput->SetData(delaunay->GetOutput(),0);
  pointsetInput->GetOutput()->GetVTKData()->Update();
  pointsetInput->GetOutput()->Update();
	
	//Setting standard material to avoid random color selection
	pointsetInput->GetMaterial()->m_Diffuse[0]=0.3;
	pointsetInput->GetMaterial()->m_Diffuse[1]=0.6;
	pointsetInput->GetMaterial()->m_Diffuse[2]=0.9;
	pointsetInput->GetMaterial()->UpdateProp();
  
	pointsetInput->Update();

  mmaMaterial *material;
  material = pointsetInput->GetMaterial();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,pointsetInput, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipePointSet *pipePointSet = new albaPipePointSet;
  pipePointSet->Create(sceneNode);
  pipePointSet->m_RenFront = m_Renderer;
  
  ////////// ACTORS List ///////////////
  vtkProp3DCollection *actorList = pipePointSet->GetAssemblyFront()->GetParts();

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    ((vtkActor *)actor)->GetProperty()->SetPointSize(5.);
    ((vtkActor *)actor)->GetProperty()->Modified();
    ((vtkActor *)actor)->Modified();
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

	int j=0;
  for(int i=0; i<=30 ; i = i+3, j = j++)
	{
		if(i > 15)
		{
			pipePointSet->Select(true);
		}

    m_Renderer->GetActiveCamera()->Azimuth(i); 
		m_Renderer->GetActiveCamera()->Elevation(j);
		m_RenderWindow->Render();
		printf("\n Visualization: azimut->%d elev->%d  - %s \n", i, j, i>15?"selected":"NOT selected");

		COMPARE_IMAGES("TestPipeExecution", i);
	}

  m_Renderer->RemoveAllProps();

  delete sceneNode;
  albaDEL(pointsetInput);
  vtkDEL(delaunay);
  vtkDEL(polyData);
  //vtkDEL(cellArray);
  vtkDEL(points);
}