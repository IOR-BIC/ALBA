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
#include "albaDirectory.h"

albaCxxTypeMacro(albaDirectory);

//------------------------------------------------------------------------------
albaDirectory::albaDirectory() 
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaDirectory::~albaDirectory() 
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
bool albaDirectory::Load(const char* name)
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
	intptr_t srchHandle = _findfirst(buf, &data);
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
bool albaDirectory::Load(const char* name)
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
const char* albaDirectory::GetFile(int index)
//------------------------------------------------------------------------------
{
  if ( index >= m_Files.size() )
  {
    //albaGenericOutputMacro( << "Bad index for GetFile on alba::albaDirectory\n");
    return 0;
  }
  
  return m_Files[index].char_str();
}
