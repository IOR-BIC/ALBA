/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVersionTest
 Authors: Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaVersionTest.h"
#include "albaVersion.h"

//----------------------------------------------------------------------------
void albaVersionTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVersionTest::TestGetALBAVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(strcmp(albaVersion::GetALBAVersion(),ALBA_VERSION)==0);
}
//----------------------------------------------------------------------------
void albaVersionTest::TestGetALBAMajorVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(albaVersion::GetALBAMajorVersion()==ALBA_MAJOR_VERSION);
}
//----------------------------------------------------------------------------
void albaVersionTest::TestGetALBAMinorVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(albaVersion::GetALBAMinorVersion()==ALBA_MINOR_VERSION);
}
//----------------------------------------------------------------------------
void albaVersionTest::TestGetALBASourceVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(strcmp(albaVersion::GetALBASourceVersion(),ALBA_SOURCE_VERSION)==0);
}
//----------------------------------------------------------------------------
void albaVersionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaVersion *version;
	version = new albaVersion();

	delete version;
}
//----------------------------------------------------------------------------
void albaVersionTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaVersion version;
}
