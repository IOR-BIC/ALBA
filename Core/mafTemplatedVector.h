/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTemplatedVector.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:16:06 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// .NAME mafTemplatedVector - map container for vtkObjects
// .SECTION Description
// This class wraps the STL map container for vtkObjects taking care of
// reference counting mechanisms.
// .SECTION SeeAlso
//  
// .SECTION ToDo
// Add Test


#ifndef __mafTemplatedVector_h
#define __mafTemplatedVector_h

#include "mafDefines.h"

template <class T>
class mafTemplatedVectorItems;

template <class T>
class MAF_EXPORT mafTemplatedVector
{
public:
  mafTemplatedVector();
  virtual ~mafTemplatedVector();

  /** Set an item with the specified Key */
  void SetItem(const mafID idx,T *object);

  /** Set an item with the specified Key */
  mafID AppendItem(T *object);

  /**
   Replace the item with the given Key in the container with a new one. Return
   false if no item with the given Key exists.*/
  int ReplaceItem(const mafID idx, T *newitem);

  /**
   Remove the item with given Key in the container.
   If no object with such Key is found the conteiner
   is uneffected and the fucntion return false.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem.*/
  int RemoveItem(const mafID idx);  

  /**
   Remove an object from the list. If object is not found,
   the list is unaffected and return false. See warning in
   description of RemoveItem(int). */
  int RemoveItem(T *);

  /**
   Remove all objects from the list. */
  void RemoveAllItems();

  /**
   Search for an object and return its ID. If ID == 0, object was not found. */
  int IsItemPresent(T *object);
 
  /**
   Get the item with given Key in the container. NULL is returned if no item
   with such key is present */
  T *GetItem(const mafID idx);

  /** Return the item index */
  bool FindItem(T *object, mafID &idx);

  /**
   Return the number of objects in the list. */
  int GetNumberOfItems();

  T *operator [](const mafID idx) {return GetItem(idx);}

protected:
  mafTemplatedVectorItems<T> *Items;
private:
  mafTemplatedVector(const mafTemplatedVector&);  // Not implemented.
  void operator=(const mafTemplatedVector&);  // Not implemented.
};

#endif 
