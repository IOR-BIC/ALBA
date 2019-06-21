/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkWSTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterLandmarkWSTEST_H
#define CPP_UNIT_albaOpExporterLandmarkWSTEST_H

#include "albaTest.h"

class albaOpExporterLandmarkWSTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpExporterLandmarkWSTest );
	CPPUNIT_TEST( TestOnLandmarkImporter );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestOnLandmarkImporter();
};

#endif
