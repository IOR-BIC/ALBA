/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartPointer.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:33:18 $
  Version:   $Revision: 1.2 $
  Authors:   based on vtkSmartPointer (www.vtk.org), rewritten by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafSmartPointer_h
#define __mafSmartPointer_h

#include "mafConfigure.h"
#include "mafBase.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafReferenceCounted;

/** Hold a reference to a T instance.
  mafAutoPointer stores a pointer to a mafSmartObjct, and keeps it registered. When 
  allocated on the stack this object allows to automatically unregister an object 
  when exiting from the scope. */
template <class T>
class MAF_EXPORT mafAutoPointer : public mafBase
{
public:  
  /** Initialize smart pointer to (optional) given object. */
  mafAutoPointer(T* r=NULL,void *owner=NULL);
  
  /**
    Initialize smart pointer with a new reference to the same object
    referenced by given smart pointer.*/
  mafAutoPointer(const mafAutoPointer& r);
  
  /** Destroy smart pointer and remove the reference to its object. */
  ~mafAutoPointer();
  
  /**
    Assign object to reference.  This removes any reference to an old
    object.*/
  mafAutoPointer& operator=(T* r);
  mafAutoPointer& operator=(const mafAutoPointer<T>& r);
  
  /** Get the contained pointer. */
  T* GetPointer() const {return m_Object;}

  /**
    Allow to pass on the smart pointer to any function requiring a "T *". */
  operator T *() const {return (T *)m_Object;}
  
  /** Provides normal pointer target member access using operator ->. */
  T* operator->() const { return static_cast<T*>(this->m_Object); }

  /** used to force releasing of internal object, specifying the owner */
  void UnRegister(void *owner);
  
protected:
  // Internal utility methods.
  void Swap(mafAutoPointer& r);
  void Register(void *owner);

  T* m_Object; ///<  Pointer to the actual object.
};

/** AutoPointer which self allocates the internal object.
  mafSmartPointer is a specialization of mafAutoPointer which automatically
  instantiate an object in the default constructor. */
template <class T>
class MAF_EXPORT mafSmartPointer: public mafAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instance of class T.*/
  mafSmartPointer() {m_Object=T::New();Register(NULL);}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  mafSmartPointer(T* r): mafAutoPointer<T>(r) {;}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  mafSmartPointer(const mafAutoPointer<T>& r): mafAutoPointer<T>(r) {;}
};

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer<T>::mafAutoPointer(const mafAutoPointer<T>& r):m_Object(r.m_Object)
//----------------------------------------------------------------------------
{
  Register(NULL);
}
  
//----------------------------------------------------------------------------
template <class T>
mafAutoPointer<T>::~mafAutoPointer()
//----------------------------------------------------------------------------
{
  UnRegister(NULL);
}

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer<T>& mafAutoPointer<T>::operator=(const mafAutoPointer<T>& r)
//----------------------------------------------------------------------------
{
  // make use of a temp auto-ptr to safely unregister, this
  // to avoid deallocation in case of self assignment
  mafAutoPointer<T> tmp(r);
  tmp.Swap(*this); // swaps the two pointers
  return *this;
}

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer<T>& mafAutoPointer<T>::operator=(T* r)
//----------------------------------------------------------------------------
{
  // make use of a temp auto-ptr to safely unregister, this
  // to avoid deallocation in case of self assignment
  mafAutoPointer(r).Swap(*this); 
  return *this;
}


//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer<T>::Swap(mafAutoPointer<T>& r)
//----------------------------------------------------------------------------
{
  T* temp = r.m_Object;
  r.m_Object = m_Object;
  m_Object = temp;
}

//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer<T>::Register(void *owner)
//----------------------------------------------------------------------------
{
  if(m_Object)
  {
    m_Object->Register(owner);
  }
}

//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer<T>::UnRegister(void *owner)
//----------------------------------------------------------------------------
{
  if(m_Object)
  {
    m_Object->UnRegister(owner);
    m_Object = NULL;
  }
}

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer<T>::mafAutoPointer(T* r, void *owner):m_Object(r)
//----------------------------------------------------------------------------
{
  Register(owner);
}

//----------------------------------------------------------------------------
template <class T>
inline bool operator == (const mafAutoPointer<T>& l, const mafAutoPointer<T>& r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l.GetPointer()) == static_cast<void*>(r.GetPointer())); }

//----------------------------------------------------------------------------
template <class T>
inline bool operator == (mafReferenceCounted* l, const mafAutoPointer<T>& r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l) == static_cast<void*>(r.GetPointer())); }

//----------------------------------------------------------------------------
template <class T>
inline bool operator == (const mafAutoPointer<T>& l, mafReferenceCounted* r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l.GetPointer()) == static_cast<void*>(r)); }

#endif

