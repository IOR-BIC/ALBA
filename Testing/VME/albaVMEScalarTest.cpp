/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEScalarTest
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
#include "albaVMEScalarTest.h"

#include "albaVMEFactory.h"

#include "albaSmartPointer.h"
#include "albaVMEScalar.h"
#include "albaVMEOutputScalar.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaVMEScalarTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEScalarTest::BeforeTest()
//----------------------------------------------------------------------------
{
  // initialized the VME factory
  albaVMEFactory::Initialize();
}
//----------------------------------------------------------------------------
void albaVMEScalarTest::TestVMEScalarFactory()
//----------------------------------------------------------------------------
{
  albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(albaVMEFactory::CreateInstance("albaVMEScalar"));
  scalar->SetData(12.5, 0.0);
  scalar->Delete();
}
//----------------------------------------------------------------------------
void albaVMEScalarTest::TestCopyVMEScalar()
//----------------------------------------------------------------------------
{
  albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(albaVMEFactory::CreateInstance("albaVMEScalar"));
  scalar->SetData(12.5, 0.0);

  albaVMEScalar *scalar_copy = albaVMEScalar::SafeDownCast(albaVMEFactory::CreateInstance("albaVMEScalar"));
  scalar_copy->DeepCopy(scalar);

  // Test the Equals method
  CPPUNIT_ASSERT(scalar_copy->Equals(scalar));

  scalar->Delete();
  scalar_copy->Delete();
}
//----------------------------------------------------------------------------
void albaVMEScalarTest::TestVMEScalarData()
//----------------------------------------------------------------------------
{
  albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(albaVMEFactory::CreateInstance("albaVMEScalar"));
  scalar->SetData(12.5, 10.5);

  double out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(albaEquals(out_data, 12.5));

  CPPUNIT_ASSERT(!scalar->IsAnimated());

  scalar->Delete();

  albaVMEScalar *scalar2 = albaVMEScalar::New();
  scalar2->SetData(3.2,2.5);
  out_data = scalar2->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(albaEquals(out_data, 3.2));
  scalar2->Delete();

  albaSmartPointer<albaVMEScalar> scalar_sp;
  scalar_sp->SetData(5.2,1.0);
  out_data = scalar_sp->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(albaEquals(out_data, 5.2));
}
//----------------------------------------------------------------------------
void albaVMEScalarTest::TestAnimatedVMEScalar()
//----------------------------------------------------------------------------
{
  double out_data;
  albaVMEScalar *scalar = albaVMEScalar::SafeDownCast(albaVMEFactory::CreateInstance("albaVMEScalar"));
  scalar->SetData(12.5, 0.0);
  scalar->SetData(20.5, 1.0);

  CPPUNIT_ASSERT(scalar->IsAnimated());

  //////////////////////////////////////////////////////////////////////////
  // Test scalar values at different timestamp.
  scalar->SetTimeStamp(0.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(albaEquals(out_data, 12.5));

  scalar->SetTimeStamp(1.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(albaEquals(out_data, 20.5));
  //////////////////////////////////////////////////////////////////////////
  
  scalar->Delete();
}
