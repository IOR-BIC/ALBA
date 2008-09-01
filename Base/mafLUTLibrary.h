/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLUTLibrary.h,v $
Language:  C++
Date:      $Date: 2008-09-01 08:15:11 $
Version:   $Revision: 1.4 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafLUTLibrary_H__
#define __mafLUTLibrary_H__

#include "mafString.h"
#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"

#include <map>
#include <vector>

/** A component to handle lookup table libraries */
class mafLUTLibrary
{
public:
  mafLUTLibrary();
  ~mafLUTLibrary();

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
  mafString m_LibraryDir;

  void RemoveLUTFromDisk(const char *lutName);
  void LoadLUT( const char *inLutFileName, vtkLookupTable *targetLut);
  void SaveLUT( vtkLookupTable *inLut, const char *outFileName); 
};
#endif
