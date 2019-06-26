/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorageElement
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaStorageElement_h__
#define __albaStorageElement_h__

#include "albaDefines.h"
#include <vector>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaStorage;
class albaStorable;
class albaMatrix;
class albaString;
class albaObject;


/** Abstract class representing the interface for the unit of information stored in the storage.
  Abstract class representing the interface for the unit of information stored into a storage. A number of utility
  functions are defined to store and restore basic objects into the element. More complex serialization algorithms can
  be implemented by specific "serializable" objects.
  Among the others, the RestoreObject() function is a function which try to restore a albaObject from element, by 
  creating a new instance from the object factory, taking the object name from the "Type" attribute,
  and then calling the Restore function of the newly create object. The newly created object must also be a
  "albaStorable" to support the Restore() function. 
  This abstract class does not implement any real encoding, and subclasses can define specialized de/serialization
  algorithm.  
  @sa albaXMLStorage albaStorageElement albaXMLElement albaStorable
  @todo
  - reimplement children list as a map
 */  
class ALBA_EXPORT albaStorageElement
{
public:
  virtual ~albaStorageElement();

  /** get the name of this element. The element name is set at creation time (@sa AppendChild()) */
  virtual const char *GetName()=0;

  /** 
    Set an attribute of this element. Attribute 'name' and
    'value' must be passed as argument. */
  virtual void SetAttribute(const char *name,const char *value)=0;
  void SetAttribute(const char *name,const albaID value);
  void SetAttribute(const char *name,const double value);

  /** Return an attribute value given its name. Return false if not found. */
  virtual bool GetAttribute(const char *name,albaString &value)=0;
  
  /** 
    Return an attribute value given its name, converting it to double.
    Return false if attribute is not found. */
  bool GetAttributeAsDouble(const char *name,double &value);

  /** 
    Return an attribute value given its name, converting it to integer.
    Return false if attribute is not found. */
  bool GetAttributeAsInteger(const char *name,albaID &value);

  /** Used to upgrade attribute value from previous MSF file version.*/
  albaString UpgradeAttribute(const char *attribute);

  virtual int StoreText(const char *name, const char *text)=0;
  virtual int StoreMatrix(const char *name,const albaMatrix *matrix)=0;
  virtual int StoreVectorN(const char *name, double *comps,int num)=0;
  virtual int StoreVectorN(const char *name, int *comps,int num)=0;
  virtual int StoreVectorN(const char *name, const std::vector<double> &comps,int num)=0;
  virtual int StoreVectorN(const char *name, const std::vector<int> &comps,int num)=0;
  /** store a vector of strings into an XML element, and stores single items in "tag" sub elements */
  virtual int StoreVectorN(const char *name, const std::vector<albaString> &comps,int num,const char *tag)=0;

  /** Store 8bit binary data. Not yet supported. */
  //virtual int StoreData(const char *name, const char *data, const int size)=0;
  /** Store 16bit binary data. Not yet supported. */
  //virtual int StoreData16(const char *name, const short *data, const int size)=0;
  /** Store 32bit binary data. Not yet supported. */
  //virtual int StoreData32(const char *name, const long *data, const int size)=0;

  /** Restore 8bit binary data. Not yet supported. */
  //virtual int RestoreData(const char *name, char *data, const int size)=0;
  /** Restore 16bit binary data. Not yet supported. */
  //virtual int RestoreData16(const char *name,short *data, const int size)=0;
  /** Restore 32bit binary data. Not yet supported. */
  //virtual int RestoreData32(const char *name,long *data, const int size)=0;
 
  int RestoreMatrix(const char *name,albaMatrix *matrix);
  int RestoreVectorN(const char *name,double *comps,unsigned int num);
  int RestoreVectorN(const char *name,int *comps,unsigned int num);
  int RestoreVectorN(const char *name,std::vector<double> &comps,unsigned int num);
  int RestoreVectorN(const char *name,std::vector<int> &comps,unsigned int num);
  /** restore a vector of strings from an XML element, where single items are stored in "tag" sub elements */
  int RestoreVectorN(const char *name,std::vector<albaString> &comps,unsigned int num,const char *tag);
  int RestoreText(const char *name,char *&buffer);
  int RestoreText(const char *name,albaString &buffer);


  virtual int RestoreMatrix(albaMatrix *matrix)=0;
  virtual int RestoreVectorN(double *comps,unsigned int num)=0;
  virtual int RestoreVectorN(int *comps,unsigned int num)=0;
  virtual int RestoreVectorN(std::vector<double> &comps,unsigned int num)=0;
  virtual int RestoreVectorN(std::vector<int> &comps,unsigned int num)=0;
  /** restore a vector of strings from an XML element, where single items are stored in "tag" sub elements */
  virtual int RestoreVectorN(std::vector<albaString> &comps,unsigned int num,const char *tag)=0;
  virtual int RestoreText(albaString &buffer)=0;

  virtual int RestoreText(char *&buffer);
  
  int StoreDouble(const char *name,const double &value);
  int RestoreDouble(const char *name,double &value);
  int RestoreDouble(double &value);

  int StoreInteger(const char *name, const int &value);
  int RestoreInteger(const char *name,int &value);
  int RestoreInteger(int &value);
  
  /**
    Function to try restoring a albaObject from a albaStorageElement. If the element has
    an attribute with name "Type", the function try to instantiate an object with the same
    name and to restore it from the element. In case of problems the function return NULL. The new
    object is always created by means of New(), this way in case of smart object it can be registered.
    The object to be restored must be both a albaObject (to stay in the object factory) and a albaStorable
    to support Restore() method.*/  
  int RestoreObject(const char *name,albaStorable * object);

  /** Restore object from given element. @sa RestoreObject(const char *name,albaObject *&) */
  albaObject *RestoreObject(const char *name);

  /** Restore object from given element. @sa RestoreObject(const char *name,albaObject *&) */
  albaObject *RestoreObject();

  /**
    Used for storing a albaObjects. The function simply creates a new element with given 'name' and add an
    attribute with name 'Type' storing the object name. Then the function calls the Store function of the
    object. The object must be a storable object to support Store() interface. The newly stored element is
    returned if OK, NULL in case of problems */
  albaStorageElement *StoreObject(const char *name,albaObject *object);

  /** 
    Store a vector of objects. Objects must be both albaObject and albaStorable @sa StoreObject() */
  virtual int StoreObjectVector(const char *name,const std::vector<albaObject *> &vector,const char *items_name="Item");

  /** Restore a vector of objects. Objects must be both albaObject and albaStorable @sa RestoreObject() */
  virtual int RestoreObjectVector(const char *name,std::vector<albaObject *> &vector,const char *items_name="Item");

  /** Restore a vector of objects. Objects must be both albaObject and albaStorable @sa RestoreObject() */
  virtual int RestoreObjectVector(albaStorageElement *element,std::vector<albaObject *> &vector,const char *items_name="Item");

  /** return a pointer to the storage who created this element */
  albaStorage *GetStorage() {return m_Storage;}

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  albaStorageElement *GetParent() {return m_Parent;}

  /** 
    Create a new child element and return its pointer. This is the only way to create a new
    element. The first element (the root) is automatically created by storage object. */
  virtual albaStorageElement *AppendChild(const char *name) = 0;
  
  /** Find a nested element by Name */
  virtual albaStorageElement *FindNestedElement(const char *name);

  typedef std::vector<albaStorageElement *> ChildrenVector;

  /** 
    Return the list of children. Subclasses must implement this to build
    the children list. */ 
  virtual ChildrenVector &GetChildren()=0;
  
  /** 
    Return the list of all children with a given name. return true if at least one found. */ 
  virtual bool GetNestedElementsByName(const char *name,std::vector<albaStorageElement *> &list);

protected:
  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  albaStorageElement(albaStorageElement *parent,albaStorage *storage);

  /** commodity function to store a storable object creating on the fly the element to store it inside. */
  albaStorageElement *StoreObject(const char *name,albaStorable *storable, const char *type_name);

  void SetStorage(albaStorage *storage) {m_Storage = storage;}
  void SetParent(albaStorageElement *element) {m_Parent = element;}

  albaStorage *m_Storage;                        ///< storage who created this element
  albaStorageElement *m_Parent;                  ///< the parent element in the hierarchy
  std::vector<albaStorageElement *> *m_Children;  ///< children elements
};
#endif // _albaStorageElement_h_
