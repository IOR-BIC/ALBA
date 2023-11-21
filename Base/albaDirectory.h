/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDirectory
 Authors: Based on itkDirectory (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDirectory_h
#define __albaDirectory_h

#include "albaObject.h"
#include <iostream>
#include <string>
#include <vector>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,std::string);
#endif

/** Portable directory/filename traversal.
  albaDirectory provides a portable way of finding the names of the files
  in a system directory.
  albaDirectory works with windows and unix only.  
*/
class ALBA_EXPORT albaDirectory : public albaObject
{
public:  
  albaDirectory();
  ~albaDirectory();
  
  albaTypeMacro(albaDirectory,albaObject);

  /** 
    Load the specified directory and load the names of the files
    in that directory. "false" is returned if the directory can not be 
    opened, "true" if it is opened. */
  bool Load(const char* dir);

  /** Return the number of files in the current directory. */
  int GetNumberOfFiles() { return m_Files.size();}

  /** Return the file at the given index, the indexing is 0 based */
  const char* GetFile(int index);

protected:
  //template class ALBA_EXPORT std::allocator<std::string>;
  //template class ALBA_EXPORT std::vector<std::string, std::allocator<std::string>>;
  std::vector<wxString> m_Files; // Array of Files
	wxString m_Path;               // Path to Open'ed directory
private:
  albaDirectory(const albaDirectory&); //purposely not implemented
  void operator=(const albaDirectory&); //purposely not implemented
}; 
  
#endif
