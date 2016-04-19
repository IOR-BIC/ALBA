/*=========================================================================

 Program: MAF2
 Module: mafMSFImporter
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafMSFImporter_h__
#define __mafMSFImporter_h__

#include "mafXMLStorage.h"
#include "mafUtility.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;
class mafVME;
class mafVME;
class mafTagArray;
class mafMatrixVector;

/** utility class representing the MSF document.
 This class is used by mafMSFImporter to restore MSF 1.x files.*/
class MAF_EXPORT mmuMSF1xDocument : public mafUtility, public mafStorable
{
public:
  mmuMSF1xDocument():m_Root(NULL) {}
  virtual ~mmuMSF1xDocument() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
  
  void SetRoot(mafVMERoot *root) {m_Root=root;}
  mafVMERoot *GetRoot() {return m_Root;}

  enum MATERIAL_TAG_ID
  {
    MAT_NAME = 0,
    MAT_AMBIENT_R,
    MAT_AMBIENT_G,
    MAT_AMBIENT_B,
    MAT_AMBIENT_INTENSITY,
    MAT_DIFFUSE_R,
    MAT_DIFFUSE_G,
    MAT_DIFFUSE_B,
    MAT_DIFFUSE_INTENSITY,
    MAT_SPECULAR_R,
    MAT_SPECULAR_G,
    MAT_SPECULAR_B,
    MAT_SPECULAR_INTENSITY,
    MAT_SPECULAR_POWER,
    MAT_OPACITY,
    MAT_REPRESENTATION,
    MAT_NUM_COMP
  };

protected:
  mafVME *CreateVMEInstance(mafString &name);
  mafVME *RestoreVME(mafStorageElement *node, mafVME *parent);
  int RestoreTagArray(mafStorageElement *node, mafTagArray *tarray);
  int RestoreVItem(mafStorageElement *node, mafVME *vme);
  int RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix);

  /** 
  Parse the path string to rebuild links*/
  mafVME *ParsePath(mafVMERoot *root,const char *path);

  /** Restore material attributes */
  void RestoreMaterial(mafVME *vme);

  /** Restore meter attributes */
  void RestoreMeterAttribute(mafVME *vme);

  mafVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class MAF_EXPORT mafMSFImporter: public mafXMLStorage
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
