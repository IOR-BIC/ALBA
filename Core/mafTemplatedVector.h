/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTemplatedVector.h,v $
  Language:  C++
  Date:      $Date: 2004-11-30 18:18:22 $
  Version:   $Revision: 1.2 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTemplatedVector_h
#define __mafTemplatedVector_h

#include "mafDefines.h"

template <class T>
class mafTemplatedVectorItems;

/** mafTemplatedVector - map container for vtkObjects
  This class wraps the STL map container. It can be used in conjunction with
  mafSmartPointers to store mafSmartObject pointers.
   
  @todo
  - Add a Test */
template <class T>
class MAF_EXPORT mafTemplatedVector
{
public:
  mafTemplatedVector();
  virtual ~mafTemplatedVector();

  /** Set an item with the specified Key */
  void SetItem(const mafID idx,const T &object);

  /** Set an item with the specified Key */
  mafID AppendItem(const T &object);

  /**
   Replace the item with the given Key in the container with a new one. Return
   false if no item with the given Key exists.*/
  bool ReplaceItem(const mafID idx, const T &newitem);

  /**
   Remove the item with given Key in the container.
   If no object with such Key is found the conteiner
   is uneffected and the fucntion return false.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem.*/
  bool RemoveItem(const mafID idx);  

  /**
   Remove an object from the list. If object is not found,
   the list is unaffected and return false. See warning in
   description of RemoveItem(int). */
  bool RemoveItem(const T &);

  /**
   Remove all objects from the list. */
  void RemoveAllItems();

  /**
   Search for an object and return its ID. If ID == 0, object was not found. */
  bool IsItemPresent(const T &object);
 
  /**
   Get the item with given Key in the container. NULL is returned if no item
   with such key is present */
  T &GetItem(const mafID idx);

  /** Return the item index */
  bool FindItem(const T &object, mafID &idx);

  /**
   Return the number of objects in the list. */
  int GetNumberOfItems();

  /** append an item */
  mafID Push(T &object);

  /** 
    retrieve an item. The item is dereferenced without deallocating, this
    way the reference count of the returned object could be 0. The retrievier
    should take care */ 
  T &Pop();


  T &operator [](const mafID idx) {return GetItem(idx);}

protected:
  mafTemplatedVectorItems<T> *Items;
private:
  mafTemplatedVector(const mafTemplatedVector&);  // Not implemented.
  void operator=(const mafTemplatedVector&);  // Not implemented.
};

#endif 
