/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkTest
 Authors: Daniele Giunchi, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterLandmarkTEST_H
#define CPP_UNIT_albaOpExporterLandmarkTEST_H

#include "albaTest.h"

class albaOpExporterLandmarkTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpExporterLandmarkTest );
  //CPPUNIT_TEST( TestOnVmeRawMotionData ); ///< this test is leaked for vmerawmotiondata
	CPPUNIT_TEST( TestOnLandmarkImporter );
  CPPUNIT_TEST( TestMultipleExports );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestOnVmeRawMotionData();
		void TestOnLandmarkImporter();
    void TestMultipleExports();
};

#endif
