/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorable
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaStorable_h__
#define __albaStorable_h__

#include "albaConfigure.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaStorageElement;
class albaObject;

/** albaStorable is an interface for serializable objects.
  The idea behind this class is to act as in interface to be inherited to
  become a serializable object. This is a abstract interface, since Store and
  Restore are not defined and should be implemented.
  When storing an element must be provided and the store will attach all necessary
  attributes, data and subnodes to that point.
  When restoring a correct element to start restoring from must be provided: the given node 
  must have been serialized with the the corresponding store function. Beware, Restore() does
  not perform a search for the right subnode to be restore, but simply try to restore the given
  element into itself (the "this" object). On the other end, for subnodes to be restored, the
  restore implementation will probably implement a search mechanism. This happens for the specialized
  RestoreXX() functions of the albaStorageElement class, which search for the subnode of with the right name.
 */  
class ALBA_EXPORT albaStorable
{
public:
  albaStorable():m_Storable(true) {}
  /**
    Storing this object as part of an XML document. The element node must be passed as argument,
    which can eventually be the XML document root. The store function will append all the necessary 
    data and attributes and new XML nodes as children of this node. */
  int Store(albaStorageElement *element);
  /** 
    Restore this object from an XML document. The XML element from where starting the restoring
    must be passed as argument. Notice when restoring an element corresponding to this kind
    of object must be passed: restore is not going to search for a subnode of the right type. This
    means the calling restore loop should have identified the node passed as argument as of type
    to be restored by this object.*/
  int Restore(albaStorageElement *element);

  /** safe cast to albaObject, to be used with double inheritance from albaObject and albaStorable */
  albaObject *CastToObject();

  /** convenience function for dynamic casting */
  static albaStorable* SafeCastToObject(albaObject *o);

  /** return true if the object should be stored */
  bool IsStorable() {return m_Storable;}
protected:
  /**
    This is called by Store() and must be reimplemented by subclasses. 
    The node element where the object should store itself is passed as argument. */
  virtual int InternalStore(albaStorageElement *node)=0;

  /** 
    This is called by Restore() and must be reimplemented by subclasses 
    The element from which the object should restore itself is passed as argument*/
  virtual int InternalRestore(albaStorageElement *node)=0;

  bool m_Storable; ///< if this flag is set to false the object does not store itself
};
#endif // __albaStorable_h__
