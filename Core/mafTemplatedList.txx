/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafTemplatedList.txx,v $
Language:  C++
Date:      $Date: 2004-11-29 21:16:05 $
Version:   $Revision: 1.1 $



=========================================================================*/
#ifndef __vtkTemplatedList_txx
#define __vtkTemplatedList_txx

#include "vtkTemplatedList.h"
#include <list>
#include <assert.h>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class T>
class vtkTemplatedListItems 
{
  public:
  typedef std::list<mflAutoPointer<T> > STLList;

  std::list<mflAutoPointer<T> > List;
  STLList::iterator Iterator;
};

//------------------------------------------------------------------------------
template <class T>
vtkTemplatedList<T>::vtkTemplatedList()
//------------------------------------------------------------------------------
{
  Items  = new vtkTemplatedListItems<T>;
}

//------------------------------------------------------------------------------
template <class T>
vtkTemplatedList<T>::~vtkTemplatedList()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete Items;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return Items->List.size();
}

//------------------------------------------------------------------------------
template <class T>
void vtkTemplatedList<T>::AppendItem(T *object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->List.push_back(object);
}

//------------------------------------------------------------------------------
template <class T>
void vtkTemplatedList<T>::PrependItem(T *object)
//------------------------------------------------------------------------------
{
  assert(object)
  Items->List.push_front(object);
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::InsertItem(unsigned int idx,T* object)
//------------------------------------------------------------------------------
{
  assert(object);
  if (idx>=Item->List.size())
    return false;
  
  vtkTemplatedListItems<T>::STLList::iterator it=Items->List.begin();
  for (unsigned int i=0;i<idx;i++,it++); // traverse the list...
  Items->List.insert(it,object);
  return true;
}

//------------------------------------------------------------------------------
template <class T>
T *vtkTemplatedList<T>::GetItem(unsigned int idx)
//------------------------------------------------------------------------------
{
  if (!IsItemPresent(idx))
    return NULL;
  
  vtkTemplatedListItems<T>::STLList::iterator it=Items->List.begin();
  for (unsigned int i=0;i<idx;i++,it++); // traverse the list...
  return *it;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::GetItemIndex(T *object)
//------------------------------------------------------------------------------
{
  if (object)
  {
    vtkTemplatedListItems<T>::STLList::iterator it=Items->List.begin();
    for (unsigned int i=0;it!=Items->List.end();it++,i++)
    {
      if (((T*)*it)==object)
      {
        return i;
      }
    }
  }  
  return -1;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::ReplaceItem(T *old_item, T *newitem)
//------------------------------------------------------------------------------
{
  vtkTemplatedListItems<T>::STLList::iterator it;
 
  assert(newitem);
  for (vtkTemplatedListItems<T>::STLList::iterator it=Items->List.begin();it!=Items->List.end();it++)
  {
    if (((T*)*it)==object)
    {
      *it=newitem;
      return true;
    }
  }
  
  return false;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::ReplaceItem(unsigned int idx, T *newitem)
//------------------------------------------------------------------------------
{
  if (!IsItemPresent(idx))
    return false;
  
  vtkTemplatedListItems<T>::STLList::iterator it;
 
  assert(newitem);

  vtkTemplatedListItems<T>::STLList::iterator it=Items->List.begin();
  for (unsigned int i=0;i<idx;i++,it++); // traverse the list...
  
  *it=newitem;
  
  return true;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::RemoveItem(unsigned int idx)
//------------------------------------------------------------------------------
{
  if (!IsItemPresent(idx))
    return false;
    
  vtkTemplatedListItems<T>::STLList::iterator it;
  for (unsigned int i=0;i<idx;i++,it++); // traverse the list...
  Items->List.erase(it);
  return true;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::RemoveItem(T *object)
//------------------------------------------------------------------------------
{
  assert(object);
  if (IsItemPresent(object))
  {
    Items->List.remove(object);
    return true;
  }

  //vtkErrorMacro("Trying to delete an object not in list");

  return false;
}

//------------------------------------------------------------------------------ 
template <class T>
void vtkTemplatedList<T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  Items->List.clear();
}

//------------------------------------------------------------------------------
template <class T>
T * vtkTemplatedList<T>::InitTraversal()
//------------------------------------------------------------------------------
{
  Items->Iterator=Items->List.begin();
  return GetItem();
}

//------------------------------------------------------------------------------
template <class T>
T *vtkTemplatedList<T>::GetNextItem()
//------------------------------------------------------------------------------
{
  ++Items->Iterator;
  return GetItem();
}
//------------------------------------------------------------------------------
template <class T>
T *vtkTemplatedList<T>::GetItem()
//------------------------------------------------------------------------------
{
  return (Items->Iterator!=Items->List.end())?(*Items->Iterator):NULL;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::IsItemPresent(T *object)
//------------------------------------------------------------------------------
{
  return GetItemIndex(object)>=0;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::IsItemPresent(unsigned int idx)
//------------------------------------------------------------------------------
{
  return Items->List.size()>idx;
}

//------------------------------------------------------------------------------
template <class T>
int vtkTemplatedList<T>::IsDoneWithTraversal()
//------------------------------------------------------------------------------
{
  return Items->Iterator==Items->List.end();
}

#endif