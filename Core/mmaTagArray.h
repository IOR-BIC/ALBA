/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaTagArray.h,v $
  Language:  C++
  Date:      $Date: 2005-02-17 00:44:27 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmaTagArray_h
#define __mmaTagArray_h

#include "mafAttribute.h"
#include "mmuTagItem.h"
#include <map>

/** An attribute used to store an associtive array of <key,value> pairs, where value is multi component.
  mmaTagArray is an attribute storing an associative array of <key,value> pairs (a map), where values are
  objects of type mafTagItem. A mafTagItem is a simple objet used to store an array of textual values.
  @sa mafAttribute mafTagItem
*/
class MAF_EXPORT mmaTagArray : public mafAttribute
{
public:
  mmaTagArray();
  ~mmaTagArray();

  mafTypeMacro(mmaTagArray,mafAttribute)

  /** attributes must define a copy rule */
  void operator=(const mmaTagArray &a);

  bool operator==(const mmaTagArray &a) const;

  /** provide access to vector items. If idx is outside the result is invalid */
  //mmuTagItem &operator [](const char *name);
  
  /** provide access to vector items. If idx is outside the result is invalid */
  //const mmuTagItem &operator [](const char *name) const;

  /**
    Get a particular tag item. The returned object is returned by reference. */
  mmuTagItem *GetTag(const char *name);

  /**
    Get a particular tag item. The object value is copied in the given item argument. */
  bool GetTag(const char *name,mmuTagItem &item);

  /**
    return true if the give TAG exists. */
  bool IsTagPresent(const char *name);

  /**
    This function searches for an item with same Tag name of the given one and 
    if it doesn't exist append a new one to the Array. The given tag is copied
    and not referenced. */
  void SetTag(const mmuTagItem &value);

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
  bool Equals(const mmaTagArray *array) const;

  /** copy the content of another array */
  void DeepCopy(const mmaTagArray *a);
	
	/**
	Search the tag array for tags of a given type and 
	put in the array pointers to them   */
	void GetTagsByType(int type, std::vector<mmuTagItem *> &array);

  /** return the number of tags stored in this object */
  int GetNumberOfTags() const;

  typedef std::map<std::string,mmuTagItem> mmuTagsMap;

  /** return the container of the tags stored in this attribute */
  mmuTagsMap *GetTagsContainer() {return &m_Tags;}

protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);


  mmuTagsMap m_Tags;
};

#endif 

