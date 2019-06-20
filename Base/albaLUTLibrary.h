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

#ifndef __albaLUTLibrary_H__
#define __albaLUTLibrary_H__

#include "albaString.h"
#include "vtkALBASmartPointer.h"
#include "vtkLookupTable.h"

#include <map>
#include <vector>

/** A component to handle lookup table libraries */
class ALBA_EXPORT albaLUTLibrary
{
public:
  albaLUTLibrary();
  ~albaLUTLibrary();

  /** set get the library directory */
  void SetDir(const char *dir);
  const char *GetDir();

  /** load lut library */
  void Load();

  /** save lut library */
  void Save();

  /** beware! if a lut with the same name exists it will be replaced */
  void Add(vtkLookupTable *inLUT, const char *lutName);

  /** remove a lut from the library: return ALBA_ERROR  if lutName does not exist, 
  ALBA_OK otherwise */
  int Remove(const char *lutName); 

  /** return the number of luts */
  int GetNumberOfLuts();

  /** return the lut names vector */
  void GetLutNames(std::vector<std::string> &names);

  /** get a lut by name */
  vtkLookupTable *GetLutByName(const char *name);
  
  /** return if the named lut is present or not  */
  bool HasLut(const char *name);

  /** Clear the library; do not remove it from disk unless removeLibraryFromDisk is true */
  void Clear(bool removeLibraryFromDisk = false);

  void PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut );

private:
  std::map<std::string, vtkLookupTable *> m_LutMap;
  albaString m_LibraryDir;

  void RemoveLUTFromDisk(const char *lutName);
  void LoadLUT( const char *inLutFileName, vtkLookupTable *targetLut);
  void SaveLUT( vtkLookupTable *inLut, const char *outFileName); 
};
#endif
