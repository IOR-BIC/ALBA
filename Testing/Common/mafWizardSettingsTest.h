/*=========================================================================

 Program: MAF2
 Module: mafWizardSettingsTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafWizardSettingsTest_H__
#define __CPP_UNIT_mafWizardSettingsTest_H__

#include "mafTest.h"

class mafWizardSettingsTest : public mafTest
{
public: 
 
  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafWizardSettingsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSettings );
  CPPUNIT_TEST_SUITE_END();

protected:
  /** Test Dynamic allocation */
  void TestDynamicAllocation();
  
  /** Test settings operation*/
  void TestSettings();
  
};



#endif
