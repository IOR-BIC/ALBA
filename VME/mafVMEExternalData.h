/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEExternalData.h,v $
  Language:  C++
  Date:      $Date: 2006-09-20 12:24:03 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEExternalData_h
#define __mafVMEExternalData_h

#include "mafVMEGroup.h"


/** mafVMEExternalData 
*/


class mafVMEExternalData : public mafVMEGroup
{
public:
  mafTypeMacro(mafVMEExternalData,mafVMEGroup);


void PrintSelf(std::ostream& os,const int indent);

  /**
  Set the file extension */
  void SetExtension(const char *ext);

  /**
  Get the file extension */
  const char *GetExtension();

  /**
  Set the file name*/
  void SetFileName(const char *filename);


  /**
  Get the file name */
  const char *GetFileName();

	/**
	Set the mime type*/
	void SetMimeType(const char *mimetype);
  
	/**
	Get the mime type*/
	const char *GetMimeType();

  /**
  Return the current absolute filename for the file: this file name is
  based on the CurrentPath, computed at runtime by the mflVMEStorage class,
  which knows about the full path for the MSF file itself.*/
  mafString GetAbsoluteFileName();
  void GetAbsoluteFileName(char *fname);

  void SetCurrentPath(mafString &path) {this->CurrentPath=path;};
  void SetCurrentPath(const char *path) {this->CurrentPath=path;};

protected:
  mafVMEExternalData();
  virtual ~mafVMEExternalData();

  mafString CurrentPath;

private:
  mafVMEExternalData(const mafVMEExternalData&); // Not implemented
  void operator=(const mafVMEExternalData&); // Not implemented

 
  
};

#endif
