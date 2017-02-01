/*=========================================================================

 Program: MAF2
 Module: mafPipeScalarMatrixTest
 Authors: Alberto Losi
 
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
#include "mafPipeScalarMatrixTest.h"
#include "mafPipeScalarMatrix.h"

#include "mafSceneNode.h"
#include "mafVMEScalarMatrix.h"
#include "mafVMERoot.h"

#include "vtkMAFAssembly.h"
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
void mafPipeScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeScalarMatrixTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(400, 400);
	m_RenderWindow->SetPosition(400, 0);
	m_Renderer->GetActiveCamera()->SetPosition(1, 0, 0);
}
//----------------------------------------------------------------------------
void mafPipeScalarMatrixTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipeScalarMatrixTest::TestPipeExecution()
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

  mafVMEScalarMatrix *vme;
  mafNEW(vme);
  vme->SetData(matrix,0.0);
  vme->GetOutput()->Update();
  vme->Update();
  
  mafVMERoot *root;
  mafNEW(root);
  
  mafSceneNode *sceneRootNode;
  sceneRootNode = new mafSceneNode(NULL,NULL,root,m_Renderer);

  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,sceneRootNode,vme,m_Renderer);

  // Create the pipe and render the actors
  mafPipeScalarMatrix *pipe = new mafPipeScalarMatrix();
  pipe->Create(sceneNode);
  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution");

  delete sceneNode;
  delete sceneRootNode;

  mafDEL(vme);
  mafDEL(root);
}