/*=========================================================================

 Program: MAF2Medical
 Module: mafPipeSliceTest
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
#include "mafPipeSliceTest.h"
#include "mafPipeSlice.h"

#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>


//----------------------------------------------------------------------------
void mafPipeSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSliceTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSliceTest::tearDown()
//----------------------------------------------------------------------------
{
}


//----------------------------------------------------------------------------
void mafPipeSliceTest::TestGetNormal1()
//----------------------------------------------------------------------------
{
  mafPipeSlice *pipeSlice = new mafPipeSlice();

  double normal[3];
  double *result_normal_tobenull = NULL;
  double result_normal[3];
  
  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.5;

  result_normal[0] = normal[0] - 1.0;
  result_normal[1] = normal[1] - 1.0;
  result_normal[2] = normal[2] - 1.0;

  pipeSlice->SetNormal(normal);
  pipeSlice->GetNormal(result_normal);

  CPPUNIT_ASSERT( result_normal );
  CPPUNIT_ASSERT( result_normal[0] == normal[0] && result_normal[1] == normal[1] && result_normal[2] == normal[2] );

  pipeSlice->GetNormal(result_normal_tobenull);
  CPPUNIT_ASSERT(result_normal_tobenull == NULL);

  cppDEL(pipeSlice);

}

//----------------------------------------------------------------------------
void mafPipeSliceTest::TesGetNormal2()
//----------------------------------------------------------------------------
{

  mafPipeSlice *pipeSlice = new mafPipeSlice();

  double normal[3];
  const double *result_normal;

  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.5;

  pipeSlice->SetNormal(normal);
  result_normal = pipeSlice->GetNormal();

  CPPUNIT_ASSERT( result_normal );
  CPPUNIT_ASSERT( result_normal[0] == normal[0] && result_normal[1] == normal[1] && result_normal[2] == normal[2] );

  cppDEL(pipeSlice);


}

//----------------------------------------------------------------------------
void mafPipeSliceTest::TestGetOrigin1()
//----------------------------------------------------------------------------
{
  mafPipeSlice *pipeSlice = new mafPipeSlice();

  double origin[3];
  double *result_origin_tobenull = NULL;
  double result_origin[3];

  origin[0] = 0.0;
  origin[1] = 1.0;
  origin[2] = 0.5;

  result_origin[0] = origin[0] - 1.0;
  result_origin[1] = origin[1] - 1.0;
  result_origin[2] = origin[2] - 1.0;

  pipeSlice->SetOrigin(origin);
  pipeSlice->GetOrigin(result_origin);

  CPPUNIT_ASSERT( result_origin );
  CPPUNIT_ASSERT( result_origin[0] == origin[0] && result_origin[1] == origin[1] && result_origin[2] == origin[2] );

  pipeSlice->GetOrigin(result_origin_tobenull);
  CPPUNIT_ASSERT(result_origin_tobenull == NULL);

  cppDEL(pipeSlice);

}

//----------------------------------------------------------------------------
void mafPipeSliceTest::TestGetOrigin2()
//----------------------------------------------------------------------------
{
  mafPipeSlice *pipeSlice = new mafPipeSlice();

  double origin[3];
  const double *result_origin;

  origin[0] = 0.0;
  origin[1] = 1.0;
  origin[2] = 0.5;

  pipeSlice->SetOrigin(origin);
  result_origin = pipeSlice->GetOrigin();

  CPPUNIT_ASSERT( result_origin );
  CPPUNIT_ASSERT( result_origin[0] == origin[0] && result_origin[1] == origin[1] && result_origin[2] == origin[2] );

  cppDEL(pipeSlice);

}

//----------------------------------------------------------------------------
void mafPipeSliceTest::TestGetSlice()
//----------------------------------------------------------------------------
{
  mafPipeSlice *pipeSlice = new mafPipeSlice();

  double origin[3], normal[3];
  double result_origin[3], result_normal[3];

  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.0;

  result_origin[0] = origin[0] - 1.0;
  result_origin[1] = origin[1] - 1.0;
  result_origin[2] = origin[2] - 1.0;

  normal[0] = 0.0;
  normal[1] = 1.0;
  normal[2] = 0.5;

  result_normal[0] = normal[0] - 1.0;
  result_normal[1] = normal[1] - 1.0;
  result_normal[2] = normal[2] - 1.0;

  pipeSlice->SetSlice(origin, normal);

  pipeSlice->GetSlice(result_origin, result_normal);

  CPPUNIT_ASSERT(result_origin && result_normal);

  CPPUNIT_ASSERT( result_origin[0] == origin[0] && result_origin[1] == origin[1] && result_origin[2] == origin[2] );

  CPPUNIT_ASSERT( result_normal[0] == normal[0] && result_normal[1] == normal[1] && result_normal[2] == normal[2] );

  cppDEL(pipeSlice);

}
