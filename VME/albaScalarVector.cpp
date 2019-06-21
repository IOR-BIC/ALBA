/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaScalarVector
 Authors: Paolo Quadrani
 
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


#include "albaScalarVector.h"
#include "albaIndent.h"

//-----------------------------------------------------------------------
albaCxxTypeMacro(albaScalarVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
albaScalarVector::albaScalarVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
albaScalarVector::~albaScalarVector()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void albaScalarVector::GetScalarVector(std::vector<double> &svector)
//-----------------------------------------------------------------------
{
  svector.clear();
  svector.resize(GetNumberOfScalars());
  int i = 0;
  for (Iterator it = BeginScalarVector(); it != EndScalarVector(); it++, i++)
  {
    svector[i] = it->second;
  }
}
//-----------------------------------------------------------------------
int albaScalarVector::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",albaString(GetNumberOfItems()));
  for (Iterator it = BeginScalarVector(); it != EndScalarVector(); it++)
  {
    double sca[2];
    sca[0] = it->first;
    sca[1] = it->second;
    if (parent->StoreVectorN("Scalar", sca , 2) != ALBA_OK)
      return ALBA_ERROR;
  }
  return ALBA_OK;
}
//-----------------------------------------------------------------------
int albaScalarVector::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  albaID num_items;
  if (node->GetAttributeAsInteger("NumberOfItems",num_items))
  {
    albaStorageElement::ChildrenVector vector_elements;
    node->GetNestedElementsByName("Scalar", vector_elements);

    assert(vector_elements.size() == num_items);

    if (vector_elements.size() != num_items)
      albaWarningMacro("Restore I/O error: found wrong number of scalar value in restored ScalarVector.");

    for (int i = 0; i < vector_elements.size(); i++)
    {
      double sca[2];
      if (vector_elements[i]->RestoreVectorN(sca, 2) != ALBA_OK)
        return ALBA_ERROR;
      AppendItem(sca[0], sca[1]);
    }
    
    return ALBA_OK;
  }

  return ALBA_ERROR;
}
