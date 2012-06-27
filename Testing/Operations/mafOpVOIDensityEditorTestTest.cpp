/*=========================================================================

 Program: MAF2
 Module: mafOpVOIDensityEditorTestTest
 Authors: Paolo Quadrani
 
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
#include "mafOpVOIDensityEditorTestTest.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpCreateVolume.h"

#include "mafVMESurfaceParametric.h"
#include "mafVMEVolumeGray.h"

#include "vtkStructuredPoints.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOpVOIDensityEditorTestTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpVOIDensityEditorTestTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafOpVOIDensityEditorTestTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpVOIDensityEditorTestTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafOpVOIDensityEditor *op = new mafOpVOIDensityEditor();
  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpVOIDensityEditorTestTest::EditVolumeScalarsTest()
//----------------------------------------------------------------------------
{
  double density = 0, fillDensity = 10.0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  mafOpCreateVolume *opVolume = new mafOpCreateVolume();
  opVolume->SetVolumeDensity(density);
  opVolume->SetVolumeSpacing(spacing);
  opVolume->SetVolumeDimensions(dimensions);
  opVolume->CreateVolume();
  mafVMEVolumeGray *vol = mafVMEVolumeGray::SafeDownCast(opVolume->GetOutput());

  double sr[2];
  vtkStructuredPoints *data = vtkStructuredPoints::SafeDownCast(vol->GetOutput()->GetVTKData());
  data->Update();
  data->GetScalarRange(sr);
  result = mafEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = mafEquals(sr[0], density);
  TEST_RESULT;
  
  mafVMESurfaceParametric *surf = NULL;
  mafNEW(surf);
  surf->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
  surf->SetSphereRadius(2.0);

  mafOpVOIDensityEditor *editDensity = new mafOpVOIDensityEditor();
  editDensity->TestModeOn();
  editDensity->SetScalarValue(fillDensity);
  editDensity->SetInput(vol);
  editDensity->SetSurface(surf);
  editDensity->EditVolumeScalars();
  
  data = vtkStructuredPoints::SafeDownCast(vol->GetOutput()->GetVTKData());
  data->Update();
  data->GetScalarRange(sr);
  result = !mafEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = mafEquals(sr[0], density);
  TEST_RESULT;
  result = mafEquals(sr[1], fillDensity);
  TEST_RESULT;

  cppDEL(opVolume);
  mafDEL(surf);
  cppDEL(editDensity);
}
