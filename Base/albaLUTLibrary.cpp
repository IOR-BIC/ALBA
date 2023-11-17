/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLUTLibrary
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

#include "albaLUTLibrary.h"

#include "albaGUILutPreset.h"

#include "vtkDirectory.h"
#include "vtkLookupTable.h"
#include "vtkALBASmartPointer.h"

//#include <iostream>
//#include <fstream>

//using namespace std;

const bool DEBUG_MODE = false;

//----------------------------------------------------------------------------
void albaLUTLibrary::Clear(bool removeLibraryFromDisk)
{
  // destroy lib
  std::map<std::string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    vtkLookupTable *lut = (*it).second;
    if (DEBUG_MODE)
      {
				std::string lutName = (*it).first;
        std::ostringstream stringStream;
        stringStream << "destroying lut:" << lutName << " at " << lut << std::endl;
        albaLogMessage(stringStream.str().c_str());
      }
    lut->Delete();
  }

  m_LutMap.clear();

  if (removeLibraryFromDisk)
  {
    assert(wxDirExists(m_LibraryDir.GetCStr()));
    wxRmdir(m_LibraryDir.GetCStr());
  }
}

albaLUTLibrary::albaLUTLibrary()
{
  m_LibraryDir = "";
}

albaLUTLibrary::~albaLUTLibrary()
{
  Clear();
}

void albaLUTLibrary::PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut )
{
  stringStream << "lut: " << std::endl;

  lut->PrintSelf(stringStream, 0);

  int n = lut->GetNumberOfTableValues();
  if(n>256) n=256;

  for(int i=0; i<n; i++)
  {
    double *rgba;
    rgba = lut->GetTableValue(i);
    stringStream << i  << '\t' << rgba[0] << '\t' << rgba[1] << '\t' << rgba[2] 
    << '\t' << rgba[3] << std::endl;
  }

}

void albaLUTLibrary::SaveLUT( vtkLookupTable *inLut, const char *outFileName)
{
  assert(inLut);
  vtkLookupTable *lut = inLut;

  std::ofstream output;

  output.open(outFileName);

  double range[2];
  lut->GetTableRange(range);
  output << "TableRange" << '\t' << range[0] << '\t' << range[1] <<  std::endl;
  output << std::endl;

  int numberOfTableValues = lut->GetNumberOfTableValues();
  if(numberOfTableValues > 256) numberOfTableValues = 256;

  output << "NumberOfTableValues" << '\t' << numberOfTableValues << std::endl;
  output << std::endl;

  output << "Id"  << '\t' << "red" << '\t' << "green" << '\t' << "blue" << '\t'
    << "alpha" << std::endl;

  for(int i=0; i<numberOfTableValues; i++)
  {
    double *rgba;
    rgba = lut->GetTableValue(i);
    output << i  << '\t' << rgba[0] << '\t' << rgba[1] << '\t' << rgba[2] 
    << '\t' << rgba[3] << std::endl;
  }

  output.close();

  assert(wxFileExists(outFileName));

}

void albaLUTLibrary::LoadLUT( const char *lutFileName, vtkLookupTable *targetLut )
{
  assert(targetLut);
  vtkLookupTable *lut = targetLut;

  std::ifstream input;

  input.open(lutFileName);

  std::string tmp;
  double range[2];
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }

  input  >> range[0] >> range[1];
  lut->SetTableRange(range);

  int numberOfTableValues;
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
  input >> numberOfTableValues;

  lut->SetNumberOfTableValues(numberOfTableValues);


  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }

  for(int i=0; i<numberOfTableValues; i++)
  {
    int toSkip;
    double rgba[4];
    input >> toSkip;   
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping: " << toSkip  << std::endl;
      albaLogMessage(stringStream.str().c_str());
    }
    input  >> rgba[0] >> rgba[1] >> rgba[2] >> rgba[3];
    lut->SetTableValue(i, rgba);
  }

  input.close();

  assert(wxFileExists(lutFileName));

}

void albaLUTLibrary::Add( vtkLookupTable *inputLUT, const char *lutName )
{

  vtkLookupTable *lut = m_LutMap[lutName];

  if (lut != NULL)
  {
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Overwriting: " << lutName << std::endl;
      albaLogMessage(stringStream.str().c_str());

      lut->Delete();
    }
  }

  vtkLookupTable *newLut = vtkLookupTable::New();
  newLut->DeepCopy(inputLUT);
  m_LutMap[lutName] = newLut;

  albaString lutFileName = m_LibraryDir + "/" + lutName + ".lut";
  SaveLUT(newLut, lutFileName.GetCStr());

  assert(wxFileExists(lutFileName.GetCStr()));
}

void albaLUTLibrary::Save()
{
  wxMkdir(m_LibraryDir.GetCStr());
  assert(wxDirExists(m_LibraryDir.GetCStr()));

  std::map<std::string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    std::string s = (*it).first;
    vtkLookupTable *lut = (*it).second;
    std::string absFileName = std::string(m_LibraryDir)  + "/" + s.append(".lut");
    SaveLUT(lut, absFileName.c_str());
  }  
}


void albaLUTLibrary::SetDir( const char *dir )
{
  m_LibraryDir = dir;
}

const char *albaLUTLibrary::GetDir()
{
  return m_LibraryDir;
}

void albaLUTLibrary::Load()
{
  Clear();
  m_LutMap.clear();

  vtkALBASmartPointer<vtkDirectory> vtkDir;
  int result = vtkDir->Open(m_LibraryDir);
  assert(result == 1);

  int numFiles = vtkDir->GetNumberOfFiles();
  for (int fileId = 0; fileId < numFiles; fileId++) 
  { 
    std::string lutLocalFileName = vtkDir->GetFile(fileId);
    size_t foundPosition;

    foundPosition=lutLocalFileName.find(".lut");

    int nameLength = lutLocalFileName.size();

    bool foundLUTExtensionInName = (foundPosition!=std::string::npos);
    bool isExtensionAtTheEndOfName = (nameLength - foundPosition)  == 4 ? true : false ;

    bool isLutFile = foundLUTExtensionInName && isExtensionAtTheEndOfName;

    if (isLutFile)
    {
      cout << "found .lut at: " << int(foundPosition) << " in " << lutLocalFileName << std::endl;
      std::string lutAbsFileName = std::string(m_LibraryDir.GetCStr()) + lutLocalFileName;
      assert(wxFileExists(lutAbsFileName.c_str()));

      std::string lutName = lutLocalFileName.erase(foundPosition);
      vtkLookupTable *vtkLut = vtkLookupTable::New();
      LoadLUT(lutAbsFileName.c_str(), vtkLut);
      m_LutMap[lutName] = vtkLut;
    }
  }  

  std::map<std::string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    std::string s = (*it).first;
    albaLogMessage(s.c_str());
  }

  assert(true);
}


int albaLUTLibrary::Remove( const char *lutName )
{

  std::map<std::string, vtkLookupTable *>::const_iterator it;
  it = m_LutMap.find(lutName);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << lutName << std::endl;
    return ALBA_ERROR;
  }
  else
  {
    cout << "removing element: " << lutName << std::endl;
    vtkLookupTable *lut = m_LutMap[lutName];
    lut->Delete();
    m_LutMap.erase(lutName);
    RemoveLUTFromDisk(lutName);
    return ALBA_OK;
  } 
}

void albaLUTLibrary::RemoveLUTFromDisk(const char *lutName)
{
  albaString lutFileName = m_LibraryDir + "/" + lutName + ".lut";
  assert(wxFileExists(lutFileName.GetCStr()));
  wxRemoveFile(lutFileName.GetCStr());
}

int albaLUTLibrary::GetNumberOfLuts()
{
  return m_LutMap.size();
}

void albaLUTLibrary::GetLutNames( std::vector<std::string> &names )
{
  std::map<std::string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    std::string lutName = (*it).first;
    names.push_back(lutName);      
  }
}

vtkLookupTable *albaLUTLibrary::GetLutByName(const char *name)
{
  if (HasLut(name))
  {
    return m_LutMap[name];
  } 
  else
  {
    return NULL;
  }
}

bool albaLUTLibrary::HasLut(const char *name)
{
  std::map<std::string, vtkLookupTable *>::const_iterator it;
  it = m_LutMap.find(name);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << name << std::endl;
    return false;
  }
  else
  {
    return true;
  }
}

