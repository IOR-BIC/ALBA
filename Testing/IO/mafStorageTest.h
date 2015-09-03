/*=========================================================================

 Program: MAF2
 Module: mafStorageTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafStorageTest_H__
#define __CPP_UNIT_mafStorageTest_H__

#include "mafTest.h"

class mafStorageTest : public mafTest
{
public:

  
	CPPUNIT_TEST_SUITE( mafStorageTest );

  CPPUNIT_TEST( SetGetURLTest );
  CPPUNIT_TEST( ForceGetParserURLTest );
  CPPUNIT_TEST( StoreTest );
  CPPUNIT_TEST( RestoreTest );
  CPPUNIT_TEST( SetGetDocumentTest );
  CPPUNIT_TEST( GetTmpFileTest );
  CPPUNIT_TEST( ReleaseTmpFileTest );
  CPPUNIT_TEST( IsFileInDirectoryTest );
  CPPUNIT_TEST( SetGetTmpFolderTest );
  CPPUNIT_TEST( SetGetErrorCodeTest );
  CPPUNIT_TEST( NeedsUpgradeTest );

	CPPUNIT_TEST_SUITE_END();

protected:

  void SetGetURLTest();
  void ForceGetParserURLTest();
  void StoreTest();
  void RestoreTest();
  void SetGetDocumentTest();
  // void ResolveInputURLTest(); Not implemented in the tested class
  // void StoreToURLTest(); Not implemented in the tested class
  // void ReleaseURL(); Not implemented in the tested class
	void GetTmpFileTest();
  void ReleaseTmpFileTest();
  void IsFileInDirectoryTest();
  void SetGetTmpFolderTest();
  void SetGetErrorCodeTest();
  void NeedsUpgradeTest();
};

#endif // __CPP_UNIT_mafStorageTest_H__