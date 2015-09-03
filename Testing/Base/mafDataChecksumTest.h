/*=========================================================================

 Program: MAF2
 Module: mafDataChecksumTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataChecksumTest_H__
#define __CPP_UNIT_mafDataChecksumTest_H__

#include "mafTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: mafDataChecksumTest
  Test class for mafDataChecksum
*/
class mafDataChecksumTest : public mafTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafDataChecksumTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );
  /** macro for test TestAdler32Checksum */
	CPPUNIT_TEST( TestAdler32Checksum );
  /** macro for test TestCombineAdler32Checksums */
	CPPUNIT_TEST( TestCombineAdler32Checksums );
  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();
  /** Test for check Adler Checksum */
	void TestAdler32Checksum();
  /** Test for check combination of Adler Checksum */
	void TestCombineAdler32Checksums();
};



#endif
