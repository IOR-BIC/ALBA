/*=========================================================================

 Program: MAF2
 Module: mafOpCreateWrappedMeterTest
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

#include "mafOpCreateWrappedMeterTest.h"
#include "mafOpCreateWrappedMeter.h"

#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"

#include "mafVMEWrappedMeter.h"


//-----------------------------------------------------------
void mafOpCreateWrappedMeterTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpCreateWrappedMeter *op = new mafOpCreateWrappedMeter();
  cppDEL(op);
}
//-----------------------------------------------------------
void mafOpCreateWrappedMeterTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpCreateWrappedMeterTest op;  
}

//-----------------------------------------------------------
void mafOpCreateWrappedMeterTest::TestAccept()
//-----------------------------------------------------------
{

  mafOpCreateWrappedMeter *m_OpCreateWrappedMeter = new mafOpCreateWrappedMeter();

  mafVMEVolumeGray *volume;
  mafNEW(volume);

  mafVMEImage *image;
  mafNEW(image);

  CPPUNIT_ASSERT(m_OpCreateWrappedMeter->Accept(volume));
  CPPUNIT_ASSERT(m_OpCreateWrappedMeter->Accept(image));
  CPPUNIT_ASSERT(!m_OpCreateWrappedMeter->Accept(NULL));

  cppDEL(m_OpCreateWrappedMeter);
  mafDEL(image);
  mafDEL(volume);

}

//-----------------------------------------------------------
void mafOpCreateWrappedMeterTest::TestOpExecute()
//-----------------------------------------------------------
{
  mafOpCreateWrappedMeter *m_OpCreateWrappedMeter = new mafOpCreateWrappedMeter();

  mafVMEVolumeGray *volume;
  mafNEW(volume);

  m_OpCreateWrappedMeter->SetInput(volume);
  m_OpCreateWrappedMeter->OpRun();
  m_OpCreateWrappedMeter->OpDo();

  mafVMEWrappedMeter *result;
  result = mafVMEWrappedMeter::SafeDownCast(m_OpCreateWrappedMeter->GetOutput());

  CPPUNIT_ASSERT(result);
  CPPUNIT_ASSERT(result->GetParent()==volume);

  cppDEL(m_OpCreateWrappedMeter);
  mafDEL(volume);
}