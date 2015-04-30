/*=========================================================================

 Program: MAF2
 Module: vtkMAFFileDataProvider
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFFileDataProvider_h
#define __vtkMAFFileDataProvider_h

#include "vtkMAFLargeDataProvider.h"
#include "vtkMAFFile.h"

class MAF_EXPORT vtkMAFFileDataProvider : public vtkMAFLargeDataProvider
{	
protected:
  vtkMAFFile* File;				  //<file source, 								
	char* FileName;				    //<file name	
	bool Attached;				    //<true, if the file is attached
	bool CloseAttachedFile;		//<true if the attached file should be closed
	bool DeleteOnClose;			  //<true if the file should be deleted

public:
	//returns filename (the returned pointer may not be released)
	inline const char* GetFileName() {
		return FileName;
	}

	//returns the underlaying file handle
	inline vtkMAFFile* GetFile() {
		return File;
	}

	//returns true, if the file is attached
	inline bool IsAttached() {
		return Attached;
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
