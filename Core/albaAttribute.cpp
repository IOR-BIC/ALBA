/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttribute
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



#include "albaAttribute.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include "assert.h"

#include <ostream>

albaCxxAbstractTypeMacro(albaAttribute);

//-------------------------------------------------------------------------
void albaAttribute::operator=(const albaAttribute &a)
//-------------------------------------------------------------------------
{
  DeepCopy(&a);
}

//-------------------------------------------------------------------------
bool albaAttribute::operator==(const albaAttribute &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
void albaAttribute::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{
  assert(a);
  assert(a->IsA(GetTypeId()));
  m_Name=a->GetName();
}

//-------------------------------------------------------------------------
albaAttribute *albaAttribute::MakeCopy()
//-------------------------------------------------------------------------
{
  albaAttribute *new_attr=NewInstance();
  assert(new_attr);
  new_attr->DeepCopy(this);
  return new_attr;
}

//-------------------------------------------------------------------------
bool albaAttribute::Equals(const albaAttribute *a) const
//-------------------------------------------------------------------------
{
  return a->IsA(GetTypeId()) && m_Name==a->GetName();
}

//-------------------------------------------------------------------------
void albaAttribute::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}
//-------------------------------------------------------------------------
const char *albaAttribute::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
int albaAttribute::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  return parent->StoreText("Name",m_Name);
}

//-------------------------------------------------------------------------
int albaAttribute::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  return node->RestoreText("Name",m_Name);
}
//-------------------------------------------------------------------------
void albaAttribute::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);

  os << indent << "Name: " << m_Name.GetCStr() << std::endl;
}
