/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.h,v $
  Language:  C++
  Date:      $Date: 2004-12-24 15:11:09 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorageElement_h__
#define __mafStorageElement_h__

#include "mafObject.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorage;
class mafMatrix;
class mafString;


/** Abstract class representing the interface for the unit of information stored in the storage.
  This object represent the interface of the unit of information stored into a storage. A number of utility
  functions are defined to store and restore basic objects into the element. More complex serialization algorithms can
  be implemented by specific "serializable" objects.
  This abstract class does not implement any real encoding, and subclasses can define specialized de/serialization
  algorithm. 
  @sa mafStorageWriter mafStorageReader mafXMLElement
 */  
class mafStorageElement
{
public:
  mafStorageElement(mafStorage *storage);
  virtual ~mafStorageElement();

  /** Store a generic text into an XML document */
  virtual void StoreText(const const char *text,const char *name="Text")=0;

  /** Store a matrix into an XML document */
  virtual void StoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;

  /** Store a vector3 into an XML document */
  virtual void StoreVector3(double comps[3],const char *name="Vector3")=0;

  /** Store a vectorN into an XML document */
  virtual void StoreVectorN(double *comps,int num,const char *name="Vector")=0;
  
  /** Restore a matrix from an XML document */
  virtual int RestoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(double *comps,unsigned int num,const char *name="Vector")=0;

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(char *&buffer,const char *name="Text")=0;

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(mafString &buffer,const char *name="Text")=0;

  /** Restore a float number from an XML document */
  virtual int RestoreDouble(double &value,const char *name="Double")=0;

  /** Restore an integer number from an XML document */
  virtual int RestoreInteger(int &value,const char *name="Integer")=0;

  /** return a pointer to the storage which created this element */
  mafStorage *GetStorage();

  /** resolve an URL and provide local filename to be used as input */
  //bool ResolveInputURL(const mafString &url, mafString &filename)=0;

  /** resolve an URL and provide a local filename to be used as output */
  //bool ResolveOutputURL(const mafString &url, mafString &filename)=0;

protected:
  
  mafStorage *m_Storage;
};
#endif // _mafStorageElement_h_
