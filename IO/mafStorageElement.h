/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.h,v $
  Language:  C++
  Date:      $Date: 2005-01-10 00:18:07 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorageElement_h__
#define __mafStorageElement_h__

#include "mafConfigure.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorage;
class mafStorable;
class mafMatrix;
class mafString;
class mafObject;
template <class T> class mafVector;

/** Abstract class representing the interface for the unit of information stored in the storage.
  Abstract class representing the interface for the unit of information stored into a storage. A number of utility
  functions are defined to store and restore basic objects into the element. More complex serialization algorithms can
  be implemented by specific "serializable" objects.
  Among the others, the RestoreObject() function is a function which try to restore a mafObject from element, by 
  creating a new instance from the object factory, taking the object name from the "Type" attribute,
  and then calling the Restore function of the newly create object. The newly created object must also be a
  "mafStorable" to support the Restore() function. 
  This abstract class does not implement any real encoding, and subclasses can define specialized de/serialization
  algorithm.  
  @sa mafXMLStorage mafStorageElement mafXMLElement mafStorable
  @todo
  - reimplement children list as a map
 */  
class mafStorageElement
{
public:
  virtual ~mafStorageElement();

  /** get the name of this element. The element name is set at creation time (@sa AppendChild()) */
  virtual const char *GetName()=0;

  /** 
    Set an attribute of this element. Attribute 'name' and
    'value' must be passed as argument. */
  virtual void SetAttribute(const char *name,const char *value)=0;

  /** Return an attribute value given its name. Return false if not found. */
  virtual bool GetAttribute(const char *name,mafString &value)=0;

  
  virtual int StoreText(const char *text,const char *name="Text")=0;
  virtual int StoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;
  virtual int StoreVectorN(double *comps,int num,const char *name="Vector")=0;
  virtual int StoreVectorN(int *comps,int num,const char *name="Vector")=0;

  /** Store 8bit binary data */
  virtual int StoreData(const char *data, const int size,const char *name="CData")=0;
  /** Store 16bit binary data */
  virtual int StoreData16(const short *data, const int size,const char *name="CData16")=0;
  /** Store 32bit binary data */
  virtual int StoreData32(const long *data, const int size,const char *name="CData32")=0;

  /** Restore 8bit binary data */
  virtual int RestoreData(char *data, const int size,const char *name="CData")=0;
  /** Restore 16bit binary data */
  virtual int RestoreData16(short *data, const int size,const char *name="CData16")=0;
  /** Restore 32bit binary data */
  virtual int RestoreData32(long *data, const int size,const char *name="CData32")=0;
  
  virtual int RestoreMatrix(mafMatrix *matrix,const char *name="Matrix")=0;
  virtual int RestoreVectorN(double *comps,unsigned int num,const char *name="Vector")=0;
  virtual int RestoreVectorN(int *comps,unsigned int num,const char *name="Vector")=0;
  virtual int RestoreText(char *&buffer,const char *name="Text")=0;
  virtual int RestoreText(mafString &buffer,const char *name="Text")=0;
  
  virtual void StoreDouble(const double &value,const char *name="Double");
  virtual int RestoreDouble(double &value,const char *name="Double");

  virtual void StoreInteger(const int &value,const char *name="Integer");
  virtual int RestoreInteger(int &value,const char *name="Integer");
  
  /**
    Function to try restoring a mafObject from a mafStorageElement. If the element has
    an attribute with name "Type", the function try to instantiate an object with the same
    name and to restore it from the element. In case of problems the function return NULL. The new
    object is always created by means of New(), this way in case of smart object it can be registered.
    The object to be restored must be both a mafObject (to stay in the object factory) and a mafStorable
    to support Restore() method.*/  
  int RestoreObject(mafObject * &object,const char *name);
  mafObject *RestoreObject(const char *name);

  /** Restore object from given element. @sa RestoreObject(mafObject *&,const char *name) */
  mafObject *RestoreObject(mafStorageElement *element);

  /**
    Used for storing a mafObjects. The function simply creates a new element with given 'name' and add an
    attribute with name 'Type' storing the object name. Then the function calls the Store function of the
    object. The object must be a storable object to support Store() interface. The newly stored element is
    returned if OK, NULL in case of problems */
  mafStorageElement *StoreObject(mafObject *object,const char *name);

  /** 
    Store a vector of objects. Objects must be both mafObject and mafStorable @sa StoreObject() */
  virtual int StoreObjectVector(mafVector<mafObject *> *vector,const char *name="ObjectVector",const char *item_name="Item");

  /** Restore a vector of objects. Objects must be both mafObject and mafStorable @sa RestoreObject() */
  virtual int RestoreObjectVector(mafVector<mafObject *> *vector,const char *name="ObjectVector");

  /** return a pointer to the storage who created this element */
  mafStorage *GetStorage() {return m_Storage;}

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  mafStorageElement *GetParent() {return m_Parent;}

  /** 
    Create a new child element and return its pointer. This is the only way to create a new
    element. The first element (the root) is automatically created by storage object. */
  virtual mafStorageElement *AppendChild(const char *name) = 0;
  
  /** Find a nested element by Name */
  virtual mafStorageElement *FindNestedElement(const char *name);

  /** 
    Return the list of children. Subclasses must impement this to constrcut
    the children list. */ 
  virtual mafVector<mafStorageElement *> *GetChildren()=0; 

protected:
  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  mafStorageElement(mafStorageElement *parent,mafStorage *storage);

  void SetStorage(mafStorage *storage) {m_Storage = storage;}
  void SetParent(mafStorageElement *element) {m_Parent = element;}

  mafStorage *m_Storage; ///< storage who created this element
  mafStorageElement *m_Parent; ///< the parent element in the hierarchy
  mafVector<mafStorageElement *> *m_Children; ///< children elements
};
#endif // _mafStorageElement_h_
