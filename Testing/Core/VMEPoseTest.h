/*=========================================================================

 Program: MAF2
 Module: mafViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEPoseTest_H__
#define __CPP_UNIT_VMEPoseTest_H__

#include "mafTest.h"
#include "mafVME.h"
#include "mafVMEGenericAbstract.h"


/** a simple VME created just for testing purposes. */
class mafVMETestHelper : public mafVMEGenericAbstract
{
public:
	mafTypeMacro(mafVMETestHelper, mafVMEGenericAbstract);

protected:
	mafVMETestHelper();
	virtual ~mafVMETestHelper();

private:
	mafVMETestHelper(const mafVMETestHelper&); // Not implemented
	void operator=(const mafVMETestHelper&); // Not implemented
};


class VMEPoseTest : public mafTest
{
  CPPUNIT_TEST_SUITE( VMEPoseTest );
  CPPUNIT_TEST( VmePoseMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VmePoseMainTest();
};

#endif