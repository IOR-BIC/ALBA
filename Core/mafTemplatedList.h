/*=========================================================================

  Program:   Multimod Foundation Library
  Module:    $RCSfile: mafTemplatedList.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:16:05 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
=========================================================================*/
// .NAME vtkTemplatedList - map container for vtkObjects
// .SECTION Description
// This class wraps the STL map container for vtkObjects taking care of
// reference counting mechanisms.
// .SECTION SeeAlso
//  
// .SECTION ToDo
// Add Test


#ifndef __vtkTemplatedList_h
#define __vtkTemplatedList_h


#include "vtkObject.h"
#include "mflCoreWin32Header.h"

template <class T>
class vtkTemplatedListItems;

template <class T>
class MFL_CORE_EXPORT vtkTemplatedList : public vtkObjectBase
{
public:

  static vtkTemplatedList *New() {return new vtkTemplatedList<T>;}

  /** 
   Add an item a the the end of the list */
  void AppendItem(T *object);
  
  /** 
   Add an item the head of the list */
  void PrependItem(T *object);
  
  /** Insert an item at given position */
  int InsertItem(unsigned int idx,T* object);

  /**
   Replace the item in the container with a new one. Return
   false if the item is not in the list.*/
  int ReplaceItem(unsigned int idx, T *newitem);
  int ReplaceItem(T *old_item, T *newitem);

  /**
   Remove an object from the list. If object is not found,
   the list is unaffected and false is returned.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem. */
  int RemoveItem(T *);
  
  /**
   Remove the object with given index from the list. If index is out of range
   the list is unaffected and false is returned.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem. */
  int RemoveItem(unsigned int idx);

  /**
   Remove all objects from the list. */
  void RemoveAllItems();

  /**
   Search for an object and return its ID. If ID == 0, object was not found. */
  int IsItemPresent(T *object);
  int IsItemPresent(unsigned int idx);
 
  /** Return index for the given item */
  int GetItemIndex(T *object);

    /**
   Get the item with given index in the container. NULL is returned if no item
   with such index is present */
  T *GetItem(unsigned int idx);

  /**
   Return the number of objects in the list. */
  int GetNumberOfItems();

  /**
   Initialize the traversal of the collection. This means the data pointer
   is set at the beginning of the list. */
  T *InitTraversal();

  /** return true if traversal is exhausted */
  int IsDoneWithTraversal();

  /**
   Get the next item in the container during traversal. NULL is returned if the container
   is exhausted. */
  T *GetNextItem();

  /**
   Get the current item in the container during traversal. NULL is returned if the container
   is exhausted. */
  T *GetItem();

protected:
  vtkTemplatedList();
  virtual ~vtkTemplatedList();

  vtkTemplatedListItems<T> *Items;

private:
  vtkTemplatedList(const vtkTemplatedList&);  // Not implemented.
  void operator=(const vtkTemplatedList&);  // Not implemented.
};

#endif 
