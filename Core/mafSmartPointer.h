/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartPointer.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:15:27 $
  Version:   $Revision: 1.1 $
  Authors:   based on vtkSmartPointer (www.vtk.org), rewritten by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafSmartPointer_h
#define __mafSmartPointer_h

#include "mafDefines.h"

/** Hold a reference to a T instance.
  mafAutoPointer stores a pointer to a mafSmartObjct, and keeps it registered. When 
  allocated on the stack this object allows to automatically unregister an object 
  when exiting from the scope.
*/
template <class T>
class MAF_EXPORT mafAutoPointer
{
public:  
  /** Initialize smart pointer to (optional) given object. */
  mafAutoPointer(T* r=NULL);
  
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
  
protected:
  // Internal utility methods.
  void Swap(mafAutoPointer& r);
  void Register();
  void UnRegister();

  T* m_Object; ///<  Pointer to the actual object.
};

/** AutoPointer which self allocates the internal object.
  mafSmartPointer is a specialization of mafAutoPointer which automatically
  instantiate an object in the default constructor. */
template <class T>
class mafSmartPointer: public mafAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instance of class T.*/
  mafSmartPointer() {m_Object=T::New();}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  mafSmartPointer(T* r): mafAutoPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  mafSmartPointer(const mafAutoPointer& r): mafAutoPointer(r) {}
};

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer::mafAutoPointer(T* r):m_Object(r)
//----------------------------------------------------------------------------
{
  Register();
}

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer::mafAutoPointer(const mafAutoPointer& r):m_Object(r.m_Object)
//----------------------------------------------------------------------------
{
  Register();
}
  
//----------------------------------------------------------------------------
template <class T>
mafAutoPointer::~mafAutoPointer()
//----------------------------------------------------------------------------
{
  UnRegister();
}
  
//----------------------------------------------------------------------------
template <class T>
mafAutoPointer& mafAutoPointer::operator=(T* r)
//----------------------------------------------------------------------------
{
  // make use of a temp auto-ptr to safely unregister, this
  // to avoid deallocation in case of self assignment
  mafAutoPointer(r).Swap(*this); 
  return *this;
}

//----------------------------------------------------------------------------
template <class T>
mafAutoPointer& mafAutoPointer::operator=(const mafAutoPointer& r)
//----------------------------------------------------------------------------
{
  // make use of a temp auto-ptr to safely unregister, this
  // to avoid deallocation in case of self assignment
  mafAutoPointer(r).Swap(*this);
  return *this;
}

//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer::Swap(mafAutoPointer& r)
//----------------------------------------------------------------------------
{
  T* temp = r.m_Object;
  r.m_Object = m_Object;
  m_Object = temp;
}

//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer::Register()
//----------------------------------------------------------------------------
{
  if(m_Object)
  {
    m_Object->Register(0);
  }
}

//----------------------------------------------------------------------------
template <class T>
void mafAutoPointer::UnRegister()
//----------------------------------------------------------------------------
{
  if(m_Object)
  {
    m_Object->UnRegister(0);
  }
}

#endif
