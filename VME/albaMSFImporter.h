/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMSFImporter
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaMSFImporter_h__
#define __albaMSFImporter_h__

#include "albaXMLStorage.h"
#include "albaUtility.h"
#include "albaStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMERoot;
class albaVME;
class albaVME;
class albaTagArray;
class albaMatrixVector;

/** utility class representing the MSF document.
 This class is used by albaMSFImporter to restore MSF 1.x files.*/
class ALBA_EXPORT mmuMSF1xDocument : public albaUtility, public albaStorable
{
public:
  mmuMSF1xDocument():m_Root(NULL) {}
  virtual ~mmuMSF1xDocument() {}
  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);
  
  void SetRoot(albaVMERoot *root) {m_Root=root;}
  albaVMERoot *GetRoot() {return m_Root;}

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
  albaVME *CreateVMEInstance(albaString &name);
  albaVME *RestoreVME(albaStorageElement *node, albaVME *parent);
  int RestoreTagArray(albaStorageElement *node, albaTagArray *tarray);
  int RestoreVItem(albaStorageElement *node, albaVME *vme);
  int RestoreVMatrix(albaStorageElement *node, albaMatrixVector *vmatrix);

  /** 
  Parse the path string to rebuild links*/
  albaVME *ParsePath(albaVMERoot *root,const char *path);

  /** Restore material attributes */
  void RestoreMaterial(albaVME *vme);

  /** Restore meter attributes */
  void RestoreMeterAttribute(albaVME *vme);

  albaVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa albaXMLStorage
  @todo
    - 
*/  
class ALBA_EXPORT albaMSFImporter: public albaXMLStorage
{
public:
  
  albaTypeMacro(albaMSFImporter,albaXMLStorage)

  albaMSFImporter();
  virtual ~albaMSFImporter();
  
  /** set the root node of this importer */
  void SetRoot(albaVMERoot *root);
  
  /** return the root node attached to this tree */
  albaVMERoot *GetRoot();

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

private:
  
  /** avoid external objects to set the root */
  void SetRoot (albaStorable *root);
};
#endif // _albaMSFImporter_h_
