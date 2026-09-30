/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMeshQualityTest
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

#include "albaOpMeshQualityTest.h"
#include "albaOpMeshQuality.h"

#include "albaString.h"
#include "albaVMESurface.h"

#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTriangleQualityRatio.h"
#include "vtkPolyDataReader.h"
#include "vtkALBASmartPointer.h"

#include <string>
#include <assert.h>


class MeshQualityDummy: public albaOpMeshQuality
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
void albaOpMeshQualityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpMeshQuality *meshQuality = new albaOpMeshQuality();
  albaDEL(meshQuality);
}
//-----------------------------------------------------------
void albaOpMeshQualityTest::TestOpRun() 
//-----------------------------------------------------------
{
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Surface/Sphere.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface>surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->Update();
  CPPUNIT_ASSERT(surface);

  surface->Update();
  vtkPolyData *data=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  CPPUNIT_ASSERT(data);


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
  //albaDEL(storage);

}
