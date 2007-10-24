/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTagItem.h,v $
  Language:  C++
  Date:      $Date: 2007-10-24 09:41:46 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafTagItem_h
#define __mafTagItem_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mmuUtility.h"
#include "mafStorable.h"
#include "mafString.h"
#include <vector>
#include <string>
#include <iosfwd>

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum MAF_TAG_IDS {MAF_MISSING_TAG=0,MAF_NUMERIC_TAG,MAF_STRING_TAG};

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** an utility class for storing <key-type-array of values> information.
  an utility class for storing <key-type-array of values> information.
  @sa mafTagArray
*/
class MAF_EXPORT mafTagItem : public mmuUtility, public mafStorable
{
public:
  mafTagItem();
  ~mafTagItem();

  /** Constructors for with implicit Tag type...*/
  mafTagItem(const char *name, const char *value, int t=MAF_STRING_TAG);
  mafTagItem(const char *name, const char **values, int numcomp, int t=MAF_STRING_TAG);
  mafTagItem(const char *name, const std::vector<mafString> &values, int numcomp, int t=MAF_STRING_TAG);
  mafTagItem(const char *name, const double value);
  mafTagItem(const char *name, const double *value, int numcomp);
  mafTagItem(const char *name, const std::vector<double> &values, int numcomp);

  mafTagItem(const mafTagItem& p);
  void operator=(const mafTagItem& p);

  bool operator==(const mafTagItem& p) const;
  bool operator!=(const mafTagItem& p) const;

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Set/Get the name of this Tag*/
  void SetName(const char *name);
  const char *GetName() const;

  /**
    Set the Value of this Tag. In case the component is specified the function operates
    on that component, otherwise it works on component 0. */
  void SetValue(const mafString value,int component=0);
  /** same as SetValue() */
  void SetComponent(const mafString value,int component=0);

  /** Set Tag value converting automatically to string and setting the type to NUMERIC. */
  void SetValue(const double value , int component=0);
  /** Set Tag value converting automatically to string and setting the type to NUMERIC. */
  void SetComponent(const double value , int component=0);
  
  /** Set array of components at a once */
  void SetComponents(const char **values, int numcomp);
  /** Set array of components at a once */
  void SetComponents(const std::vector<mafString> components);

  /** same as SetComponents() */
  void SetValues(const std::vector<mafString> values);
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
  const std::vector<mafString> *GetComponents() const {return &m_Components;};

  /**
    return the array of values as a single string, representing
    the array of components as a tuple of the form:
    "( <component1>, <component2>,...)" */
  void GetValueAsSingleString(mafString &str) const;
  void GetValueAsSingleString(std::string &str) const;
  

  /** 
    Set the type of this Tag. Default available types are MAF_MISSING_TAG=0,
    , MAF_NUMERIC_TAG=1, MAF_STRING_TAG=2 where the first one means a NULL value 
    (no component) is present. Custom ids can be defined by applications. */
  void SetType (int t) {m_Type=t;};

  /** 
    return the type of this tag. Default available types are MAF_MISSING_TAG=0,
    MAF_NUMERIC_TAG=1, MAF_STRING_TAG=2 where the first one means a NULL value 
    (no component) is present. Custom ids can be defined by applications. */
  int GetType() const {return m_Type;};

  /** 
    return the type of this tag item as a string. Default available types
    are "MIS", "NUM", "STR" where the first means a NULL value (no component)
    is present. Custom ids can be defined by applications and a reported by this
    functions as numerical strings. */
  void GetTypeAsString(mafString &value) const;
  void GetTypeAsString(std::string &value) const;

  /**
    Set the NumberOfComponents of the value corresponding to this Tag.
    When a new NumberOfComponents is specified, where possible old value are
    preserved.*/
  void SetNumberOfComponents(int n);
  int GetNumberOfComponents() const;
  
  /** Compare two Tag items*/
  bool Equals(const mafTagItem *item) const;

  /** copy contents of the given tag item */
  void DeepCopy(const mafTagItem *item);

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  void Initialize();
  
  mafString m_Name;
  int m_Type;
  std::vector<mafString> m_Components;
};

#endif 

