/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEScalarTest.cpp,v $
Language:  C++
Date:      $Date: 2008-01-24 12:30:22 $
Version:   $Revision: 1.6 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEScalarTest.h"

#include "mafNodeFactory.h"
#include "mafVMEFactory.h"

#include "mafSmartPointer.h"
#include "mafVMEScalar.h"
#include "mafVMEOutputScalar.h"

#include <iostream>

//----------------------------------------------------------------------------
void mafVMEScalarTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::setUp()
//----------------------------------------------------------------------------
{
  // initialized the VME factory
  mafVMEFactory::Initialize();
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::TestVMEScalarFactory()
//----------------------------------------------------------------------------
{
  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalar"));
  scalar->SetData(12.5, 0.0);
  scalar->Delete();
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::TestCopyVMEScalar()
//----------------------------------------------------------------------------
{
  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalar"));
  scalar->SetData(12.5, 0.0);

  mafVMEScalar *scalar_copy = mafVMEScalar::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalar"));
  scalar_copy->DeepCopy(scalar);

  // Test the Equals method
  CPPUNIT_ASSERT(scalar_copy->Equals(scalar));

  scalar->Delete();
  scalar_copy->Delete();
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::TestVMEScalarData()
//----------------------------------------------------------------------------
{
  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalar"));
  scalar->SetData(12.5, 10.5);

  double out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data, 12.5));

  CPPUNIT_ASSERT(!scalar->IsAnimated());

  scalar->Delete();

  mafVMEScalar *scalar2 = mafVMEScalar::New();
  scalar2->SetData(3.2,2.5);
  out_data = scalar2->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data, 3.2));
  scalar2->Delete();

  mafSmartPointer<mafVMEScalar> scalar_sp;
  scalar_sp->SetData(5.2,1.0);
  out_data = scalar_sp->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data, 5.2));
}
//----------------------------------------------------------------------------
void mafVMEScalarTest::TestAnimatedVMEScalar()
//----------------------------------------------------------------------------
{
  double out_data;
  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalar"));
  scalar->SetData(12.5, 0.0);
  scalar->SetData(20.5, 1.0);

  CPPUNIT_ASSERT(scalar->IsAnimated());

  //////////////////////////////////////////////////////////////////////////
  // Test scalar values at different timestamp.
  scalar->SetTimeStamp(0.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data, 12.5));

  scalar->SetTimeStamp(1.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data, 20.5));
  //////////////////////////////////////////////////////////////////////////
  
  scalar->Delete();
}
