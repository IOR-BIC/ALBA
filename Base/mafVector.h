/*=========================================================================

 Program: MAF2
 Module: mafVector
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVector_h
#define __mafVector_h

#include "mafDefines.h"
#include "mafBase.h"
#include "mafUtility.h"
#include <vector>

template <class T> class mmuVectorItems;

/** mafVector - vector container wrapping STL vectors.
  This class simply wraps the STL vector container. It can be used in conjunction with
  mafSmartPointers to store mafReferenceCounted pointers.
*/
template <class T>
class mafVector : public mafBase
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

  static T m_NullItem;
  
protected:

  mmuVectorItems<T> *m_Items;
  
private:

  mafVector(const mafVector&);  // Not implemented.
  void operator=(const mafVector&);  // Not implemented.

};

//////////////////////////////////TEMPLATE  IMPLEMENTATION////////////////////////////
template <class T>
T mafVector<T>::m_NullItem;

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
template <class T>
class mmuVectorItems: public mafUtility 
{
  public:
  std::vector<T> Vector;
};

//------------------------------------------------------------------------------
template <class T>
mafVector<T>::mafVector()
//------------------------------------------------------------------------------
{
  m_Items  = new mmuVectorItems<T>;
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
		return m_NullItem;
	}
}

//------------------------------------------------------------------------------
template <class T>
T &mafVector<T>::operator [](const mafID idx)
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:m_NullItem;
}

//------------------------------------------------------------------------------
template <class T>
const T &mafVector<T>::operator [](const mafID idx) const
//------------------------------------------------------------------------------
{
  return (idx<m_Items->Vector.size())?m_Items->Vector[idx]:m_NullItem;
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
    return RemoveItem(idx);
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

