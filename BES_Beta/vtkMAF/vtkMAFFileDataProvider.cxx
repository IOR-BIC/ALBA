/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFFileDataProvider.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma warning(disable: 4996) // deprecated

#include "vtkMAFFileDataProvider.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFFileDataProvider, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFFileDataProvider);

#include "mafMemDbg.h"

//ctor / dtor
vtkMAFFileDataProvider::vtkMAFFileDataProvider() 
{
	this->FileName = NULL;	
  this->File = NULL;
}

vtkMAFFileDataProvider::~vtkMAFFileDataProvider()
{ 
	CloseFile();
	delete[] this->FileName;
}


//Attaches the given file. If bAutoClose is true, the attached file will be closed.
//If bDeleteOnClose is set to true, the file is considered to be temporary and
//will be removed during the close.
/*virtual*/ void vtkMAFFileDataProvider
	::AttachFile(vtkMAFFile* fhandle, const char* fname, bool bAutoClose, bool bDeleteOnClose)
{
	CloseFile();	//close the current underlaying file
	
	this->File = fhandle;
  if (fhandle != NULL)
    fhandle->Register(this);

	if (fname != NULL) {
		this->FileName = new char[strlen(fname) + 1];
		strcpy(this->FileName, fname);
	}

	this->CloseAttachedFile = bAutoClose;
	this->DeleteOnClose = bDeleteOnClose;
}

//Detaches the underlaying file
/*virtual*/ vtkMAFFile* vtkMAFFileDataProvider::DetachFile() 
{
	vtkMAFFile* ret = File;
	File = NULL;

	delete[] FileName;
	FileName = NULL;
	return ret;
}

//Opens the specified file for reading/writing operations
//If the file does not exist and bOpenAlways is set to true, the
//file is created. Underlaying opened file is closed in prior to this operation.
//If bDeleteOnClose is set to true, the file is considered to be temporary and
//will be removed during the close.
//Returns 0 if an error occurs.
/*virtual*/ bool vtkMAFFileDataProvider
	::OpenFile(const char* fname, bool bOpenForRO, bool bDeleteOnClose)
{
	if (fname == NULL) 
	{
		vtkErrorMacro(<< "Filename must be specified.");
		return 0;
	}

	// Close file from any previous call
	CloseFile();

  vtkMAFFile* f = vtkMAFFile::New();
  if (!f->Open(fname, bOpenForRO))
  {
    if (bOpenForRO || !f->Create(fname))
    {
      f->Delete();
      return false;
    }
  }

	AttachFile(f, fname, true, bDeleteOnClose);
	this->Attached = false;
  f->Delete();  //AttachFile increased reference
	return true;
}

//Closes the underlaying file.
/*virtual*/ void vtkMAFFileDataProvider::CloseFile()
{
	if (this->File != NULL)
	{
		if (!this->Attached || this->CloseAttachedFile) {
			this->File->Close();

			if (this->DeleteOnClose)
#pragma warning(suppress: 6031) // warning C6031: Return value ignored: '_unlink'
				_unlink(FileName);
		}

    this->File->UnRegister(this);
		this->File = NULL;
	}

	delete[] FileName;
	FileName = NULL;
}


//Copies the binary data from the underlaying source into the given buffer. 
//Copying starts at startOffset position and at most count bytes are copied.
//The routine returns number of bytes successfully transfered.  
/*virtual*/ int vtkMAFFileDataProvider
	::ReadBinaryData(vtkIdType64 startOffset, void* buffer, int count)
{
	if (!Seek(startOffset))
		return 0;

	return this->File->Read(buffer, count);
}

//Copies the binary data from the given buffer into the underlaying data set at
//startOffset position. If the underlaying data set is not capable to hold the
//specified amount of bytes to be copied (count), it is automatically enlarge.	
//The routine returns number of bytes successfully transfered.	
/*virtual*/ int vtkMAFFileDataProvider
	::WriteBinaryData(vtkIdType64 startOffset, void* buffer, int count)
{
	if (!Seek(startOffset))
		return 0;

	return this->File->Write(buffer, count);
}

//Seeks the underlaying file
/*virtual*/ bool vtkMAFFileDataProvider::Seek(vtkIdType64 startOffset)
{
  return this->File->Seek(startOffset);	 
}