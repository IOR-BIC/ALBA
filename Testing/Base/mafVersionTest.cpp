/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVersionTest.cpp,v $
Language:  C++
Date:      $Date: 2007-11-05 13:27:39 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVersionTest.h"
#include "mafVersion.h"

//----------------------------------------------------------------------------
void mafVersionTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVersionTest::TestGetMAFVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(strcmp(mafVersion::GetMAFVersion(),MAF_VERSION)==0);
}
//----------------------------------------------------------------------------
void mafVersionTest::TestGetMAFMajorVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(mafVersion::GetMAFMajorVersion()==MAF_MAJOR_VERSION);
}
//----------------------------------------------------------------------------
void mafVersionTest::TestGetMAFMinorVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(mafVersion::GetMAFMinorVersion()==MAF_MINOR_VERSION);
}
//----------------------------------------------------------------------------
void mafVersionTest::TestGetMAFBuildVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(mafVersion::GetMAFBuildVersion()==MAF_BUILD_VERSION);
}
//----------------------------------------------------------------------------
void mafVersionTest::TestGetMAFSourceVersion()
//----------------------------------------------------------------------------
{
	CPPUNIT_ASSERT(strcmp(mafVersion::GetMAFSourceVersion(),MAF_SOURCE_VERSION)==0);
}
//----------------------------------------------------------------------------
void mafVersionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafVersion *version;
	version = new mafVersion();

	delete version;
}
//----------------------------------------------------------------------------
void mafVersionTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafVersion version;
}
