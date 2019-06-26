/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformInterfaceTest
 Authors: Matteo Giacomoni
 
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

#include "albaGUITransformInterfaceTest.h"
#include "albaGUITransformInterface.h"

#include "albaVMEGroup.h"

class albaGUITransformInterfaceDummy : public albaGUITransformInterface
{
public:
  albaGUITransformInterfaceDummy():albaGUITransformInterface(){};
  ~albaGUITransformInterfaceDummy(){};

  /*virtual*/ void EnableWidgets(bool enable){};
protected:
private:
};

//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaGUITransformInterfaceDummy *transform = new albaGUITransformInterfaceDummy();

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaGUITransformInterfaceDummy transform;
}
//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestTestModeOn()
//----------------------------------------------------------------------------
{
  albaGUITransformInterfaceDummy *transform = new albaGUITransformInterfaceDummy();

  transform->TestModeOn();

  CPPUNIT_ASSERT(transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestTestModeOff()
//----------------------------------------------------------------------------
{
  albaGUITransformInterfaceDummy *transform = new albaGUITransformInterfaceDummy();

  transform->TestModeOff();

  CPPUNIT_ASSERT(!transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaGUITransformInterfaceTest::TestSetRefSys()
//----------------------------------------------------------------------------
{
  albaGUITransformInterfaceDummy *transform = new albaGUITransformInterfaceDummy();

  albaSmartPointer<albaVMEGroup> refSys;

  transform->SetRefSys(refSys);

  CPPUNIT_ASSERT(transform->GetRefSys() == refSys.GetPointer());

  cppDEL(transform);
}
