/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafVector.txx,v $
Language:  C++
Date:      $Date: 2004-12-20 20:47:09 $
Version:   $Revision: 1.4 $



=========================================================================*/
#ifndef __mafVector_txx
#define __mafVector_txx

#include "mafVector.h"
//#include "mafSmartPointer.h"
#include <vector>

template <class T>
T mafVector<T>::NullItem;

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
  m_Items  = new mafVectorItems<T>;
}

//------------------------------------------------------------------------------
template <class T>
mafVector<T>::~mafVector()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete m_Items;
}

//------------------------------------------------------------------------------
template <class T>
void mafVector<T>::SetItem(mafID idx,const T &object)
//------------------------------------------------------------------------------
{
  if (m_Items->Vector.size()<=idx)
  {
	m_Items->Vector.resize(idx+1);
  }
	

  m_Items->Vector[idx]=object;
}

//------------------------------------------------------------------------------
template <class T>
mafID mafVector<T>::AppendItem(const T &object)
//------------------------------------------------------------------------------
{
  m_Items->Vector.push_back(object);
  return m_Items->Vector.size()-1;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::GetItem(mafID idx,T &obj)
//------------------------------------------------------------------------------
{
	if (idx>=0 && idx<m_Items->Vector.size())
	{
		obj=m_Items->Vector[idx];
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
template <class T>
T &mafVector<T>::GetItem(const mafID idx)
//------------------------------------------------------------------------------
{
	if (idx>=0 && idx<m_Items->Vector.size())
	{
		return m_Items->Vector[idx];
	}
	else
	{
		return NullItem;
	}
}

//------------------------------------------------------------------------------
template <class T>
T &mafVector<T>::operator [](const mafID idx)
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:NullItem;
}

//------------------------------------------------------------------------------
template <class T>
const T &mafVector<T>::operator [](const mafID idx) const
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:NullItem;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::ReplaceItem(mafID idx, const T &newitem)
//------------------------------------------------------------------------------
{
  if (idx<m_Items->Vector.size())
  {
	m_Items->Vector[idx]=newitem;

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::RemoveItem(const mafID idx)
//------------------------------------------------------------------------------
{
  if (idx<m_Items->Vector.size())
  {
    m_Items->Vector.erase(m_Items->Vector.begin()+idx);

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::FindAndRemoveItem(const T &object)
//------------------------------------------------------------------------------
{
  mafID idx;
  if (FindItem(object,idx))
  {   
    return RemoveItem(m_Items->Vector.begin()+idx);
  }

  return false;
}

//------------------------------------------------------------------------------ 
template <class T>
void mafVector<T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  m_Items->Vector.clear();
}

//------------------------------------------------------------------------------
template <class T>
bool mafVector<T>::FindItem(const T &object, mafID &idx)
//------------------------------------------------------------------------------
{
  for (mafID i=0;i<m_Items->Vector.size();i++)
    if (m_Items->Vector[i]==object)
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
	 
  return false;	
}

//------------------------------------------------------------------------------
template <class T>
mafID mafVector<T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return m_Items->Vector.size();
}

#endif

