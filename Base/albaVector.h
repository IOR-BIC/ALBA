/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVector
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVector_h
#define __albaVector_h

#include "albaDefines.h"
#include "albaBase.h"
#include "albaUtility.h"
#include <vector>

template <class T> class mmuVectorItems;

/** albaVector - vector container wrapping STL vectors.
  This class simply wraps the STL vector container. It can be used in conjunction with
  albaSmartPointers to store albaReferenceCounted pointers.
*/
template <class T>
class albaVector : public albaBase
{
public:
  albaVector();
  virtual ~albaVector();

  /** return number of items in the array */
  albaID GetNumberOfItems();

  /** Set an item with the specified Key */
  void SetItem(const albaID idx,const T &object);

  /** Set an item with the specified Key */
  albaID AppendItem(const T &object);

  /**
   Replace the item with the given Key in the container with a new one. Return
   false if no item with the given Key exists.*/
  bool ReplaceItem(const albaID idx, const T &newitem);

  /**
   Remove the item with given Key in the container.
   If no object with such Key is found the container
   is uneffected and the function return false.
   @attention Be careful if using this function during traversal of the list using 
   GetNextItem.*/
  bool RemoveItem(const albaID idx);  

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
  bool GetItem(const albaID idx,T &obj);

  /**
   Get the item with given Key in the container. NullItem is returned if no item
   with such index is present */
  T &GetItem(const albaID idx);

  /** Return the item index */
  bool FindItem(const T &object, albaID &idx);

  /**
   Return the number of objects in the list. */
  //int GetNumberOfItems();

  /** append an item */
  albaID Push(const T &obj);

  /** Retrieve an item */ 
  bool Pop(T &obj);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  T &operator [](const albaID idx);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  const T &operator [](const albaID idx) const;

  static T m_NullItem;
  
protected:

  mmuVectorItems<T> *m_Items;
  
private:

  albaVector(const albaVector&);  // Not implemented.
  void operator=(const albaVector&);  // Not implemented.

};

//////////////////////////////////TEMPLATE  IMPLEMENTATION////////////////////////////
template <class T>
T albaVector<T>::m_NullItem;

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class T>
class mmuVectorItems: public albaUtility 
{
  public:
  std::vector<T> Vector;
};

//------------------------------------------------------------------------------
template <class T>
albaVector<T>::albaVector()
//------------------------------------------------------------------------------
{
  m_Items  = new mmuVectorItems<T>;
}

//------------------------------------------------------------------------------
template <class T>
albaVector<T>::~albaVector()
//------------------------------------------------------------------------------
{
  RemoveAllItems();
  delete m_Items;
}

//------------------------------------------------------------------------------
template <class T>
void albaVector<T>::SetItem(albaID idx,const T &object)
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
albaID albaVector<T>::AppendItem(const T &object)
//------------------------------------------------------------------------------
{
  m_Items->Vector.push_back(object);
  return m_Items->Vector.size()-1;
}

//------------------------------------------------------------------------------
template <class T>
bool albaVector<T>::GetItem(albaID idx,T &obj)
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
T &albaVector<T>::GetItem(const albaID idx)
//------------------------------------------------------------------------------
{
	if (idx>=0 && idx<m_Items->Vector.size())
	{
		return m_Items->Vector[idx];
	}
	else
	{
		return m_NullItem;
	}
}

//------------------------------------------------------------------------------
template <class T>
T &albaVector<T>::operator [](const albaID idx)
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:m_NullItem;
}

//------------------------------------------------------------------------------
template <class T>
const T &albaVector<T>::operator [](const albaID idx) const
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:m_NullItem;
}

//------------------------------------------------------------------------------
template <class T>
bool albaVector<T>::ReplaceItem(albaID idx, const T &newitem)
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
bool albaVector<T>::RemoveItem(const albaID idx)
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
bool albaVector<T>::FindAndRemoveItem(const T &object)
//------------------------------------------------------------------------------
{
  albaID idx;
  if (FindItem(object,idx))
  {   
    return RemoveItem(idx);
  }

  return false;
}

//------------------------------------------------------------------------------ 
template <class T>
void albaVector<T>::RemoveAllItems()
//------------------------------------------------------------------------------
{
  m_Items->Vector.clear();
}

//------------------------------------------------------------------------------
template <class T>
bool albaVector<T>::FindItem(const T &object, albaID &idx)
//------------------------------------------------------------------------------
{
  for (albaID i=0;i<m_Items->Vector.size();i++)
    if (m_Items->Vector[i]==object)
	{
	  idx=i;
      return true;
	}

  return false;
}


//------------------------------------------------------------------------------
template <class T>
bool albaVector<T>::IsItemPresent(const T &object)
//------------------------------------------------------------------------------
{
  albaID idx;
  return FindItem(object,idx);
}

//------------------------------------------------------------------------------
template <class T>
albaID albaVector<T>::Push(const T &object)
//------------------------------------------------------------------------------
{
  return AppendItem(object);
}

//------------------------------------------------------------------------------
template <class T>
bool albaVector<T>::Pop(T &obj)
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
albaID albaVector<T>::GetNumberOfItems()
//------------------------------------------------------------------------------
{
  return m_Items->Vector.size();
}


#endif 

