/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.h,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:25 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafXMLElement_h__
#define __mafXMLElement_h__

#include "mafStorageElement.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLStorage;

/** mafXMLElement is an interface for serializable objects.
  The idea behind this class is to act as in interface to be inherited to
  become a serializable object. This is a abstract interface, since Store and
  Restore are not defined and should be implemented.
  When storing an anchor element must be provided and the store will attach all necessary
  subnoded to that point.
  When restoring a correct element to start restoring from must be provided: the given node 
  must have been serialized with the the corresponding store function. Restore() does not perfrom
  a search for the right node. On the other end, for subnodes to be restored, the restore implementation
  will probably implement a search mechanism. This happens for the specialized RestoreXX() functions,
  which search for subnoded of with the right name.
 */  
class mafXMLElement : public mafStorageElement
{
public:
  mafXMLElement(DOMElement *element,mafStorageElement *parent,mafStorage *storage);
  virtual ~mafXMLElement();

  /** set the name of this element */
  virtual void SetName(const char *name)=0;

  /** get the name of this element */
  virtual const char *GetName()=0;

  

  /** Store a generic text into an XML document */
  virtual void StoreText(const const char *text,const char *name="Text");

  /** Store a float number into an XML document */
  virtual void StoreDouble(const double &value,const char *name="Double");

  /** Store a integer number into an XML document */
  virtual void StoreInteger(const int &value,const char *name="Integer");

  /** Store a matrix into an XML document */
  virtual void StoreMatrix(mafMatrix *matrix,const char *name="Matrix");

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
  mafXMLStorage *GetXMLStorage() {return (mafXMLStorage *) m_Storage;}

  /** return a pointer to the parent element, i.e. the element upper in the hierarchy */
  mafXMLElement *GetXMLParent() {return (mafXMLElement *)m_Parent;}

  /** Create a new child element and return its pointer */
  virtual mafStorageElement *AddChild(const char *name);
  mafXMLElement *AddXMLChild(const char *name);

  /** this writes text inside an XML element */
  void WriteText(mafXMLElement *element,const char *text);

protected:
  DOMElement *m_XMLElement; ///< XML element wrapped by this object 
};
#endif // _mafXMLElement_h_
