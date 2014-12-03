/*=========================================================================

 Program: MAF2
 Module: mafOpMeshQualityTest
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

#include "mafOpMeshQualityTest.h"
#include "mafOpMeshQuality.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTriangleQualityRatio.h"
#include "vtkPolyDataReader.h"
#include "vtkMAFSmartPointer.h"

#include <string>
#include <assert.h>


class MeshQualityDummy: public mafOpMeshQuality
{
public:
  
  double GetMeanRatio();
  double GetMaxRatio();
  double GetMinRatio();

  void OpExecute();
};

double MeshQualityDummy::GetMeanRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMeanRatio();
}

double MeshQualityDummy::GetMaxRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMaxRatio();
}

double MeshQualityDummy::GetMinRatio()
{
  assert(m_CheckMeshQuality);

  return m_CheckMeshQuality->GetMinRatio();
}

void MeshQualityDummy::OpExecute()
{
  vtkNEW(m_CheckMeshQuality);
  TestModeOn();

  CreatePolydataPipeline();
}

//-----------------------------------------------------------
void mafOpMeshQualityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpMeshQuality *meshQuality = new mafOpMeshQuality();
  mafDEL(meshQuality);
}
//-----------------------------------------------------------
void mafOpMeshQualityTest::TestOpRun() 
//-----------------------------------------------------------
{
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Surface/Sphere.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface>surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();
  CPPUNIT_ASSERT(surface);

  surface->Update();
  surface->GetOutput()->GetVTKData()->Update();
  vtkPolyData *data=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  CPPUNIT_ASSERT(data);

  data->Update();

  vtkTriangleQualityRatio *triangleQualityRatio;
  vtkNEW(triangleQualityRatio);
  triangleQualityRatio->SetInput(data);
  triangleQualityRatio->Update(); 

  MeshQualityDummy *meshQuality = new MeshQualityDummy();
  meshQuality->SetInput(surface);
  meshQuality->OpExecute();

  CPPUNIT_ASSERT(meshQuality->GetMaxRatio() == triangleQualityRatio->GetMaxRatio());
  CPPUNIT_ASSERT(meshQuality->GetMinRatio() == triangleQualityRatio->GetMinRatio());
  CPPUNIT_ASSERT(meshQuality->GetMeanRatio() == triangleQualityRatio->GetMeanRatio());

  cppDEL(meshQuality);
  vtkDEL(triangleQualityRatio);
  //mafDEL(storage);

}
