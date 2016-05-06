/*=========================================================================

 Program: MAF2
 Module: mafAttribute
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafAttribute_h
#define __mafAttribute_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafReferenceCounted.h" 
#include "mafStorable.h"
#include "mafString.h"

/** An abstract class for objects representing an attribute for mafVMEs.
  This abstract class represent the interface of an attribute for mafVMEs. An attribute
  is a bunch of data that can be attached to a node. Attributes can be specialized classes
  providing any kind of information and functionality. The minimal required features are
  RTTI, object factory registration and Storability. Any attribute can be serialized and 
  then recreated from the factory and unserialized.
  @sa mafVME mafStorable mafObject
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
    inside the Attributes associative map stored inside a mafVME */
  void SetName(const char *name);
  const char *GetName() const;

  /** defined to allow the definition of MakeCopy(). For mafAttribute abstract base class return NULL */
  mafObject *NewObjectInstance() const {return NULL;}
  /** defined to allow the definition of MakeCopy(). For mafAttribute abstract base class return NULL */
  mafAttribute *NewInstance() const {return mafAttribute::SafeDownCast(NewObjectInstance());}

  /** dump the object to output stream */
  virtual void Print(std::ostream& os, const int tabs=0) const;
protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mafString m_Name;
};

#endif 

