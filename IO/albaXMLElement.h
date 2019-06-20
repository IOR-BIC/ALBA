/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLElement
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaXMLElement_h__
#define __albaXMLElement_h__

#include "albaStorageElement.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaXMLStorage;
class albaXMLString;
class mmuXMLDOMElement;

/** concrete implementation of albaStorageElement as an XML-DOM element
  This class represent a concrete implementation of a albaStorageElement in terms
  of a DOM-XML element. The real DOM-XML element object is embedded in this wrapper
  object and can be retrieved with GetXMLElement(). Utility functions allow a albaStorable
  object to serialize itself independently from the the serialization format. While storing to file
  the albaXMLElement are created by attaching new children to existing nodes by means of AppendChild().
  All the created elements are automatically destroyed at the end of the storing procedure by
  albaStorage object.
  During restoring, the DOM tree is created by DOMParser in the albaXMLStorage object, and the wrapping
  albaXMLElements are created by means of FindNestedElement() function. All the albaXMLElements so created
  during restoring are automatically destroyed by albaStorage object at the end of the restoring.
  @sa albaXMLStorage
 */  
class ALBA_EXPORT albaXMLElement : public albaStorageElement
{
public:
  albaXMLElement(mmuXMLDOMElement *element,albaXMLElement *parent,albaXMLStorage *storage);
  virtual ~albaXMLElement();

  /** get the name of this XML element */
  virtual const char *GetName();

  /** Store a generic text into an XML document */
  virtual int StoreText(const char *name, const char *text);

  /** Store a matrix into an XML document */
  virtual int StoreMatrix(const char *name,const albaMatrix *matrix);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const char *name,const std::vector<double> &comps,int num);
  virtual int StoreVectorN(const char *name,double *comps,int num);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const char *name,const std::vector<int> &comps,int num);
  virtual int StoreVectorN(const char *name,int *comps,int num);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const char *name,const std::vector<albaString> &comps,int num,const char *tag);

  /** Store 8bit binary data. Not yet supported. */
  //virtual int StoreData(const char *name,const char *data, const int size);

  /** Store 16bit binary data. Not yet supported. */
  //virtual int StoreData16(const char *name,const short *data, const int size);

  /** Store 32bit binary data. Not yet supported. */
  //virtual int StoreData32(const char *name,const long *data, const int size);

  /** Store 8bit binary data. Not yet supported. */
  //virtual int RestoreData(const char *name,char *data, const int size);

  /** Store 16bit binary data. Not yet supported. */
  //virtual int RestoreData16(const char *name,short *data, const int size);

  /** Store 32bit binary data. Not yet supported. */
  //virtual int RestoreData32(const char *name,long *data, const int size);
  
  /** Restore a matrix from an XML document */
  virtual int RestoreMatrix(albaMatrix *matrix);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<double> &comps,unsigned int num);
  virtual int RestoreVectorN(double *comps,unsigned int num);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<int> &comps,unsigned int num);
  virtual int RestoreVectorN(int *comps,unsigned int num);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<albaString> &comps,unsigned int num,const char *tag);

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(albaString &buffer);

  /** return a pointer to the storage who created this element */
  albaXMLStorage *GetXMLStorage();

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  albaXMLElement *GetXMLParent();

  /** 
    Return the list of children. The list is created from DOM-Tree at the first access
    during restoring. */ 
  virtual ChildrenVector &GetChildren();
  
  /** 
    Create a new XML child element and return its pointer. This is the only way to create a new
    XML element. The first element (the root) is automatically created by storage object and
    is the DOM document root element */
  virtual albaStorageElement *AppendChild(const char *name);
  albaXMLElement *AppendXMLChild(const char *name);

  /** Find a nested XML element by Name */
  albaXMLElement *FindNestedXMLElement(const char *name);

  /** return DOM-XML element stored inside this albaXMLElement */
  mmuXMLDOMElement *GetXMLElement();

  /** 
    Append an XML attribute to this element. Attribute 'name' and
    'value' must be passed as argument. This utility function takes care
    of string conversion problems. */
  virtual void SetAttribute(const char *name,const char *value);

  /** 
    Return find an attribute given its name and return its value.
    Return false if not found. This function takes care of string
    conversion problems. */
  virtual bool GetAttribute(const char *name,albaString &value);

  /** this writes text inside a DOM-XML element */
  void WriteXMLText(const char *text);

  /** Internally used to extract vector values from Text data */
  int ParseData(std::vector<double> &vector,int size);
  int ParseData(std::vector<int> &vector,int size);
  int ParseData(double *vector,int size);
  int ParseData(int *vector,int size);

  /** return true if an element with that name exists */
  bool ExistElement(const char *name);

protected:

  mmuXMLDOMElement *m_DOMElement; ///< XML element wrapped by this object (USING PIMPL due to Internal Compile errors of VS7)
  albaXMLString  *m_Name; ///< Convenient copy of etagName

};
#endif // _albaXMLElement_h_

