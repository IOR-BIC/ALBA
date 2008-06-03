/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLUTLibrary.h,v $
Language:  C++
Date:      $Date: 2008-06-03 16:41:02 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

using namespace std;

#ifndef __CPP_UNIT_mafPipeMeshSliceTest_H__
#define __CPP_UNIT_mafPipeMeshSliceTest_H__

#include <map>
#include <vector>

#include "mafString.h"
#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"

class mafLUTLibrary
{

public:

  mafLUTLibrary::mafLUTLibrary();
  mafLUTLibrary::~mafLUTLibrary();

  /** set get the library directory */
  void SetDir(const char *dir);
  const char *GetDir();

  /** load lut library */
  void Load();

  /** save lut library */
  void Save();

  /** beware! if a lut with the same name exists it will be replaced */
  void Add(vtkLookupTable *inLUT, const char *lutName);

  /** remove a lut from the library: return MAF_ERROR  if lutName does not exist, 
  MAF_OK otherwise */
  int Remove(const char *lutName); 

  /** return the number of luts */
  int GetNumberOfLuts();

  /** return the lut names vector */
  void GetLutNames(vector<string> &names);

  /** get a lut by name */
  vtkLookupTable *GetLutByName(const char *name);
  
  /** return if the named lut is present or not  */
  bool HasLut(const char *name);

  /** Clear the library; do not remove it from disk unless removeLibraryFromDisk is true */
  void Clear(bool removeLibraryFromDisk = false);

  void PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut );

private:

  map<string, vtkLookupTable *> m_LutMap;
  mafString m_LibraryDir;

  void RemoveLUTFromDisk(const char *lutName);
  void LoadLUT( const char *inLutFileName, vtkLookupTable *targetLut);
  void SaveLUT( vtkLookupTable *inLut, const char *outFileName);
  
 
};
#endif
