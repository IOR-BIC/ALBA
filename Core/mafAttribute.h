/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttribute.h,v $
  Language:  C++
  Date:      $Date: 2005-02-17 00:42:46 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafAttribute_h
#define __mafAttribute_h

#include "mafReferenceCounted.h" 
#include "mafStorable.h"


/** An abstract class for objects representing an attribute for mafNodes.
  This abstract class represent the interface of an attribute for mafNodes. An attribute
  is a bunch of data that can be attached to a node. Attributes can be specialized classes
  providing any kind of information and functionality. The minimal required features are
  RTTI, object factory registration and Storability. Any attribute can be serialized and 
  then recreated from the factory and unserialized.
  @sa mafNode mafStorable mafObject
*/
class MAF_EXPORT mafAttribute : public mafReferenceCounted, public mafStorable
{
public:
  /** attributes must define a copy rule */
  void operator=(const mafAttribute &a);

  bool operator==(const mafAttribute &a) const;

  /** copy the content of the given attribute. Attributes must be type compatible */
  void DeepCopy(const mafAttribute *a);

  /** return true if this attribute equals the given one */
  bool Equals(const mafAttribute *a) const;
};

#endif 

