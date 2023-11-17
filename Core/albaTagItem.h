/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagItem
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaTagItem_h
#define __albaTagItem_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaUtility.h"
#include "albaStorable.h"
#include "albaString.h"
#include <vector>
#include <string>
#include <iosfwd>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaString);
#endif

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum ALBA_TAG_IDS {ALBA_MISSING_TAG=0,ALBA_NUMERIC_TAG,ALBA_STRING_TAG};

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** an utility class for storing <key-type-array of values> information.
  an utility class for storing <key-type-array of values> information.
  @sa albaTagArray
*/
class ALBA_EXPORT albaTagItem : public albaUtility, public albaStorable
{
public:
  albaTagItem();
  ~albaTagItem();

  /** Constructors for with implicit Tag type...*/
  albaTagItem(const char *name, const char *value, int t=ALBA_STRING_TAG);
  albaTagItem(const char *name, const char **values, int numcomp, int t=ALBA_STRING_TAG);
  albaTagItem(const char *name, const std::vector<albaString> &values, int numcomp, int t=ALBA_STRING_TAG);
  albaTagItem(const char *name, const double value);
  albaTagItem(const char *name, const double *value, int numcomp);
  albaTagItem(const char *name, const std::vector<double> &values, int numcomp);

  albaTagItem(const albaTagItem& p);
  void operator=(const albaTagItem& p);

  bool operator==(const albaTagItem& p) const;
  bool operator!=(const albaTagItem& p) const;

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Set/Get the name of this Tag*/
  void SetName(const char *name);
  const char *GetName() const;

  /**
    Set the Value of this Tag. In case the component is specified the function operates
    on that component, otherwise it works on component 0. */
  void SetValue(const albaString value,int component=0);
  /** same as SetValue() */
  void SetComponent(const albaString value,int component=0);

  /** Set Tag value converting automatically to string and setting the type to NUMERIC. */
  void SetValue(const double value , int component=0);
  /** Set Tag value converting automatically to string and setting the type to NUMERIC. */
  void SetComponent(const double value , int component=0);
  
  /** Set array of components at a once */
  void SetComponents(const char **values, int numcomp);
  /** Set array of components at a once */
  void SetComponents(const std::vector<albaString> components);

  /** same as SetComponents() */
  void SetValues(const std::vector<albaString> values);
  /** same as SetComponents() */
  void SetValues(const char **values, int numcomp);

  /** Remove a value  */
  void RemoveValue(int component); //Added by Mucci 22/10/2007
  
  /** Set array of components at a once, specifying an array of numeric values. */
  void SetValues(const double *values, int numcomp);
  /** Set array of components at a once, specifying an array of numeric values. */
  void SetValues(const std::vector<double> values);
  
  /** Set array of components at a once, specifying an array of numeric values. */
  void SetComponents(const double *components, int numcomp);

  /** Set array of components at a once, specifying an array of numeric values. */
  void SetComponents(const std::vector<double> components);

  /**
    return the value stored in this item. By default the function 
    work on the first component, but specifying the component number
    it's possible to retain the specific component.*/
  const char *GetValue(int component=0) const;

  /** same as GetValue() */
  const char *GetComponent(int comp) const;

  /** return the value stored in this item converting to a double. */
  double GetValueAsDouble(int component=0) const;
  double GetComponentAsDouble(int comp) const;

  /** return all the array of values */
  const std::vector<albaString> *GetComponents() const {return &m_Components;};

	/** return vector of values as doubles*/
	std::vector<double> GetComponentsAsDoubles() const;;


  /**
    return the array of values as a single string, representing
    the array of components as a tuple of the form:
    "( <component1>, <component2>,...)" */
  void GetValueAsSingleString(albaString &str) const;
  void GetValueAsSingleString(std::string &str) const;
  

  /** 
    Set the type of this Tag. Default available types are ALBA_MISSING_TAG=0,
    , ALBA_NUMERIC_TAG=1, ALBA_STRING_TAG=2 where the first one means a NULL value 
    (no component) is present. Custom ids can be defined by applications. */
  void SetType (int t) {m_Type=t;};

  /** 
    return the type of this tag. Default available types are ALBA_MISSING_TAG=0,
    ALBA_NUMERIC_TAG=1, ALBA_STRING_TAG=2 where the first one means a NULL value 
    (no component) is present. Custom ids can be defined by applications. */
  int GetType() const {return m_Type;};

  /** 
    return the type of this tag item as a string. Default available types
    are "MIS", "NUM", "STR" where the first means a NULL value (no component)
    is present. Custom ids can be defined by applications and a reported by this
    functions as numerical strings. */
  void GetTypeAsString(albaString &value) const;
  void GetTypeAsString(std::string &value) const;

  /**
    Set the NumberOfComponents of the value corresponding to this Tag.
    When a new NumberOfComponents is specified, where possible old value are
    preserved.*/
  void SetNumberOfComponents(int n);
  int GetNumberOfComponents() const;
  
  /** Compare two Tag items*/
  bool Equals(const albaTagItem *item) const;

  /** copy contents of the given tag item */
  void DeepCopy(const albaTagItem *item);

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  void Initialize();
  
  albaString m_Name;
  int m_Type;
  std::vector<albaString> m_Components;
};

#endif 

