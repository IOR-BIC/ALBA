/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDirectoryTest
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
#include "albaDirectoryTest.h"
#include "albaDirectory.h"

#include "albaString.h"

#define NUMBER_OF_FILES 2

enum FILES
{
	FILE1,
	FILE2,
};
//----------------------------------------------------------------------------
void albaDirectoryTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaDirectory m;
}
//----------------------------------------------------------------------------
void albaDirectoryTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaDirectory *m = new albaDirectory();
	cppDEL(m);
}
//----------------------------------------------------------------------------
void albaDirectoryTest::TestLoad()
//----------------------------------------------------------------------------
{
	m_Directory = new albaDirectory();
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Test_albaDirectory";

	bool result = m_Directory->Load(filename);
	CPPUNIT_ASSERT(result);
	cppDEL(m_Directory);
}
//----------------------------------------------------------------------------
void albaDirectoryTest::TestGetNumberOfFiles()
//----------------------------------------------------------------------------
{
	m_Directory = new albaDirectory();
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Test_albaDirectory";

	bool result = m_Directory->Load(filename);

	int numberOfFiles = 0;
	for(int i=0;i<m_Directory->GetNumberOfFiles();i++)
	{
		if(strcmp(m_Directory->GetFile(i),".")!=0 && strcmp(m_Directory->GetFile(i),"..")!=0 && strcmp(m_Directory->GetFile(i),"CVS")!=0)
		{
			numberOfFiles++;
		}
	}
	
	CPPUNIT_ASSERT(NUMBER_OF_FILES == numberOfFiles);
	cppDEL(m_Directory);
}
//----------------------------------------------------------------------------
void albaDirectoryTest::TestGetFile()
//----------------------------------------------------------------------------
{
	m_Directory = new albaDirectory();
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Test_albaDirectory";

	bool result = m_Directory->Load(filename);

	int numberOfFiles = 0;
	for(int i=0;i<m_Directory->GetNumberOfFiles();i++)
	{
		if(strcmp(m_Directory->GetFile(i),".")!=0 && strcmp(m_Directory->GetFile(i),"..")!=0 && strcmp(m_Directory->GetFile(i),"CVS")!=0)
		{
			switch (numberOfFiles)
			{
					case FILE1:
					CPPUNIT_ASSERT(strcmp(m_Directory->GetFile(i),"testFile1.vtk")==0);
					numberOfFiles++;
					break;
					case FILE2:
					CPPUNIT_ASSERT(strcmp(m_Directory->GetFile(i),"testFile2.vtk")==0);
					numberOfFiles++;
					break;
					default:
					CPPUNIT_ASSERT(false);
					break;
			}
		}
	}
	cppDEL(m_Directory);
}

//----------------------------------------------------------------------------
void albaDirectoryTest::TestFixture()
//----------------------------------------------------------------------------
{

}
