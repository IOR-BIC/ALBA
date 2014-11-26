/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFMultiFileDataProvider.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  This large data provider contains data in one or more files.
  It can be used to load a set of RAW images.
  =========================================================================
*/
#ifndef vtkMAFMultiFileDataProvider_h__
#define vtkMAFMultiFileDataProvider_h__

#pragma once
#include "mafConfigure.h"
#include "vtkMAFLargeDataProvider.h"
#include "vtkMAFFile.h"

class MAF_EXPORT vtkMAFMultiFileDataProvider : public vtkMAFLargeDataProvider
{
protected:
  //describes one file 
  typedef struct FILE_DESC  //<this structure holds the information for one file
  {
    char* szFileName;           //<file name
    vtkMAFFile* pFile;				  //<file source,    
    
    vtkIdType64 nStartOffset;   //<global offset of the first data item that is in this file
    vtkIdType64 nCorrection;    //<correction to be applied to convert global offset into local 

    FILE_DESC *pLast, *pNext;   //<links 
  } FILE_DESC;

  FILE_DESC* PFDHead;       //<header of list of file descriptors
  FILE_DESC* PFDTail;       //<tail of list of file descriptors
  FILE_DESC* PFDLastUsed;   //<last used file descriptor

  //the size of header in bytes for the 2nd+ file
  //the header for the first file is in HeaderSize  
  vtkIdType64 HeaderSize2;
  
  //global offsets
  vtkTimeStamp OffsetsComputeTime;

  char* InternalFileName;     //<Internal name
  char* FilePattern;          //<pattern used to create file names
  int FileNameSliceOffset;    //<the initial slice number
  int FileNameSliceSpacing;   //<the spacing in numbering
   
  bool ReadOnly;            //<true, if files should be opened in RO mode
  bool DeleteOnClose;			  //<true, if files should be deleted when closed
 
//getter/setters
public:
  //Get/Sets the header size (in bytes) for the other files
  //N.B. unlike HeaderSize, the value set to HeaderSize
  //is applied even if DefaultLayout is turned off  
  inline virtual vtkIdType64 GetHeaderSize2() {
    return HeaderSize2;
  }

  inline virtual void SetHeaderSize2(vtkIdType64 header) 
  {
    if (HeaderSize2 != header) {
      HeaderSize2 = header;
      this->Modified();
    }
  }

  //Get/Set if the file should be deleted during close operation
  vtkGetMacro(DeleteOnClose, bool);
  vtkSetMacro(DeleteOnClose, bool);

  /** Returns the name of file with index nFile 
  N.B. the returned pointer may not be released */
  virtual const char* GetInternalFileName(int nFile);

public:
  vtkTypeRevisionMacro(vtkMAFMultiFileDataProvider, vtkMAFLargeDataProvider);
  static vtkMAFMultiFileDataProvider* New();

public:
  /** Opens the multi file for reading/writing operations
  szFilePatter is sprintf format string that is used to create names for files 
  it is supposed that the string is valid and contains only one %d (or similar)
  entry that will be replaced by formatted file index.
  File indices goes from nFileNameSliceOffset with the increment 
  nFileNameSliceSpacing between two indices. There is nFileCount files. 
  For instance, if szFilePattern is 'd:\av_%04d.raw', nFileCount is 3,
  nFileNameSliceOffset is 1 and nFileNameSliceSpacing is 2, then files:
  'd:\av_0001.raw', 'd:\av_0003.raw' and 'd:\av_0005.raw' are opened.
  All files that match the given constraints are opened for reading
  and, if bOpenForRO is false, also for writing operations.  
  If bDeleteOnClose is set to true, the multi file is considered to be temporary and
  all files will be removed during the close.
  If there is already opened multi file, it is close first.
  Returns false if an error occurs.
  */
  virtual bool OpenMultiFile(const char* szFilePattern, int nFileCount,
    int nFileNameSliceOffset = 0, int nFileNameSliceSpacing = 1,
    bool bOpenForRO = true, bool bDeleteOnClose = false);

  //Closes all underlaying files.
  virtual void CloseMultiFile();

  //Copies the binary data from the underlaying source into the given buffer. 
  //Copying starts at startOffset position and at most count bytes are copied.
  //The routine returns number of bytes successfully transfered.  
  virtual int ReadBinaryData(vtkIdType64 startOffset, void* buffer, int count);

  //Copies the binary data from the given buffer into the underlaying data set at
  //startOffset position. If the underlaying data set is not capable to hold the
  //specified amount of bytes to be copied (count), it is automatically enlarge.	
  //The routine returns number of bytes successfully transfered.	
  virtual int WriteBinaryData(vtkIdType64 startOffset, void* buffer, int count); 	

protected:
  vtkMAFMultiFileDataProvider(void);
  virtual ~vtkMAFMultiFileDataProvider(void);

  /** Seeks the underlaying multi-file
  It searches for the file with the given startOffset and set it to m_pFDLastUsed
  Returns number of bytes that can be read / written from the m_pFDLastUsed file
  (at most count, which is the number of requested bytes); or -1, if an error occurs*/
  virtual int Seek(vtkIdType64 startOffset, int count);

  /** Updates the information in m_pFDxxx list */
  virtual void ComputeOffsets();

private:
  vtkMAFMultiFileDataProvider(const vtkMAFMultiFileDataProvider&);  // Not implemented.
  void operator = (const vtkMAFMultiFileDataProvider&);  // Not implemented.	
};
#endif // vtkMAFMultiFileDataProvider_h__
