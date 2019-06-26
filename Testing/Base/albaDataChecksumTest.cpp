/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataChecksumTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDataChecksumTest.h"
#include "albaDataChecksum.h"



//----------------------------------------------------------------------------
void albaDataChecksumTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaDataChecksum *checksum = new albaDataChecksum();
	cppDEL(checksum);
}
//----------------------------------------------------------------------------
void albaDataChecksumTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaDataChecksum checksum;
}
//----------------------------------------------------------------------------
void albaDataChecksumTest::TestAdler32Checksum()
//----------------------------------------------------------------------------
{
	/**
  Algorithm:
  http://en.wikipedia.org/wiki/Adler-32

  Test Case:
  ASCII code          A                   B
  (shown as base 10)
  W: 87           1 +  87 =  88        0 +  88 =   88
  i: 105         88 + 105 = 193       88 + 193 =  281
  k: 107        193 + 107 = 300      281 + 300 =  581
  i: 105        300 + 105 = 405      581 + 405 =  986
  p: 112        405 + 112 = 517      986 + 517 = 1503
  e: 101        517 + 101 = 618     1503 + 618 = 2121
  d: 100        618 + 100 = 718     2121 + 718 = 2839
  i: 105        718 + 105 = 823     2839 + 823 = 3662
  a: 97         823 +  97 = 920     3662 + 920 = 4582

  A = 920  =  398 hex (base 16)
  B = 4582 = 11E6 hex

  Output = 300286872 = 11E60398 hex

  */
  const char *value = "Wikipedia";
  albaDataChecksum checksum;
  unsigned long result = checksum.Adler32Checksum((unsigned char*) value, 9);

  CPPUNIT_ASSERT(300286872 == result);

}
//----------------------------------------------------------------------------
void albaDataChecksumTest::TestCombineAdler32Checksums()
//----------------------------------------------------------------------------
{
  const char *value1 = "Wiki";
  albaDataChecksum checksum1;
  unsigned long result1 = checksum1.Adler32Checksum((unsigned char*) value1, 16);

  const char *value2 = "pedia";
  albaDataChecksum checksum2;
  unsigned long result2 = checksum1.Adler32Checksum((unsigned char*) value2, 11);

  albaDataChecksum combinedChecksum;
  unsigned long resultCombined = combinedChecksum.CombineAdler32Checksums(result1, result2);

  /** Warning: during test it is discovered that the combinedResult is the simple sum of the 
  two cheksum inputs. Maybe it can be substitued with a sum, but
  it is not completely understood if there are case in which that 
  output is not the simple sum.*/
  CPPUNIT_ASSERT(resultCombined == result1 + result2);
}