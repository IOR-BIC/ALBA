/*=========================================================================

 Program: MAF2
 Module: mafOpCreateVolumeTest
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
#include "mafOpCreateVolumeTest.h"
#include "mafOpCreateVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEOutputVolume.h"

#include "vtkStructuredPoints.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafOpCreateVolume *op = new mafOpCreateVolume();
  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::VolumeParametersTest()
//----------------------------------------------------------------------------
{
  double density = 0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  mafOpCreateVolume *op = new mafOpCreateVolume();
  op->SetVolumeDensity(density);
  result = mafEquals(density, op->GetVolumeDensity());
  TEST_RESULT;

  double spc[3];
  op->SetVolumeSpacing(spacing);
  op->GetVolumeSpacing(spc);

  result = mafEquals(spacing[0], spc[0]);
  TEST_RESULT;
  result = mafEquals(spacing[1], spc[1]);
  TEST_RESULT;
  result = mafEquals(spacing[2], spc[2]);
  TEST_RESULT;

  double dim[3];
  op->SetVolumeDimensions(dimensions);
  op->GetVolumeDimensions(dim);

  result = mafEquals(dimensions[0], dim[0]);
  TEST_RESULT;
  result = mafEquals(dimensions[1], dim[1]);
  TEST_RESULT;
  result = mafEquals(dimensions[2], dim[2]);
  TEST_RESULT;

  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpCreateVolumeTest::VolumeCreatedTest()
//----------------------------------------------------------------------------
{
  double density = 0;
  double spacing[3] = {1.0,1.0,2.0};
  double dimensions[3] = {10.0,10.0,20.0};

  mafOpCreateVolume *op = new mafOpCreateVolume();
  op->SetVolumeDensity(density);
  op->SetVolumeSpacing(spacing);
  op->SetVolumeDimensions(dimensions);
  op->CreateVolume();
  mafVMEVolumeGray *vol = mafVMEVolumeGray::SafeDownCast(op->GetOutput());
  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vol->GetVolumeOutput()->GetVTKData());
  sp->Update();
  
  double spc[3], sr[2], dim[3], b[6];
  sp->GetSpacing(spc);

  result = mafEquals(spacing[0], spc[0]);
  TEST_RESULT;
  result = mafEquals(spacing[1], spc[1]);
  TEST_RESULT;
  result = mafEquals(spacing[2], spc[2]);
  TEST_RESULT;

  sp->GetScalarRange(sr);
  result = mafEquals(sr[0], sr[1]);
  TEST_RESULT;
  result = mafEquals(sr[0], density);
  TEST_RESULT;
  sp->GetBounds(b);
  dim[0] = b[1] - b[0];
  dim[1] = b[3] - b[2];
  dim[2] = b[5] - b[4];

  result = mafEquals(dimensions[0], dim[0]);
  TEST_RESULT;
  result = mafEquals(dimensions[1], dim[1]);
  TEST_RESULT;
  result = mafEquals(dimensions[2], dim[2]);
  TEST_RESULT;

  cppDEL(op);
}
