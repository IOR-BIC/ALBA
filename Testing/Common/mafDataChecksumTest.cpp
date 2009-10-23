/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDataChecksumTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-23 17:00:34 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDataChecksumTest.h"
#include "mafDataChecksum.h"

//----------------------------------------------------------------------------
void mafDataChecksumTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void mafDataChecksumTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void mafDataChecksumTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafDataChecksum *checksum = new mafDataChecksum();
	cppDEL(checksum);
}
//----------------------------------------------------------------------------
void mafDataChecksumTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafDataChecksum checksum;
}
//----------------------------------------------------------------------------
void mafDataChecksumTest::TestAdler32Checksum()
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
  mafDataChecksum checksum;
  unsigned long result = checksum.Adler32Checksum((unsigned char*) value, 9);

  CPPUNIT_ASSERT(300286872 == result);

}
//----------------------------------------------------------------------------
void mafDataChecksumTest::TestCombineAdler32Checksums()
//----------------------------------------------------------------------------
{
  const char *value1 = "Wiki";
  mafDataChecksum checksum1;
  unsigned long result1 = checksum1.Adler32Checksum((unsigned char*) value1, 16);

  const char *value2 = "pedia";
  mafDataChecksum checksum2;
  unsigned long result2 = checksum1.Adler32Checksum((unsigned char*) value2, 11);

  mafDataChecksum combinedChecksum;
  unsigned long resultCombined = combinedChecksum.CombineAdler32Checksums(result1, result2);

  /** Warning: during test it is discovered that the combinedResult is the simple sum of the 
  two cheksum inputs. Maybe it can be substitued with a sum, but
  it is not completely understood if there are case in which that 
  output is not the simple sum.*/
  CPPUNIT_ASSERT(resultCombined == result1 + result2);
}