/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafTemplatedVector.txx,v $
Language:  C++
Date:      $Date: 2004-11-30 18:18:22 $
Version:   $Revision: 1.2 $



=========================================================================*/
#ifndef __mafTemplatedVector_txx
#define __mafTemplatedVector_txx

#include "mafTemplatedVector.h"
#include "mafSmartPointer.h"
#include <vector>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class T>
class mafTemplatedVectorItems 
{
  public:
  std::vector<mafID, T > Vector;
};

//------------------------------------------------------------------------------
template <class T>
mafTemplatedVector<T>::mafTemplatedVector()
//------------------------------------------------------------------------------
{
  Items  = new mafTemplatedVectorItems<mafID,T>;
}

//------------------------------------------------------------------------------
template <class T>
mafTemplatedVector<T>::~mafTemplatedVector()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete Items;
}

//------------------------------------------------------------------------------
template <class T>
mafID mafTemplatedVector<T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return Items->Vector.size();
}

//------------------------------------------------------------------------------
template <class T>
void mafTemplatedVector<T>::SetItem(mafID idx,T &object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->Vector[idx]=object;
}

//------------------------------------------------------------------------------
template <class T>
mafID mafTemplatedVector<T>::AppendItem(T &object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->Vector.push_back(T);
  return Items->Vector.size()-1;
}

//------------------------------------------------------------------------------
template <class T>
T &mafTemplatedVector<T>::GetItem(mafID idx)
//------------------------------------------------------------------------------
{
  return (idx<Items->Vector.size())?Items->Vector[idx]:NULL;
}

//------------------------------------------------------------------------------
template <class T>
bool mafTemplatedVector<T>::ReplaceItem(mafID idx, T &newitem)
//------------------------------------------------------------------------------
{
  assert(newitem);

  if (idx<Items->Vector.size())
  {
	Items->Vector[idx]=newitem;

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafTemplatedVector<T>::RemoveItem(mafID idx)
//------------------------------------------------------------------------------
{
  if (idx<Items->Vector.size())
  {
    Items->Vector.erase(it);

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafTemplatedVector<T>::RemoveItem(T &object)
//------------------------------------------------------------------------------
{
  assert(object);

  mafID idx;
  if (FindItem(object,idx))
  {   
    return RemoveItem(idx);
  }

  return false;
}

//------------------------------------------------------------------------------ 
template <class T>
void mafTemplatedVector<T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  Items->Vector.clear();
}

//------------------------------------------------------------------------------
template <class T>
bool mafTemplatedVector<T>::FindItem(T &object, mafID &idx)
//------------------------------------------------------------------------------
{
  for (mafID i=0;i<Items->Vector.size();i++)
    if (Items->Vector[i].GetPointer()==object)
	{
	  idx=i;
      return true;
	}

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafTemplatedVector<T>::IsItemPresent(T &object)
//------------------------------------------------------------------------------
{
  mafID idx;
  return FindItem(object,idx);
}

//------------------------------------------------------------------------------
template <class T>
mafID Push(T &object)
//------------------------------------------------------------------------------
{
	AppendItem(object);
}

//------------------------------------------------------------------------------
template <class T>
T &Pop()
//------------------------------------------------------------------------------
{
	T obj=GetItem(GetNumberOfItems()-1);	
	RemoveItem(GetNumberOfItems()-1);
	return obj;
}

#endif