/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:42:23 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafXMLElement_h__
#define __mafXMLElement_h__

#include "mafStorageElement.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLStorage;
class mafXMLString;
class mmuXMLDOMElement;

/** concrete implementation of mafStorageElement as an XML-DOM element
  This class represent a concrete implementation of a mafStorageElement in terms
  of a DOM-XML element. The real DOM-XML element object is embedded in this wrapper
  object and can be retrieved with GetXMLElement(). Utility functions allow a mafStorable
  object to serialize itself independently from the the serialization format. While storing to file
  the mafXMLElement are created by attaching new children to existing nodes by means of AppendChild().
  All the created elements are automatically destroyed at the end of the storing procedure by
  mafStorage object.
  During restoring, the DOM tree is created by DOMParser in the mafXMLStorage object, and the wrapping
  mafXMLElements are created by means of FindNestedElement() function. All the mafXMLElements so created
  during restoring are automatically destroyed by mafStorage object at the end of the restoring.
  @sa mafXMLStorage
 */  
class mafXMLElement : public mafStorageElement
{
public:
  mafXMLElement(mmuXMLDOMElement *element,mafXMLElement *parent,mafXMLStorage *storage);
  virtual ~mafXMLElement();

  /** get the name of this XML element */
  virtual const char *GetName();

  /** Store a generic text into an XML document */
  virtual int StoreText(const char *text,const char *name);

  /** Store a matrix into an XML document */
  virtual int StoreMatrix(mafMatrix *matrix,const char *name);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const std::vector<double> &comps,int num,const char *name);
  virtual int StoreVectorN(double *comps,int num,const char *name);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const std::vector<int> &comps,int num,const char *name);
  virtual int StoreVectorN(int *comps,int num,const char *name);

  /** Store a vectorN into an XML document */
  virtual int StoreVectorN(const std::vector<mafString> &comps,int num,const char *name,const char *tag);

  /** Store 8bit binary data. Not yet supported. */
  //virtual int StoreData(const char *data, const int size,const char *name);

  /** Store 16bit binary data. Not yet supported. */
  //virtual int StoreData16(const short *data, const int size,const char *name);

  /** Store 32bit binary data. Not yet supported. */
  //virtual int StoreData32(const long *data, const int size,const char *name);

  /** Store 8bit binary data. Not yet supported. */
  //virtual int RestoreData(char *data, const int size,const char *name);

  /** Store 16bit binary data. Not yet supported. */
  //virtual int RestoreData16(short *data, const int size,const char *name);

  /** Store 32bit binary data. Not yet supported. */
  //virtual int RestoreData32(long *data, const int size,const char *name);
  
  /** Restore a matrix from an XML document */
  virtual int RestoreMatrix(mafMatrix *matrix,const char *name);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<double> &comps,unsigned int num,const char *name);
  virtual int RestoreVectorN(double *comps,unsigned int num,const char *name);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<int> &comps,unsigned int num,const char *name);
  virtual int RestoreVectorN(int *comps,unsigned int num,const char *name);

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(std::vector<mafString> &comps,unsigned int num,const char *name,const char *tag);

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(char *&buffer,const char *name);

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(mafString &buffer,const char *name);

  /** return a pointer to the storage who created this element */
  mafXMLStorage *GetXMLStorage();

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  mafXMLElement *GetXMLParent();

  /** 
    Return the list of children. The list is created from DOM-Tree at the first access
    during restoring. */ 
  virtual ChildrenVector &GetChildren();
  
  /** 
    Create a new XML child element and return its pointer. This is the only way to create a new
    XML element. The first element (the root) is automatically created by storage object and
    is the DOM document root element */
  virtual mafStorageElement *AppendChild(const char *name);
  mafXMLElement *AppendXMLChild(const char *name);

  /** Find a nested XML element by Name */
  mafXMLElement *FindNestedXMLElement(const char *name);

  /** return DOM-XML element stored inside this mafXMLElement */
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
  virtual bool GetAttribute(const char *name,mafString &value);

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
  mafXMLString  *m_Name; ///< Convenient copy of etagName

};
#endif // _mafXMLElement_h_
