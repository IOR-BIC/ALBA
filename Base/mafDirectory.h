/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDirectory.h,v $
  Language:  C++
  Date:      $Date: 2007-11-06 12:48:05 $
  Version:   $Revision: 1.7 $
  Authors:   Based on itkDirectory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDirectory_h
#define __mafDirectory_h

#include "mafObject.h"
#include <iostream>
#include <string>
#include <vector>

/** Portable directory/filename traversal.
  mafDirectory provides a portable way of finding the names of the files
  in a system directory.
  mafDirectory works with windows and unix only.  
*/
class MAF_EXPORT mafDirectory : public mafObject
{
public:  
  mafDirectory();
  ~mafDirectory();
  
  mafTypeMacro(mafDirectory,mafObject);

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
  //template class MAF_EXPORT std::allocator<std::string>;
  //template class MAF_EXPORT std::vector<std::string, std::allocator<std::string>>;
  std::vector<std::string> m_Files; // Array of Files
  std::string m_Path;               // Path to Open'ed directory
private:
  mafDirectory(const mafDirectory&); //purposely not implemented
  void operator=(const mafDirectory&); //purposely not implemented
}; 
  
#endif
