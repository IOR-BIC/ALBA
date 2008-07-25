/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLUTLibrary.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 08:47:02 $
Version:   $Revision: 1.4 $
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

#include "mafLUTLibrary.h"

#include "mafGUILutPreset.h"

#include "vtkDirectory.h"
#include "vtkLookupTable.h"
#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <fstream>

using namespace std;

const bool DEBUG_MODE = false;

//----------------------------------------------------------------------------
void mafLUTLibrary::Clear(bool removeLibraryFromDisk)
{
  // destroy lib
  map<string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    vtkLookupTable *lut = (*it).second;
    if (DEBUG_MODE)
      {
        string lutName = (*it).first;
        std::ostringstream stringStream;
        stringStream << "destroying lut:" << lutName << " at " << lut << std::endl;
        mafLogMessage(stringStream.str().c_str());
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

mafLUTLibrary::mafLUTLibrary()
{
  m_LibraryDir = "";
}

mafLUTLibrary::~mafLUTLibrary()
{
  Clear();
}

void mafLUTLibrary::PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut )
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

void mafLUTLibrary::SaveLUT( vtkLookupTable *inLut, const char *outFileName)
{
  assert(inLut);
  vtkLookupTable *lut = inLut;

  std::string fileName = outFileName;

  std::ofstream output;

  output.open(fileName.c_str());

  double range[2];
  lut->GetTableRange(range);
  output << "TableRange" << '\t' << range[0] << '\t' << range[1] <<  std::endl;
  output << std::endl;

  int numberOfTableValues = lut->GetNumberOfTableValues();
  if(numberOfTableValues > 256) numberOfTableValues = 256;

  output << "NumberOfTableValues" << '\t' << numberOfTableValues << std::endl;
  output << std::endl;

  output << "id"  << '\t' << "red" << '\t' << "green" << '\t' << "blue" << '\t'
    << "alpha" << std::endl;

  for(int i=0; i<numberOfTableValues; i++)
  {
    double *rgba;
    rgba = lut->GetTableValue(i);
    output << i  << '\t' << rgba[0] << '\t' << rgba[1] << '\t' << rgba[2] 
    << '\t' << rgba[3] << std::endl;
  }

  output.close();

  assert(wxFileExists(fileName.c_str()));

}

void mafLUTLibrary::LoadLUT( const char *lutFileName, vtkLookupTable *targetLut )
{
  assert(targetLut);
  vtkLookupTable *lut = targetLut;

  std::string fileName = lutFileName;

  std::ifstream input;

  input.open(fileName.c_str());

  std::string tmp;
  double range[2];
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  input  >> range[0] >> range[1];
  lut->SetTableRange(range);

  int numberOfTableValues;
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  input >> numberOfTableValues;

  lut->SetNumberOfTableValues(numberOfTableValues);


  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(stringStream.str().c_str());
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
      mafLogMessage(stringStream.str().c_str());
    }
    input  >> rgba[0] >> rgba[1] >> rgba[2] >> rgba[3];
    lut->SetTableValue(i, rgba);
  }

  input.close();

  assert(wxFileExists(fileName.c_str()));

}

void mafLUTLibrary::Add( vtkLookupTable *inputLUT, const char *lutName )
{

  vtkLookupTable *lut = m_LutMap[lutName];

  if (lut != NULL)
  {
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Overwriting: " << lutName << std::endl;
      mafLogMessage(stringStream.str().c_str());

      lut->Delete();
    }
  }

  vtkLookupTable *newLut = vtkLookupTable::New();
  newLut->DeepCopy(inputLUT);
  m_LutMap[lutName] = newLut;

  mafString lutFileName = m_LibraryDir + "/" + lutName + ".lut";
  SaveLUT(newLut, lutFileName.GetCStr());

  assert(wxFileExists(lutFileName.GetCStr()));
}

void mafLUTLibrary::Save()
{
  wxMkdir(m_LibraryDir.GetCStr());
  assert(wxDirExists(m_LibraryDir.GetCStr()));

  map<string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    string s = (*it).first;
    vtkLookupTable *lut = (*it).second;
    string absFileName = string(m_LibraryDir)  + "/" + s.append(".lut");
    SaveLUT(lut, absFileName.c_str());
  }  
}


void mafLUTLibrary::SetDir( const char *dir )
{
  m_LibraryDir = dir;
}

const char *mafLUTLibrary::GetDir()
{
  return m_LibraryDir;
}

void mafLUTLibrary::Load()
{
  Clear();
  m_LutMap.clear();

  vtkMAFSmartPointer<vtkDirectory> vtkDir;
  int result = vtkDir->Open(m_LibraryDir);
  assert(result == 1);

  int numFiles = vtkDir->GetNumberOfFiles();
  for (int fileId = 0; fileId < numFiles; fileId++) 
  { 
    string lutLocalFileName = vtkDir->GetFile(fileId);
    size_t foundPosition;

    foundPosition=lutLocalFileName.find(".lut");

    int nameLength = lutLocalFileName.size();

    bool foundLUTExtensionInName = (foundPosition!=string::npos);
    bool isExtensionAtTheEndOfName = (nameLength - foundPosition)  == 4 ? true : false ;

    bool isLutFile = foundLUTExtensionInName && isExtensionAtTheEndOfName;

    if (isLutFile)
    {
      cout << "found .lut at: " << int(foundPosition) << " in " << lutLocalFileName << endl;
      string lutAbsFileName = string(m_LibraryDir.GetCStr()) + lutLocalFileName;
      assert(wxFileExists(lutAbsFileName.c_str()));

      string lutName = lutLocalFileName.erase(foundPosition);
      vtkLookupTable *vtkLut = vtkLookupTable::New();
      LoadLUT(lutAbsFileName.c_str(), vtkLut);
      m_LutMap[lutName] = vtkLut;
    }
  }  

  map<string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    string s = (*it).first;
    mafLogMessage(s.c_str());
  }

  assert(true);
}


int mafLUTLibrary::Remove( const char *lutName )
{

  map<string, vtkLookupTable *>::const_iterator it;
  it = m_LutMap.find(lutName);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << lutName << endl;
    return MAF_ERROR;
  }
  else
  {
    cout << "removing element: " << lutName << endl;
    vtkLookupTable *lut = m_LutMap[lutName];
    lut->Delete();
    m_LutMap.erase(lutName);
    RemoveLUTFromDisk(lutName);
    return MAF_OK;
  } 
}

void mafLUTLibrary::RemoveLUTFromDisk(const char *lutName)
{
  mafString lutFileName = m_LibraryDir + "/" + lutName + ".lut";
  assert(wxFileExists(lutFileName.GetCStr()));
  wxRemoveFile(lutFileName.GetCStr());
}

int mafLUTLibrary::GetNumberOfLuts()
{
  return m_LutMap.size();
}

void mafLUTLibrary::GetLutNames( vector<string> &names )
{
  map<string, vtkLookupTable *>::iterator it;

  for(it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    string lutName = (*it).first;
    names.push_back(lutName);      
  }
}

vtkLookupTable *mafLUTLibrary::GetLutByName(const char *name)
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

bool mafLUTLibrary::HasLut(const char *name)
{
  map<string, vtkLookupTable *>::const_iterator it;
  it = m_LutMap.find(name);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << name << endl;
    return false;
  }
  else
  {
    return true;
  }
}

