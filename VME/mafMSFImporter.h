/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-07-08 16:32:29 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafMSFImporter_h__
#define __mafMSFImporter_h__

#include "mafXMLStorage.h"
#include "mmuUtility.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;
class mafVME;
class mafTagArray;
class mafMatrixVector;

/** utility class representing the MSF document.
 This class is used by mafMSFImporter to restore MSF 1.x files.*/
class mmuMSF1xDocument : public mmuUtility, public mafStorable
{
public:
  mmuMSF1xDocument():m_Root(NULL) {}
  virtual ~mmuMSF1xDocument() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
  
  void SetRoot(mafVMERoot *root);
  mafVMERoot *GetRoot();
  
protected:
  mafVME *CreateVMEInstance(mafString &name);
  mafVME *RestoreVME(mafStorageElement *node, mafVME *parent);
  int RestoreTagArray(mafStorageElement *node, mafTagArray *tarray);
  int RestoreVItem(mafStorageElement *node, mafVME *vme);
  int RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix);

  mafVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class mafMSFImporter: public mafXMLStorage
{
public:
  
  mafTypeMacro(mafMSFImporter,mafXMLStorage)

  mafMSFImporter();
  virtual ~mafMSFImporter();
  
  /** set the root node of this importer */
  void SetRoot(mafVMERoot *root);
  
  /** return the root node attached to this tree */
  mafVMERoot *GetRoot();

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

private:
  
  /** avoid external objects to set the root */
  void SetRoot (mafStorable *root);
};
#endif // _mafMSFImporter_h_
