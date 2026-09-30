/**=======================================================================
  
  File:    	 vtkALBAFile.h
  Language:  C++
  Date:      11:2:2008   12:36
  Version:   $Revision: 1.1.2.4 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This class provides a platform independent support for reading/writing
files larger than 2GB that are not supported by C++ iostream classes 
under _WIN32 (although they are supported for _WIN64)
=========================================================================*/

#ifndef vtkALBAFile_h__
#define vtkALBAFile_h__

#include "albaConfigure.h"

#include <stdio.h>
#include <errno.h>
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#pragma once

//This is nothrow version
class ALBA_EXPORT vtkALBAFile : public vtkObject
{
protected:
#ifdef _WIN32
  HANDLE m_HFile;   //associated file HANDLE
#else
  FILE* PFile;    //associated FILE
#endif // _WIN32

public:
  vtkTypeMacro(vtkALBAFile, vtkObject);
  static vtkALBAFile* New();

protected:
  vtkALBAFile() {
#ifdef _WIN32
    m_HFile = INVALID_HANDLE_VALUE;
#else
    PFile = NULL;
#endif // _WIN32
  }

  ~vtkALBAFile() {
    Close();
  }

private:
  vtkALBAFile(const vtkALBAFile&);          // Not implemented.
  void operator = (const vtkALBAFile&);    // Not implemented.	


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
inline void vtkALBAFile::Close()
{
#ifdef _WIN32
  if (m_HFile != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_HFile);
    m_HFile = INVALID_HANDLE_VALUE;
  }
#else
 if (PFile != NULL)
  {
    fclose(PFile);
    PFile = NULL;
  }
#endif // _WIN32
}

//reads count bytes from the file into buffer
//returns number of bytes read
inline int vtkALBAFile::Read(void* buffer, int count) 
{
#ifdef _WIN32
  DWORD dwRead;
  if (!ReadFile(m_HFile, buffer, (DWORD)count, &dwRead, NULL))
    dwRead = 0; //error => set dwRead to zero

  return (int)dwRead;
#else
  return fread(buffer, 1, count, PFile);
#endif // _WIN32
}

//writes count bytes from the buffer into the file
//returns number of bytes written
inline int vtkALBAFile::Write(void* buffer, int count)
{
#ifdef _WIN32
  DWORD dwWritten;
  WriteFile(m_HFile, buffer, (DWORD)count, &dwWritten, NULL);
  return (int)dwWritten;
#else
  return fwrite(buffer, 1, count, PFile);
#endif // _WIN32
}

//set the current position in file to pos
inline bool vtkALBAFile::Seek(long long pos, int origin)
{
#ifdef _WIN32  
  return false != SetFilePointerEx(m_HFile, 
      *((LARGE_INTEGER*)&pos), NULL, (DWORD)origin);  
#else
  return fseeko64( PFile, (off64_t)pos, origin ) >= 0;  
#endif  
}

//returns the current position in file, throws an exception if an error occurs
inline long long vtkALBAFile::GetCurrentPos() throw(...)
{
#ifdef _WIN32
  LARGE_INTEGER liCurPos;
  liCurPos.QuadPart = 0;
  if (false == SetFilePointerEx(m_HFile, liCurPos, &liCurPos, FILE_CURRENT))
    return (long long)-1;  //error

  return (long long)liCurPos.QuadPart;
//  __int64 res = _ftelli64( m_pFile);  
#else
  fpos64_t res;  
  if (fgetpos64(PFile, &res) < 0)  
    res = (fpos64_t)-1;

  return (long long)res;
#endif        
}



//This class is vtkALBAFile that throws exceptions when something goes wrong  
class ALBA_EXPORT vtkALBAFile2 : public vtkALBAFile
{
public:
  vtkTypeMacro(vtkALBAFile2, vtkALBAFile);
  static vtkALBAFile2* New();

protected:
  vtkALBAFile2() {
#ifdef _WIN32
    m_HFile = INVALID_HANDLE_VALUE;
#else
    PFile = NULL;
#endif // _WIN32
  }

  ~vtkALBAFile2() {
    Close();
  }

private:
  vtkALBAFile2(const vtkALBAFile2&);          // Not implemented.
  void operator = (const vtkALBAFile2&);    // Not implemented.	


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
inline void vtkALBAFile2::Write(void* buffer, int count) throw(...)
{
#ifdef _WIN32
  DWORD dwWritten;
  if (!WriteFile(m_HFile, buffer, (DWORD)count, &dwWritten, NULL))
#else
  if (fwrite(buffer, 1, count, PFile) != count)
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
inline void vtkALBAFile2::Read(void* buffer, int count) throw(...)
{
#ifdef _WIN32
  DWORD dwRead;
  if (!ReadFile(m_HFile, buffer, (DWORD)count, &dwRead, NULL))
    throw std::ios::failure(("Unspecified I/O error while reading data."));

  if (dwRead != (DWORD)count)
    throw std::ios::failure(("Reached the end of the file (EOF). The file is corrupted and unreadable."));  
#else
	int r = fread(buffer, 1, count, PFile);
	if (r != count)
	{
		throw std::ios::failure((feof(PFile) ?
			("Reached the end of the file (EOF). The file is corrupted and unreadable.") :
      ("Unspecified I/O error while reading data."))
			);	
	}
#endif // _WIN32
}

//set the current position in file to pos
inline void vtkALBAFile2::Seek(long long pos, int origin) throw(...)
{
  if (!vtkALBAFile::Seek(pos, origin))
    throw std::ios::failure(("Unable to seek in the specified file."));
}

//returns the current position in file, throws an exception if an error occurs
inline long long vtkALBAFile2::GetCurrentPos() throw(...)
{
  long long ret = vtkALBAFile::GetCurrentPos();
  if (ret < 0)
     throw std::ios::failure(("Unable to seek in the specified file."));

  return ret;
}
#endif // vtkALBAFile_h__
