/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafMap.txx,v $
Language:  C++
Date:      $Date: 2004-12-02 21:07:33 $
Version:   $Revision: 1.1 $



=========================================================================*/
#ifndef __vtkTemplatedMap_txx
#define __vtkTemplatedMap_txx

#include "vtkTemplatedMap.h"
#include "vtkObjectFactory.h"
#include "vtkMutexLock.h"
#include "mflEvent.h"
#include "vtkString.h"

#include <map>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class Key,class T>
class vtkTemplatedMapItems 
{
  public:
  typedef std::map<Key, mflAutoPointer<T> > STLMap;

  std::map<Key, mflAutoPointer<T> > Map;
  STLMap::iterator Iterator;
};

//------------------------------------------------------------------------------
template <class Key,class T>
vtkTemplatedMap<Key,T>::vtkTemplatedMap()
//------------------------------------------------------------------------------
{
  Items  = new vtkTemplatedMapItems<Key,T>;
}

//------------------------------------------------------------------------------
template <class Key,class T>
vtkTemplatedMap<Key,T>::~vtkTemplatedMap()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete Items;
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return Items->Map.size();
}

//------------------------------------------------------------------------------
template <class Key,class T>
void vtkTemplatedMap<Key,T>::SetItem(Key key,T *object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->Map[key]=object;
}

//------------------------------------------------------------------------------
template <class Key,class T>
T *vtkTemplatedMap<Key,T>::GetItem(Key key)
//------------------------------------------------------------------------------
{
  vtkTemplatedMapItems<Key,T>::STLMap::iterator it=Items->Map.find(key);
  return (it!=Items->Map.end())?it->second:NULL;
}

//------------------------------------------------------------------------------
template <class Key,class T>
T *vtkTemplatedMap<Key,T>::GetItemByIndex(int idx)
//------------------------------------------------------------------------------
{
  if (Items->Map.size()>=idx)
    return NULL;

  vtkTemplatedMapItems<Key,T>::STLMap::iterator it=Items->Map.begin();
  it+=idx;
  return (it!=Items->Map.end())?it->second:NULL;
}

//------------------------------------------------------------------------------
template <class Key,class T>
bool vtkTemplatedMap<Key,T>::GetItemKey(T *object, Key &key)
//------------------------------------------------------------------------------
{
  if (object)
  {
    for (vtkTemplatedMapItems<Key,T>::STLMap::iterator it=Items->Map.begin();it!=Items->Map.end();it++)
    {
      T *ptr=it->second;

      if (ptr==object)      
        key=it->first;
        return true;
    }
  }  
  return false;
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::ReplaceItem(Key key, T *newitem)
//------------------------------------------------------------------------------
{
  vtkTemplatedMapItems<Key,T>::STLMap::iterator it;
 
  assert(newitem);

  if ((it=Items->Map.find(key))!=Items->Map.end())
  {
	(*it).second=newitem;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::RemoveItem(Key key)
//------------------------------------------------------------------------------
{
  vtkTemplatedMapItems<Key,T>::STLMap::iterator it;
  
  if ((it=Items->Map.find(key))!=Items->Map.end())
  {
    Items->Map.erase(it);

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::RemoveItem(T *object)
//------------------------------------------------------------------------------
{
  assert(object);
  if (IsItemPresent(object))
  {
    Key key;
    if (GetItemKey(object,key))
      return this->RemoveItem(key);
  }

  //vtkErrorMacro("Trying to delete an object not in list");

  return false;
}

//------------------------------------------------------------------------------ 
template <class Key,class T>
void vtkTemplatedMap<Key,T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  Items->Map.clear();
}

//------------------------------------------------------------------------------
template <class Key,class T>
T *vtkTemplatedMap<Key,T>::InitTraversal()
//------------------------------------------------------------------------------
{
  Items->Iterator=Items->Map.begin();
  return GetItem();
}

//------------------------------------------------------------------------------
template <class Key,class T>
T *vtkTemplatedMap<Key,T>::GetNextItem()
//------------------------------------------------------------------------------
{
  Items->Iterator++;
  return GetItem();
}

//------------------------------------------------------------------------------
template <class Key,class T>
T *vtkTemplatedMap<Key,T>::GetItem()
//------------------------------------------------------------------------------
{
  return (Items->Iterator!=Items->Map.end())?(*Items->Iterator).second:NULL;
}

//------------------------------------------------------------------------------
template <class Key,class T>
bool vtkTemplatedMap<Key,T>::GetItemKey(Key &key)
//------------------------------------------------------------------------------
{
  if (Items->Iterator;!=Items->Map.end())
  {
    key =(*Items->Iterator;).first;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::IsItemPresent(T *object)
//------------------------------------------------------------------------------
{
  Key key;
  return GetItemKey(object,key);
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::IsItemPresent(Key id)
//------------------------------------------------------------------------------
{
  vtkTemplatedMapItems<Key,T>::STLMap::iterator it;
  return Items->Map.find(key))!=Items->Map.end();
}

//------------------------------------------------------------------------------
template <class Key,class T>
int vtkTemplatedMap<Key,T>::IsDoneWithTraversal()
//------------------------------------------------------------------------------
{
  return Items->Iterator==Items->Map.end();
}

#endif