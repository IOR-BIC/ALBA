/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMAFSmartPointer.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:29:48 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __vtkMAFSmartPointer_h
#define __vtkMAFSmartPointer_h

#include "mafConfigure.h"
#include "vtkSmartPointer.h"

/** Hold a reference to a vtkObjectBase instance.
  vtkMAFSmartPointer is a specialization of vtkSmartPointer which automatically
  instantiate an object in the default constructor.
*/
template <class T>
class MAF_EXPORT vtkMAFAutoPointer: public vtkSmartPointer<T>
{
public:
  /** Initialize smart pointer to a new instanse of class T.*/
  vtkMAFAutoPointer():vtkSmartPointer<T>() {}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  vtkMAFAutoPointer(T* r): vtkSmartPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  vtkMAFAutoPointer(const vtkSmartPointerBase& r): vtkSmartPointerBase(r) {}
  /**
  Allows passing the smart pointer to any function requiring a "T *".*/
  operator T *() const {return (T *)this->Object;}
};

template <class T>
class MAF_EXPORT vtkMAFSmartPointer: public vtkMAFAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instanse of class T.*/
  vtkMAFSmartPointer() {this->Object=T::New();}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  vtkMAFSmartPointer(T* r): vtkMAFAutoPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  vtkMAFSmartPointer(const vtkSmartPointerBase& r): vtkSmartPointerBase(r) {}
};

#endif
