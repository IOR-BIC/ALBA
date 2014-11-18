/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFMultiFileDataProvider.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-11-04 10:36:29 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFMultiFileDataProvider.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFMultiFileDataProvider, "$Revision: 1.1.2.4 $");
vtkStandardNewMacro(vtkMAFMultiFileDataProvider);

#include "mafMemDbg.h"

vtkMAFMultiFileDataProvider::vtkMAFMultiFileDataProvider(void)
{
  HeaderSize2 = 0;
  PFDHead = PFDTail = PFDLastUsed = NULL;
  InternalFileName = NULL;
  FilePattern = NULL;
  FileNameSliceOffset = 0;
  FileNameSliceSpacing = 1;
  ReadOnly = true;
  DeleteOnClose = false;    
}

vtkMAFMultiFileDataProvider::~vtkMAFMultiFileDataProvider(void)
{
  CloseMultiFile(); 
}

//------------------------------------------------------------------------
//Opens the multi file for reading/writing operations
//szFilePatter is sprintf format string that is used to create names for files 
//it is supposed that the string is valid and contains only one %d (or similar)
//entry that will be replaced by formatted file index.
//File indices goes from nFileNameSliceOffset with the increment 
//nFileNameSliceSpacing between two indices. There is nFileCount files. 
//For instance, if szFilePattern is 'd:\av_%04d.raw', nFileCount is 3,
//nFileNameSliceOffset is 1 and nFileNameSliceSpacing is 2, then files:
//'d:\av_0001.raw', 'd:\av_0003.raw' and 'd:\av_0005.raw' are opened.
//All files that match the given constraints are opened for reading
//and, if bOpenForRO is false, also for writing operations.  
//If bDeleteOnClose is set to true, the multi file is considered to be temporary and
//all files will be removed during the close.
//If there is already opened multi file, it is close first.
//Returns false if an error occurs.
/*virtual*/ bool vtkMAFMultiFileDataProvider::OpenMultiFile(const char* szFilePattern, int nFileCount,
                           int nFileNameSliceOffset, int nFileNameSliceSpacing,
                           bool bOpenForRO, bool bDeleteOnClose)
//------------------------------------------------------------------------
{
  CloseMultiFile();

#if defined(_MSC_VER) && _MSC_VER >= 1400
  FilePattern = _strdup(szFilePattern);
#else
  int nLen = (int)strlen(szFilePattern);
  FilePattern = new char[nLen + 1];
  strcpy(FilePattern, szFilePattern);
#endif

  FileNameSliceOffset = nFileNameSliceOffset;
  FileNameSliceSpacing = nFileNameSliceSpacing;
  ReadOnly = bOpenForRO;
  DeleteOnClose = bDeleteOnClose;

  for (int i = 0; i < nFileCount; i++)
  {
    PFDLastUsed = new FILE_DESC;
    memset(PFDLastUsed, 0, sizeof(*PFDLastUsed));

    if (PFDTail != NULL)
      PFDTail->pNext = PFDLastUsed;
    else
      PFDHead = PFDLastUsed;
    PFDLastUsed->pLast = PFDTail;
    PFDTail = PFDLastUsed;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    PFDLastUsed->szFileName = _strdup(GetInternalFileName(i));
#else
    GetInternalFileName(i); //force construction of InternalFileName
    nLen = (int)strlen(InternalFileName);
    PFDLastUsed->szFileName = new char[nLen + 1];
    strcpy(PFDLastUsed->szFileName, InternalFileName);
#endif

    PFDLastUsed->pFile = vtkMAFFile::New();
    if (!PFDLastUsed->pFile->Open(PFDLastUsed->szFileName, ReadOnly))
    {
      vtkWarningMacro(<< "Unable to open file: '" << PFDLastUsed->szFileName << "'");
      return false; //error, unable to write file
    }
  }

  if (InternalFileName != NULL)
  {
    delete[] InternalFileName;
    InternalFileName = NULL;
  }

  this->Modified();
  return true;
}

//------------------------------------------------------------------------
//Closes all underlaying files.
/*virtual*/ void vtkMAFMultiFileDataProvider::CloseMultiFile()
//------------------------------------------------------------------------
{
  while (PFDHead != NULL)
  {
    PFDLastUsed = PFDHead;
    if (PFDLastUsed->pFile != NULL)
    {
      PFDLastUsed->pFile->Delete();       //this will close the file as well
      if (DeleteOnClose) {                  //temporary file, remove it
#pragma warning(suppress: 6031) // warning C6031: Return value ignored: '_unlink'
        _unlink(PFDLastUsed->szFileName);
      }
    }

    if (PFDLastUsed->szFileName != NULL) {
      delete[] PFDLastUsed->szFileName;
    }

    PFDHead = PFDHead->pNext;
    delete PFDLastUsed;
  }
  PFDTail = PFDLastUsed = NULL;

  if (FilePattern != NULL)
  {
    delete[] FilePattern;
    FilePattern = NULL;
  }

  if (InternalFileName != NULL)
  {
    delete[] InternalFileName;
    InternalFileName = NULL;
  }
}

//------------------------------------------------------------------------
//Returns the name of file with index nFile 
//N.B. the returned pointer may not be released
/*virtual*/ const char* vtkMAFMultiFileDataProvider::GetInternalFileName(int nFile)
//------------------------------------------------------------------------
{
  // delete any old filename
  if (this->InternalFileName)
  {
    delete [] this->InternalFileName;
    this->InternalFileName = NULL;
  }
  
  int slicenum = nFile * this->FileNameSliceSpacing + this->FileNameSliceOffset;       
  int len = _scprintf(this->FilePattern, slicenum) + 1;        
  this->InternalFileName = new char [len];  

#if defined(_MSC_VER) && _MSC_VER >= 1400
  sprintf_s(this->InternalFileName, len, 
#else
  sprintf(this->InternalFileName, 
#endif            
    this->FilePattern, slicenum);

  return this->InternalFileName;
}

//------------------------------------------------------------------------
//Seeks the underlaying multi-file
//It searches for the file with the given startOffset and set it to m_pFDLastUsed
//Returns number of bytes that can be read / written from the m_pFDLastUsed file 
//(at most count, which is the number of requested bytes); or -1, if an error occurs
/*virtual*/ int vtkMAFMultiFileDataProvider::Seek(vtkIdType64 startOffset, int count)
//------------------------------------------------------------------------
{
  //RELEASE NOTE: startOffset is already shifted by HeaderSize
  ComputeOffsets();

  if (PFDLastUsed == NULL)
    return -1;  //fatal error

  if (PFDLastUsed->nStartOffset <= startOffset)
  {
    //search forward
    while (PFDLastUsed->pNext != NULL &&
      PFDLastUsed->pNext->nStartOffset <= startOffset) {
        PFDLastUsed = PFDLastUsed->pNext;   //advance to the next
    }
  }
  else
  {
    //search backward
    do
    {
      PFDLastUsed = PFDLastUsed->pLast;
      if (PFDLastUsed == NULL)
        return -1;  //fatal error
    }
    while (PFDLastUsed->nStartOffset > startOffset);
  }

  //m_pFDLastUsed now contains the description of file with startOffset
  int nBytesAvailable;
  if (PFDLastUsed->pNext != NULL)
    nBytesAvailable = PFDLastUsed->pNext->nStartOffset - startOffset;
  else
    nBytesAvailable = INT_MAX;  //read as much as possible
  
  if (nBytesAvailable > count)
    nBytesAvailable = count;
  
  if (!PFDLastUsed->pFile->Seek(startOffset + PFDLastUsed->nCorrection, SEEK_SET))
    return -1;  //seek error!

  return nBytesAvailable;
}

//------------------------------------------------------------------------
//Updates the information in m_pFDxxx list
/*virtual*/ void vtkMAFMultiFileDataProvider::ComputeOffsets()
//------------------------------------------------------------------------
{
  if (GetMTime() <= OffsetsComputeTime) {
    return; //no change
  }

  //as Seek is called with startOffset that is already shifted by HeaderSize
  //there is no correction for the offset of the first file
  //for others, the correction = -nStartOffset + HeaderSize2    
  vtkIdType64 nOfs = HeaderSize;
  vtkIdType64 nCor = HeaderSize;
  
  PFDLastUsed = PFDHead;
  while (PFDLastUsed != NULL)
  {
    PFDLastUsed->nStartOffset = nOfs;
    PFDLastUsed->nCorrection = nCor - nOfs;
        
    nOfs += (vtkIdType64)PFDLastUsed->pFile->GetFileSize() - nCor;
    nCor = HeaderSize2;

    PFDLastUsed = PFDLastUsed->pNext;
  }

  PFDHead->nStartOffset = 0;     

  PFDLastUsed = PFDHead;
  OffsetsComputeTime.Modified();
}

//------------------------------------------------------------------------
//Copies the binary data from the underlaying source into the given buffer. 
//Copying starts at startOffset position and at most count bytes are copied.
//The routine returns number of bytes successfully transfered.  
/*virtual*/ int vtkMAFMultiFileDataProvider::ReadBinaryData(vtkIdType64 startOffset, 
                                                            void* buffer, int count)
//------------------------------------------------------------------------
{
  //RELEASE NOTE: startOffset is already shifted by HeaderSize
  BYTE* pBuf = (BYTE*)buffer;
  int nTotalRead = 0;

  while (count != 0)
  {
    int nAvail = Seek(startOffset, count);
    if (nAvail <= 0)
      break;  //error

    int nRead = PFDLastUsed->pFile->Read(pBuf, nAvail);
    if (nRead == 0)
      break;  //EOF
    
    //advance counters
    startOffset += nRead;
    nTotalRead += nRead;    
    count -= nRead;
    pBuf += nRead;
  }

  return nTotalRead;
}

//------------------------------------------------------------------------
//Copies the binary data from the given buffer into the underlaying data set at
//startOffset position. If the underlaying data set is not capable to hold the
//specified amount of bytes to be copied (count), it is automatically enlarge.	
//The routine returns number of bytes successfully transfered.	
/*virtual*/ int vtkMAFMultiFileDataProvider::WriteBinaryData(vtkIdType64 startOffset, 
                                                             void* buffer, int count)
//------------------------------------------------------------------------
{
  BYTE* pBuf = (BYTE*)buffer;
  int nTotalWritten = 0;

  while (count != 0)
  {
    int nAvail = Seek(startOffset, count);
    if (nAvail <= 0)
      break;  //error

    int nWritten = PFDLastUsed->pFile->Write(pBuf, nAvail);
    if (nWritten == 0)
      break;  //EOF

    //advance counters
    startOffset += nWritten;
    nTotalWritten += nWritten;    
    count -= nWritten;
    pBuf += nWritten;
  }

  return nTotalWritten;
}