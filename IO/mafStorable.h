/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorable.h,v $
  Language:  C++
  Date:      $Date: 2004-12-24 15:11:09 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorable_h__
#define __mafStorable_h__

#include "mafObject.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLElement;
class mafMatrix;
class mafString;


/** mafStorable is an interface for serializable objects.
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
class mafStorable
{
public:
  /**
    Storing this object as part of an XML document. The parent node must be passed as argument,
    which can eventually be the XML document root. The store function will append as children
    of this node all the necessary new XML nodes. */
  void Store(mafXMLElement *parent);
  /** 
    Restore this object from an XML document. The XML element from where starting the restoring
    must must be passed as argument. Notice when restoring an element corresponding to this kind
    of object must be passed: restore is not going to search for a subnode of the right type. This
    means the calling restore loop should have identified the node passed as argument as of type
    to be restored by this object.*/
  int Restore(mafXMLElement *node);

  /** utility function to serialize a generic text into an XML document */
  static void StoreText(mafXMLElement *parent,const const char *text,const char *name="Text");

  /** utility function to serialize a generic text into an XML document */
  void StoreText(,const const char *text,const char *name="Text");

  /** utility function to serialize a matrix into an XML document */
  static void StoreMatrix(mafXMLElement *parent,mafMatrix *matrix,const char *name="Matrix");

  /** utility function to serialize a matrix into an XML document */
  void StoreMatrix(mafMatrix *matrix,const char *name="Matrix");

  /** utility function to serialize a vector3 into an XML document */
  static void StoreVector3(mafXMLElement *parent,double comps[3],const char *name="Vector3");

  /** utility function to serialize a vector3 into an XML document */
  void StoreVector3(double comps[3],const char *name="Vector3");

  /** utility function to serialize a vectorN into an XML document */
  static void StoreVectorN(mafXMLElement *parent,double *comps,int num,const char *name="Vector");

  /** utility function to serialize a vectorN into an XML document */
  void StoreVectorN(double *comps,int num,const char *name="Vector");

  /** utility function to restore a matrix from an XML document */
  static int RestoreMatrix(mafXMLElement *node,mafMatrix *matrix,const char *name="Matrix");
  
  /** utility function to restore a matrix from an XML document */
  int RestoreMatrix(mafMatrix *matrix,const char *name="Matrix");

  /** utility function to restore a vectorN from an XML document */
  static int RestoreVectorN(mafXMLElement *node,double *comps,unsigned int num,const char *name="Vector");

  /** utility function to restore a vectorN from an XML document */
  int RestoreVectorN(double *comps,unsigned int num,const char *name="Vector");

  /** utility function to restore a generic text string from an XML document */
  static int RestoreText(mafXMLElement *node,char *&buffer,const char *name="Text");

  /** utility function to restore a generic text string from an XML document */
  int RestoreText(char *&buffer,const char *name="Text");

  /** utility function to restore a generic text string from an XML document */
  static int RestoreText(mafXMLElement *node,mafString &buffer,const char *name="Text");

  /** utility function to restore a generic text string from an XML document */
  int RestoreText(mafString &buffer,const char *name="Text");

  /** utility function to restore a float number from an XML document */
  static int RestoreDouble(mafXMLElement *node,double &value,const char *name="Double");

  /** utility function to restore a float number from an XML document */
  int RestoreDouble(double &value,const char *name="Double");

  /** utility function to restore an integer number from an XML document */
  static int RestoreInteger(mafXMLElement *node,int &value,const char *name="Integer");

  /** utility function to restore an integer number from an XML document */
  int RestoreInteger(int &value,const char *name="Integer");

protected:
  /**
    This is called by Store() and must be reimplemented by subclasses. 
    The parent element is set in m_Element. */
  virtual void InternalStore()=0;

  /** 
    This is called by Restore() and must be reimplemented by subclasses 
    The node to be */
  virtual int InternalRestore()=0;

  mafXMLElement *m_Element;  ///< stores the element being stored or restored
};
#endif // _mafStorable_h_
