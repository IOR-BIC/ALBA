/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEPoseTest_H__
#define __CPP_UNIT_VMEPoseTest_H__

#include "albaTest.h"
#include "albaVME.h"
#include "albaVMEGenericAbstract.h"


/** a simple VME created just for testing purposes. */
class albaVMETestHelper : public albaVMEGenericAbstract
{
public:
	albaTypeMacro(albaVMETestHelper, albaVMEGenericAbstract);

protected:
	albaVMETestHelper();
	virtual ~albaVMETestHelper();

private:
	albaVMETestHelper(const albaVMETestHelper&); // Not implemented
	void operator=(const albaVMETestHelper&); // Not implemented
};


class VMEPoseTest : public albaTest
{
  CPPUNIT_TEST_SUITE( VMEPoseTest );
  CPPUNIT_TEST( VmePoseMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VmePoseMainTest();
};

#endif