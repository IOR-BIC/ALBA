/*=========================================================================

 Program: MAF2
 Module: mmaMeterTest
 Authors: Daniele Giunchi
 
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
#include "mmaMeterTest.h"

#include "mmaMeter.h"
#include "mafVMEMeter.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mmaMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmaMeterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mmaMeter attributeMeter;
}
//----------------------------------------------------------------------------
void mmaMeterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mmaMeter *attributeMeter = new mmaMeter();
  cppDEL(attributeMeter);
}
//----------------------------------------------------------------------------
void mmaMeterTest::TestPrint()
//----------------------------------------------------------------------------
{
	//only print Test
  mmaMeter *attributeMeter = new mmaMeter();
	attributeMeter->Print(std::cout);
  cppDEL(attributeMeter);
}
//----------------------------------------------------------------------------
void mmaMeterTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mmaMeter *attributeMeter = new mmaMeter();
	attributeMeter->SetName("MeterAttributes");
	attributeMeter->m_LabelVisibility= 1;
	attributeMeter->m_ColorMode      = mafVMEMeter::ONE_COLOR;
	attributeMeter->m_Representation = mafVMEMeter::LINE_REPRESENTATION;
	attributeMeter->m_Capping        = 0;
	attributeMeter->m_MeterMode      = mafVMEMeter::POINT_DISTANCE;
	attributeMeter->m_TubeRadius     = 1.0;
	attributeMeter->m_InitMeasure    = 0.0;
	attributeMeter->m_GenerateEvent  = 0;
	attributeMeter->m_ThresholdEvent = 0;
	attributeMeter->m_DeltaPercent   = 10.0;
	attributeMeter->m_MeasureType    = mafVMEMeter::ABSOLUTE_MEASURE;
	attributeMeter->m_DistanceRange[0] = 0.0;
	attributeMeter->m_DistanceRange[1] = 1.0;

	mmaMeter *attributeMeterCopied = new mmaMeter();
	attributeMeterCopied->SetName("MeterAttributesCopied");
	attributeMeterCopied->m_LabelVisibility= 0;
	attributeMeterCopied->m_ColorMode      = mafVMEMeter::RANGE_COLOR;
	attributeMeterCopied->m_Representation = mafVMEMeter::TUBE_REPRESENTATION;
	attributeMeterCopied->m_Capping        = 1;
	attributeMeterCopied->m_MeterMode      = mafVMEMeter::LINE_DISTANCE;
	attributeMeterCopied->m_TubeRadius     = 2.0;
	attributeMeterCopied->m_InitMeasure    = 1.0;
	attributeMeterCopied->m_GenerateEvent  = 1;
	attributeMeterCopied->m_ThresholdEvent = 1;
	attributeMeterCopied->m_DeltaPercent   = 20.0;
	attributeMeterCopied->m_MeasureType    = mafVMEMeter::RELATIVE_MEASURE;
	attributeMeterCopied->m_DistanceRange[0] = 5.0;
	attributeMeterCopied->m_DistanceRange[1] = 6.0;

	attributeMeterCopied->DeepCopy(attributeMeter);
	m_Result = attributeMeterCopied->Equals(attributeMeter) == true;
	TEST_RESULT;

  cppDEL(attributeMeter);
	cppDEL(attributeMeterCopied);
}
//----------------------------------------------------------------------------
void mmaMeterTest::TestEquals()
//----------------------------------------------------------------------------
{
	mmaMeter *attributeMeter = new mmaMeter();
	attributeMeter->SetName("MeterAttributes");
	attributeMeter->m_LabelVisibility= 1;
	attributeMeter->m_ColorMode      = mafVMEMeter::ONE_COLOR;
	attributeMeter->m_Representation = mafVMEMeter::LINE_REPRESENTATION;
	attributeMeter->m_Capping        = 0;
	attributeMeter->m_MeterMode      = mafVMEMeter::POINT_DISTANCE;
	attributeMeter->m_TubeRadius     = 1.0;
	attributeMeter->m_InitMeasure    = 0.0;
	attributeMeter->m_GenerateEvent  = 0;
	attributeMeter->m_ThresholdEvent = 0;
	attributeMeter->m_DeltaPercent   = 10.0;
	attributeMeter->m_MeasureType    = mafVMEMeter::ABSOLUTE_MEASURE;
	attributeMeter->m_DistanceRange[0] = 0.0;
	attributeMeter->m_DistanceRange[1] = 1.0;

	mmaMeter *attributeMeterCopied = new mmaMeter();
	attributeMeterCopied->SetName("MeterAttributesCopied");
	attributeMeterCopied->m_LabelVisibility= 0;
	attributeMeterCopied->m_ColorMode      = mafVMEMeter::RANGE_COLOR;
	attributeMeterCopied->m_Representation = mafVMEMeter::TUBE_REPRESENTATION;
	attributeMeterCopied->m_Capping        = 1;
	attributeMeterCopied->m_MeterMode      = mafVMEMeter::LINE_DISTANCE;
	attributeMeterCopied->m_TubeRadius     = 2.0;
	attributeMeterCopied->m_InitMeasure    = 1.0;
	attributeMeterCopied->m_GenerateEvent  = 1;
	attributeMeterCopied->m_ThresholdEvent = 1;
	attributeMeterCopied->m_DeltaPercent   = 20.0;
	attributeMeterCopied->m_MeasureType    = mafVMEMeter::RELATIVE_MEASURE;
	attributeMeterCopied->m_DistanceRange[0] = 5.0;
	attributeMeterCopied->m_DistanceRange[1] = 6.0;


	m_Result = attributeMeterCopied->Equals(attributeMeter) == false && attributeMeter->Equals(attributeMeterCopied) == false;
	TEST_RESULT;

	attributeMeterCopied->SetName("MeterAttributes");
	attributeMeterCopied->m_LabelVisibility= 1;
	attributeMeterCopied->m_ColorMode      = mafVMEMeter::ONE_COLOR;
	attributeMeterCopied->m_Representation = mafVMEMeter::LINE_REPRESENTATION;
	attributeMeterCopied->m_Capping        = 0;
	attributeMeterCopied->m_MeterMode      = mafVMEMeter::POINT_DISTANCE;
	attributeMeterCopied->m_TubeRadius     = 1.0;
	attributeMeterCopied->m_InitMeasure    = 0.0;
	attributeMeterCopied->m_GenerateEvent  = 0;
	attributeMeterCopied->m_ThresholdEvent = 0;
	attributeMeterCopied->m_DeltaPercent   = 10.0;
	attributeMeterCopied->m_MeasureType    = mafVMEMeter::ABSOLUTE_MEASURE;
	attributeMeterCopied->m_DistanceRange[0] = 0.0;
	attributeMeterCopied->m_DistanceRange[1] = 1.0;

	m_Result = attributeMeterCopied->Equals(attributeMeter) == true && attributeMeter->Equals(attributeMeterCopied) == true;
	TEST_RESULT;

	cppDEL(attributeMeter);
	cppDEL(attributeMeterCopied);
}