/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuTagItem.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-13 09:10:00 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmuTagItem.h"
#include "mafString.h"
#include "mafIndent.h"
#include <vector>

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
  SetType(mmuTagItem::MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mmuTagItem::operator=(const mmuTagItem& p)
//-------------------------------------------------------------------------
{
  this->SetName(p.GetName());
  this->SetNumberOfComponents(p.GetNumberOfComponents());
  for (int i=0;i<GetNumberOfComponents();i++)
  {
    this->SetValue(p.GetValue(i),i);
  }

  this->SetType(p.GetType());

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
  SetType(mmuTagItem::MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mmuTagItem::SetComponent(const char *value,int component)
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
void mmuTagItem::SetValue(const char *value,int component)
//-------------------------------------------------------------------------
{
  m_Components[component]=value;
  SetType(mmuTagItem::MAF_STRING_TAG);
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
  if (GetNumberOfComponents()>1)
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

      str<<m_Components[i];    
    }

    // tailing parenthesis
    str<<")";
  }
  else if (GetNumberOfComponents()==1)
  {
    // single component value (no parenthisis)
    str=m_Components[0];
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
  case (mmuTagItem::MAF_MISSING_TAG):
    str = "MIS";
    break;
  case (mmuTagItem::MAF_NUMERIC_TAG):
    str = "NUM";
    break;
  case (mmuTagItem::MAF_STRING_TAG):
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
  case (mmuTagItem::MAF_MISSING_TAG):
    tstr="MISSING";
    break;
  case (mmuTagItem::MAF_NUMERIC_TAG):
    tstr="NUMERIC";
    break;
  case (mmuTagItem::MAF_STRING_TAG):
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


