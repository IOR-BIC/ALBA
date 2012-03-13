/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLUTLibraryTest.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 07:04:00 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni
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
#include "mafLUTLibraryTest.h"

#include "mafGUILutPreset.h"

#include "vtkDirectory.h"
#include "vtkLookupTable.h"
#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <fstream>

using namespace std;

const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
void mafLUTLibraryTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafLUTLibraryTest::setUp()
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
void mafLUTLibraryTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_LutDefault);
  vtkDEL(m_LutEField);
  vtkDEL(m_LutGlow);
}
//----------------------------------------------------------------------------
void mafLUTLibraryTest::TestSetGetDir()
//----------------------------------------------------------------------------
{ 
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  // test some luts

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    lutLib->PrintLut(stringStream, m_LutGlow);

    mafLogMessage(stringStream.str().c_str());

    std::string dirPrefix = MAF_DATA_ROOT;
    dirPrefix  +=  "/Test_LUTStorage/";

    CPPUNIT_ASSERT(wxDirExists(dirPrefix.c_str()));

    std::string fileName = dirPrefix;
    fileName  +=  "mafLUTStorageTestLog.txt";

    std::ofstream output;

    output.open(fileName.c_str());
    output << stringStream.str();
    output.close();

    CPPUNIT_ASSERT(wxFileExists(fileName.c_str()));

  }

  CPPUNIT_ASSERT(true);

  // store lookup table to file
  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  CPPUNIT_ASSERT(wxDirExists(dirPrefix.c_str()));

  std::string libDir = dirPrefix;
  libDir += "/LUTLib/";

  CPPUNIT_ASSERT(wxDirExists(libDir.c_str()));
  
  lutLib->SetDir(libDir.c_str());
  lutLib->Add(m_LutDefault, "lutDefault");
  lutLib->Add(m_LutEField, "lutEField");
  lutLib->Add(m_LutGlow, "lutGlow");
  
  string dir = lutLib->GetDir();
  CPPUNIT_ASSERT(dir == libDir);

  cppDEL(lutLib);
}

void mafLUTLibraryTest::TestAdd()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";
  
  lutLib->SetDir(oldDir.c_str());
  lutLib->Load();

  string newDir = dirPrefix  + "TestAddLutToLibraryDir/";
  lutLib->SetDir(newDir.c_str());
  lutLib->Save();

  vtkMAFSmartPointer<vtkLookupTable> anotherLUT;
  lutPreset(4, anotherLUT);

  lutLib->Add(anotherLUT, "anotherLUT"); 
  
  cppDEL(lutLib);
}

void mafLUTLibraryTest::TestRemove()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());

  lutLib->Load();

  lutLib->Remove("anotherLUT"); 

  cppDEL(lutLib);
}

void mafLUTLibraryTest::TestLoadSave()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());

  lutLib->Load();

  lutLib->Remove("anotherLUT"); 
  
  std::string newDirPrefix = MAF_DATA_ROOT;
  newDirPrefix  +=  "/Test_LUTStorage/";

  string newDir = newDirPrefix  + "TestSave/";
  
  lutLib->SetDir(newDir.c_str());
  lutLib->Save();

  cppDEL(lutLib);
}

void mafLUTLibraryTest::TestGetNumberOfLuts()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());
  lutLib->Load();
  int num = lutLib->GetNumberOfLuts();
  CPPUNIT_ASSERT(num == 3);

  cppDEL(lutLib);
}

void mafLUTLibraryTest::TestGetLutNames()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());
  lutLib->Load();

  vector<string> lutNames;
  lutLib->GetLutNames(lutNames);
  
  CPPUNIT_ASSERT(lutNames.size() == 3);

  CPPUNIT_ASSERT(lutNames[0] == "lutDefault");
  CPPUNIT_ASSERT(lutNames[1] == "lutEField");
  CPPUNIT_ASSERT(lutNames[2] == "lutGlow");

  cppDEL(lutLib);  

}

void mafLUTLibraryTest::TestGetLutByName()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());
  lutLib->Load();

  vtkLookupTable *lut = lutLib->GetLutByName("lutDefault");
  CPPUNIT_ASSERT(lut->GetNumberOfTableValues() == 256);

  lut = lutLib->GetLutByName("lutPippo");
  CPPUNIT_ASSERT(lut == NULL);

  cppDEL(lutLib);  

}

void mafLUTLibraryTest::TestHasLut()
{
  mafLUTLibrary *lutLib = new mafLUTLibrary();

  std::string dirPrefix = MAF_DATA_ROOT;
  dirPrefix  +=  "/Test_LUTStorage/";

  string oldDir = dirPrefix  + "LUTLib/";

  lutLib->SetDir(oldDir.c_str());
  lutLib->Load();

  bool ret = lutLib->HasLut("lutDefault");
  CPPUNIT_ASSERT(ret == true);

  ret = lutLib->HasLut("lutPippo");
  CPPUNIT_ASSERT(ret == false);

  cppDEL(lutLib);  

}
