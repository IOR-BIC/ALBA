/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDirectory.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-09 15:31:00 $
  Version:   $Revision: 1.1 $
  Authors:   Based on itkDirectory (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDirectory.h"


//------------------------------------------------------------------------------
mafDirectory::mafDirectory() 
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafDirectory::~mafDirectory() 
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
// First microsoft compilers
//------------------------------------------------------------------------------

#ifdef _MSC_VER
#include <windows.h>
#include <winbase.h>
#include <io.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

//------------------------------------------------------------------------------
bool mafDirectory::Load(const char* name)
//------------------------------------------------------------------------------
{
  char* buf;
  int n = static_cast<int>( strlen(name) );
  if ( name[n - 1] == '/' ) 
  {
    buf = new char[n + 1 + 1];
    sprintf(buf, "%s*", name);
  } 
  else
  {
    buf = new char[n + 2 + 1];
    sprintf(buf, "%s/*", name);
  }
  struct _finddata_t data;  // data of current file
  
  // Now put them into the file array
  long srchHandle = _findfirst(buf, &data);
  delete [] buf;
  
  if ( srchHandle == -1 )
  {
    return 0;
  }
  
  // Loop through names
  do 
  {
    m_Files.push_back(data.name);
  } 
  while ( _findnext(srchHandle, &data) != -1 );
  m_Path = name;
  return _findclose(srchHandle) != -1;
}

#else

//------------------------------------------------------------------------------
// Now the POSIX style directory access
//------------------------------------------------------------------------------

#include <sys/types.h>
#include <dirent.h>

//------------------------------------------------------------------------------
bool mafDirectory::Load(const char* name)
//------------------------------------------------------------------------------
{
  DIR* dir = opendir(name);
  if ( !dir ) 
  {
    return 0;
  }
  for (dirent* d = readdir(dir); d; d = readdir(dir) )
  {
    m_Files.push_back(d->d_name);
  }
  m_Path = name;
  closedir(dir);
  return 1;
}

#endif

//------------------------------------------------------------------------------
const char* mafDirectory::GetFile(int index)
//------------------------------------------------------------------------------
{
  if ( index >= m_Files.size() )
  {
    //mafGenericOutputMacro( << "Bad index for GetFile on maf::mafDirectory\n");
    return 0;
  }
  
  return m_Files[index].c_str();
}
