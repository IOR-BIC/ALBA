/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorable.h,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:26 $
  Version:   $Revision: 1.3 $
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
class mafStorageElement;
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
  void Store(mafStorageElement *parent);
  /** 
    Restore this object from an XML document. The XML element from where starting the restoring
    must must be passed as argument. Notice when restoring an element corresponding to this kind
    of object must be passed: restore is not going to search for a subnode of the right type. This
    means the calling restore loop should have identified the node passed as argument as of type
    to be restored by this object.*/
  int Restore(mafStorageElement *node);

protected:
  /**
    This is called by Store() and must be reimplemented by subclasses. 
    The parent element being stored is passed as argument. */
  virtual void InternalStore(mafStorageElement *parent)=0;

  /** 
    This is called by Restore() and must be reimplemented by subclasses 
    The element to be restored is passed as argument*/
  virtual int InternalRestore(mafStorageElement *node)=0;
};
#endif // __mafStorable_h__
