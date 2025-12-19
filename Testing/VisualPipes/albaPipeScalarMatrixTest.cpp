/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeScalarMatrixTest
 Authors: Alberto Losi
 
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
#include "albaPipeScalarMatrixTest.h"
#include "albaPipeScalarMatrix.h"

#include "albaSceneNode.h"
#include "albaVMEScalarMatrix.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkCamera.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkTimerLog.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaPipeScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeScalarMatrixTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	m_Renderer->GetActiveCamera()->SetPosition(1, 0, 0);
}
//----------------------------------------------------------------------------
void albaPipeScalarMatrixTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeScalarMatrixTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  vtkTimerLog::SetLogging(0); // Must shutdown logging otherwise it will generate leaks

  // Create the scalar matrix
  vnl_matrix<double> matrix;
  matrix.set_size(10,10);
  for(int r = 0; r < 10; r++)
  {
    for(int c = 0; c < 10; c++)
    {
      if(r == 0)
        matrix.put(r,c,c);
      else if(c == 0)
        matrix.put(r,c,r);
      else
        matrix.put(r,c,c + r);
    }
  }

  albaVMEScalarMatrix *vme;
  albaNEW(vme);
  vme->SetData(matrix,0.0);
  vme->GetOutput()->Update();
  vme->Update();
  
  albaVMERoot *root;
  albaNEW(root);
  
  albaSceneNode *sceneRootNode;
  sceneRootNode = new albaSceneNode(NULL,NULL,root,m_Renderer);

  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,sceneRootNode,vme,m_Renderer);

  // Create the pipe and render the actors
  albaPipeScalarMatrix *pipe = new albaPipeScalarMatrix();
  pipe->Create(sceneNode);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution");

  delete sceneNode;
  delete sceneRootNode;

  albaDEL(vme);
  albaDEL(root);
}