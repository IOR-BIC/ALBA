/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttribute.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:12:37 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafAttribute_h
#define __mafAttribute_h

#include "mafReferenceCounted.h" 
#include "mafStorable.h"
#include "mafString.h"

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
  mafAbstractTypeMacro(mafAttribute,mafReferenceCounted);

  /** attributes must define a copy rule */
  void operator=(const mafAttribute &a);

  bool operator==(const mafAttribute &a) const;

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const mafAttribute *a);

  /** create a copy of this attribute */
  mafAttribute *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const mafAttribute *a) const;

  /** 
    Set the name of this attribute. this is typically used to identify it
    inside the Attributes associative map stored inside a mafNode */
  void SetName(const char *name);
  const char *GetName() const;

  /** defined to allow the definition of MakeCopy(). For mafAttribute abstract base class return NULL */
  mafObject *NewObjectInstance() const {return NULL;}
  /** defined to allow the definition of MakeCopy(). For mafAttribute abstract base class return NULL */
  mafAttribute *NewInstance() const {return mafAttribute::SafeDownCast(NewObjectInstance());}
protected:
  mafString m_Name;
};

#endif 

