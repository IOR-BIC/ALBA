/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFMultiFileDataProvider.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-09-14 15:55:39 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFMultiFileDataProvider.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFMultiFileDataProvider, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMAFMultiFileDataProvider);

#include "mafMemDbg.h"

vtkMAFMultiFileDataProvider::vtkMAFMultiFileDataProvider(void)
{
  m_HeaderSize2 = 0;
  m_PFDHead = m_PFDTail = m_PFDLastUsed = NULL;
  m_InternalFileName = NULL;
  m_FilePattern = NULL;
  m_FileNameSliceOffset = 0;
  m_FileNameSliceSpacing = 1;
  m_ReadOnly = true;
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
  m_FilePattern = _strdup(szFilePattern);
#else
  int nLen = (int)strlen(szFilePattern);
  m_FilePattern = new char[nLen + 1];
  strcpy(m_FilePattern, szFilePattern);
#endif

  m_FileNameSliceOffset = nFileNameSliceOffset;
  m_FileNameSliceSpacing = nFileNameSliceSpacing;
  m_ReadOnly = bOpenForRO;
  DeleteOnClose = bDeleteOnClose;

  for (int i = 0; i < nFileCount; i++)
  {
    m_PFDLastUsed = new FILE_DESC;
    memset(m_PFDLastUsed, 0, sizeof(*m_PFDLastUsed));

    if (m_PFDTail != NULL)
      m_PFDTail->pNext = m_PFDLastUsed;
    else
      m_PFDHead = m_PFDLastUsed;
    m_PFDLastUsed->pLast = m_PFDTail;
    m_PFDTail = m_PFDLastUsed;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    m_PFDLastUsed->szFileName = _strdup(GetInternalFileName(i));
#else
    GetInternalFileName(i); //force construction of InternalFileName
    nLen = (int)strlen(m_InternalFileName);
    m_PFDLastUsed->szFileName = new char[nLen + 1];
    strcpy(m_PFDLastUsed->szFileName, m_InternalFileName);
#endif

    m_PFDLastUsed->pFile = vtkMAFFile::New();
    if (!m_PFDLastUsed->pFile->Open(m_PFDLastUsed->szFileName, m_ReadOnly))
    {
      vtkWarningMacro(<< "Unable to open file: '" << m_PFDLastUsed->szFileName << "'");
      return false; //error, unable to write file
    }
  }

  if (m_InternalFileName != NULL)
  {
    delete[] m_InternalFileName;
    m_InternalFileName = NULL;
  }

  this->Modified();
  return true;
}

//------------------------------------------------------------------------
//Closes all underlaying files.
/*virtual*/ void vtkMAFMultiFileDataProvider::CloseMultiFile()
//------------------------------------------------------------------------
{
  while (m_PFDHead != NULL)
  {
    m_PFDLastUsed = m_PFDHead;
    if (m_PFDLastUsed->pFile != NULL)
    {
      m_PFDLastUsed->pFile->Delete();       //this will close the file as well
      if (DeleteOnClose) {                  //temporary file, remove it
#pragma warning(suppress: 6031) // warning C6031: Return value ignored: '_unlink'
        _unlink(m_PFDLastUsed->szFileName);
      }
    }

    if (m_PFDLastUsed->szFileName != NULL) {
      delete[] m_PFDLastUsed->szFileName;
    }

    m_PFDHead = m_PFDHead->pNext;
    delete m_PFDLastUsed;
  }
  m_PFDTail = m_PFDLastUsed = NULL;

  if (m_FilePattern != NULL)
  {
    delete[] m_FilePattern;
    m_FilePattern = NULL;
  }

  if (m_InternalFileName != NULL)
  {
    delete[] m_InternalFileName;
    m_InternalFileName = NULL;
  }
}

//------------------------------------------------------------------------
//Returns the name of file with index nFile 
//N.B. the returned pointer may not be released
/*virtual*/ const char* vtkMAFMultiFileDataProvider::GetInternalFileName(int nFile)
//------------------------------------------------------------------------
{
  // delete any old filename
  if (this->m_InternalFileName)
  {
    delete [] this->m_InternalFileName;
    this->m_InternalFileName = NULL;
  }
  
  int slicenum = nFile * this->m_FileNameSliceSpacing + this->m_FileNameSliceOffset;       
  int len = _scprintf(this->m_FilePattern, slicenum) + 1;        
  this->m_InternalFileName = new char [len];  

#if defined(_MSC_VER) && _MSC_VER >= 1400
  sprintf_s(this->m_InternalFileName, len, 
#else
  sprintf(this->m_InternalFileName, 
#endif            
    this->m_FilePattern, slicenum);

  return this->m_InternalFileName;
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

  if (m_PFDLastUsed == NULL)
    return -1;  //fatal error

  if (m_PFDLastUsed->nStartOffset <= startOffset)
  {
    //search forward
    while (m_PFDLastUsed->pNext != NULL &&
      m_PFDLastUsed->pNext->nStartOffset <= startOffset) {
        m_PFDLastUsed = m_PFDLastUsed->pNext;   //advance to the next
    }
  }
  else
  {
    //search backward
    do
    {
      m_PFDLastUsed = m_PFDLastUsed->pLast;
      if (m_PFDLastUsed == NULL)
        return -1;  //fatal error
    }
    while (m_PFDLastUsed->nStartOffset > startOffset);
  }

  //m_pFDLastUsed now contains the description of file with startOffset
  int nBytesAvailable;
  if (m_PFDLastUsed->pNext != NULL)
    nBytesAvailable = m_PFDLastUsed->pNext->nStartOffset - startOffset;
  else
    nBytesAvailable = INT_MAX;  //read as much as possible
  
  if (nBytesAvailable > count)
    nBytesAvailable = count;
  
  if (!m_PFDLastUsed->pFile->Seek(startOffset + m_PFDLastUsed->nCorrection, SEEK_SET))
    return -1;  //seek error!

  return nBytesAvailable;
}

//------------------------------------------------------------------------
//Updates the information in m_pFDxxx list
/*virtual*/ void vtkMAFMultiFileDataProvider::ComputeOffsets()
//------------------------------------------------------------------------
{
  if (GetMTime() <= m_OffsetsComputeTime) {
    return; //no change
  }

  //as Seek is called with startOffset that is already shifted by HeaderSize
  //there is no correction for the offset of the first file
  //for others, the correction = -nStartOffset + HeaderSize2    
  vtkIdType64 nOfs = m_HeaderSize;
  vtkIdType64 nCor = m_HeaderSize;
  
  m_PFDLastUsed = m_PFDHead;
  while (m_PFDLastUsed != NULL)
  {
    m_PFDLastUsed->nStartOffset = nOfs;
    m_PFDLastUsed->nCorrection = nCor - nOfs;
        
    nOfs += (vtkIdType64)m_PFDLastUsed->pFile->GetFileSize() - nCor;
    nCor = m_HeaderSize2;

    m_PFDLastUsed = m_PFDLastUsed->pNext;
  }

  m_PFDHead->nStartOffset = 0;     

  m_PFDLastUsed = m_PFDHead;
  m_OffsetsComputeTime.Modified();
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

    int nRead = m_PFDLastUsed->pFile->Read(pBuf, nAvail);
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

    int nWritten = m_PFDLastUsed->pFile->Write(pBuf, nAvail);
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