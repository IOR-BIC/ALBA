/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagArray
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaTagArray_h
#define __albaTagArray_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaAttribute.h"
#include "albaTagItem.h"
#include <map>
#include <string>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,std::string,albaTagItem);
#endif

/** An attribute used to store an associtive array of <key,value> pairs, where value is multi component.
  albaTagArray is an attribute storing an associative array of <key,value> pairs (a map), where values are
  objects of type albaTagItem. A albaTagItem is a simple objet used to store an array of textual values.
  @sa albaAttribute albaTagItem
*/
class ALBA_EXPORT albaTagArray : public albaAttribute
{
public:
  albaTagArray();
  ~albaTagArray();

  albaTypeMacro(albaTagArray,albaAttribute)

  /** attributes must define a copy rule */
  void operator=(const albaTagArray &a);

  bool operator==(const albaTagArray &a) const;

  /** provide access to vector items. If idx is outside the result is invalid */
  //albaTagItem &operator [](const char *name);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  //const albaTagItem &operator [](const char *name) const;

  /**
    Get a particular tag item. The returned object is returned by reference. */
  albaTagItem *GetTag(const char *name);

  /**
    Get a particular tag item. The object value is copied in the given item argument. */
  bool GetTag(const char *name,albaTagItem &item);

  /**
    return true if the give TAG exists. */
  bool IsTagPresent(const char *name);

  /**
    This function searches for an item with same Tag name of the given one and 
    if it doesn't exist append a new one to the Array. The given tag is copied
    and not referenced. */
  void SetTag(const albaTagItem &value);

  /**
    This function searches for an item with given name and if it doesn't exist
    append a new one to the Array, setting the value to the given value. The given
    strings are copied and not referenced */
  void SetTag(const char *name, const char *value,int type=ALBA_MISSING_TAG);

	/**
	This function searches for an item with given name and if it doesn't exist
	append a new one to the Array, setting the value to the given value. */
	void SetTag(const char *name, double value);


  /** Remove an item from the array */
  void DeleteTag(const char *name);
  
  /**
    Return the list of Tag names as an array of strings. */
  void GetTagList(std::vector<std::string> &list);
  
  /**
  Compare two tag arrays. Order of items is significative for
  the comparison*/
  bool Equals(const albaTagArray *array) const;

  /** copy the content of another array */
  void DeepCopy(const albaTagArray *a);

  void DeepCopy(const albaAttribute *a);
	
	/**
	Search the tag array for tags of a given type and 
	put in the array pointers to them   */
	void GetTagsByType(int type, std::vector<albaTagItem *> &array);

  /** return the number of tags stored in this object */
  int GetNumberOfTags() const;

  typedef std::map<std::string,albaTagItem> mmuTagsMap;

  /** return the container of the tags stored in this attribute */
  mmuTagsMap *GetTagsContainer() {return &m_Tags;}

  /** dump tags stored into this array */
  virtual void Print(std::ostream& os, const int tabs=0) const;

protected:
  
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);


  mmuTagsMap m_Tags;
};

//-------------------------------------------------------------------------
template <class TType>
inline TType albaRestoreNumericFromTag(albaTagArray *array,const char *name, TType &variable, TType unset_value, TType default_value)
//-------------------------------------------------------------------------
{
  if (variable==unset_value)
  {
    if (albaTagItem *item=array->GetTag(name))
    {
      variable = item->GetValueAsDouble();
    }
    else 
    {
      variable = default_value;
    }
  }

  return variable;
}

//-------------------------------------------------------------------------
template <class TType>
inline TType albaRestoreStringFromTag(albaTagArray *array,const char *name, TType &variable, const char *unset_value, const char *default_value)
//-------------------------------------------------------------------------
{
  if (variable==unset_value)
  {
    if (albaTagItem *item=array->GetTag(name))
    {
      variable = item->GetValue();
    }
    else 
    {
      variable = default_value;
    }
  }

  return variable;
}


#endif 

