/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuTagItem.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-17 00:45:34 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmuTagItem.h"
#include "mafString.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include <vector>
#include <assert.h>

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem()
//-------------------------------------------------------------------------
{
  Initialize();
}

//-------------------------------------------------------------------------
mmuTagItem::~mmuTagItem()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name,  const char *value, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetValue(value);
  SetName(name);
  SetType(t);
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name, const char **values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values,numcomp);
  SetType(t);
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name, const std::vector<mafString> &values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(t);
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name, const double value)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValue(value);
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name, const double *value, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(value,numcomp);

}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const char *name, const std::vector<double> &values, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mmuTagItem::DeepCopy(const mmuTagItem *item)
//-------------------------------------------------------------------------
{
  assert(item);
  this->SetName(item->GetName());
  this->SetNumberOfComponents(item->GetNumberOfComponents());
  for (int i=0;i<GetNumberOfComponents();i++)
  {
    this->SetValue(item->GetValue(i),i);
  }

  this->SetType(item->GetType());

}
//-------------------------------------------------------------------------
void mmuTagItem::operator=(const mmuTagItem& p)
//-------------------------------------------------------------------------
{
  DeepCopy(&p);
}

//-------------------------------------------------------------------------
mmuTagItem::mmuTagItem(const mmuTagItem& p)
//-------------------------------------------------------------------------
{
  Initialize();
  *this=p;
}

//-------------------------------------------------------------------------
bool mmuTagItem::operator==(const mmuTagItem& p) const
//-------------------------------------------------------------------------
{
  return Equals(&p);
}

//-------------------------------------------------------------------------
bool mmuTagItem::operator!=(const mmuTagItem& p) const
//-------------------------------------------------------------------------
{
  return !Equals(&p);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}

//-------------------------------------------------------------------------
const char *mmuTagItem::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValue(double value,int component)
//-------------------------------------------------------------------------
{
  mafString tmp(value);
  SetValue(tmp,component);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponent(const mafString value,int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponent(const double value , int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValue(const mafString value,int component)
//-------------------------------------------------------------------------
{
  if (component>=GetNumberOfComponents())
    SetNumberOfComponents(component+1);
  m_Components[component]=value;
  SetType(MAF_STRING_TAG);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValues(const char **values, int numcomp)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (numcomp!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(numcomp);
  }

  // copy data
  for (int i=0;i<numcomp;i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValues(const std::vector<mafString> values)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (values.size()!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(values.size());
  }

  // copy data
  for (int i=0;i<values.size();i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValues(const double *values, int numcomp)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (numcomp!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(numcomp);
  }

  // copy data
  for (int i=0;i<numcomp;i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mmuTagItem::SetValues(const std::vector<double> values)
//-------------------------------------------------------------------------
{
  // if the number of component differs, reallocate memory
  if (values.size()!=GetNumberOfComponents())
  {
    this->SetNumberOfComponents(values.size());
  }

  // copy data
  for (int i=0;i<values.size();i++)
  {
    this->SetValue(values[i],i);
  }
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponents(const char **values, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(values,numcomp);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponents(const std::vector<mafString> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponents(const double *components, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(components,numcomp);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponents(const std::vector<double> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
const char *mmuTagItem::GetValue(int component) const
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()>component)
  {
    return m_Components[component];
  }
  else
  {
    return NULL;
  }
}

//-------------------------------------------------------------------------
const char *mmuTagItem::GetComponent(int comp) const
//-------------------------------------------------------------------------
{
  return GetValue(comp);
}

//-------------------------------------------------------------------------
double mmuTagItem::GetValueAsDouble(int component) const
//-------------------------------------------------------------------------
{
  const char *tmp=this->GetValue(component);
  if (tmp)
  {
    return atof(tmp);
  }
  else
  {
    return 0;
  }
}

//-------------------------------------------------------------------------
double mmuTagItem::GetComponentAsDouble(int comp) const
//-------------------------------------------------------------------------
{
  return GetValueAsDouble(comp);
}

//-------------------------------------------------------------------------
void mmuTagItem::Initialize()
//-------------------------------------------------------------------------
{
  m_Name="";
  m_Type=MAF_MISSING_TAG;
  m_Components.clear();
}

//-------------------------------------------------------------------------
int mmuTagItem::GetNumberOfComponents() const
//-------------------------------------------------------------------------
{
  return m_Components.size();
}

//-------------------------------------------------------------------------
void mmuTagItem::SetNumberOfComponents(int n)
//-------------------------------------------------------------------------
{
  m_Components.resize(n);  
}

//-------------------------------------------------------------------------
void mmuTagItem::GetValueAsSingleString(mafString &str) const
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()>0)
  {
    int i;
    // the size of the single string is given by the sum of 
    // the sizes of the single components, plus 1 char for
    // "," for each component, plus 2 chars for brackets and
    // one for the trailing "\0".

    // leading parenthesis
    str="(";

    for (i=0;i<GetNumberOfComponents();i++)
    {
      // colon separator
      if (i>0) str<<",";

      str<<"\""<<m_Components[i]<<"\"";    
    }

    // tailing parenthesis
    str<<")";
  }
  else
  {
    // no components
    str="";
  }

}

//-------------------------------------------------------------------------
void mmuTagItem::GetValueAsSingleString(std::string &str) const
//-------------------------------------------------------------------------
{
  mafString tmp;
  GetValueAsSingleString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
void mmuTagItem::GetTypeAsString(mafString &str) const
//-------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case (MAF_MISSING_TAG):
    str = "MIS";
    break;
  case (MAF_NUMERIC_TAG):
    str = "NUM";
    break;
  case (MAF_STRING_TAG):
    str = "STR";
    break;
  default:
    str = "UNK";
  }
}

//-------------------------------------------------------------------------
void mmuTagItem::GetTypeAsString(std::string &str) const
//-------------------------------------------------------------------------
{
  mafString tmp;
  GetTypeAsString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
bool mmuTagItem::Equals(const mmuTagItem *item) const
//-------------------------------------------------------------------------
{
  if (!item)
    return false;

  if (m_Name!=item->m_Name || \
    GetNumberOfComponents()!=item->GetNumberOfComponents() || \
    m_Type!=item->m_Type)
  {
    return false;
  }
  
  // compare single components 
  for (int i=0;i<this->GetNumberOfComponents();i++)
  {
    if (m_Components[i].IsEmpty()&&item->m_Components[i].IsEmpty())
      continue;

    if (m_Components[i]!=item->m_Components[i])
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mmuTagItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent <<"Name: \"" << (m_Name?m_Name:"(NULL)") <<"\"";

  int t=this->GetType();
  char *tstr;

  switch (t)
  {
  case (MAF_MISSING_TAG):
    tstr="MISSING";
    break;
  case (MAF_NUMERIC_TAG):
    tstr="NUMERIC";
    break;
  case (MAF_STRING_TAG):
    tstr="STRING";
    break;
  default:
    tstr="UNKNOWN";
  }


  os << " Type: " << tstr << " (" << t << ") ";

  os << " Components: ";

  if (GetNumberOfComponents()>0)
  {
    mafString tmp;
    GetValueAsSingleString(tmp);
    os  << tmp;
    os  <<" NumComp: " << GetNumberOfComponents();
  }

  os << std::endl; // end of single line printing
}

//-------------------------------------------------------------------------
int mmuTagItem::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("Tag", GetName());
  parent->SetAttribute("Mult",mafString(GetNumberOfComponents()));
  mafString type;
  GetTypeAsString(type);
  parent->SetAttribute("Type",type);

  if (parent->StoreVectorN(m_Components,GetNumberOfComponents(),"TItem","TC")==MAF_ERROR)
    return MAF_ERROR;

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mmuTagItem::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (!node->GetAttribute("Name",m_Name))
    return MAF_ERROR;

  mafString type;
  if (!node->GetAttribute("Type",type))
    return MAF_ERROR;
  
  if (type=="NUM")
  {
    SetType(MAF_NUMERIC_TAG);
  }
  else if (type=="STR")
  {
    SetType(MAF_STRING_TAG);
  }
  else if (type=="MIS")
  {
    SetType(MAF_MISSING_TAG);
  }
  else
  {
    SetType(atof(type));
  }

  mafString num;
  if (!node->GetAttribute("Mult",num))
    return MAF_ERROR;
  
  SetNumberOfComponents(atof(num));
  
  return node->RestoreVectorN(m_Components,GetNumberOfComponents(),"TItem","TC");
}

