/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVolumeTest
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
#include "albaOpCreateVolumeTest.h"
#include "albaOpCreateVolume.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEOutputVolume.h"

#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOpCreateVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCreateVolumeTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaOpCreateVolume *op = new albaOpCreateVolume();
  cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpCreateVolumeTest::VolumeParametersTest()
//----------------------------------------------------------------------------
{
  double density = 0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  albaOpCreateVolume *op = new albaOpCreateVolume();
  op->SetVolumeDensity(density);
  result = albaEquals(density, op->GetVolumeDensity());
  TEST_RESULT;

  double spc[3];
  op->SetVolumeSpacing(spacing);
  op->GetVolumeSpacing(spc);

  result = albaEquals(spacing[0], spc[0]);
  TEST_RESULT;
  result = albaEquals(spacing[1], spc[1]);
  TEST_RESULT;
  result = albaEquals(spacing[2], spc[2]);
  TEST_RESULT;

  double dim[3];
  op->SetVolumeDimensions(dimensions);
  op->GetVolumeDimensions(dim);

  result = albaEquals(dimensions[0], dim[0]);
  TEST_RESULT;
  result = albaEquals(dimensions[1], dim[1]);
  TEST_RESULT;
  result = albaEquals(dimensions[2], dim[2]);
  TEST_RESULT;

  cppDEL(op);
}

//----------------------------------------------------------------------------
void albaOpCreateVolumeTest::VolumeCreatedTest()
//----------------------------------------------------------------------------
{
  double density = 0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  albaOpCreateVolume *op = new albaOpCreateVolume();
  op->SetVolumeDensity(density);
  op->SetVolumeSpacing(spacing);
  op->SetVolumeDimensions(dimensions);
  op->CreateVolume();
  albaVMEVolumeGray *vol = albaVMEVolumeGray::SafeDownCast(op->GetOutput());
  vtkImageData *sp = vtkImageData::SafeDownCast(vol->GetVolumeOutput()->GetVTKData());
  
  double spc[3], sr[2], dim[3], b[6];
  sp->GetSpacing(spc);

  result = albaEquals(spacing[0], spc[0]);
  TEST_RESULT;
  result = albaEquals(spacing[1], spc[1]);
  TEST_RESULT;
  result = albaEquals(spacing[2], spc[2]);
  TEST_RESULT;

  sp->GetScalarRange(sr);
  result = albaEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = albaEquals(sr[0], density);
  TEST_RESULT;
  sp->GetBounds(b);
  dim[0] = b[1] - b[0];
  dim[1] = b[3] - b[2];
  dim[2] = b[5] - b[4];

  result = albaEquals(dimensions[0], dim[0]);
  TEST_RESULT;
  result = albaEquals(dimensions[1], dim[1]);
  TEST_RESULT;
  result = albaEquals(dimensions[2], dim[2]);
  TEST_RESULT;

  cppDEL(op);
}
