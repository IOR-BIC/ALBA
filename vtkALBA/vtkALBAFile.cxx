/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBAFile.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-11-04 10:55:17 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkALBAFile.h"

#if defined(_MSC_VER)
  #include <share.h>
#endif

vtkStandardNewMacro(vtkALBAFile);

vtkStandardNewMacro(vtkALBAFile2);

//creates a new file
//returns false if an error occurs
bool vtkALBAFile::Create(const char* fname) throw(...)
{
  Close();  //close previously associated FILE

#ifdef _WIN32
  //m_pFile = _fsopen(fname, "wb+", _SH_DENYWR);
  m_HFile = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, 
    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
  return (m_HFile != INVALID_HANDLE_VALUE);
#else
  PFile = fopen(fname, "wb+");
  return (PFile != NULL);
#endif  
}

//opens an existing file for R/W or RO (bRO == true)  
//returns false if an error occurs
bool vtkALBAFile::Open(const char* fname, bool bRO) throw(...)
{
  Close();  //close previously associated FILE

#ifdef _WIN32
  //m_pFile = _fsopen(fname, (bRO ? "rb" : "rb+"), _SH_DENYNO);
  m_HFile = CreateFileA(fname, (bRO ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE)), 
    (bRO ? (FILE_SHARE_READ | FILE_SHARE_WRITE) : FILE_SHARE_READ), NULL, 
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
  return (m_HFile != INVALID_HANDLE_VALUE);
#else
  PFile = fopen(fname, (bRO ? "rb" : "rb+"));
  return (PFile != NULL);
#endif  
}

//returns the current file length, 
//returns -1, if an error occurs
long long vtkALBAFile::GetFileSize()
{
#ifdef _WIN32
  LARGE_INTEGER liSize;
  if (!GetFileSizeEx(m_HFile, &liSize))
    return (long long)-1;
  
  return (long long)liSize.QuadPart;
#else
  long long lastpos = GetCurrentPos();
  if (lastpos < 0)
    return lastpos;

  if (!Seek(0, SEEK_END))
    return -1;

  //this should be OK
  long long ret = GetCurrentPos();
  Seek(lastpos, SEEK_SET);
  return ret;
#endif
}

//returns file size or returns -1, if an error occurs
/*static*/ long long vtkALBAFile::GetFileSize(const char* fname)
{
  vtkALBAFile f;
  if (!f.Open(fname, true))
    return -1;

  return f.GetFileSize(); //dtor will close it
}

//creates a new file
//throws std::exceptions if an error occurs
void vtkALBAFile2::Create(const char* fname) throw(...)
{
  if (!vtkALBAFile::Create(fname))
  {
    throw std::ios::failure(
      (const char*)albaString::Format(_("Cannot create '%s'. Error: %d"), fname, errno));
  }
}

//opens an existing file for R/W or RO (bRO == true)  
//throws std::exceptions if an error occurs
void vtkALBAFile2::Open(const char* fname, bool bRO) throw(...)
{
  if (!vtkALBAFile::Open(fname))
  {
    throw std::ios::failure(
      (const char*)albaString::Format(_("Cannot open '%s'. Error: %d"), fname, errno));
  }
}

//returns the current file length, throws an exception if an error occurs
long long vtkALBAFile2::GetFileSize() throw(...)
{
#ifdef _WIN32
  LARGE_INTEGER liSize;
  if (!GetFileSizeEx(m_HFile, &liSize))
    throw std::ios::failure(("Unable to seek in the specified file."));

  return (long long)liSize.QuadPart;
#else
  unsigned long long lastpos = GetCurrentPos();
  Seek(0, SEEK_END);

  unsigned long long ret = GetCurrentPos();
  Seek(lastpos, SEEK_SET);
  return ret;
#endif
}

//returns file size, throwing exception if an error occurs (e.g., file not found)
/*static*/ long long vtkALBAFile2::GetFileSize(const char* fname) throw(...)
{
  vtkALBAFile2 f;
  f.Open(fname, true);
  return f.GetFileSize(); //dtor will close it
}