/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTagItem.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:04:56 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafTagItem.h"
#include "mafString.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include <vector>
#include <assert.h>

//-------------------------------------------------------------------------
mafTagItem::mafTagItem()
//-------------------------------------------------------------------------
{
  Initialize();
}

//-------------------------------------------------------------------------
mafTagItem::~mafTagItem()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name,  const char *value, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetValue(value);
  SetName(name);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name, const char **values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values,numcomp);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name, const std::vector<mafString> &values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(t);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name, const double value)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValue(value);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name, const double *value, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(value,numcomp);

}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const char *name, const std::vector<double> &values, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::DeepCopy(const mafTagItem *item)
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
void mafTagItem::operator=(const mafTagItem& p)
//-------------------------------------------------------------------------
{
  DeepCopy(&p);
}

//-------------------------------------------------------------------------
mafTagItem::mafTagItem(const mafTagItem& p)
//-------------------------------------------------------------------------
{
  Initialize();
  *this=p;
}

//-------------------------------------------------------------------------
bool mafTagItem::operator==(const mafTagItem& p) const
//-------------------------------------------------------------------------
{
  return Equals(&p);
}

//-------------------------------------------------------------------------
bool mafTagItem::operator!=(const mafTagItem& p) const
//-------------------------------------------------------------------------
{
  return !Equals(&p);
}

//-------------------------------------------------------------------------
void mafTagItem::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}

//-------------------------------------------------------------------------
const char *mafTagItem::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void mafTagItem::SetValue(double value,int component)
//-------------------------------------------------------------------------
{
  mafString tmp(value);
  SetValue(tmp,component);
  SetType(MAF_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponent(const mafString value,int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponent(const double value , int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void mafTagItem::SetValue(const mafString value,int component)
//-------------------------------------------------------------------------
{
  if (component>=GetNumberOfComponents())
    SetNumberOfComponents(component+1);
  m_Components[component]=value;
  SetType(MAF_STRING_TAG);
}

//-------------------------------------------------------------------------
void mafTagItem::SetValues(const char **values, int numcomp)
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
void mafTagItem::SetValues(const std::vector<mafString> values)
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
void mafTagItem::SetValues(const double *values, int numcomp)
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
void mafTagItem::SetValues(const std::vector<double> values)
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
void mafTagItem::SetComponents(const char **values, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(values,numcomp);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const std::vector<mafString> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const double *components, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(components,numcomp);
}

//-------------------------------------------------------------------------
void mafTagItem::SetComponents(const std::vector<double> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
const char *mafTagItem::GetValue(int component) const
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
const char *mafTagItem::GetComponent(int comp) const
//-------------------------------------------------------------------------
{
  return GetValue(comp);
}

//-------------------------------------------------------------------------
double mafTagItem::GetValueAsDouble(int component) const
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
double mafTagItem::GetComponentAsDouble(int comp) const
//-------------------------------------------------------------------------
{
  return GetValueAsDouble(comp);
}

//-------------------------------------------------------------------------
void mafTagItem::Initialize()
//-------------------------------------------------------------------------
{
  m_Name="";
  m_Type=MAF_MISSING_TAG;
  m_Components.clear();
}

//-------------------------------------------------------------------------
int mafTagItem::GetNumberOfComponents() const
//-------------------------------------------------------------------------
{
  return m_Components.size();
}

//-------------------------------------------------------------------------
void mafTagItem::SetNumberOfComponents(int n)
//-------------------------------------------------------------------------
{
  m_Components.resize(n);  
}

//-------------------------------------------------------------------------
void mafTagItem::GetValueAsSingleString(mafString &str) const
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
void mafTagItem::GetValueAsSingleString(std::string &str) const
//-------------------------------------------------------------------------
{
  mafString tmp;
  GetValueAsSingleString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
void mafTagItem::GetTypeAsString(mafString &str) const
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
void mafTagItem::GetTypeAsString(std::string &str) const
//-------------------------------------------------------------------------
{
  mafString tmp;
  GetTypeAsString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
bool mafTagItem::Equals(const mafTagItem *item) const
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
void mafTagItem::Print(std::ostream& os, const int tabs) const
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
int mafTagItem::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("Name", GetName());
  parent->SetAttribute("Mult",mafString(GetNumberOfComponents()));
  mafString type;
  GetTypeAsString(type);
  parent->SetAttribute("Type",type);

  if (parent->StoreVectorN("TItem",m_Components,GetNumberOfComponents(),"TC")==MAF_ERROR)
    return MAF_ERROR;

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafTagItem::InternalRestore(mafStorageElement *node)
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

  mafID num;
  if (!node->GetAttributeAsInteger("Mult",num))
    return MAF_ERROR;
  
  SetNumberOfComponents(num);
  
  return node->RestoreVectorN("TItem",m_Components,GetNumberOfComponents(),"TC");
}

