/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCryptTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBACRYPTTEST_H__
#define __CPP_UNIT_ALBACRYPTTEST_H__

#include "albaTest.h"

class albaCryptTest : public albaTest
{

public:

 CPPUNIT_TEST_SUITE( albaCryptTest );
 CPPUNIT_TEST(TestFixture);
 CPPUNIT_TEST(TestMafCalculateteChecksumFromString);
 CPPUNIT_TEST(TestMafCalculateteChecksumFromFile);
 CPPUNIT_TEST(TestMafEncryptMafDecryptFile);
 CPPUNIT_TEST(TestMafDecryptFileInMemory);
 CPPUNIT_TEST(TestMafEncryptFileFromMemory);
 CPPUNIT_TEST(TestMafDefaultEncryptFileMafDefaultDecryptFile);
 CPPUNIT_TEST(TestMafDefaultDecryptFileInMemory);
 CPPUNIT_TEST(TestMafDefaultEncryptFileFromMemory);
 CPPUNIT_TEST(TestMafEncryptFromMemoryMafDecryptInMemory);
 CPPUNIT_TEST(TestMafDefaultEncryptFromMemoryMafDefaultDecryptInMemory);
 CPPUNIT_TEST_SUITE_END();

protected:

 void TestFixture();
 void TestMafCalculateteChecksumFromString();
 void TestMafCalculateteChecksumFromFile();
 void TestMafEncryptMafDecryptFile();
 void TestMafDecryptFileInMemory();
 void TestMafEncryptFileFromMemory();
 void TestMafDefaultEncryptFileMafDefaultDecryptFile();
 void TestMafDefaultDecryptFileInMemory();
 void TestMafDefaultEncryptFileFromMemory();

 // NOT WORKING: This is showcasing a known Crypto++ problem.
 // See source code for more details.
 void TestMafEncryptFromMemoryMafDecryptInMemory();

 // NOT WORKING: This is showcasing a known Crypto++ problem.
 // See source code for more details.
 void TestMafDefaultEncryptFromMemoryMafDefaultDecryptInMemory();

};

#endif
