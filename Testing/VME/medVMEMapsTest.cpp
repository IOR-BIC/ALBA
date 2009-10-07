/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMapsTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-07 15:17:59 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medVMEMapsTest.h"
#include <iostream>

#include "medVMEMaps.h"

#include "mafVMEVolumeGray.h"

#include "vtkActor.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkRenderer.h";
#include "vtkRenderWindow.h""
#include "vtkRenderWindowInteractor.h";
#include "vtkDataSetMapper.h";
#include "vtkSphereSource.h"

#include <vnl\vnl_matrix.h>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void medVMEMapsTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
  
  m_Maps          = NULL;
  m_SurfaceToMap  = NULL;
  m_Volume        = NULL;

  m_DensityDistance = 0;
  m_FirstThreshold = 10;
  m_SecondThreshold = 200;
  m_MaxDistance = 2;
}
//----------------------------------------------------------------------------
void medVMEMapsTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------
void medVMEMapsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafNEW(m_Maps);
  mafDEL(m_Maps);
}
//---------------------------------------------------------
void medVMEMapsTest::TestPrint()
//---------------------------------------------------------
{
  medVMEMaps *maps = NULL;
  mafNEW(maps);
  //only print
  maps->Print(std::cout);
  mafDEL(maps);
}

//---------------------------------------------------------
void medVMEMapsTest::TestGetLocalTimeStamps()
//---------------------------------------------------------
{
  //actually this method invokes TestGetLocalTimeBounds, so it determines the same results
  
  /*medVMEMaps *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 5;
  int nTimeStamps = 3;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  std::vector<double> timeVector;
  analog->GetLocalTimeStamps(timeVector);

  //control time vector
  m_Result = timeVector.size() == 3 &&\
    timeVector[0] == 0 &&\
    timeVector[1] == 1 &&\
    timeVector[2] == 2;

  TEST_RESULT;

  mafDEL(analog);*/
}

//---------------------------------------------------------
void medVMEMapsTest::CreateVMEMaps()
//---------------------------------------------------------
{
  vtkPolyData       *data   = NULL;
  vtkSphereSource   *sphere = NULL;
  
  vtkNEW(data);
  vtkNEW(sphere);
  sphere->SetCenter(0,0,0);
  sphere->SetRadius(10);
  data = sphere->GetOutput();
  
  mafNEW(m_SurfaceToMap);
  m_SurfaceToMap->SetData(data, 0.0);

  CreateVMEVolume();

  mafNEW(m_Maps);
  m_Maps->SetSourceVMELink(m_Volume);
  m_Maps->SetMappedVMELink(m_SurfaceToMap);

  m_Maps->SetDensityDistance(m_DensityDistance);
  m_Maps->SetFirstThreshold(m_FirstThreshold);
  m_Maps->SetSecondThreshold(m_SecondThreshold);

}

//---------------------------------------------------------
void medVMEMapsTest::CreateVMEVolume()
//---------------------------------------------------------
{

  // create some VTK data
  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  vtkMAFSmartPointer<vtkFloatArray> scalars;
  scalars->SetNumberOfComponents(1);
  for (int s = 0; s < 25; s++)
  {
    scalars->InsertNextValue(s * 1.0);
  }

  // create structured data
  vtkImageData *imageData = vtkImageData::New();
  imageData->SetDimensions(5, 5, 5);
  imageData->SetOrigin(-1, -1, -1);
  imageData->SetSpacing(1,1,1);

  // create rectilinear grid data
  vtkFloatArray *xCoordinates = vtkFloatArray::New();
  xCoordinates->SetNumberOfValues(5);
  xCoordinates->SetValue(0, 0.f);
  xCoordinates->SetValue(1, 1.f);
  xCoordinates->SetValue(2, 2.f);
  xCoordinates->SetValue(3, 1.f);
  xCoordinates->SetValue(4, 0.f); 

  vtkFloatArray *yCoordinates = vtkFloatArray::New();
  yCoordinates->SetNumberOfValues(5);
  yCoordinates->SetValue(0, 0.f);
  yCoordinates->SetValue(1, 1.f);
  yCoordinates->SetValue(2, 2.f);
  yCoordinates->SetValue(3, 1.f);
  yCoordinates->SetValue(4, 0.f); 

  vtkFloatArray *zCoordinates = vtkFloatArray::New();
  zCoordinates->SetNumberOfValues(5);
  zCoordinates->SetValue(0, 0.f);
  zCoordinates->SetValue(1, 1.f);
  zCoordinates->SetValue(2, 2.f);
  zCoordinates->SetValue(3, 1.f);
  zCoordinates->SetValue(4, 0.f); 

  rectilinearGrid->SetDimensions(5,5,5);
  rectilinearGrid->SetXCoordinates(xCoordinates);
  rectilinearGrid->SetYCoordinates(yCoordinates);
  rectilinearGrid->SetZCoordinates(zCoordinates);

  // create vme volume
  mafNEW(m_Volume);

  // try to set this data to the volume
  int returnValue = -1;

  imageData->GetPointData()->SetScalars(scalars);
  m_Volume->SetData(imageData, 0);

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetDensityDistance()
//---------------------------------------------------------ù
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_DensityDistance == m_Maps->GetDensityDistance());

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetMaxDistance()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT(m_MaxDistance == m_Maps->GetMaxDistance());

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetFirstThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_FirstThreshold == m_Maps->GetFirstThreshold());

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetSecondThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_SecondThreshold = m_Maps->GetSecondThreshold());

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetMappedVMELink()
//---------------------------------------------------------
{

  CreateVMEMaps();

  mafNode *mappedVME = m_Maps->GetMappedVMELink();

  CPPUNIT_ASSERT(mafVMESurface::SafeDownCast(mappedVME));
  CPPUNIT_ASSERT(mappedVME == m_SurfaceToMap);

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetSourceVMELink()
//---------------------------------------------------------
{
  CreateVMEMaps();

  mafNode *sourceVME = m_Maps->GetSourceVMELink();

  CPPUNIT_ASSERT(mafVMEVolumeGray::SafeDownCast(sourceVME));
  CPPUNIT_ASSERT(sourceVME == m_Volume);

}

//---------------------------------------------------------
void medVMEMapsTest::TestGetVisualPipe()
//---------------------------------------------------------
{
  mafNEW(m_Maps);

  CPPUNIT_ASSERT(strcmp(m_Maps->GetVisualPipe(), "mafPipeSurface") == 0);
}