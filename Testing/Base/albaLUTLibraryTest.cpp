/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLUTLibraryTest
 Authors: Stefano Perticoni
 
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
#include "albaLUTLibraryTest.h"

#include "albaGUILutPreset.h"

#include "vtkDirectory.h"
#include "vtkLookupTable.h"
#include "vtkALBASmartPointer.h"

#include <iostream>
#include <fstream>

using namespace std;

const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
void albaLUTLibraryTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void albaLUTLibraryTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_LutDefault = vtkLookupTable::New();
  lutPreset(0, m_LutDefault);

  m_LutEField = vtkLookupTable::New();
  lutPreset(2, m_LutEField);

  m_LutGlow = vtkLookupTable::New();
  lutPreset(3, m_LutGlow);
}
//----------------------------------------------------------------------------
void albaLUTLibraryTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_LutDefault);
  vtkDEL(m_LutEField);
  vtkDEL(m_LutGlow);
}
//----------------------------------------------------------------------------
void albaLUTLibraryTest::TestSetGetDir()
//----------------------------------------------------------------------------
{ 
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  // test some luts

	if (DEBUG_MODE)
	{
		std::ostringstream stringStream;
		lutLib->PrintLut(stringStream, m_LutGlow);

		albaLogMessage(stringStream.str().c_str());

		std::string dirPrefix = GET_TEST_DATA_DIR() + "\\";

		CPPUNIT_ASSERT(wxDirExists(dirPrefix.char_str()));

		std::string fileName = dirPrefix;
		fileName += "albaLUTStorageTestLog.txt";

		std::ofstream output;

		output.open(fileName.char_str());
		output << stringStream.str();
		output.close();

		CPPUNIT_ASSERT(wxFileExists(fileName.char_str()));
	}

  // store lookup table to file
  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  CPPUNIT_ASSERT(wxDirExists(dirPrefix.char_str()));

  std::string libDir = dirPrefix;
  libDir += "/LUTLib/";

  CPPUNIT_ASSERT(wxDirExists(libDir.char_str()));
  
  lutLib->SetDir(libDir.char_str());
  lutLib->Add(m_LutDefault, "lutDefault");
  lutLib->Add(m_LutEField, "lutEField");
  lutLib->Add(m_LutGlow, "lutGlow");
  
  string dir = lutLib->GetDir();
  CPPUNIT_ASSERT(dir == libDir);

  cppDEL(lutLib);
}

void albaLUTLibraryTest::TestAdd()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

	std::string oldDir = dirPrefix  + "LUTLib/";
  
  lutLib->SetDir(oldDir.char_str());
  lutLib->Load();

  string newDir = dirPrefix  + "TestAddLutToLibraryDir/";
  lutLib->SetDir(newDir.char_str());
  lutLib->Save();

  vtkALBASmartPointer<vtkLookupTable> anotherLUT;
  lutPreset(4, anotherLUT);

  lutLib->Add(anotherLUT, "anotherLUT"); 
  
  cppDEL(lutLib);
}

void albaLUTLibraryTest::TestRemove()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());

  lutLib->Load();

  lutLib->Remove("anotherLUT"); 

  cppDEL(lutLib);
}

void albaLUTLibraryTest::TestLoadSave()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());

  lutLib->Load();

  lutLib->Remove("anotherLUT"); 
  
  std::string newDirPrefix = ALBA_DATA_ROOT;
  newDirPrefix  +=  "/Test_LUTStorage/";

  string newDir = newDirPrefix  + "TestSave/";
  
  lutLib->SetDir(newDir.char_str());
  lutLib->Save();

  cppDEL(lutLib);
}

void albaLUTLibraryTest::TestGetNumberOfLuts()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());
  lutLib->Load();
  int num = lutLib->GetNumberOfLuts();
  CPPUNIT_ASSERT(num == 3);

  cppDEL(lutLib);
}

void albaLUTLibraryTest::TestGetLutNames()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());
  lutLib->Load();

  vector<string> lutNames;
  lutLib->GetLutNames(lutNames);
  
  CPPUNIT_ASSERT(lutNames.size() == 3);

  CPPUNIT_ASSERT(lutNames[0] == "lutDefault");
  CPPUNIT_ASSERT(lutNames[1] == "lutEField");
  CPPUNIT_ASSERT(lutNames[2] == "lutGlow");

  cppDEL(lutLib);  

}

void albaLUTLibraryTest::TestGetLutByName()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());
  lutLib->Load();

  vtkLookupTable *lut = lutLib->GetLutByName("lutDefault");
  CPPUNIT_ASSERT(lut->GetNumberOfTableValues() == 256);

  lut = lutLib->GetLutByName("lutPippo");
  CPPUNIT_ASSERT(lut == NULL);

  cppDEL(lutLib);  

}

void albaLUTLibraryTest::TestHasLut()
{
  albaLUTLibrary *lutLib = new albaLUTLibrary();

  std::string dirPrefix = ALBA_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.char_str());
  lutLib->Load();

  bool ret = lutLib->HasLut("lutDefault");
  CPPUNIT_ASSERT(ret == true);

  ret = lutLib->HasLut("lutPippo");
  CPPUNIT_ASSERT(ret == false);

  cppDEL(lutLib);  

}
