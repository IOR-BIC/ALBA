/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformInterfaceGUITest
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

#include "albaTransformInterfaceGUITest.h"
#include "albaTransformInterfaceGUI.h"

#include "albaVMEGroup.h"

class albaTransformInterfaceGUIDummy : public albaTransformInterfaceGUI
{
public:
  albaTransformInterfaceGUIDummy():albaTransformInterfaceGUI(){};
  ~albaTransformInterfaceGUIDummy(){};

  /*virtual*/ void EnableWidgets(bool enable){};
protected:
private:
};

//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaTransformInterfaceGUIDummy *transform = new albaTransformInterfaceGUIDummy();

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaTransformInterfaceGUIDummy transform;
}
//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestTestModeOn()
//----------------------------------------------------------------------------
{
  albaTransformInterfaceGUIDummy *transform = new albaTransformInterfaceGUIDummy();

  transform->TestModeOn();

  CPPUNIT_ASSERT(transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestTestModeOff()
//----------------------------------------------------------------------------
{
  albaTransformInterfaceGUIDummy *transform = new albaTransformInterfaceGUIDummy();

  transform->TestModeOff();

  CPPUNIT_ASSERT(!transform->GetTestMode());

  cppDEL(transform);
}
//----------------------------------------------------------------------------
void albaTransformInterfaceGUITest::TestSetRefSys()
//----------------------------------------------------------------------------
{
  albaTransformInterfaceGUIDummy *transform = new albaTransformInterfaceGUIDummy();

  albaSmartPointer<albaVMEGroup> refSys;

  transform->SetRefSys(refSys);

  CPPUNIT_ASSERT(transform->GetRefSys() == refSys.GetPointer());

  cppDEL(transform);
}
