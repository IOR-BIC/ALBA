/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateWrappedMeterTest
 Authors: Eleonora Mambrini
 
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

#include "albaOpCreateWrappedMeterTest.h"
#include "albaOpCreateWrappedMeter.h"

#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"

#include "albaVMEWrappedMeter.h"


//-----------------------------------------------------------
void albaOpCreateWrappedMeterTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpCreateWrappedMeter *op = new albaOpCreateWrappedMeter();
  cppDEL(op);
}
//-----------------------------------------------------------
void albaOpCreateWrappedMeterTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpCreateWrappedMeterTest op;  
}

//-----------------------------------------------------------
void albaOpCreateWrappedMeterTest::TestAccept()
//-----------------------------------------------------------
{

  albaOpCreateWrappedMeter *m_OpCreateWrappedMeter = new albaOpCreateWrappedMeter();

  albaVMEVolumeGray *volume;
  albaNEW(volume);

  albaVMEImage *image;
  albaNEW(image);

  CPPUNIT_ASSERT(m_OpCreateWrappedMeter->Accept(volume));
  CPPUNIT_ASSERT(m_OpCreateWrappedMeter->Accept(image));
  CPPUNIT_ASSERT(!m_OpCreateWrappedMeter->Accept(NULL));

  cppDEL(m_OpCreateWrappedMeter);
  albaDEL(image);
  albaDEL(volume);

}

//-----------------------------------------------------------
void albaOpCreateWrappedMeterTest::TestOpExecute()
//-----------------------------------------------------------
{
  albaOpCreateWrappedMeter *m_OpCreateWrappedMeter = new albaOpCreateWrappedMeter();

  albaVMEVolumeGray *volume;
  albaNEW(volume);

  m_OpCreateWrappedMeter->SetInput(volume);
  m_OpCreateWrappedMeter->OpRun();
  m_OpCreateWrappedMeter->OpDo();

  albaVMEWrappedMeter *result;
  result = albaVMEWrappedMeter::SafeDownCast(m_OpCreateWrappedMeter->GetOutput());

  CPPUNIT_ASSERT(result);
  CPPUNIT_ASSERT(result->GetParent()==volume);

  cppDEL(m_OpCreateWrappedMeter);
  albaDEL(volume);
}