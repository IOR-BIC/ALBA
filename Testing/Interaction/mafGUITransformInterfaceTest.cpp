/*=========================================================================

 Program: MAF2
 Module: mafGUITransformInterfaceTest
 Authors: Matteo Giacomoni
 
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

#include "mafGUITransformInterfaceTest.h"
#include "mafGUITransformInterface.h"

#include "mafVMEGroup.h"

class mafGUITransformInterfaceDummy : public mafGUITransformInterface
{
public:
  mafGUITransformInterfaceDummy():mafGUITransformInterface(){};
  ~mafGUITransformInterfaceDummy(){};

  /*virtual*/ void EnableWidgets(bool enable){};
protected:
private:
};

//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUITransformInterfaceDummy *transform = new mafGUITransformInterfaceDummy();

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafGUITransformInterfaceDummy transform;
}
//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestTestModeOn()
//----------------------------------------------------------------------------
{
  mafGUITransformInterfaceDummy *transform = new mafGUITransformInterfaceDummy();

  transform->TestModeOn();

  CPPUNIT_ASSERT(transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestTestModeOff()
//----------------------------------------------------------------------------
{
  mafGUITransformInterfaceDummy *transform = new mafGUITransformInterfaceDummy();

  transform->TestModeOff();

  CPPUNIT_ASSERT(!transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void mafGUITransformInterfaceTest::TestSetRefSys()
//----------------------------------------------------------------------------
{
  mafGUITransformInterfaceDummy *transform = new mafGUITransformInterfaceDummy();

  mafSmartPointer<mafVMEGroup> refSys;

  transform->SetRefSys(refSys);

  CPPUNIT_ASSERT(transform->GetRefSys() == refSys.GetPointer());

  cppDEL(transform);
}
