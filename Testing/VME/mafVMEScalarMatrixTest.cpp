/*=========================================================================

 Program: MAF2
 Module: mafVMEScalarMatrixTest
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
#include "mafVMEScalarMatrixTest.h"

#include "mafVMEFactory.h"
#include "mafVMEScalarMatrix.h"
#include "mafVMEOutputScalarMatrix.h"

#include <iostream>

//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::setUp()
//----------------------------------------------------------------------------
{
  in_data.set_size(10,20);
  in_data.fill(1.0);

  // initialized the VME factory
  mafVMEFactory::Initialize();
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestVMEScalarFactory()
//----------------------------------------------------------------------------
{
  mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));
  scalar->SetData(in_data,0.0);
  scalar->Delete();
  CPPUNIT_ASSERT(true);
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestCopyVMEScalar()
//----------------------------------------------------------------------------
{
  mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));
  mafVMEScalarMatrix *scalar_copy = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));

  scalar->SetScalarArrayOrientation(mafVMEScalarMatrix::COLUMNS);
  scalar->SetScalarIdForXCoordinate(2);
  scalar->SetTypeForXCoordinates(mafVMEScalarMatrix::USE_PROGRESS_NUMBER);

  scalar_copy->DeepCopy(scalar);
  // Test changed methods that has to be copied
  CPPUNIT_ASSERT(scalar_copy->GetScalarArrayOrientation() == mafVMEScalarMatrix::COLUMNS);
  CPPUNIT_ASSERT(scalar_copy->GetScalarIdForXCoordinate() == 2);
  CPPUNIT_ASSERT(scalar_copy->GetTypeForXCoordinates() == mafVMEScalarMatrix::USE_PROGRESS_NUMBER);
  // Test the Equals method
  CPPUNIT_ASSERT(scalar_copy->Equals(scalar));

  scalar->Delete();
  scalar_copy->Delete();
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestVMEScalarData()
//----------------------------------------------------------------------------
{
  int r,c;
  vnl_matrix<double> out_data;

  mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));
  scalar->SetData(in_data,0.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  c = out_data.columns();
  CPPUNIT_ASSERT(c == 20);
  r = out_data.rows();
  CPPUNIT_ASSERT(r == 10);

  CPPUNIT_ASSERT(!scalar->IsAnimated());

  scalar->Delete();
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestVMEScalarMethods()
//----------------------------------------------------------------------------
{
  mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));
  scalar->SetData(in_data,0.0);

  //////////////////////////////////////////////////////////////////////////
  // Test default settings.
  CPPUNIT_ASSERT(scalar->GetScalarArrayOrientation() == mafVMEScalarMatrix::ROWS);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // input values are clamped to ROWS, COLUMNS
  scalar->SetScalarArrayOrientation(10);
  CPPUNIT_ASSERT(scalar->GetScalarArrayOrientation() == mafVMEScalarMatrix::COLUMNS);

  scalar->SetScalarArrayOrientation(-1);
  CPPUNIT_ASSERT(scalar->GetScalarArrayOrientation() == mafVMEScalarMatrix::ROWS);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Test standard method call
  scalar->SetScalarArrayOrientation(mafVMEScalarMatrix::COLUMNS);
  CPPUNIT_ASSERT(scalar->GetScalarArrayOrientation() == mafVMEScalarMatrix::COLUMNS);

  scalar->SetTypeForXCoordinates(mafVMEScalarMatrix::USE_TIME);
  CPPUNIT_ASSERT(scalar->GetTypeForXCoordinates() == mafVMEScalarMatrix::USE_TIME);

  scalar->SetTypeForYCoordinates(mafVMEScalarMatrix::USE_PROGRESS_NUMBER);
  CPPUNIT_ASSERT(scalar->GetTypeForYCoordinates() == mafVMEScalarMatrix::USE_PROGRESS_NUMBER);

  scalar->SetTypeForZCoordinates(mafVMEScalarMatrix::USE_SCALAR);
  CPPUNIT_ASSERT(scalar->GetTypeForZCoordinates() == mafVMEScalarMatrix::USE_SCALAR);

  scalar->SetScalarIdForXCoordinate(1);
  CPPUNIT_ASSERT(scalar->GetScalarIdForXCoordinate() == 1);

  scalar->SetScalarIdForYCoordinate(10);
  CPPUNIT_ASSERT(scalar->GetScalarIdForYCoordinate() == 10);

  scalar->SetScalarIdForZCoordinate(8);
  CPPUNIT_ASSERT(scalar->GetScalarIdForZCoordinate() == 8);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Test scalar ID clamping to avoid access not existing scalar value
  // input data has 10 rows and 20 columns.
  scalar->SetScalarArrayOrientation(mafVMEScalarMatrix::COLUMNS);
  scalar->SetScalarIdForXCoordinate(25);
  CPPUNIT_ASSERT(scalar->GetScalarIdForXCoordinate() == 20);
  //////////////////////////////////////////////////////////////////////////

  scalar->Delete();
}
//----------------------------------------------------------------------------
void mafVMEScalarMatrixTest::TestAnimatedVMEScalar()
//----------------------------------------------------------------------------
{
  vnl_matrix<double> out_data;
  vnl_matrix<double> in_data_t1;
  in_data_t1.set_size(10,20);
  in_data_t1.fill(5.0);

  mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(mafVMEFactory::CreateInstance("mafVMEScalarMatrix"));
  scalar->SetData(in_data,0.0);
  scalar->SetData(in_data_t1,1.0);

  CPPUNIT_ASSERT(scalar->IsAnimated());

  //////////////////////////////////////////////////////////////////////////
  // Test scalar values at different timestamp.
  scalar->SetTimeStamp(0.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data.get(0,0),1.0));

  scalar->SetTimeStamp(1.0);
  out_data = scalar->GetScalarOutput()->GetScalarData();
  CPPUNIT_ASSERT(mafEquals(out_data.get(0,0),5.0));
  //////////////////////////////////////////////////////////////////////////
  
  scalar->Delete();
}


