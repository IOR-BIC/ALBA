/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.h,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:27 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafXMLElement_h__
#define __mafXMLElement_h__

#include "mafStorageElement.h"

// Sorry, very difficult to use PIMPL with this awful namespace
#include <xercesc/dom/DOM.hpp>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLStorage;
class mafXMLString;

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif

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
  mafXMLElement(DOMElement *element,mafXMLElement *parent,mafXMLStorage *storage);
  virtual ~mafXMLElement();

  /** get the name of this XML element */
  virtual const char *GetName();

  /** Store a generic text into an XML document */
  virtual void StoreText(const const char *text,const char *name="Text");

  /** Store a float number into an XML document */
  virtual void StoreDouble(const double &value,const char *name="Double");

  /** Store a integer number into an XML document */
  virtual void StoreInteger(const int &value,const char *name="Integer");

  /** Store a matrix into an XML document */
  virtual void StoreMatrix(mafMatrix *matrix,const char *name="Matrix");

  /** Store a vector of objects into an XML document */
  virtual void StoreObjectVector(mafVector<mafStorable *> *vector,const char *name="ObjectVector");

  /** Store a vector3 into an XML document */
  virtual void StoreVector3(double comps[3],const char *name="Vector3");

  /** Store a vectorN into an XML document */
  virtual void StoreVectorN(double *comps,int num,const char *name="Vector");
  
  /** Restore a matrix from an XML document */
  virtual int RestoreMatrix(mafMatrix *matrix,const char *name="Matrix");

  /** Restore a vectorN from an XML document */
  virtual int RestoreVectorN(double *comps,unsigned int num,const char *name="Vector");

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(char *&buffer,const char *name="Text");

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(mafString &buffer,const char *name="Text");

  /** Restore a float number from an XML document */
  virtual int RestoreDouble(double &value,const char *name="Double");

  /** Restore an integer number from an XML document */
  virtual int RestoreInteger(int &value,const char *name="Integer");

  /** return a pointer to the storage who created this element */
  mafXMLStorage *GetXMLStorage();

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  mafXMLElement *GetXMLParent();

  /** 
    Return the list of children. The list is created from DOM-Tree at the first access
    during restoring. */ 
  virtual mafVector<mafStorageElement *> *GetChildren();
  
  /** 
    Create a new XML child element and return its pointer. This is the only way to create a new
    XML element. The first element (the root) is automatically created by storage object and
    is the DOM document root element */
  virtual mafStorageElement *AppendChild(const char *name);
  mafXMLElement *AppendXMLChild(const char *name);

  /** Find a nested XML element by Name */
  mafXMLElement *FindNestedXMLElement(const char *name);

  /** return DOM-XML element stored inside this mafXMLElement */
  DOMElement *GetXMLElement();

  /** 
    Append an XML attribute to this element. Attribute 'name' and
    'value' must be passed as argument. This utility function takes care
    of string conversion problems. */
  void SetXMLAttribute(const char *name,const char *value);

  /** 
    Return find an attribute given its name and return its value.
    Return false if not found. This function takes care of string
    conversion problems. */
  bool GetXMLAttribute(const char *name,mafString &value);

  /** this writes text inside a DOM-XML element */
  void WriteXMLText(const char *text);

  /** Internally used to extract vector values from Text data*/
  int ParseData(const char *text,double *vector,int size);

  /** Internally used to extract vector values from Text data */
  int ParseData(double *vector,int size);
  int ParseData(int *vector,int size);

protected:

  DOMElement *m_XMLElement; ///< XML element wrapped by this object 
  mafXMLString  *m_Name; ///< Convenient copy of tagName

private:
  

};
#endif // _mafXMLElement_h_
