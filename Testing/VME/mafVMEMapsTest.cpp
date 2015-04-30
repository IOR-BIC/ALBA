/*=========================================================================

 Program: MAF2
 Module: mafVMEMapsTest
 Authors: Eleonora Mambrini
 
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
#include "mafVMEMapsTest.h"
#include <iostream>

#include "mafVMEMaps.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"

#include "vtkImageData.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataSetMapper.h"
#include "vtkSphereSource.h"
#include "mafVMERoot.h"
#include "vtkRectilinearGridReader.h"
#include "vtkDataSetReader.h"

#include <vnl\vnl_matrix.h>


//----------------------------------------------------------------------------
void mafVMEMapsTest::setUp()
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

  mafNEW(m_Maps);
  mafNEW(m_SurfaceToMap);
  mafNEW(m_Volume);
  mafNEW(m_Root);

  vtkNEW(sphere);
}
//----------------------------------------------------------------------------
void mafVMEMapsTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_Maps);
  mafDEL(m_SurfaceToMap);
  mafDEL(m_Volume);
  mafDEL(m_Root);
  
  vtkDEL(sphere);
}

//---------------------------------------------------------
void mafVMEMapsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMEMaps *maps = NULL;
  mafNEW(maps);
  mafDEL(maps);
}
//---------------------------------------------------------
void mafVMEMapsTest::TestPrint()
//---------------------------------------------------------
{
  //only print
  m_Maps->Print(std::cout);
}

//---------------------------------------------------------
void mafVMEMapsTest::CreateVMEMaps()
//---------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();

  m_Volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  m_Volume->ReparentTo(m_Root);
  m_Volume->GetOutput()->Update();
  m_Volume->Update();

  sphere->SetCenter(0,0,0);
  sphere->SetRadius(10);
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
void mafVMEMapsTest::TestGetDensityDistance()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_DensityDistance == m_Maps->GetDensityDistance());

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetMaxDistance()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT(m_MaxDistance == m_Maps->GetMaxDistance());

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetFirstThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_FirstThreshold == m_Maps->GetFirstThreshold());

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetSecondThreshold()
//---------------------------------------------------------
{

  CreateVMEMaps();

  CPPUNIT_ASSERT(m_SecondThreshold = m_Maps->GetSecondThreshold());

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetMappedVMELink()
//---------------------------------------------------------
{

  CreateVMEMaps();

  mafNode *mappedVME = m_Maps->GetMappedVMELink();

  CPPUNIT_ASSERT(mafVMESurface::SafeDownCast(mappedVME));
  CPPUNIT_ASSERT(mappedVME == m_SurfaceToMap);

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetSourceVMELink()
//---------------------------------------------------------
{
  CreateVMEMaps();

  mafNode *sourceVME = m_Maps->GetSourceVMELink();

  CPPUNIT_ASSERT(mafVMEVolume::SafeDownCast(sourceVME));
  CPPUNIT_ASSERT(sourceVME == m_Volume);

}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetVisualPipe()
//---------------------------------------------------------
{

  CPPUNIT_ASSERT(strcmp(m_Maps->GetVisualPipe(), "mafPipeSurface") == 0);
}

//---------------------------------------------------------
void mafVMEMapsTest::TestDeepCopy()
//---------------------------------------------------------
{
  CreateVMEMaps();

  mafVMEMaps *maps;
  mafNEW(maps);

  maps->DeepCopy(m_Maps);

  CPPUNIT_ASSERT(maps->GetDensityDistance() == m_DensityDistance);
  CPPUNIT_ASSERT(maps->GetFirstThreshold() == m_FirstThreshold);
  CPPUNIT_ASSERT(maps->GetSecondThreshold() == m_SecondThreshold);
  CPPUNIT_ASSERT(maps->GetMaxDistance() == m_MaxDistance);

  mafDEL(maps);

}

//---------------------------------------------------------
void mafVMEMapsTest::TestEquals()
//---------------------------------------------------------
{
  CreateVMEMaps();

  mafVMEMaps *maps;
  mafNEW(maps);

  maps->DeepCopy(m_Maps);

  CPPUNIT_ASSERT(maps->Equals(m_Maps));

  m_Maps->SetSecondThreshold(m_SecondThreshold - 30);

  CPPUNIT_ASSERT(!maps->Equals(m_Maps));

  mafDEL(maps);
}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetVolume()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT( (m_Maps->GetVolume())->Equals(m_Volume) );
}

//---------------------------------------------------------
void mafVMEMapsTest::TestGetSurfaceOutput()
//---------------------------------------------------------
{
  CreateVMEMaps();

  CPPUNIT_ASSERT( mafVMEOutputSurface::SafeDownCast(m_Maps->GetSurfaceOutput()) );

}
