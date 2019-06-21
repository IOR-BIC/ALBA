/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEExternalData
 Authors: Marco Petrone - Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEExternalData_h
#define __albaVMEExternalData_h

#include "albaVMEGroup.h"


/** albaVMEExternalData: VME that links an external file */


class ALBA_EXPORT albaVMEExternalData : public albaVMEGroup
{
public:
  albaTypeMacro(albaVMEExternalData,albaVMEGroup);


void PrintSelf(std::ostream& os,const int indent);

  /**  Set the file extension */
  void SetExtension(const char *ext);

  /**  Get the file extension */
  const char *GetExtension();

  /**  Set the file name*/
  void SetFileName(const char *filename);


  /**  Get the file name */
  const char *GetFileName();

	/**	Set the mime type*/
	void SetMimeType(const char *mimetype);
  
	/**	Get the mime type*/
	const char *GetMimeType();

  /**
  Return the current absolute filename for the file: this file name is
  based on the m_CurrentPath, computed at runtime by the mflVMEStorage class,
  which knows about the full path for the MSF file itself.*/
  albaString GetAbsoluteFileName();
  void GetAbsoluteFileName(char *fname);

  /** TODO: REFACTOR THIS 
  This API is not clear */
  void SetCurrentPath(albaString &path) {this->m_TmpPath=path;};
  void SetCurrentPath(const char *path) {this->m_TmpPath=path;};

  /** Copy the contents of another VMEExternalData into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VMEExternalData. */
  virtual bool Equals(albaVME *vme);

protected:

  /** This is used to allow nested serialization of subclasses.
  This function is called by Store and is reimplemented in subclasses.
  Each subclass can store its own subelements which are
  closed inside the "Device" element. Reimplemented functions
  should first call Superclass implementation. */
  int InternalStore(albaStorageElement *parent);

  /** 
  This function fills in the device with settings restored from the node.
  Subclasses should reimplement it to restore custom settings. Reimplemented
  functions should first call Superclass implementation. */
  int InternalRestore(albaStorageElement *parent);

  /** Get path of the msf file from storage */
  void InitializeCurrentPath();

  /** Get path of the external file imported not stored yet */
  albaString GetTmpPath();

  albaString GetCurrentPath();

  albaVMEExternalData();
  virtual ~albaVMEExternalData();

  albaString m_MSFPath;
  albaString m_TmpPath;

private:
  albaVMEExternalData(const albaVMEExternalData&); // Not implemented
  void operator=(const albaVMEExternalData&); // Not implemented

  // test friend
  friend class albaVMEExternalDataTest;

};


#endif
