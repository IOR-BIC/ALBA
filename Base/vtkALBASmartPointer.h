/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBASmartPointer
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkALBASmartPointer_h
#define __vtkALBASmartPointer_h

#include "albaConfigure.h"
#include "vtkSmartPointer.h"

/** Hold a reference to a vtkObjectBase instance.
  vtkALBASmartPointer is a specialization of vtkSmartPointer which automatically
  instantiate an object in the default constructor.
*/
template <class T>
class vtkALBAAutoPointer: public vtkSmartPointer<T>
{
public:
  /** Initialize smart pointer to a new instanse of class T.*/
  vtkALBAAutoPointer():vtkSmartPointer<T>() {}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  vtkALBAAutoPointer(T* r): vtkSmartPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  vtkALBAAutoPointer(const vtkSmartPointerBase& r): vtkSmartPointer<T>(r) {}
  /**
  Allows passing the smart pointer to any function requiring a "T *".*/
  operator T *() const {return (T *)this->Object;}
};

template <class T>
class vtkALBASmartPointer: public vtkALBAAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instanse of class T.*/
  vtkALBASmartPointer() {this->Object=T::New();}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  vtkALBASmartPointer(T* r): vtkALBAAutoPointer<T>(r) {}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  vtkALBASmartPointer(const vtkSmartPointerBase& r): vtkSmartPointerBase(r) {}
};

#endif
