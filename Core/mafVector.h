/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVector.h,v $
  Language:  C++
  Date:      $Date: 2004-12-22 14:06:35 $
  Version:   $Revision: 1.5 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVector_h
#define __mafVector_h

#include "mafDefines.h"
#include "mafBase.h"

template <class T>
class mafVectorItems;

/** mafVector - map container for vtkObjects
  This class simply wraps the STL map container. It can be used in conjunction with
  mafSmartPointers to store mafSmartObject pointers.
   
  @todo
  - Add a Test */
template <class T>
class MAF_EXPORT mafVector : public mafBase
{
public:
  mafVector();
  virtual ~mafVector();

  /** return number of items in the array */
  mafID GetNumberOfItems();

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
   If no object with such Key is found the container
   is uneffected and the function return false.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem.*/
  bool RemoveItem(const mafID idx);  

  /**
   Remove an object from the list. If object is not found,
   the list is unaffected and return false. See warning in
   description of RemoveItem(int). */
  bool FindAndRemoveItem(const T &object);

  /**
   Remove all objects from the list. */
  void RemoveAllItems();

  /**
   Search for an object and return its ID. If ID == 0, object was not found. */
  bool IsItemPresent(const T &object);
 
  /**
   Get the item with given Key in the container. false is returned if no item
   with such index is present */
  bool GetItem(const mafID idx,T &obj);

  /**
   Get the item with given Key in the container. NullItem is returned if no item
   with such index is present */
  T &GetItem(const mafID idx);

  /** Return the item index */
  bool FindItem(const T &object, mafID &idx);

  /**
   Return the number of objects in the list. */
  //int GetNumberOfItems();

  /** append an item */
  mafID Push(const T &obj);

  /** Retrieve an item */ 
  bool Pop(T &obj);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  T &operator [](const mafID idx);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  const T &operator [](const mafID idx) const;

  static T NullItem;
  
protected:

  mafVectorItems<T> *m_Items;
  

private:

  mafVector(const mafVector&);  // Not implemented.
  void operator=(const mafVector&);  // Not implemented.

};

#endif 

