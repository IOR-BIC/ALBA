/*=========================================================================

 Program: MAF2
 Module: mafPipePolylineGraphEditorTest
 Authors: Matteo Giacomoni
 
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
#include "mafPipePolylineGraphEditorTest.h"
#include "mafPipePolylineGraphEditor.h"
#include "mafVMEPolylineEditor.h"

#include "mmaMaterial.h"
#include "mafSceneNode.h"

#include "mafVMERoot.h"

#include "vtkMAFSmartPointer.h"
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
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkSphereSource.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>

enum TESTS_PIPE_POLYLINE_GRAPH_EDITOR
{
  BASE_TEST,
  SELECT_TEST,
  CUT_TEST,
  NUMBER_OF_TEST,
};



//----------------------------------------------------------------------------
void mafPipePolylineGraphEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipePolylineGraphEditorTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Polydata);
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineGraphEditorTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Polydata);
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineGraphEditorTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME ////////////////////

  CreateExamplePolydata();

  vtkMAFSmartPointer<vtkDoubleArray> scalars;
  scalars->SetName("SCALARS");

  int nPoints = m_Polydata->GetNumberOfPoints();
  for (int i=0;i<nPoints;i++)
  {
    scalars->InsertNextTuple1(0.0);
  }

  m_Polydata->GetPointData()->AddArray(scalars);
  m_Polydata->GetPointData()->SetActiveScalars("SCALARS");
  m_Polydata->Update();

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(0.5);
  sphere->Update();
  vtkMAFSmartPointer<vtkGlyph3D> glyph;
  glyph->SetInput(m_Polydata);
  glyph->SetSource(sphere->GetOutput());
  glyph->SetScaleModeToDataScalingOff();//without this with a scalar value equal to 0 the sphere disappears
  glyph->Update();

  vtkMAFSmartPointer<vtkTubeFilter> tube;
  tube->SetInput(m_Polydata);
  tube->SetRadius(0.2);
  tube->Update();

  int n = glyph->GetOutput()->GetNumberOfPoints();
  vtkMAFSmartPointer<vtkAppendPolyData> append;
  append->SetInput(glyph->GetOutput());
  append->AddInput(tube->GetOutput());
  append->Update();

  mafSmartPointer<mafVMEPolylineEditor> surface;
  surface->SetData(append->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipePolylineGraphEditor *pipe = new mafPipePolylineGraphEditor;
  pipe->Create(sceneNode);
  pipe->m_RenFront = m_Renderer;

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  const char *strings[NUMBER_OF_TEST];
  strings[0] = "BASE_TEST";
  strings[1] = "SELECT_TEST";
  strings[2] = "CUT_TEST";

  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    if(i == BASE_TEST)
    {
      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == SELECT_TEST)
    {

      vtkMAFSmartPointer<vtkDoubleArray> scalars;
      scalars->SetName("SCALARS");

      int nPoints = m_Polydata->GetNumberOfPoints();
      for (int i=0;i<nPoints;i++)
      {

        if (i == 0)//Select the 0th point
        {
          scalars->InsertNextTuple1((double)1.0);
        }
        else
        {
          scalars->InsertNextTuple1((double)0.0);
        }

      }

      m_Polydata->GetPointData()->AddArray(scalars);
      m_Polydata->GetPointData()->SetActiveScalars("SCALARS");
      m_Polydata->Update();

      vtkMAFSmartPointer<vtkSphereSource> sphere;
      sphere->SetRadius(0.5);
      sphere->Update();
      vtkMAFSmartPointer<vtkGlyph3D> glyph;
      glyph->SetInput(m_Polydata);
      glyph->SetSource(sphere->GetOutput());
      glyph->SetScaleModeToDataScalingOff();//without this with a scalar value equal to 0 the sphere disappears
      glyph->Update();

      surface->SetData(glyph->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == CUT_TEST)
    {

      vtkMAFSmartPointer<vtkDoubleArray> scalars;
      scalars->SetName("SCALARS");

      int nPoints = m_Polydata->GetNumberOfPoints();
      for (int i=0;i<nPoints;i++)
      {
        scalars->InsertNextTuple1((double)0.0);
      }

      m_Polydata->GetPointData()->AddArray(scalars);
      m_Polydata->GetPointData()->SetActiveScalars("SCALARS");
      m_Polydata->Update();

      vtkMAFSmartPointer<vtkTubeFilter> tube;
      tube->SetInput(m_Polydata);
      tube->SetRadius(0.5);
      tube->Update();

      surface->SetData(tube->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

      double origin[3] = {5,2,0};
      double normal[3] = {0,0,1};
      pipe->SetSlice(origin,normal);
      pipe->SetModalitySlice();

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }

    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, 0);
    CPPUNIT_ASSERT(surfaceActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();
    printf("\n Visualizzazione: %s \n", strings[i]);
    mafSleep(800);
    CompareImages(i);

  }


  vtkDEL(actorList);

  delete pipe;
  delete sceneNode;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipePolylineGraphEditorTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipePolylineGraphEditor/";
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
    imageFile<<"/Test_PipePolylineGraphEditor/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipePolylineGraphEditor/";
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
  imageFileOrig<<"/Test_PipePolylineGraphEditor/";
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
vtkProp *mafPipePolylineGraphEditorTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
//------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

                        17
                        |
                        16
                        |
                        15 - 18 - 19 - 20 - 21
                        |
                        14
   2                    |
  /                     13
 /                      |                        
1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
|   |\
|   | \
0   5  4

*/  
void mafPipePolylineGraphEditorTest::CreateExamplePolydata()
//------------------------------------------------------------------------------
{
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;

  int i ;

  // coordinates of vertices
  static double vertices[22][3] ={
    {0,0,0},
    {1,2,0},
    {2,4,0},
    {2,1,0},
    {3,1,0},
    {3,0,0},

    {3,2,0},
    {4,1,0},
    {5,2,0},
    {6,1,0},
    {7,2,0},
    {8,1,0},
    {9,2,0},

    {7,4,0},
    {9,6,0},
    {10,8,0},
    {13,10,0},
    {14,12,0},

    {11,9,0},
    {12,8,0},
    {13,8,0},
    {16,10,0}
  } ;

  // indices of simple lines and polylines
  static vtkIdType lineids[7][10] = {
    {0,1},
    {1,2},
    {3,4},
    {3,5},
    {1, 3, 6, 7, 8, 9, 10, 11, 12},
    {10, 13, 14, 15, 16, 17},
    {15, 18, 19, 20, 21}
  };

  // insert points
  for (i = 0 ;  i < 22 ;  i++)
    points->InsertNextPoint(vertices[i]) ;

  // insert lines and polylines
  lines->InsertNextCell(2, lineids[0]) ;
  lines->InsertNextCell(2, lineids[1]) ;
  lines->InsertNextCell(2, lineids[2]) ;
  lines->InsertNextCell(2, lineids[3]) ;
  lines->InsertNextCell(9, lineids[4]) ;
  lines->InsertNextCell(6, lineids[5]) ;
  lines->InsertNextCell(5, lineids[6]) ;

  // put points and cells in polydata
  m_Polydata->SetPoints(points) ;
  m_Polydata->SetLines(lines) ;

  points->Delete() ;
  lines->Delete() ;

}
