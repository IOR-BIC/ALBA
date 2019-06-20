/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardSettingsTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardSettingsTest_H__
#define __CPP_UNIT_albaWizardSettingsTest_H__

#include "albaTest.h"

class albaWizardSettingsTest : public albaTest
{
public: 
 
  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaWizardSettingsTest );
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
