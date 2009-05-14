/**=======================================================================
  
  File:    	 vtkMAFFile.h
  Language:  C++
  Date:      11:2:2008   12:36
  Version:   $Revision: 1.1.2.1 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This class provides a platform independent support for reading/writing
files larger than 2GB that are not supported by C++ iostream classes 
under _WIN32 (although they are supported for _WIN64)
=========================================================================*/

#ifndef vtkMAFFile_h__
#define vtkMAFFile_h__

#include <stdio.h>
#include <errno.h>
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#pragma once

//This is nothrow version
class VTK_COMMON_EXPORT vtkMAFFile : public vtkObject
{
protected:
#ifdef _WIN32
  HANDLE m_hFile;   //associated file HANDLE
#else
  FILE* m_pFile;    //associated FILE
#endif // _WIN32

public:
  vtkTypeRevisionMacro(vtkMAFFile, vtkObject);
  static vtkMAFFile* New();

protected:
  vtkMAFFile() {
#ifdef _WIN32
    m_hFile = INVALID_HANDLE_VALUE;
#else
    m_pFile = NULL;
#endif // _WIN32
  }

  ~vtkMAFFile() {
    Close();
  }

private:
  vtkMAFFile(const vtkMAFFile&);          // Not implemented.
  void operator = (const vtkMAFFile&);    // Not implemented.	


public:
  //creates a new file
  //returns false if an error occurs
  bool Create(const char* fname);

  //opens an existing file for R/W or RO (bRO == true)  
  //returns false if an error occurs
  bool Open(const char* fname, bool bRO = true);

  //reads count bytes from the file into buffer
  //returns number of bytes read
  inline int Read(void* buffer, int count);   

  //writes count bytes from the buffer into the file
  //returns number of bytes written
  inline int Write(void* buffer, int count);  

  //closes the file (with no exception!) and invalidates it
  inline void Close();

  //set the current position in file to a new location according to
  //pos and origin. If origin is SEEK_CUR - pos is given as the distance 
  //from the current position, SEEK_END - pos is the distance from the 
  //end of file. SEEK_SET (default) - from the beginning of file
  //returns false, if an error occurs
  inline bool Seek(long long pos, int origin = SEEK_SET);

  //returns the current position in file, 
  //returns -1, if an error occurs
  inline long long GetCurrentPos() throw(...);

  //returns the current file length, 
  //returns -1, if an error occurs
  long long GetFileSize();

  //returns file size or -1, if an error occurs (e.g., because file not found)
  static long long GetFileSize(const char* fname);
};


//closes the file (with no exception!) and invalidates it
inline void vtkMAFFile::Close()
{
#ifdef _WIN32
  if (m_hFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
  }
#else
 if (m_pFile != NULL)
  {
    fclose(m_pFile);
    m_pFile = NULL;
  }
#endif // _WIN32
}

//reads count bytes from the file into buffer
//returns number of bytes read
inline int vtkMAFFile::Read(void* buffer, int count) 
{
#ifdef _WIN32
  DWORD dwRead;
  if (!ReadFile(m_hFile, buffer, (DWORD)count, &dwRead, NULL))
    dwRead = 0; //error => set dwRead to zero

  return (int)dwRead;
#else
  return fread(buffer, 1, count, m_pFile);
#endif // _WIN32
}

//writes count bytes from the buffer into the file
//returns number of bytes written
inline int vtkMAFFile::Write(void* buffer, int count)
{
#ifdef _WIN32
  DWORD dwWritten;
  WriteFile(m_hFile, buffer, (DWORD)count, &dwWritten, NULL);
  return (int)dwWritten;
#else
  return fwrite(buffer, 1, count, m_pFile);
#endif // _WIN32
}

//set the current position in file to pos
inline bool vtkMAFFile::Seek(long long pos, int origin)
{
#ifdef _WIN32  
  return FALSE != SetFilePointerEx(m_hFile, 
      *((LARGE_INTEGER*)&pos), NULL, (DWORD)origin);  
#else
  return fseeko64( m_pFile, (off64_t)pos, origin ) >= 0;  
#endif  
}

//returns the current position in file, throws an exception if an error occurs
inline long long vtkMAFFile::GetCurrentPos() throw(...)
{
#ifdef _WIN32
  LARGE_INTEGER liCurPos;
  liCurPos.QuadPart = 0;
  if (FALSE == SetFilePointerEx(m_hFile, liCurPos, &liCurPos, FILE_CURRENT))
    return (long long)-1;  //error

  return (long long)liCurPos.QuadPart;
//  __int64 res = _ftelli64( m_pFile);  
#else
  fpos64_t res;  
  if (fgetpos64(m_pFile, &res) < 0)  
    res = (fpos64_t)-1;

  return (long long)res;
#endif        
}



//This class is vtkMAFFile that throws exceptions when something goes wrong  
class VTK_COMMON_EXPORT vtkMAFFile2 : public vtkMAFFile
{
public:
  vtkTypeRevisionMacro(vtkMAFFile2, vtkMAFFile);
  static vtkMAFFile2* New();

protected:
  vtkMAFFile2() {
#ifdef _WIN32
    m_hFile = INVALID_HANDLE_VALUE;
#else
    m_pFile = NULL;
#endif // _WIN32
  }

  ~vtkMAFFile2() {
    Close();
  }

private:
  vtkMAFFile2(const vtkMAFFile2&);          // Not implemented.
  void operator = (const vtkMAFFile2&);    // Not implemented.	


public:
  //creates a new file
  //throws std::exceptions if an error occurs
  void Create(const char* fname) throw(...);

  //opens an existing file for R/W or RO (bRO == true)  
  //throws std::exceptions if an error occurs
  void Open(const char* fname, bool bRO = true) throw(...);

	//reads count bytes from the file into buffer
	//throws std::exceptions if an error occurs
	inline void Read(void* buffer, int count) throw(...);

	//writes count bytes from the buffer into the file
	//throws std::exceptions if an error occurs
	inline void Write(void* buffer, int count) throw(...);	

	//set the current position in file to a new location according to
  //pos and origin. If origin is SEEK_CUR - pos is given as the distance 
  //from the current position, SEEK_END - pos is the distance from the 
  //end of file. SEEK_SET (default) - from the beginning of file
	inline void Seek(long long pos, int origin = SEEK_SET) throw(...);

  //returns the current position in file, throws an exception if an error occurs
  inline long long GetCurrentPos() throw(...);

  //returns the current file length, throws an exception if an error occurs
  long long GetFileSize() throw(...);

	//returns file size, throwing exception if an error occurs (e.g., file not found)
	static long long GetFileSize(const char* fname) throw(...);
};

//writes count bytes from the buffer into the file
//throws std::exceptions if an error occurs
inline void vtkMAFFile2::Write(void* buffer, int count) throw(...)
{
#ifdef _WIN32
  DWORD dwWritten;
  if (!WriteFile(m_hFile, buffer, (DWORD)count, &dwWritten, NULL))
#else
  if (fwrite(buffer, 1, count, m_pFile) != count)
#endif // _WIN32	
	{
		throw std::ios::failure(
			(
#ifdef _WIN32
      GetLastError() == ERROR_DISK_FULL
#else
      errno == ENOSPC 
#endif
      ? ("There is not enough space on disk.") :
		  ("Unspecified I/O error while storing data."))
			);	
	}
}

//reads count bytes from the file into the buffer
//throws std::exceptions if an error occurs
inline void vtkMAFFile2::Read(void* buffer, int count) throw(...)
{
#ifdef _WIN32
  DWORD dwRead;
  if (!ReadFile(m_hFile, buffer, (DWORD)count, &dwRead, NULL))
    throw std::ios::failure(("Unspecified I/O error while reading data."));

  if (dwRead != (DWORD)count)
    throw std::ios::failure(("Reached the end of the file (EOF). The file is corrupted and unreadable."));  
#else
	int r = fread(buffer, 1, count, m_pFile);
	if (r != count)
	{
		throw std::ios::failure((feof(m_pFile) ?
			("Reached the end of the file (EOF). The file is corrupted and unreadable.") :
      ("Unspecified I/O error while reading data."))
			);	
	}
#endif // _WIN32
}

//set the current position in file to pos
inline void vtkMAFFile2::Seek(long long pos, int origin) throw(...)
{
  if (!vtkMAFFile::Seek(pos, origin))
    throw std::ios::failure(("Unable to seek in the specified file."));
}

//returns the current position in file, throws an exception if an error occurs
inline long long vtkMAFFile2::GetCurrentPos() throw(...)
{
  long long ret = vtkMAFFile::GetCurrentPos();
  if (ret < 0)
     throw std::ios::failure(("Unable to seek in the specified file."));

  return ret;
}
#endif // vtkMAFFile_h__
