/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttribute
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaAttribute_h
#define __albaAttribute_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaReferenceCounted.h" 
#include "albaStorable.h"
#include "albaString.h"

/** An abstract class for objects representing an attribute for albaVMEs.
  This abstract class represent the interface of an attribute for albaVMEs. An attribute
  is a bunch of data that can be attached to a node. Attributes can be specialized classes
  providing any kind of information and functionality. The minimal required features are
  RTTI, object factory registration and Storability. Any attribute can be serialized and 
  then recreated from the factory and unserialized.
  @sa albaVME albaStorable albaObject
*/
class ALBA_EXPORT albaAttribute : public albaReferenceCounted, public albaStorable
{
public:
  albaAbstractTypeMacro(albaAttribute,albaReferenceCounted);

  /** attributes must define a copy rule */
  void operator=(const albaAttribute &a);

  bool operator==(const albaAttribute &a) const;

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const albaAttribute *a);

  /** create a copy of this attribute */
  albaAttribute *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const albaAttribute *a) const;

  /** 
    Set the name of this attribute. this is typically used to identify it
    inside the Attributes associative map stored inside a albaVME */
  void SetName(const char *name);
  const char *GetName() const;

  /** defined to allow the definition of MakeCopy(). For albaAttribute abstract base class return NULL */
  albaObject *NewObjectInstance() const {return NULL;}
  /** defined to allow the definition of MakeCopy(). For albaAttribute abstract base class return NULL */
  albaAttribute *NewInstance() const {return albaAttribute::SafeDownCast(NewObjectInstance());}

  /** dump the object to output stream */
  virtual void Print(std::ostream& os, const int tabs=0) const;
protected:
  
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  albaString m_Name;
};

#endif 

