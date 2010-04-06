/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFExtendedGlyph3DTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-06 15:07:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010
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

#include "vtkMAFExtendedGlyph3DTest.h"
#include "mafString.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFExtendedGlyph3D.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGReader.h"
#include "vtkJPEGWriter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkArrowSource.h"

enum ID_TEST_LIST
{
  ID_EXECUTION_TEST = 0,
};

static int TestNumber = 0;

//-----------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::setUp()
//-----------------------------------------------------------
{
  TestNumber++;

  CreatePointsSet();
  CreateWindow();
}
//-----------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::tearDown()
//-----------------------------------------------------------
{
  DeleteWindow();

  m_Points->Delete();

  vtkTimerLog::CleanupLog();
}
//-----------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestFixture()
//-----------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::CompareImages()
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  std::string path(file);
  int slashIndex =  name.find_last_of('\\');


  name = name.substr(slashIndex+1);
  path = path.substr(0,slashIndex);

  int pointIndex =  name.find_last_of('.');

  name = name.substr(0, pointIndex);


  mafString controlOriginFile;
  controlOriginFile<<path.c_str();
  controlOriginFile<<"\\";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<m_TestNumber;
  controlOriginFile<<".jpg";

  fstream controlStream;
  controlStream.open(controlOriginFile.GetCStr()); 

  // visualization control
  m_RenWin->OffScreenRenderingOn();
  vtkWindowToImageFilter *w2i;
  vtkNEW(w2i);
  w2i->SetInput(m_RenWin);
  //w2i->SetMagnification(magnification);
  w2i->Update();
  m_RenWin->OffScreenRenderingOff();

  //write comparing image
  vtkJPEGWriter *w;
  vtkNEW(w);
  w->SetInput(w2i->GetOutput());
  mafString imageFile="";

  if(!controlStream)
  {
    imageFile<<path.c_str();
    imageFile<<"\\";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<path.c_str();
    imageFile<<"\\";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<m_TestNumber;
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
  mafString imageFileOrig="";
  imageFileOrig<<path.c_str();
  imageFileOrig<<"\\";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<m_TestNumber;
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
  //CPPUNIT_ASSERT(ComparingImagesDetailed(imDataOrig,imDataComp));

  // end visualization control
  vtkDEL(rO);
  vtkDEL(rC);
  vtkDEL(imageMath);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter;

  //////////////////////////////////////////////////////////////////////////
  filter = vtkMAFExtendedGlyph3D::New();
  filter->Delete();
  //////////////////////////////////////////////////////////////////////////
  vtkNEW(filter);
  vtkDEL(filter);
  //////////////////////////////////////////////////////////////////////////
  vtkMAFSmartPointer<vtkMAFExtendedGlyph3D> filterSmart;
  //////////////////////////////////////////////////////////////////////////

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetNumberOfSources()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  for(int i=1;i<10;i++)
  {
    filter->SetNumberOfSources(i);

    CPPUNIT_ASSERT( filter->GetNumberOfSources() == i );
  }

  filter->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::CreatePointsSet()
//----------------------------------------------------------------------------
{  
  vtkPoints *pts = vtkPoints::New();
  m_Points = vtkPolyData::New();

  pts->InsertNextPoint(0,0,0);
  pts->InsertNextPoint(0,10,0);
  pts->InsertNextPoint(10,0,0);
//   pts->InsertNextPoint(0,1,0);
//   pts->InsertNextPoint(0,0,1);

  m_Points->SetPoints(pts);
  m_Points->Update();

  pts->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::DeleteWindow()
//----------------------------------------------------------------------------
{
  m_RenWin->Delete();
  m_Renderer->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::CreateWindow()
//----------------------------------------------------------------------------
{
  // create windows
  m_Renderer = vtkRenderer::New();
  m_RenWin= vtkRenderWindow::New();
  m_RenWin->AddRenderer( m_Renderer );

  // prepare for rendering
  m_Renderer->SetBackground(0.05f, 0.05f, 0.05f);
  m_RenWin->SetSize(1024, 768); 

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetSource()
//----------------------------------------------------------------------------
{  
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();
  vtkConeSource *cone = vtkConeSource::New();
  cone->Update();
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->Update();

  filter->SetNumberOfSources(2);
  filter->SetSource(cone->GetOutput());
  filter->SetSource(1,cube->GetOutput());

  CPPUNIT_ASSERT( filter->GetSource() == cone->GetOutput() );
  CPPUNIT_ASSERT( filter->GetSource(1) == cube->GetOutput() );

  filter->Delete();
  cone->Delete();
  cube->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetScaling()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(1.0);
  scalars->InsertNextTuple1(2.0);
  scalars->InsertNextTuple1(3.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->SetScalars(scalars);
  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->ScalingOff();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaling() == FALSE );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] == scalars->GetRange()[0] );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] == scalars->GetRange()[1] );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(filter->GetOutput());
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenWin->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  filter->ScalingOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaling() == TRUE );

  m_RenWin->Render();

  TestNumber++;
  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetScaleFactor()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(1.0);
  scalars->InsertNextTuple1(2.0);
  scalars->InsertNextTuple1(3.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->SetScalars(scalars);
  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->SetScaleFactor(2.0);
  filter->ScalingOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetScaleFactor() == 2.0 );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(filter->GetOutput());
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenWin->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetRange()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");
  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->SetRange(15.0,20.0);
  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 15.0 && filter->GetRange()[1] == 20.0 );

  filter->Delete();
  sphere->Delete();
  scalars->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetGeneratePointIds()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->GeneratePointIdsOn();
  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetPointData()->GetArray("InputPointIds") != NULL );

  filter->Delete();
  sphere->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetPointIdsName()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->GeneratePointIdsOn();
  filter->SetPointIdsName("TEST");
  filter->Update();

  CPPUNIT_ASSERT( filter->GetOutput()->GetPointData()->GetArray("TEST") != NULL );

  filter->Delete();
  sphere->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetScalarVisibility()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(0.0);//0 value make the point not visible
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");
  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->ScalarVisibilityOn();
  filter->Update();  

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(filter->GetOutput());
  mapper->SetScalarVisibility(TRUE);
  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(filter->GetOutput()->GetScalarRange()[0],filter->GetOutput()->GetScalarRange()[1]);
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenWin->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetOrient()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkArrowSource *arrow = vtkArrowSource::New();
  arrow->Update();

  vtkDoubleArray *normals = vtkDoubleArray::New();
  normals->SetNumberOfComponents(3);
  normals->InsertNextTuple3(1,0,0);
  normals->InsertNextTuple3(0,1,0);
  normals->InsertNextTuple3(0,0,1);
  normals->SetName("NORMALS");

  m_Points->GetPointData()->AddArray(normals);
  m_Points->GetPointData()->SetActiveNormals("NORMALS");
  m_Points->Update();

  vtkDataArray *n = m_Points->GetPointData()->GetNormals();

  filter->SetSource(arrow->GetOutput());
  filter->SetInput(m_Points);
  filter->SetVectorModeToUseNormal();
  filter->OrientOn();
  filter->Update();  

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(filter->GetOutput());
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenWin->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  arrow->Delete();
  normals->Delete();

}
//----------------------------------------------------------------------------
void vtkMAFExtendedGlyph3DTest::TestSetClamping()
//----------------------------------------------------------------------------
{
  vtkMAFExtendedGlyph3D *filter = vtkMAFExtendedGlyph3D::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(15.0);
  scalars->InsertNextTuple1(20.0);
  scalars->SetName("SCALARS");

  m_Points->GetPointData()->AddArray(scalars);
  m_Points->GetPointData()->SetActiveScalars("SCALARS");
  m_Points->Update();

  filter->SetSource(sphere->GetOutput());
  filter->SetInput(m_Points);
  filter->SetRange(15.0,20.0);
  filter->ClampingOn();//Clamping normalize to [0,1] the scale values
  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 15.0 && filter->GetRange()[1] == 20.0 );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] >= 0.0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] <= 1.0 );

  // create pipe
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(filter->GetOutput());
  mapper->SetScalarVisibility(TRUE);
  mapper->SetScalarModeToUsePointData();
  mapper->SetScalarRange(filter->GetOutput()->GetScalarRange()[0],filter->GetOutput()->GetScalarRange()[1]);
  mapper->Update();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  m_Renderer->ResetCamera(filter->GetOutput()->GetBounds());
  m_RenWin->Render();

  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  filter->SetRange(12.0,20.0);

  filter->Update();  

  CPPUNIT_ASSERT( filter->GetRange()[0] == 12.0 && filter->GetRange()[1] == 20.0 );

  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[0] >= 0.0 );
  CPPUNIT_ASSERT( filter->GetOutput()->GetScalarRange()[1] <= 1.0 );

  m_RenWin->Render();

  TestNumber++;
  m_TestNumber = ID_EXECUTION_TEST + TestNumber;
  CompareImages();
  mafSleep(2000);

  actor->Delete();
  mapper->Delete();

  filter->Delete();
  sphere->Delete();
  scalars->Delete();

}
