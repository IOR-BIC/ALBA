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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaTagItem.h"
#include "albaString.h"
#include "albaIndent.h"
#include "albaStorageElement.h"
#include <vector>
#include <assert.h>

//-------------------------------------------------------------------------
albaTagItem::albaTagItem()
//-------------------------------------------------------------------------
{
  Initialize();
}

//-------------------------------------------------------------------------
albaTagItem::~albaTagItem()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name,  const char *value, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetValue(value);
  SetName(name);
  SetType(t);
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name, const char **values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values,numcomp);
  SetType(t);
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name, const std::vector<albaString> &values, int numcomp, int t)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(t);
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name, const double value)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValue(value);
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name, const double *value, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(value,numcomp);

}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const char *name, const std::vector<double> &values, int numcomp)
//-------------------------------------------------------------------------
{
  Initialize();
  SetName(name);
  SetValues(values);
  SetType(ALBA_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void albaTagItem::DeepCopy(const albaTagItem *item)
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
void albaTagItem::operator=(const albaTagItem& p)
//-------------------------------------------------------------------------
{
  DeepCopy(&p);
}

//-------------------------------------------------------------------------
albaTagItem::albaTagItem(const albaTagItem& p)
//-------------------------------------------------------------------------
{
  Initialize();
  *this=p;
}

//-------------------------------------------------------------------------
bool albaTagItem::operator==(const albaTagItem& p) const
//-------------------------------------------------------------------------
{
  return Equals(&p);
}

//-------------------------------------------------------------------------
bool albaTagItem::operator!=(const albaTagItem& p) const
//-------------------------------------------------------------------------
{
  return !Equals(&p);
}

//-------------------------------------------------------------------------
void albaTagItem::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}

//-------------------------------------------------------------------------
const char *albaTagItem::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void albaTagItem::SetValue(double value,int component)
//-------------------------------------------------------------------------
{
  albaString tmp(value);
  SetValue(tmp,component);
  SetType(ALBA_NUMERIC_TAG);
}

//-------------------------------------------------------------------------
void albaTagItem::SetComponent(const albaString value,int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void albaTagItem::SetComponent(const double value , int component)
//-------------------------------------------------------------------------
{
  SetValue(value,component);
}

//-------------------------------------------------------------------------
void albaTagItem::SetValue(const albaString value,int component)
//-------------------------------------------------------------------------
{
  if (component>=GetNumberOfComponents())
    SetNumberOfComponents(component+1);
  m_Components[component]=value;
  SetType(ALBA_STRING_TAG);
}

//-------------------------------------------------------------------------
void albaTagItem::SetValues(const char **values, int numcomp)
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
void albaTagItem::SetValues(const std::vector<albaString> values)
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
void albaTagItem::SetValues(const double *values, int numcomp)
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
void albaTagItem::SetValues(const std::vector<double> values)
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
void albaTagItem::SetComponents(const char **values, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(values,numcomp);
}

//-------------------------------------------------------------------------
void albaTagItem::SetComponents(const std::vector<albaString> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
void albaTagItem::SetComponents(const double *components, int numcomp)
//-------------------------------------------------------------------------
{
  SetValues(components,numcomp);
}

//-------------------------------------------------------------------------
void albaTagItem::SetComponents(const std::vector<double> components)
//-------------------------------------------------------------------------
{
  SetValues(components);
}

//-------------------------------------------------------------------------
const char *albaTagItem::GetValue(int component) const
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
void albaTagItem::RemoveValue(int component)
//-------------------------------------------------------------------------
{
  if (GetNumberOfComponents()>component)
  {
    
    m_Components.erase(m_Components.begin() + component);
  }
  else
  {
    return;
  }
}

//-------------------------------------------------------------------------
const char *albaTagItem::GetComponent(int comp) const
//-------------------------------------------------------------------------
{
  return GetValue(comp);
}

//-------------------------------------------------------------------------
double albaTagItem::GetValueAsDouble(int component) const
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
double albaTagItem::GetComponentAsDouble(int comp) const
//-------------------------------------------------------------------------
{
  return GetValueAsDouble(comp);
}

//-------------------------------------------------------------------------
void albaTagItem::Initialize()
//-------------------------------------------------------------------------
{
  m_Name="";
  m_Type=ALBA_MISSING_TAG;
  m_Components.clear();
}

//-------------------------------------------------------------------------
int albaTagItem::GetNumberOfComponents() const
//-------------------------------------------------------------------------
{
  return m_Components.size();
}

//-------------------------------------------------------------------------
void albaTagItem::SetNumberOfComponents(int n)
//-------------------------------------------------------------------------
{
  m_Components.resize(n);  
}

//----------------------------------------------------------------------------
std::vector<double> albaTagItem::GetComponentsAsDoubles() const
{
	std::vector<double> values;
	for (int i = 0; i < m_Components.size(); i++)
		values.push_back(GetComponentAsDouble(i));
	return values;
}

//-------------------------------------------------------------------------
void albaTagItem::GetValueAsSingleString(albaString &str) const
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
void albaTagItem::GetValueAsSingleString(std::string &str) const
//-------------------------------------------------------------------------
{
  albaString tmp;
  GetValueAsSingleString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
void albaTagItem::GetTypeAsString(albaString &str) const
//-------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case (ALBA_MISSING_TAG):
    str = "MIS";
    break;
  case (ALBA_NUMERIC_TAG):
    str = "NUM";
    break;
  case (ALBA_STRING_TAG):
    str = "STR";
    break;
  default:
    str = "UNK";
  }
}

//-------------------------------------------------------------------------
void albaTagItem::GetTypeAsString(std::string &str) const
//-------------------------------------------------------------------------
{
  albaString tmp;
  GetTypeAsString(tmp);
  str=tmp;
}

//-------------------------------------------------------------------------
bool albaTagItem::Equals(const albaTagItem *item) const
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
void albaTagItem::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  os << indent <<"Name: \"" << (m_Name?m_Name.GetCStr():"(NULL)") <<"\"";

  int t=this->GetType();
  char *tstr;

  switch (t)
  {
  case (ALBA_MISSING_TAG):
    tstr="MISSING";
    break;
  case (ALBA_NUMERIC_TAG):
    tstr="NUMERIC";
    break;
  case (ALBA_STRING_TAG):
    tstr="STRING";
    break;
  default:
    tstr="UNKNOWN";
  }


  os << " Type: " << tstr << " (" << t << ") ";

  os << " Components: ";

  if (GetNumberOfComponents()>0)
  {
    albaString tmp;
    GetValueAsSingleString(tmp);
    os  << tmp.GetCStr();
    os  <<" NumComp: " << GetNumberOfComponents();
  }

  os << std::endl; // end of single line printing
}

//-------------------------------------------------------------------------
int albaTagItem::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("Name", GetName());
  parent->SetAttribute("Mult",albaString(GetNumberOfComponents()));
  albaString type;
  GetTypeAsString(type);
  parent->SetAttribute("Type",type);

  if (parent->StoreVectorN("TItem",m_Components,GetNumberOfComponents(),"TC")==ALBA_ERROR)
    return ALBA_ERROR;

  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaTagItem::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  if (!node->GetAttribute("Name",m_Name))
    return ALBA_ERROR;

  albaString type;
  if (!node->GetAttribute("Type",type))
    return ALBA_ERROR;
  
  if (type=="NUM")
  {
    SetType(ALBA_NUMERIC_TAG);
  }
  else if (type=="STR")
  {
    SetType(ALBA_STRING_TAG);
  }
  else if (type=="MIS")
  {
    SetType(ALBA_MISSING_TAG);
  }
  else
  {
    SetType(atof(type));
  }

  albaID num;
  if (!node->GetAttributeAsInteger("Mult",num))
    return ALBA_ERROR;
  
  SetNumberOfComponents(num);
  
  return node->RestoreVectorN("TItem",m_Components,GetNumberOfComponents(),"TC");
}

