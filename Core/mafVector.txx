/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafVector.txx,v $
Language:  C++
Date:      $Date: 2004-12-01 18:42:53 $
Version:   $Revision: 1.1 $



=========================================================================*/
#ifndef __mafVector_txx
#define __mafVector_txx

#include "mafVector.h"
//#include "mafSmartPointer.h"
#include <vector>
#include <assert.h>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class T>
class mafVectorItems 
{
  public:
  std::vector<T> Vector;
};

//------------------------------------------------------------------------------
template <class T>
mafVector<T>::mafVector()
//------------------------------------------------------------------------------
{
  Items  = new mafVectorItems<T>;
}

//------------------------------------------------------------------------------
template <class T>
mafVector<T>::~mafVector()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete Items;
}

//------------------------------------------------------------------------------
template <class T>
void mafVector<T>::SetItem(mafID idx,const T &object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->Vector[idx]=object;
}

//------------------------------------------------------------------------------
template <class T>
mafID mafVector<T>::AppendItem(const T &object)
//------------------------------------------------------------------------------
{
  assert(object);
  Items->Vector.push_back(object);
  return Items->Vector.size()-1;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::GetItem(mafID idx, T &object)
//------------------------------------------------------------------------------
{
	if (idx<Items->Vector.size())
	{
		object = Items->Vector[idx];
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
template <class T>
T &mafVector<T>::operator [](const mafID idx)
//------------------------------------------------------------------------------
{
  return Items->Vector[idx];
}

//------------------------------------------------------------------------------
template <class T>
T mafVector<T>::operator [](const mafID idx) const
//------------------------------------------------------------------------------
{
  return Items->Vector[idx];
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::ReplaceItem(mafID idx, const T &newitem)
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
bool mafVector<T>::RemoveItem(mafID idx)
//------------------------------------------------------------------------------
{
  if (idx<Items->Vector.size())
  {
    Items->Vector.erase(&Items->Vector[idx]);

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::RemoveItem(const T &object)
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
void mafVector<T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  Items->Vector.clear();
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::FindItem(const T &object, mafID &idx)
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
bool mafVector<T>::IsItemPresent(const T &object)
//------------------------------------------------------------------------------
{
  mafID idx;
  return FindItem(object,idx);
}

//------------------------------------------------------------------------------
template <class T>
mafID mafVector<T>::Push(const T &object)
//------------------------------------------------------------------------------
{
	return AppendItem(object);
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::Pop(T &obj)
//------------------------------------------------------------------------------
{
	if (GetItem(GetNumberOfItems()-1,obj))
	{
		RemoveItem(GetNumberOfItems()-1);
		return true;
	}	
	else
	{
		return false;
	}	
}

//------------------------------------------------------------------------------
template <class T>
mafID mafVector<T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return Items->Vector.size();
}


#endif