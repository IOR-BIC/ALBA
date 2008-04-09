/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesListTest.cpp,v $
Language:  C++
Date:      $Date: 2008-04-09 11:05:49 $
Version:   $Revision: 1.2 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDefines.h" 

#include <cppunit/config/SourcePrefix.h>

#include "vtkMEDPastValuesList.h"
#include "vtkMEDPastValuesListTest.h"

#include "mafConfigure.h"


//#define TESTDATA MAF_DATA_ROOT"/FEM/ANSYS"
#define FTOL 0.0000001

static const bool renderingOn = false ;  // switch interactive rendering on

void vtkMEDPastValuesListTest::setUp()
{
}

void vtkMEDPastValuesListTest::tearDown()
{
}


void vtkMEDPastValuesListTest::TestFixture()
{
}



//------------------------------------------------------------------------------
// Test with 20 values
void vtkMEDPastValuesListTest::TestPastValues() 
//------------------------------------------------------------------------------
{
  int i ;

  // test data produced by MS Excel
  double testData[22] = {122,310,939,824,587,998,218,446,122,298,146,893,438,949,252,95,883,482,330,784,784,1000} ;
  double min3[22] = {122,122,122,310,587,587,218,218,122,122,122,146,146,438,252,95,95,95,330,330,330,784} ;
  double max3[22] = {122,310,939,939,939,998,998,998,446,446,298,893,893,949,949,949,883,883,883,784,784,1000} ;
  double median3[22] = {122,216,310,824,824,824,587,446,218,298,146,298,438,893,438,252,252,482,482,482,784,784} ;
  double median4[22] = {122,216,310,567,705.5,881.5,705.5,516.5,332,258,222,222,368,665.5,665.5,345,567.5,367,406,633,633,784} ;
  double mean4[22] = {122,216,457,548.75,665,837,656.75,562.25,446,271,253,364.75,443.75,606.5,633,433.5,544.75,428,447.5,619.75,595,724.5} ;

  // set up an example to contain 3 past values and load data into it
  // At each step we check that the stats are correct.
  vtkMEDPastValuesList *PastVals3 = new vtkMEDPastValuesList(3) ;
  for (i = 0 ;  i < 22 ;  i++){
    PastVals3->AddNewValue(testData[i]) ;
    CPPUNIT_ASSERT(PastVals3->GetMin() == min3[i]) ;
    CPPUNIT_ASSERT(PastVals3->GetMax() == max3[i]) ;
    CPPUNIT_ASSERT(PastVals3->GetMedian() == median3[i]) ;
  }

  // set up an example to contain 4 past values and load data into it
  // At each step we check that the stats are correct.
  vtkMEDPastValuesList *PastVals4 = new vtkMEDPastValuesList(4) ;
  for (i = 0 ;  i < 22 ;  i++){
    PastVals4->AddNewValue(testData[i]) ;
    CPPUNIT_ASSERT(PastVals4->GetMedian() == median4[i]) ;
    CPPUNIT_ASSERT(PastVals4->GetMean() == mean4[i]) ;
  }

  delete PastVals3 ;
  delete PastVals4 ;
}
