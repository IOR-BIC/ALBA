/*=========================================================================

 Program: MAF2
 Module: mafOpExporterLandmarkWSTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterLandmarkWSTEST_H
#define CPP_UNIT_mafOpExporterLandmarkWSTEST_H

#include "mafTest.h"

class mafOpExporterLandmarkWSTest : public mafTest
{

public:

  CPPUNIT_TEST_SUITE( mafOpExporterLandmarkWSTest );
	CPPUNIT_TEST( TestOnLandmarkImporter );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestOnLandmarkImporter();
};

#endif
