/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDirectory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:31 $
  Version:   $Revision: 1.5 $
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
  std::vector<std::string> m_Files; // Array of Files
  std::string m_Path;               // Path to Open'ed directory
private:
  mafDirectory(const mafDirectory&); //purposely not implemented
  void operator=(const mafDirectory&); //purposely not implemented
}; 
  
#endif
