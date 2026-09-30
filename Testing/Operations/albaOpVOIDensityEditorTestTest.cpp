/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensityEditorTestTest
 Authors: Paolo Quadrani
 
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
#include "albaOpVOIDensityEditorTestTest.h"
#include "albaOpVOIDensityEditor.h"
#include "albaOpCreateVolume.h"

#include "albaVMESurfaceParametric.h"
#include "albaVMEVolumeGray.h"

#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpVOIDensityEditorTestTest::TestFixture()
//----------------------------------------------------------------------------
{
}


//----------------------------------------------------------------------------
void albaOpVOIDensityEditorTestTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaOpVOIDensityEditor *op = new albaOpVOIDensityEditor();
  cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpVOIDensityEditorTestTest::EditVolumeScalarsTest()
//----------------------------------------------------------------------------
{
  double density = 0, fillDensity = 10.0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  albaOpCreateVolume *opVolume = new albaOpCreateVolume();
  opVolume->SetVolumeDensity(density);
  opVolume->SetVolumeSpacing(spacing);
  opVolume->SetVolumeDimensions(dimensions);
  opVolume->CreateVolume();
  albaVMEVolumeGray *vol = albaVMEVolumeGray::SafeDownCast(opVolume->GetOutput());

  double sr[2];
  vtkImageData *data = vtkImageData::SafeDownCast(vol->GetOutput()->GetVTKData());
  data->GetScalarRange(sr);
  result = albaEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = albaEquals(sr[0], density);
  TEST_RESULT;
  
  albaVMESurfaceParametric *surf = NULL;
  albaNEW(surf);
  surf->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
  surf->SetSphereRadius(2.0);

  albaOpVOIDensityEditor *editDensity = new albaOpVOIDensityEditor();
  editDensity->TestModeOn();
  editDensity->SetScalarValue(fillDensity);
  editDensity->SetInput(vol);
  editDensity->SetSurface(surf);
  editDensity->EditVolumeScalars();
  
  data = vtkImageData::SafeDownCast(vol->GetOutput()->GetVTKData());
  data->GetScalarRange(sr);
  result = !albaEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = albaEquals(sr[0], density);
  TEST_RESULT;
  result = albaEquals(sr[1], fillDensity);
  TEST_RESULT;

  cppDEL(opVolume);
  albaDEL(surf);
  cppDEL(editDensity);
}
