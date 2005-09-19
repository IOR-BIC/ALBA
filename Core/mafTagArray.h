/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTagArray.h,v $
  Language:  C++
  Date:      $Date: 2005-09-19 11:32:46 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafTagArray_h
#define __mafTagArray_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafAttribute.h"
#include "mafTagItem.h"
#include <map>

/** An attribute used to store an associtive array of <key,value> pairs, where value is multi component.
  mafTagArray is an attribute storing an associative array of <key,value> pairs (a map), where values are
  objects of type mafTagItem. A mafTagItem is a simple objet used to store an array of textual values.
  @sa mafAttribute mafTagItem
*/
class MAF_EXPORT mafTagArray : public mafAttribute
{
public:
  mafTagArray();
  ~mafTagArray();

  mafTypeMacro(mafTagArray,mafAttribute)

  /** attributes must define a copy rule */
  void operator=(const mafTagArray &a);

  bool operator==(const mafTagArray &a) const;

  /** provide access to vector items. If idx is outside the result is invalid */
  //mafTagItem &operator [](const char *name);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  //const mafTagItem &operator [](const char *name) const;

  /**
    Get a particular tag item. The returned object is returned by reference. */
  mafTagItem *GetTag(const char *name);

  /**
    Get a particular tag item. The object value is copied in the given item argument. */
  bool GetTag(const char *name,mafTagItem &item);

  /**
    return true if the give TAG exists. */
  bool IsTagPresent(const char *name);

  /**
    This function searches for an item with same Tag name of the given one and 
    if it doesn't exist append a new one to the Array. The given tag is copied
    and not referenced. */
  void SetTag(const mafTagItem &value);

  /**
    This function searches for an item with given name and if it doesn't exist
    append a new one to the Array, setting the value to the given value. The given
    strings are copied and not referenced */
  void SetTag(const char *name, const char *value,int type=MAF_MISSING_TAG);

  /** Remove an item from the array */
  void DeleteTag(const char *name);
  
  /**
    Return the list of Tag names as an array of strings. */
  void GetTagList(std::vector<std::string> &list);
  
  /**
  Compare two tag arrays. Order of items is significative for
  the comparison*/
  bool Equals(const mafTagArray *array) const;

  /** copy the content of another array */
  void DeepCopy(const mafTagArray *a);

  void DeepCopy(const mafAttribute *a);
	
	/**
	Search the tag array for tags of a given type and 
	put in the array pointers to them   */
	void GetTagsByType(int type, std::vector<mafTagItem *> &array);

  /** return the number of tags stored in this object */
  int GetNumberOfTags() const;

  typedef std::map<std::string,mafTagItem> mmuTagsMap;

  /** return the container of the tags stored in this attribute */
  mmuTagsMap *GetTagsContainer() {return &m_Tags;}

  /** dump tags stored into this array */
  virtual void Print(std::ostream& os, const int tabs=0) const;

protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);


  mmuTagsMap m_Tags;
};

//-------------------------------------------------------------------------
template <class TType>
inline TType mafRestoreNumericFromTag(mafTagArray *array,const char *name, TType &variable, TType unset_value, TType default_value)
//-------------------------------------------------------------------------
{
  if (variable==unset_value)
  {
    if (mafTagItem *item=array->GetTag(name))
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
inline TType mafRestoreStringFromTag(mafTagArray *array,const char *name, TType &variable, const char *unset_value, const char *default_value)
//-------------------------------------------------------------------------
{
  if (variable==unset_value)
  {
    if (mafTagItem *item=array->GetTag(name))
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

