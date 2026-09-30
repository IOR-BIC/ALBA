/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMapsTest
 Authors: Eleonora Mambrini
 
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
#include "albaVMEMapsTest.h"
#include <iostream>

#include "albaVMEMaps.h"

#include "albaVMEVolumeGray.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"

#include "vtkImageData.h"
#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataSetMapper.h"
#include "vtkSphereSource.h"
#include "albaVMERoot.h"
#include "vtkRectilinearGridReader.h"
#include "vtkDataSetReader.h"

#include <vnl\vnl_matrix.h>


//----------------------------------------------------------------------------
void albaVMEMapsTest::BeforeTest()
//----------------------------------------------------------------------------
{  
  m_Root          = NULL;
  m_Maps          = NULL;
  m_SurfaceToMap  = NULL;
  m_Volume        = NULL;

  polydata   = NULL;
  sphere = NULL;

  m_DensityDistance = 0;
  m_FirstThreshold = 10;
  m_SecondThreshold = 200;
  m_MaxDistance = 2;

  albaNEW(m_Maps);
  albaNEW(m_SurfaceToMap);
  albaNEW(m_Volume);
  albaNEW(m_Root);

  vtkNEW(sphere);
}
//----------------------------------------------------------------------------
void albaVMEMapsTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_Maps);
  albaDEL(m_SurfaceToMap);
  albaDEL(m_Volume);
  albaDEL(m_Root);
  
  vtkDEL(sphere);
}

//---------------------------------------------------------
void albaVMEMapsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaVMEMaps *maps = NULL;
  albaNEW(maps);
  albaDEL(maps);
}
//---------------------------------------------------------
void albaVMEMapsTest::TestPrint()
//---------------------------------------------------------
{
  //only print
  m_Maps->Print(std::cout);
}

//---------------------------------------------------------
void albaVMEMapsTest::CreateVMEMaps()
//---------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();

  m_Volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  m_Volume->ReparentTo(m_Root);
  m_Volume->GetOutput()->Update();
  m_Volume->Update();

  sphere->SetCenter(0,0,0);
  sphere->SetRadius(10);
	sphere->Update();
  polydata = sphere->GetOutput();

  m_SurfaceToMap->SetData(polydata, 0.0);
  m_SurfaceToMap->ReparentTo(m_Root);

  m_Maps->SetSourceVMELink(m_Volume);
  m_Maps->SetMappedVMELink(m_SurfaceToMap);

  m_Maps->SetDensityDistance(m_DensityDistance);
  m_Maps->SetFirstThreshold(m_FirstThreshold);
  m_Maps->SetSecondThreshold(m_SecondThreshold);

  vtkDEL(Importer);
}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetDensityDistance()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_DensityDistance == m_Maps->GetDensityDistance());

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetMaxDistance()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT(m_MaxDistance == m_Maps->GetMaxDistance());

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetFirstThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_FirstThreshold == m_Maps->GetFirstThreshold());

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetSecondThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_SecondThreshold = m_Maps->GetSecondThreshold());

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetMappedVMELink()
//---------------------------------------------------------
{

  CreateVMEMaps();

  albaVME *mappedVME = m_Maps->GetMappedVMELink();

  CPPUNIT_ASSERT(albaVMESurface::SafeDownCast(mappedVME));
  CPPUNIT_ASSERT(mappedVME == m_SurfaceToMap);

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetSourceVMELink()
//---------------------------------------------------------
{
  CreateVMEMaps();

  albaVME *sourceVME = m_Maps->GetSourceVMELink();

  CPPUNIT_ASSERT(albaVMEVolume::SafeDownCast(sourceVME));
  CPPUNIT_ASSERT(sourceVME == m_Volume);

}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetVisualPipe()
//---------------------------------------------------------
{

  CPPUNIT_ASSERT(strcmp(m_Maps->GetVisualPipe(), "albaPipeSurface") == 0);
}

//---------------------------------------------------------
void albaVMEMapsTest::TestDeepCopy()
//---------------------------------------------------------
{
  CreateVMEMaps();

  albaVMEMaps *maps;
  albaNEW(maps);

  maps->DeepCopy(m_Maps);

  CPPUNIT_ASSERT(maps->GetDensityDistance() == m_DensityDistance);
  CPPUNIT_ASSERT(maps->GetFirstThreshold() == m_FirstThreshold);
  CPPUNIT_ASSERT(maps->GetSecondThreshold() == m_SecondThreshold);
  CPPUNIT_ASSERT(maps->GetMaxDistance() == m_MaxDistance);

  albaDEL(maps);

}

//---------------------------------------------------------
void albaVMEMapsTest::TestEquals()
//---------------------------------------------------------
{
  CreateVMEMaps();

  albaVMEMaps *maps;
  albaNEW(maps);

  maps->DeepCopy(m_Maps);

  CPPUNIT_ASSERT(maps->Equals(m_Maps));

  m_Maps->SetSecondThreshold(m_SecondThreshold - 30);

  CPPUNIT_ASSERT(!maps->Equals(m_Maps));

  albaDEL(maps);
}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetVolume()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT( (m_Maps->GetVolume())->Equals(m_Volume) );
}

//---------------------------------------------------------
void albaVMEMapsTest::TestGetSurfaceOutput()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT( albaVMEOutputSurface::SafeDownCast(m_Maps->GetSurfaceOutput()) );

}
