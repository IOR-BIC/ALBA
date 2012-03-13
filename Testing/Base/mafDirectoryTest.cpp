/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDirectoryTest.cpp,v $
Language:  C++
Date:      $Date: 2007-06-21 09:10:46 $
Version:   $Revision: 1.2 $
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
#include "mafDirectoryTest.h"
#include "mafDirectory.h"

#include "mafString.h"

#define NUMBER_OF_FILES 2

enum FILES
{
	FILE1,
	FILE2,
};
//----------------------------------------------------------------------------
void mafDirectoryTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafDirectory m;
}
//----------------------------------------------------------------------------
void mafDirectoryTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafDirectory *m = new mafDirectory();
	cppDEL(m);
}
//----------------------------------------------------------------------------
void mafDirectoryTest::TestLoad()
//----------------------------------------------------------------------------
{
	m_Directory = new mafDirectory();
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_mafDirectory";

	bool result = m_Directory->Load(filename);
	CPPUNIT_ASSERT(result);
	cppDEL(m_Directory);
}
//----------------------------------------------------------------------------
void mafDirectoryTest::TestGetNumberOfFiles()
//----------------------------------------------------------------------------
{
	m_Directory = new mafDirectory();
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_mafDirectory";

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
void mafDirectoryTest::TestGetFile()
//----------------------------------------------------------------------------
{
	m_Directory = new mafDirectory();
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Test_mafDirectory";

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
void mafDirectoryTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDirectoryTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDirectoryTest::TestFixture()
//----------------------------------------------------------------------------
{

}
