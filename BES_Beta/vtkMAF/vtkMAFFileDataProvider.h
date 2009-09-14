/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFFileDataProvider.h,v $ 
  Language: C++ 
  Date: $Date: 2009-09-14 15:55:39 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  This provider keeps large data into an opened file
  =========================================================================*/

#ifndef __vtkMAFFileDataProvider_h
#define __vtkMAFFileDataProvider_h

#include "vtkMAFLargeDataProvider.h"
#include "vtkMAFFile.h"

class VTK_COMMON_EXPORT vtkMAFFileDataProvider : public vtkMAFLargeDataProvider
{	
protected:
  vtkMAFFile* m_File;				  //<file source, 								
	char* m_FileName;				    //<file name	
	bool m_Attached;				    //<true, if the file is attached
	bool CloseAttachedFile;		//<true if the attached file should be closed
	bool DeleteOnClose;			  //<true if the file should be deleted

public:
	//returns filename (the returned pointer may not be released)
	inline const char* GetFileName() {
		return m_FileName;
	}

	//returns the underlaying file handle
	inline vtkMAFFile* GetFile() {
		return m_File;
	}

	//returns true, if the file is attached
	inline bool IsAttached() {
		return m_Attached;
	}

	//Get/Set if attached files can be closed
	vtkGetMacro(CloseAttachedFile, bool);
	vtkSetMacro(CloseAttachedFile, bool);	

	//Get/Set if the file should be deleted during close operation
	vtkGetMacro(DeleteOnClose, bool);
	vtkSetMacro(DeleteOnClose, bool);

public:
	vtkTypeRevisionMacro(vtkMAFFileDataProvider, vtkMAFLargeDataProvider);
	static vtkMAFFileDataProvider* New();


	//Attaches the given file. If bAutoClose is true, the attached file will be closed.
	//If bDeleteOnClose is set to true, the file is considered to be temporary and
	//will be removed during the close.
	virtual void AttachFile(vtkMAFFile* fhandle, const char* fname, bool bAutoClose = true, 
		bool bDeleteOnClose = false);

	//Detaches the underlaying file
	virtual vtkMAFFile* DetachFile();

	//Opens the specified file for reading/writing operations
	//Underlaying opened file is closed in prior to this operation.
	//If bDeleteOnClose is set to true, the file is considered to be temporary and
	//will be removed during the close.
	//Returns false if an error occurs.
	virtual bool OpenFile(const char* fname, bool bOpenForRO = true, 
		bool bDeleteOnClose = false);

	//Closes the underlaying file.
	virtual void CloseFile();

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
	vtkMAFFileDataProvider();
	virtual ~vtkMAFFileDataProvider();

	//Seeks the underlaying file
	//Returns false, if an error occurs
	virtual bool Seek(vtkIdType64 startOffset);

private:
	vtkMAFFileDataProvider(const vtkMAFFileDataProvider&);  // Not implemented.
	void operator = (const vtkMAFFileDataProvider&);  // Not implemented.	
};

#endif
