/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttribute.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-06 21:20:32 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafAttribute.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "assert.h"

#include <ostream>

mafCxxAbstractTypeMacro(mafAttribute);

//-------------------------------------------------------------------------
void mafAttribute::operator=(const mafAttribute &a)
//-------------------------------------------------------------------------
{
  DeepCopy(&a);
}

//-------------------------------------------------------------------------
bool mafAttribute::operator==(const mafAttribute &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
void mafAttribute::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  assert(a);
  assert(a->IsA(GetTypeId()));
  m_Name=a->GetName();
}

//-------------------------------------------------------------------------
mafAttribute *mafAttribute::MakeCopy()
//-------------------------------------------------------------------------
{
  mafAttribute *new_attr=NewInstance();
  assert(new_attr);
  new_attr->DeepCopy(this);
  return new_attr;
}

//-------------------------------------------------------------------------
bool mafAttribute::Equals(const mafAttribute *a) const
//-------------------------------------------------------------------------
{
  return a->IsA(GetTypeId()) && m_Name==a->GetName();
}

//-------------------------------------------------------------------------
void mafAttribute::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}
//-------------------------------------------------------------------------
const char *mafAttribute::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
int mafAttribute::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  return parent->StoreText("Name",m_Name);
}

//-------------------------------------------------------------------------
int mafAttribute::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  return node->RestoreText("Name",m_Name);
}
//-------------------------------------------------------------------------
void mafAttribute::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "Name: " << m_Name.GetCStr() << std::endl;
}