/*=========================================================================

 Program: MAF2
 Module: mafCryptTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFCRYPTTEST_H__
#define __CPP_UNIT_MAFCRYPTTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafCryptTest : public CPPUNIT_NS::TestFixture
{

public:
 // CPPUNIT fixture: executed before each test
 void setUp();

 // CPPUNIT fixture: executed after each test
 void tearDown();

 CPPUNIT_TEST_SUITE( mafCryptTest );
 CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks

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


int
 main( int argc, char* argv[] )
{
 // Create the event manager and test controller
 CPPUNIT_NS::TestResult controller;

 // Add a listener that colllects test result
 CPPUNIT_NS::TestResultCollector result;
 controller.addListener( &result );        

 // Add a listener that print dots as test run.
 CPPUNIT_NS::BriefTestProgressListener progress;
 controller.addListener( &progress );      

 // Add the top suite to the test runner
 CPPUNIT_NS::TestRunner runner;
 runner.addTest( mafCryptTest::suite());
 runner.run( controller );

 // Print test in a compiler compatible format.
 CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
 outputter.write(); 

 return result.wasSuccessful() ? 0 : 1;
}

#endif
