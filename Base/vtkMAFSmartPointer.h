/*=========================================================================

 Program: MAF2
 Module: vtkMAFSmartPointer
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class vtkMAFAutoPointer: public vtkSmartPointer<T>
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
  vtkMAFAutoPointer(const vtkSmartPointerBase& r): vtkSmartPointer<T>(r) {}
  /**
  Allows passing the smart pointer to any function requiring a "T *".*/
  operator T *() const {return (T *)this->Object;}
};

template <class T>
class vtkMAFSmartPointer: public vtkMAFAutoPointer<T>
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
