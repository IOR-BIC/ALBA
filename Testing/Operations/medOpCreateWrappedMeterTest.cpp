/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCreateWrappedMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2010-01-29 10:34:55 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpCreateWrappedMeterTest.h"
#include "medOpCreateWrappedMeter.h"

#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"

#include "medVMEWrappedMeter.h"


//-----------------------------------------------------------
void medOpCreateWrappedMeterTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpCreateWrappedMeter *op = new medOpCreateWrappedMeter();
  cppDEL(op);
}
//-----------------------------------------------------------
void medOpCreateWrappedMeterTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpCreateWrappedMeterTest op;  
}

//-----------------------------------------------------------
void medOpCreateWrappedMeterTest::TestAccept()
//-----------------------------------------------------------
{

  medOpCreateWrappedMeter *m_OpCreateWrappedMeter = new medOpCreateWrappedMeter();

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
void medOpCreateWrappedMeterTest::TestOpExecute()
//-----------------------------------------------------------
{
  medOpCreateWrappedMeter *m_OpCreateWrappedMeter = new medOpCreateWrappedMeter();

  mafVMEVolumeGray *volume;
  mafNEW(volume);

  m_OpCreateWrappedMeter->SetInput(volume);
  m_OpCreateWrappedMeter->OpRun();
  m_OpCreateWrappedMeter->OpDo();

  medVMEWrappedMeter *result;
  result = medVMEWrappedMeter::SafeDownCast(m_OpCreateWrappedMeter->GetOutput());

  CPPUNIT_ASSERT(result);
  CPPUNIT_ASSERT(result->GetParent()==volume);

  cppDEL(m_OpCreateWrappedMeter);
  mafDEL(volume);
}