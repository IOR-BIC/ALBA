/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSmartPointer
 Authors: based on vtkSmartPointer (www.vtk.org), rewritten by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaSmartPointer_h
#define __albaSmartPointer_h

#include "albaConfigure.h"
#include "albaBase.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaReferenceCounted;

/** Hold a reference to a T instance.
  albaAutoPointer stores a pointer to a albaSmartObjct, and keeps it registered. When 
  allocated on the stack this object allows to automatically unregister an object 
  when exiting from the scope. */
template <class T>
class albaAutoPointer : public albaBase
{
public:  
  /** Initialize smart pointer to (optional) given object. */
	albaAutoPointer(T* r=NULL,void *owner=NULL){
		m_Object=r;
		Register(owner);
	}

  
  /**
    Initialize smart pointer with a new reference to the same object
    referenced by given smart pointer.*/
	albaAutoPointer(const albaAutoPointer& r){m_Object=r.m_Object;Register(NULL);}
  
  /** Destroy smart pointer and remove the reference to its object. */
	~albaAutoPointer(){UnRegister(NULL);}
  
  /**
    Assign object to reference.  This removes any reference to an old
    object.*/
	albaAutoPointer& operator=(T* r){
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		albaAutoPointer(r).Swap(*this); 
		return *this;
	}

	albaAutoPointer& operator=(const albaAutoPointer<T>& r){
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		albaAutoPointer<T> tmp(r);
		tmp.Swap(*this); // swaps the two pointers
		return *this;
	}

  bool operator==(const albaAutoPointer<T>& r) const {return r.m_Object==m_Object;}
  bool operator==(const T *r) const {return r==m_Object;}
  
  /** Get the contained pointer. */
  T* GetPointer() const {return m_Object;}

  /**
    Allow to pass on the smart pointer to any function requiring a "T *". */
  operator T *() const {return (T *)m_Object;}
  
  /** Provides normal pointer target member access using operator ->. */
  T* operator->() const { return static_cast<T*>(this->m_Object); }

  /** used to force releasing of internal object, specifying the owner */
	void UnRegister(void *owner){
		if(m_Object)
		{
			m_Object->UnRegister(owner);
			m_Object = NULL;
		}
	}
  
protected:
  // Internal utility methods.
	void Swap(albaAutoPointer& r){
		T* temp = r.m_Object;
		r.m_Object = m_Object;
		m_Object = temp;
	}
	void Register(void *owner){
		if(m_Object)
		{
			m_Object->Register(owner);
		}
	}

  T* m_Object; ///<  Pointer to the actual object.
};

/** AutoPointer which self allocates the internal object.
  albaSmartPointer is a specialization of albaAutoPointer which automatically
  instantiate an object in the default constructor. */
template <class T>
class albaSmartPointer: public albaAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instance of class T.*/
  albaSmartPointer() {this->m_Object=T::New();this->Register(NULL);}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  albaSmartPointer(T* r): albaAutoPointer<T>(r) {;}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  albaSmartPointer(const albaAutoPointer<T>& r): albaAutoPointer<T>(r) {;}
};


//----------------------------------------------------------------------------
template <class T>
inline bool operator == (const albaAutoPointer<T>& l, const albaAutoPointer<T>& r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l.GetPointer()) == static_cast<void*>(r.GetPointer())); }

//----------------------------------------------------------------------------
template <class T>
inline bool operator == (albaReferenceCounted* l, const albaAutoPointer<T>& r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l) == static_cast<void*>(r.GetPointer())); }

//----------------------------------------------------------------------------
template <class T>
inline bool operator == (const albaAutoPointer<T>& l, albaReferenceCounted* r) \
//----------------------------------------------------------------------------
{ return (static_cast<void*>(l.GetPointer()) == static_cast<void*>(r)); }


#endif

