/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.h,v $
  Language:  C++
  Date:      $Date: 2004-12-28 19:45:26 $
  Version:   $Revision: 1.3 $
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
template <class T> class mafVector;

/** Abstract class representing the interface for the unit of information stored in the storage.
  Abstract class representing the interface for the unit of information stored into a storage. A number of utility
  functions are defined to store and restore basic objects into the element. More complex serialization algorithms can
  be implemented by specific "serializable" objects.
  This abstract class does not implement any real encoding, and subclasses can define specialized de/serialization
  algorithm.
  
  @sa mafXMLStorage mafStorageElement mafXMLElement mafStorable
 */  
class mafStorageElement
{
public:
  virtual ~mafStorageElement();

  /** get the name of this element. The element name is set at creation time (@sa AppendChild()) */
  virtual const char *GetName()=0;

  /** Store a float number */
  virtual void StoreDouble(const double &value,const char *name="Double")=0;

  /** Store a integer number */
  virtual void StoreInteger(const int &value,const char *name="Integer")=0;

  /** Store a generic text */
  virtual void StoreText(const const char *text,const char *name="Text")=0;

  /** Store a matrix */
  virtual void StoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;

  /** Store a vector of objects */
  virtual void StoreObjectVector(mafVector<mafStorable *> *vector,const char *name="ObjectVector")=0;

  /** Store a vector3 */
  virtual void StoreVector3(double comps[3],const char *name="Vector3")=0;

  /** Store a vectorN */
  virtual void StoreVectorN(double *comps,int num,const char *name="Vector")=0;
  
  /** Restore a matrix */
  virtual int RestoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;

  /** Restore a vectorN */
  virtual int RestoreVectorN(double *comps,unsigned int num,const char *name="Vector")=0;

  /** Restore a generic text string */
  virtual int RestoreText(char *&buffer,const char *name="Text")=0;

  /** Restore a generic text string */
  virtual int RestoreText(mafString &buffer,const char *name="Text")=0;

  /** Restore a float number */
  virtual int RestoreDouble(double &value,const char *name="Double")=0;

  /** Restore an integer number */
  virtual int RestoreInteger(int &value,const char *name="Integer")=0;

  /** return a pointer to the storage who created this element */
  mafStorage *GetStorage() {return m_Storage;}

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  mafStorageElement *GetParent() {return m_Parent;}

  /** 
    Create a new child element and return its pointer. This is the only way to create a new
    element. The first element (the root) is automatically created by storage object. */
  virtual mafStorageElement *AppendChild(const char *name) = 0;

  /** Find a nested element by Name */
  virtual mafStorageElement *FindNestedElement(const char *name)=0;

protected:
  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  mafStorageElement(mafStorageElement *parent,mafStorage *storage);

  void SetStorage(mafStorage *storage) {m_Storage = storage;}
  void SetParent(mafStorageElement *element) {m_Parent = element;}
  
  mafStorage *m_Storage; ///< storage who created this element
  mafStorageElement *m_Parent; ///< the parent element in the hierarchy
  mafVector<mafStorageElement> *m_Children; ///< children elements
};
#endif // _mafStorageElement_h_
